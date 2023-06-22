#include "pch.h"
#include "PlayerController.h"


#include "Camera.h"
#include "GameObject.h"
#include "Input.h"
#include "Jul.h"
#include "SpriteRenderer.h"
#include "GameTime.h"
#include "Transform.h"

#include "Rocket.h"
#include "Animator.h"
#include "AudioManager.h"
#include "Bullet.h"
#include "Collider.h"
#include "GameManager.h"
#include "LevelManager.h"
#include "Prefabs.h"

class Animator;
class Rocket;
class Bullet;

PlayerController::PlayerController(const bool enabled, const std::string& name) :
	Behaviour{name},

	m_ActiveMovementSettings{SKY_MOVEMENT_SETTINGS},
	m_AimLocation{DEFAULT_AIM_LOCATION},

	m_SmoothAimLocation{m_AimLocation},
	m_RocketAmmoCount{50},
	m_FireTimer{0},
	m_FireCount{0},
	m_BarrleRollAngle{0},
	m_TimeHoldingSide{0},
	m_RocketFireSide{ false },

	m_InBarrleRoll{0},

	m_MoveToAimTarget{ true },
	m_ManualCameraControl{false}
{
}

void PlayerController::Awake()
{

	// Get Sprite renderer
	m_PlayerSpriteComponentWPtr = GetRawGameObject()->GetComponent<SpriteRenderer>();
	m_PlayerSpriteComponentWPtr.lock()->SetIgnoreCameraRoll(true);

	// Spawn Aim target
	m_CrosshairWPtr = GameObject::AddGameObject("Crosshair", GetTransform());
	const auto aimTargetWPtr{ m_CrosshairWPtr.lock()->AddComponent<SpriteRenderer>("AimTarget",30) };
	aimTargetWPtr.lock()->SetIgnoreCameraRoll(true);



	// Spawn RefuelPlane
	m_RefuelPlaneGoWPtr = GameObject::AddGameObject("RefuelPlane", GetTransform(),Tag::None,false);
	m_RefuelPlaneGoWPtr.lock()->AddComponent<SpriteRenderer>("FluePlane",-5,Renderer::RenderMode::Screen);

	// // Spawn Camera for player
	// const auto playerCamera{ GameObject::AddGameObject("PlayerCamera", Tag::PlayerCamera)};
	// playerCamera.lock()->GetTransform()->SetPosition(Vector3f{0,0,-GlobalSettings::MAIN_GAME_ZOOM});
	// m_PlayerCameraPtr = playerCamera->AddComponent(new Camera{ GlobalSettings::MAIN_GAME_ZOOM, GlobalSettings::MAIN_GAME_ORTHO_SIZE , GlobalSettings::PIXELATE });

	m_MainCameraWPtr = Camera::GetMainCamera();

	// Reset main camera position in case of it not being properly reset after an animation
	if (!m_MainCameraWPtr.expired())
		m_MainCameraWPtr.lock()->GetRawTransform()->SetPosition(0.0f, 0.0f, -GlobalSettings::MAIN_GAME_ZOOM);

	m_GameManagerWPtr = GameManager::Instance;

	m_RocketFireTimerPtr = Time::StartTimer(GetSelfPtr());

	IEvents::Invoke(&IEvents::OnAmmoChange, m_RocketAmmoCount);

	LaunchPlane();
}

void PlayerController::Update()
{
	if(m_MainCameraWPtr.expired() || m_PlayerSpriteComponentWPtr.expired() || m_CrosshairWPtr.expired())
		return;

	// Hide crosshair when player is not at z0
	m_CrosshairWPtr.lock()->SetActive(GetRawTransform()->GetPosition().z == 0);

	UpdateAimTargetBasedOnInput();

	if (m_ActivePlayerState == PlayerState::Flying || m_ActivePlayerState == PlayerState::Respawning)
		HandleGunFire();

	m_AimLocation.x = Jul::Clamp01(m_AimLocation.x);
	m_AimLocation.y = Jul::Clamp01(m_AimLocation.y);

	m_SmoothAimLocation.x = Jul::Lerp(m_SmoothAimLocation.x, m_AimLocation.x, Time::GetDeltaTime() * m_ActiveMovementSettings.SmoothAmount);
	m_SmoothAimLocation.y = Jul::Lerp(m_SmoothAimLocation.y, m_AimLocation.y, Time::GetDeltaTime() * m_ActiveMovementSettings.SmoothAmount);


	m_PlayerAimLocation.x = Jul::MapValueInRangeClamped(m_SmoothAimLocation.x, m_ActiveMovementSettings.PlayerMoveRange, Jul::OneMinus(m_ActiveMovementSettings.PlayerMoveRange), 0.0f, 1.0f);
	m_PlayerAimLocation.y = Jul::MapValueInRangeClamped(m_SmoothAimLocation.y, m_ActiveMovementSettings.PlayerMoveRange, Jul::OneMinus(m_ActiveMovementSettings.PlayerMoveRange), 0.0f, 1.0f);

	IEvents::Invoke(&IEvents::OnPlayerAimTargetChange, m_PlayerAimLocation);

	if (!m_ManualCameraControl)
	{
		m_MainCameraWPtr.lock()->GetRawTransform()->SetPosition
		(
			Jul::Lerp(m_ActiveMovementSettings.CameraPositionExtends.left, m_ActiveMovementSettings.CameraPositionExtends.right, m_SmoothAimLocation.x),
			Jul::Lerp(m_ActiveMovementSettings.CameraPositionExtends.bottom, m_ActiveMovementSettings.CameraPositionExtends.top, m_SmoothAimLocation.y)
		);

		m_MainCameraWPtr.lock()->GetRawTransform()->SetRotation
		(
			Jul::Lerp(m_ActiveMovementSettings.CameraRotationExtends.minPitch, m_ActiveMovementSettings.CameraRotationExtends.maxPitch, m_SmoothAimLocation.y), /*Pitch*/
			Jul::Lerp(m_ActiveMovementSettings.CameraRotationExtends.minYaw, m_ActiveMovementSettings.CameraRotationExtends.maxYaw, m_SmoothAimLocation.x), /*Yaw*/
			Jul::Lerp(m_ActiveMovementSettings.CameraRotationExtends.minRoll, m_ActiveMovementSettings.CameraRotationExtends.maxRoll, m_SmoothAimLocation.x) + m_BarrleRollAngle /*Roll*/
		);
	}

	if(m_MoveToAimTarget)
		ApplyMovementBasedOnAimTarget();


	UpdateTargets();
	HandleCollision();


	if (Input::GetKeyDown("fireRocket"))
	{
		if (m_RocketFireTimerPtr->GetElapsedTime() > ROCKET_FIRE_DELAY)
		{
			if (m_RocketAmmoCount > 0)
			{
				if (m_ActivePlayerState == PlayerState::Flying || m_ActivePlayerState == PlayerState::Respawning)
				{
					FireRocket();
					m_RocketFireTimerPtr->Reset();
				}
			}
		}
	}

}


void PlayerController::OnDeath()
{
	if(m_InBarrleRoll || m_ActivePlayerState != PlayerState::Flying)
	{
		SetHealth(m_MaxHealth);
		return;
	}

	if(!m_Invincible)
		DestroyPlane();
}

void PlayerController::OnPlaneRefuel()
{
	if(m_ActivePlayerState == PlayerState::ReFueling)
		return;

	RefuelPlane();
}

void PlayerController::OnPlayerRespawn()
{
	Respawn();
}

void PlayerController::OnGameOver(bool won)
{
	if (won)
	{
		LandPlane();
	}
	else
	{
		Time::StartTimer(0.0f, true, GetSelfPtr(), [&](Time::Timer* timer)
			{
				m_AimLocation = Jul::MoveTowards(m_AimLocation, GAME_OVER_AIM_LOCATION, Time::GetDeltaTime() * 0.2f);
		if (m_AimLocation == GAME_OVER_AIM_LOCATION)
			timer->Cancel();
			});
	}
}

void PlayerController::OnStageChange(int stage, bool bonusStage)
{
	m_LastActiveMovementSettings = m_ActiveMovementSettings;
	m_TargetMovementSettings = bonusStage ? GROUND_MOVEMENT_SETTINGS : SKY_MOVEMENT_SETTINGS;

	Time::StartLerpTimer(MOVEMENT_MODE_SWITCH_TIME, GetSelfPtr(), [&](float alpha)
		{
			m_ActiveMovementSettings.CrosshairPositionExtends = Jul::SmoothLerp(m_LastActiveMovementSettings.CrosshairPositionExtends, m_TargetMovementSettings.CrosshairPositionExtends, 0.5f);
			m_ActiveMovementSettings.PlanePositionExtends =		Jul::SmoothLerp(m_LastActiveMovementSettings.PlanePositionExtends, m_TargetMovementSettings.PlanePositionExtends, alpha);
			m_ActiveMovementSettings.CameraPositionExtends =	Jul::SmoothLerp(m_LastActiveMovementSettings.CameraPositionExtends, m_TargetMovementSettings.CameraPositionExtends, alpha);
			m_ActiveMovementSettings.CameraRotationExtends =	Jul::SmoothLerp(m_LastActiveMovementSettings.CameraRotationExtends, m_TargetMovementSettings.CameraRotationExtends, alpha);
			m_ActiveMovementSettings.PlayerMoveRange =			Jul::SmoothLerp(m_LastActiveMovementSettings.PlayerMoveRange, m_TargetMovementSettings.PlayerMoveRange, alpha);
			m_ActiveMovementSettings.SmoothAmount =				Jul::SmoothLerp(m_LastActiveMovementSettings.SmoothAmount, m_TargetMovementSettings.SmoothAmount, alpha);
			m_ActiveMovementSettings.MoveSpeed =				Jul::SmoothLerp(m_LastActiveMovementSettings.MoveSpeed, m_TargetMovementSettings.MoveSpeed, alpha);
			m_ActiveMovementSettings.HoldTimeBeforeBarrelRoll = Jul::SmoothLerp(m_LastActiveMovementSettings.HoldTimeBeforeBarrelRoll, m_TargetMovementSettings.HoldTimeBeforeBarrelRoll, alpha);
			m_ActiveMovementSettings.TimeWindowsForBarrelRoll = Jul::SmoothLerp(m_LastActiveMovementSettings.TimeWindowsForBarrelRoll, m_TargetMovementSettings.TimeWindowsForBarrelRoll, alpha);
			m_ActiveMovementSettings.CanBarrelRoll = m_TargetMovementSettings.CanBarrelRoll;
		});
}


void PlayerController::UpdateAimTargetBasedOnInput()
{
	const Vector2f inputDirection{ Input::GetDirection("playerMovement") };

	if (m_InBarrleRoll != 0)
	{
		m_BarrleRollAngle += Time::GetDeltaTime() * float(m_InBarrleRoll) * 360;

		m_AimLocation.x += Time::GetDeltaTime() * float(m_InBarrleRoll);

		if (abs(m_BarrleRollAngle) > 360)
		{
			m_BarrleRollAngle = 0;
			m_InBarrleRoll = 0;
		}
	}
	else
	{
		if (!(m_ActivePlayerState == PlayerState::Flying || m_ActivePlayerState == PlayerState::Respawning))
			return;


		if ((m_AimLocation.x == 0 && inputDirection.x < 0) || (m_AimLocation.x == 1 && inputDirection.x > 0))
			m_TimeHoldingSide += Time::GetDeltaTime();
		else
			m_TimeHoldingSide -= Time::GetDeltaTime();

		m_TimeHoldingSide = Jul::Clamp(m_TimeHoldingSide, 0.0f, m_ActiveMovementSettings.HoldTimeBeforeBarrelRoll + m_ActiveMovementSettings.TimeWindowsForBarrelRoll);

		if (!(m_AimLocation.x == 0 || m_AimLocation.x == 1))
			m_TimeHoldingSide = 0;



		if (m_TimeHoldingSide > m_ActiveMovementSettings.HoldTimeBeforeBarrelRoll)
		{
			if (m_AimLocation.x == 0 && inputDirection.x > 0)
			{
				m_InBarrleRoll = 1;
				m_TimeHoldingSide = 0;
			}
			else if (m_AimLocation.x == 1 && inputDirection.x < 0)
			{
				m_InBarrleRoll = -1;
				m_TimeHoldingSide = 0;
			}
		}

		m_AimLocation += inputDirection * Time::GetDeltaTime() * m_ActiveMovementSettings.MoveSpeed;
	}
}

void PlayerController::ApplyMovementBasedOnAimTarget() const
{
	const Vector2f targetPos
	{
		Jul::Lerp(m_ActiveMovementSettings.CrosshairPositionExtends.left, m_ActiveMovementSettings.CrosshairPositionExtends.right, m_PlayerAimLocation.x),
		Jul::Lerp(m_ActiveMovementSettings.CrosshairPositionExtends.bottom, m_ActiveMovementSettings.CrosshairPositionExtends.top,  m_PlayerAimLocation.y)
	};
	m_CrosshairWPtr.lock()->GetRawTransform()->SetPosition(m_MainCameraWPtr.lock()->FlatToWorldPoint(targetPos));



	const Vector2f screenPosition
	{
		Jul::Lerp(m_ActiveMovementSettings.PlanePositionExtends.left, m_ActiveMovementSettings.PlanePositionExtends.right, m_PlayerAimLocation.x),
		Jul::Lerp(m_ActiveMovementSettings.PlanePositionExtends.bottom, m_ActiveMovementSettings.PlanePositionExtends.top, m_PlayerAimLocation.y)
	};
	GetRawTransform()->SetPosition(m_MainCameraWPtr.lock()->FlatToWorldPoint(screenPosition));


	m_PlayerSpriteComponentWPtr.lock()->SetDrawCell(int(roundf(m_PlayerAimLocation.y * 2.0f)), int(roundf(m_PlayerAimLocation.x * 4.0f)));
}


void PlayerController::HandleGunFire()
{
	//Time::StartTimerOnObject(s)

	if (m_FireCount < BULLET_BURST_BULLET_COUNT)
	{
		if (m_FireTimer > BULLET_FIRE_DELAY)
		{
			if(m_FireCount == 0)
				AudioManager::PlaySoundEffect("BulletFire",0.5f);

			m_FireTimer -= BULLET_FIRE_DELAY;

			// Prevent the timer from overshooting when timescale is really really high
			if (m_FireTimer > BULLET_FIRE_DELAY)
				m_FireTimer = 0.0f;

			m_FireCount++;

			// The random is hard coded add a clean solution later!
			for (int i = 0; i < 2; ++i)
				Time::StartTimer(rand() % 10 / 100.0f, false,GetSelfPtr(), [=]() {FireBullet(i);});
		}
	}
	else
	{
		if (m_FireTimer > BULLET_BURST_DELAY)
		{
			m_FireCount = 0;
			m_FireTimer -= BULLET_BURST_DELAY;
		}
	}

	m_FireTimer += Time::GetDeltaTime();
}

void PlayerController::UpdateTargets()
{
	if (m_MainCameraWPtr.expired())
		return;

	// If target is expired remove it from the vector
	for (auto iterator = m_Targets.begin(); iterator != m_Targets.end();)
	{
		if ((*iterator).expired())
		{
			iterator = m_Targets.erase(iterator);
			continue;
		}

		++iterator;
	}

	const auto& enemyPtrs{ GameObject::FindGameObjectsWithTag(Tag::TargetEnemy) };

	for (const std::weak_ptr<GameObject>& enemyWPtr : enemyPtrs)
	{
		const auto targetTransform{ enemyWPtr.lock()->GetTransform() };

		const Vector3f enemyPosition{ targetTransform.lock()->GetPosition() };
		const Vector2f enemyScreenPosition{ m_MainCameraWPtr.lock()->WorldToFlatPoint(enemyPosition) };
		const Vector2f crosshairScreenPosition{ m_MainCameraWPtr.lock()->WorldToFlatPoint(m_CrosshairWPtr.lock()->GetRawTransform()->GetPosition()) };

		const float distanceToEnemy{ Jul::GetDistanceBetweenPoints2D(crosshairScreenPosition, enemyScreenPosition) };


		if (distanceToEnemy < MIN_DISTANCE_FOR_LOCK)
		{
			// Reset time if target already exists
			for (auto iterator = m_Targets.begin(); iterator != m_Targets.end();)
			{
				const std::weak_ptr<TargetVisual>& targetWPtr{ (*iterator) };

				// If target is the same as the found target continue
				if (targetWPtr.lock()->GetTarget().lock() == targetTransform.lock())
				{
					targetWPtr.lock()->ResetTime();
					return;
				}

				++iterator;
			}

			m_Targets.push_back(Prefabs::SpawnTargetVisual(targetTransform));
		}
	}
}


void PlayerController::FireBullet(bool side) const
{

	const int sideDirection{ (side ? 1 : -1) };

	const float distance{ float(sideDirection) * INITIAL_BULLET_SPREAD };
	const float height{ BULLET_FIRE_HEIGHT_OFFSET };
	const Vector3f sideOffset{ GetRawTransform()->Right2D() * distance };
	const Vector3f heightOffset{ GetRawTransform()->Up2D() * height };

	// Get aim target
	Vector3f aimTargetWorldPositionAtDistance{ GetAimTargetWorldPosition() };
	aimTargetWorldPositionAtDistance += sideOffset;
	aimTargetWorldPositionAtDistance += heightOffset;

	const Vector3f fireLocation{ GetRawTransform()->GetPosition() + sideOffset + heightOffset };


	const Vector3f fireDirection{ (aimTargetWorldPositionAtDistance - fireLocation).Normalized() };

	const auto bulletGoWPtr{ GameObject::AddGameObject("Bullet", GetTransform(),Tag::None,true, fireLocation) };
	bulletGoWPtr.lock()->AddComponent<SpriteRenderer>("Bullet");
	bulletGoWPtr.lock()->AddComponent<Bullet>(fireDirection * BULLET_FIRE_SPEED);
}

void PlayerController::FireRocket()
{
	if (m_MainCameraWPtr.expired())
		return;

	m_RocketAmmoCount--;
	m_RocketAmmoCount = Jul::Clamp(0,MAX_AMMO,m_RocketAmmoCount);
	IEvents::Invoke(&IEvents::OnAmmoChange, m_RocketAmmoCount);

	AudioManager::PlaySoundEffect("RocketFire_00",0.8f);
	AudioManager::PlaySoundEffect("FireScream",0.8f);


	const auto rocketGoWPtr{ GameObject::AddGameObject("Rocket", GetTransform()) };
	rocketGoWPtr.lock()->GetRawTransform()->SetPosition(GetRawTransform()->GetPosition() + Vector3f::Forward() * 0.1f);
	rocketGoWPtr.lock()->GetRawTransform()->SetScale(0.8f);

	rocketGoWPtr.lock()->AddComponent<SpriteRenderer>("Rocket");
	rocketGoWPtr.lock()->AddComponent<Animator>();


	m_RocketFireSide = !m_RocketFireSide;

	const Vector3f startDirection
	{
		-0.1f * float(m_RocketFireSide ? 1 : -1),
		0.03f,
		1
	};

	if (m_Targets.empty())
	{
		// Fire rocket left and right with no target

		const Vector3f defaultTarget{ 50 * float(m_RocketFireSide ? 1 : -1),0,0 };

		rocketGoWPtr.lock()->AddComponent<Rocket>(startDirection, defaultTarget, std::weak_ptr<Transform>{},
			0.35f,
			5.0f,
			150.0f,
			1.7f,
			10.0f,
			100.0f
			);
	}
	else
	{
		// Fire rocket at enemy
		const auto firstTargetWPtr{ m_Targets.front() };
		m_Targets.pop_front();
	
		rocketGoWPtr.lock()->AddComponent<Rocket>
		(
			startDirection,
			Vector3f{ 0,0,0 },
			firstTargetWPtr.lock()->GetTarget(),
			0.35f, 	//float dropTime
			5.0f,   //float dropSpeed
			150.0f, //float flySpeed
			2.0f,   //float rotateSpeed
			10.0f,  //float maxLifeTime
			100.0f,	//float hitDamage
			8.0f,	//float rotateSpeedDistanceAddition
			0.0f	//float rotationCancelDistance
		);
	}
}

void PlayerController::AddRocket()
{
	m_RocketAmmoCount++;
	AudioManager::PlaySoundEffect("NewRocket");
	IEvents::Invoke(&IEvents::OnAmmoChange, m_RocketAmmoCount);
}

void PlayerController::HandleCollision()
{
	if(m_ActivePlayerState != PlayerState::Flying)
		return;

	const Vector3f nextPosition{ GetRawTransform()->GetPosition() + (Vector3f::Forward() * -100.0f * Time::GetDeltaTime()) };	Collider::CollisionInfo collisionInfo;
	if (Collider::HasCollisionPredictivePlanerGlobal(GetRawTransform()->GetPosition(), nextPosition, collisionInfo,GetRawGameObject()->GetTag()))
	{
		// Check tag
		if (collisionInfo.hitCollider->GetRawGameObject()->CompareTag(Tag::BlockEnemy))
			ChangeHealth(-100.0f);
	}
}


void PlayerController::Respawn()
{
	// Lerps the fly speed and moves player to center
	Time::StartTimer(0.0f, true, GetSelfPtr(), [&](Time::Timer* timer)
		{
			m_AimLocation = Jul::MoveTowards(m_AimLocation, DEFAULT_AIM_LOCATION, Time::GetDeltaTime());

			const float currentFlySpeed{ m_GameManagerWPtr.lock()->GetFlySpeed() };
			m_GameManagerWPtr.lock()->SetFlySpeed(Jul::MoveTowards(currentFlySpeed, DEFAULT_FLY_SPEED, Time::GetDeltaTime() * 100));

			if (currentFlySpeed == DEFAULT_FLY_SPEED && m_AimLocation == DEFAULT_AIM_LOCATION)
			{
				m_ActivePlayerState = PlayerState::Respawning;
				m_AimLocation = Vector2f{ DEFAULT_AIM_LOCATION };
				m_MoveToAimTarget = true;
				BlinkPlayer();

				timer->Cancel();
			}
		});
}

void PlayerController::BlinkPlayer()
{
	m_PlayerSpriteComponentWPtr.lock()->SetVisible(true);
	AudioManager::PlaySoundEffect("GetReady");

	Time::StartTimer(0.15f, true, GetSelfPtr(), [&](Time::Timer* timer)
		{
			if (timer->GetTimesLooped() >= 8)
			{
				m_ActivePlayerState = PlayerState::Flying;
				timer->Cancel();
				return;
			}

			m_PlayerSpriteComponentWPtr.lock()->SetVisible(!Jul::IsEven(timer->GetTimesLooped()));
		});
}


void PlayerController::LaunchPlane()
{
	m_ActivePlayerState = PlayerState::Launching;
	m_MoveToAimTarget = false;

	m_PlayerSpriteComponentWPtr.lock()->SetDrawCell(0, 2);

	AudioManager::PlaySoundEffect("GetReady");

	Time::StartTimer(0.5f, false, GetSelfPtr(), [&]()
		{
			AudioManager::PlaySoundEffect("BurnStart");
			AudioManager::PlaySoundEffect("RocketFire_01");


			Time::StartTimer(0.07f, true, GetSelfPtr(), [&](Time::Timer* timer)
				{
					if (timer->GetTimesLooped() > LAUNCH_ANIMATION_CLOUD_COUNT)
						timer->Cancel();


					const float lifetime{0.6f};

					for (int direction = -1; direction <= 1; ++direction)
					{
						if(direction == 0)
							continue;

						const Vector3f cloudSpawnLocation{ GetRawTransform()->GetPosition() + Vector3f::Right() * 0.5f * float(direction) + Vector3f::Up() * -0.3f };

						const auto mainSceneParentTransform{ GameManager::Instance.lock()->GetMainSceneTransform() };
						const auto cloudGoWPtr{ GameObject::AddGameObject("Cloud",mainSceneParentTransform,Tag::None,true,cloudSpawnLocation) };

						cloudGoWPtr.lock()->AddComponent<SpriteRenderer>("Cloud", 0, Renderer::RenderMode::World);
						cloudGoWPtr.lock()->AddComponent<Animator>().lock()->PlayAnimation("Idle", false, 0, 1.0f / lifetime);

						const float drag{ std::min(1.0f, float(timer->GetTimesLooped()) / float(LAUNCH_ANIMATION_CLOUD_COUNT / 2.0f)) };
						LevelManager::AddToWorld(cloudGoWPtr, drag);

						GameObject::Destroy(cloudGoWPtr, lifetime);
					}
				});
		});

	Time::StartLerpTimer(LAUNCH_DURATION, GetSelfPtr(), [&](float alpha)
		{
			const float smoothAlpha = Jul::SmoothStartLerp(0.0f,1.0f, alpha * alpha);

			GetRawTransform()->SetPosition(Jul::Lerp(LAUNCH_ANIMATION_START_LOCATION,LAUNCH_ANIMATION_END_LOCATION, smoothAlpha));

			if(alpha >= 1)
			{
				m_ActivePlayerState = PlayerState::Flying;
				m_MoveToAimTarget = true;
				IEvents::Invoke(&IEvents::OnPlaneAirborne);
			}
		}
	);

}

void PlayerController::RefuelPlane()
{
	if (m_ActivePlayerState != PlayerState::Flying)
		return;

	m_ActivePlayerState = PlayerState::ReFueling;
	m_RefuelingState = 0;
	m_RocketsRefueled = 0;

	m_RefuelPlaneGoWPtr.lock()->SetActive(true);


	Time::StartTimer(0.0f, true, GetSelfPtr(), [&](Time::Timer* timer)
		{
			switch (m_RefuelingState)
			{
				case 0:
				{
					m_AimLocation = Jul::MoveTowards(m_AimLocation, REFUEL_ANIMATION_AIM_LOCATION_1, Time::GetDeltaTime() * 0.2f);

					if (m_AimLocation == REFUEL_ANIMATION_AIM_LOCATION_1 && m_BarrleRollAngle == 0.0f)
					{
						timer->Reset();
						Time::StartLerpTimer(4.0f, GetSelfPtr(), [&](float alpha)
							{
								const float smoothAlpha{ Jul::SmoothEndLerp(0.0f,1.0f,alpha) };
								const float rounded{ truncf(smoothAlpha * REFUEL_PLANE_PRECISION) / REFUEL_PLANE_PRECISION };

								m_RefuelPlaneGoWPtr.lock()->GetRawTransform()->SetPosition(0, 104.0f * (1.0f - rounded) + GlobalSettings::RENDER_HEIGHT);
							});

						m_RefuelingState++;
					}
				}break;

				case 1:
				{
					if (timer->GetElapsedTime() > 0.5f)
					{
						m_RefuelingState++;

						m_StartLocationRefuel = GetRawTransform()->GetPosition();

						m_MoveToAimTarget = false;
						m_CrosshairWPtr.lock()->SetActive(false);
						m_LastLocationRefuel = GetRawTransform()->GetPosition();
						timer->Reset();
					}
				}break;

				case 2:
				{
					GetRawTransform()->SetPosition(Jul::MoveTowards(GetRawTransform()->GetPosition(), Vector3f{ 0,0,20 }, Time::GetDeltaTime() * 15));

					const float localAlpha{ Jul::MapValueInRangeClamped(timer->GetElapsedTime(),0.0f,1.5f,0.0f,1.0f) };

					GetRawTransform()->SetPosition(Jul::Lerp(m_LastLocationRefuel, REFUEL_ANIMATION_LOCATION_2, localAlpha));

					if (localAlpha == 1.0f)
					{
						m_RefuelingState++;
						m_LastLocationRefuel = GetRawTransform()->GetPosition();
						timer->Reset();
					}
				}break;

				case 3:
				{
					GetRawTransform()->SetPosition(Jul::MoveTowards(GetRawTransform()->GetPosition(), Vector3f{ 0,0,20 }, Time::GetDeltaTime() * 15));

					const float localAlpha{ Jul::MapValueInRangeClamped(timer->GetElapsedTime(),0.0f,1.5f,0.0f,1.0f) };

					GetRawTransform()->SetPosition(Jul::Lerp(m_LastLocationRefuel, REFUEL_ANIMATION_LOCATION_3, localAlpha));


					if (localAlpha == 1.0f)
					{
						m_RefuelingState++;
						m_LastLocationRefuel = GetRawTransform()->GetPosition();
						timer->SetDelay(REFUEL_ROCKET_DELAY);
						timer->Reset();
					}
				}break;


				case 4: // Plane connected
				{
						if(m_RocketsRefueled < REFUEL_ROCKET_COUNT && m_RocketAmmoCount < MAX_AMMO)
						{
							m_RocketsRefueled++;
							AddRocket();
						}
						else
						{
							Time::StartLerpTimer(4.0f, GetSelfPtr(), [&](float alpha)
								{
									const float smoothAlpha{ Jul::SmoothStartLerp(1.0f,0.0f,alpha) };
									const float rounded{ truncf(smoothAlpha * REFUEL_PLANE_PRECISION) / REFUEL_PLANE_PRECISION };

									m_RefuelPlaneGoWPtr.lock()->GetRawTransform()->SetPosition(0, 104.0f * (1.0f - rounded) + GlobalSettings::RENDER_HEIGHT);

									if (alpha == 1.0f)
										m_RefuelPlaneGoWPtr.lock()->SetActive(false);
								});

							m_RefuelingState++;

							timer->SetDelay(0.0f);
							timer->Reset();

						}
					
				}break;


				case 5:
				{
					GetRawTransform()->SetPosition(Jul::MoveTowards(GetRawTransform()->GetPosition(), Vector3f{ 0,0,20 }, Time::GetDeltaTime() * 15));

					const float localAlpha{ Jul::MapValueInRangeClamped(timer->GetElapsedTime(),0.0f,1.5f,0.0f,1.0f) };

					GetRawTransform()->SetPosition(Jul::Lerp(m_LastLocationRefuel, REFUEL_ANIMATION_LOCATION_2, localAlpha));

					if (localAlpha == 1.0f)
					{
						m_LastLocationRefuel = GetRawTransform()->GetPosition();
						m_RefuelingState++;
						timer->Reset();
					}
				}break;



				case 6:
				{
					GetRawTransform()->SetPosition(Jul::MoveTowards(GetRawTransform()->GetPosition(), Vector3f{ 0,0,20 }, Time::GetDeltaTime() * 15));

					const float localAlpha{ Jul::MapValueInRangeClamped(timer->GetElapsedTime(),0.0f,1.5f,0.0f,1.0f) };

					GetRawTransform()->SetPosition(Jul::Lerp(m_LastLocationRefuel, m_StartLocationRefuel, localAlpha));

					if (localAlpha == 1.0f)
					{
						m_MoveToAimTarget = true;
						m_ActivePlayerState = PlayerState::Flying;
						m_RefuelingState++;
					}
				}break;

			}


		});

}

void PlayerController::LandPlane()
{
	m_MoveToAimTarget = true;
	m_CrosshairWPtr.lock()->SetActive(false);
	m_LandingState = 0;

	Time::StartTimer(0.0f, true, GetSelfPtr(), [&](Time::Timer* timer)
		{
			switch (m_LandingState)
			{
				case 0:
				{
					if (timer->GetElapsedTime() >= 3.0f)
					{
						m_ActivePlayerState = PlayerState::Landing;
						m_LandingState++;
						timer->Reset();
					}
				}break;

				case 1:
				{
					m_AimLocation = Jul::MoveTowards(m_AimLocation, LAND_ANIMATION_AIM_LOCATION_1, Time::GetDeltaTime() * 0.3f);

					if (m_AimLocation == LAND_ANIMATION_AIM_LOCATION_1 && m_BarrleRollAngle == 0.0f)
					{
						m_LandingState++;
						timer->Reset();
					}
				}break;

				case 2:
				{
					m_AimLocation = Jul::MoveTowards(m_AimLocation, LAND_ANIMATION_AIM_LOCATION_2, Time::GetDeltaTime() * 0.3f);

					if (m_AimLocation == LAND_ANIMATION_AIM_LOCATION_2 && m_BarrleRollAngle == 0.0f)
					{
						m_MoveToAimTarget = false;
						m_ManualCameraControl = true;
						m_LandingState++;
						m_LandAnimationLastCameraRotation = m_MainCameraWPtr.lock()->GetRawTransform()->GetRotation();
						m_LandAnimationLastCameraPosition = m_MainCameraWPtr.lock()->GetRawTransform()->GetPosition();
						m_LastLocationBeforeLanding = GetRawTransform()->GetPosition();

						Time::StartLerpTimer(3.0f, GetSelfPtr(), [&](float alpha)
							{
								const Vector3f newRotation{Jul::SmoothEndLerp(m_LandAnimationLastCameraRotation,LAND_ANIMATION_DOWN_CAMERA_ROTATION,alpha) };
								const Vector3f newPosition{Jul::SmoothEndLerp(m_LandAnimationLastCameraPosition,LAND_ANIMATION_DOWN_CAMERA_POSITION,alpha) };

								m_MainCameraWPtr.lock()->GetRawTransform()->SetRotation(newRotation);
								m_MainCameraWPtr.lock()->GetRawTransform()->SetPosition(newPosition);

								if (alpha == 1.0f)
								{
									m_LandingState++;
								}
							});

						timer->Reset();
					}
				}break;

				case 3:
				{
					// Wait for first camera movement

				}break;


				case 4:
				{
					if(timer->GetElapsedTime() > 2.0f)
					{
						m_LandingState++;

						//Time::StartTimer(1.0f, false, GetSelfPtr(), [&]()
						//	{
								IEvents::Invoke(&IEvents::OnPlaneLanding);
							//});

						timer->Reset();
					}
				}break;

				case 5:
				{
					Time::StartLerpTimer(5.0f, GetSelfPtr(), [&](float alpha)
						{
							const Vector3f newRotation{ Jul::SmoothLerp(LAND_ANIMATION_DOWN_CAMERA_ROTATION,LAND_ANIMATION_STRAIGHT_CAMERA_ROTATION,alpha) };
							const Vector3f newPosition{ Jul::SmoothLerp(LAND_ANIMATION_DOWN_CAMERA_POSITION,LAND_ANIMATION_STRAIGHT_CAMERA_POSITION,alpha) };

							m_MainCameraWPtr.lock()->GetRawTransform()->SetRotation(newRotation);
							m_MainCameraWPtr.lock()->GetRawTransform()->SetPosition(newPosition);
						});

					Time::StartLerpTimer(5.0f, GetSelfPtr(), [&](float alpha)
						{
							const Vector3f planePosition{ Jul::SmoothLerp(m_LastLocationBeforeLanding,LAND_ANIMATION_PLANE_LOCATION,alpha) };

							GetRawTransform()->SetPosition(planePosition);
						});

					m_LandingState++;
				}break;


			}
		});
}

void PlayerController::DestroyPlane()
{
	if(m_ActivePlayerState != PlayerState::Flying)
		return;

	m_ActivePlayerState = PlayerState::Crashing;
	m_DyingState = 0;


	Time::StartTimer(0.07f, true, GetSelfPtr(), [&](Time::Timer* timer)
		{
			if (m_ActivePlayerState != PlayerState::Crashing)
				timer->Cancel();

			if (m_DyingState < 5)
				Prefabs::SpawnCloud(0.0f,GetRawTransform()->GetPosition());
		});

	Time::StartTimer(0.07f, true, GetSelfPtr(), [&](Time::Timer* timer)
		{
			if (m_ActivePlayerState != PlayerState::Crashing)
				timer->Cancel();

			if(m_DyingState == 5)
				Prefabs::SpawnBigExplosion(GetRawTransform()->GetPosition(),0.85f);
		});


	// Play Animation
	Time::StartTimer(0.0f, true, GetSelfPtr(),[&](Time::Timer* timer)
	{
			// Quick if the game force respawn, can be removed
			if(m_ActivePlayerState != PlayerState::Crashing)
			{
				timer->Cancel();
				return;
			}


			switch (m_DyingState)
			{
			case 0: // Move to top right

				m_AimLocation = Jul::MoveTowards(m_AimLocation, DEATH_ANIMATION_AIM_LOCATION_1, Time::GetDeltaTime());

				if (m_AimLocation == DEATH_ANIMATION_AIM_LOCATION_1 && m_BarrleRollAngle == 0.0f)
				{
					timer->Reset();
					Prefabs::SpawnBigExplosion(GetRawTransform()->GetPosition());
					m_DyingState++;
				}

				break;

			case 1: // Wait in top right

				if (timer->GetElapsedTime() > 1.2f)
				{
					m_DyingState++;
					Prefabs::SpawnBigExplosion(GetRawTransform()->GetPosition());
				}
				break;

			case 2: // Move to middle
				m_AimLocation = Jul::MoveTowards(m_AimLocation, DEATH_ANIMATION_AIM_LOCATION_2, Time::GetDeltaTime());

				if (m_AimLocation == DEATH_ANIMATION_AIM_LOCATION_2)
				{
					timer->Reset();
					m_DyingState++;
				}
				break;


			case 3: // Move to bottom middle
				m_AimLocation = Jul::MoveTowards(m_AimLocation, DEATH_ANIMATION_AIM_LOCATION_3, Time::GetDeltaTime());

				if (m_AimLocation == DEATH_ANIMATION_AIM_LOCATION_3)
				{
					timer->Reset();
					m_MoveToAimTarget = false;
					m_DyingState++;
				}
				break;

			case 4: // Move to ground
				m_AimLocation = Jul::MoveTowards(m_AimLocation, DEATH_ANIMATION_AIM_LOCATION_4, Time::GetDeltaTime());


				GetRawTransform()->SetPosition(Jul::MoveTowards(GetRawTransform()->GetPosition(), Vector3f{ 0,0,20 }, Time::GetDeltaTime() * 15));

				if (GetRawTransform()->GetPosition() == Vector3f{ 0,0,20 })
				{
					timer->Reset();
					m_PlayerSpriteComponentWPtr.lock()->SetVisible(false);
					m_LastPositionBeforeDeath = GetTransform().lock()->GetPosition();
					m_DyingState++;
				}
				break;

			case 5: // Slide on ground
			{
				const float localAlpha{ Jul::MapValueInRangeClamped(timer->GetElapsedTime(),0.0f,2.0f,0.0f,1.0f) };

				GetRawTransform()->SetPosition(Jul::Lerp(m_LastPositionBeforeDeath, Vector3f{ 0,0,40 }, localAlpha));
				//m_MainCameraWPtr.lock()->GetRawTransform()->SetRotation(Jul::Lerp(Vector3f{ 0,0,0 }, Vector3f{ 0,0,0 }, localAlpha * 5));

				m_GameManagerWPtr.lock()->SetFlySpeed(Jul::Lerp(100.0f,0.0f, localAlpha));

				if (localAlpha == 1.0f)
				{
					timer->Reset();
					m_DyingState++;
				}
			}
				break;

			case 6: // Wait before restarting

				if (timer->GetElapsedTime() > 1.0f)
					m_DyingState++;

				break;
			default:

				IEvents::Invoke(&IEvents::OnPlayerDestroyed);
				timer->Cancel();
			}
	});

}


Vector3f PlayerController::GetAimTargetWorldPosition() const
{
	const Vector3f aimTargetWorldPosition{ m_CrosshairWPtr.lock()->GetRawTransform()->GetPosition() };
	return  Vector3f{ m_MainCameraWPtr.lock()->FlatToWorldPoint(m_MainCameraWPtr.lock()->WorldToFlatPoint(aimTargetWorldPosition),AIM_TARGET_WORLD_DISTANCE) };
}

PlayerController::PlayerState PlayerController::GetActiveState() const
{
	return m_ActivePlayerState;
}




