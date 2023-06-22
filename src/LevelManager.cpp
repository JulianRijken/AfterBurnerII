#include "pch.h"
#include "LevelManager.h"

#include "GameManager.h"
#include "GameObject.h"
#include "GlobalSettings.h"
#include "PathEnemy.h"
#include "SpriteRenderer.h"
#include "SpriteColorReference.h"
#include "GameTime.h"
#include "Transform.h"
#include "Collider.h"
#include "PlayerController.h"
#include "StaticEnemy.h"


LevelManager::LevelManager(const std::string& name) :
	Behaviour{ name },
	m_SpawnTimer{0},
	m_DistanceTravelled{0},
	m_ActiveLevelState{LevelState::OnDeck},
	m_ActiveSetIndex{0},
	m_ActiveStageIndex{GlobalSettings::StartingStage },
	m_ActiveStage{STAGES[m_ActiveStageIndex]},

	m_SkyColor{m_ActiveStage.skyColor},
	m_GroundPlaneColor{m_ActiveStage.groundPlaneColor},
	m_GroundDetailColor{m_ActiveStage.groundDetailColor},
	m_BushColor{m_ActiveStage.bushColor},
	m_RockColor{m_ActiveStage.rockColor},
	m_DiskColor{m_ActiveStage.diskColor}
{}


void LevelManager::Awake()
{
	m_GameManager = GameManager::Instance;

	// Spawn Ground Plane
	m_GroundPlaneSpriteWPtr = GetRawGameObject()->AddComponent<SpriteRenderer>("GroundPlane",-100,Renderer::RenderMode::Screen);
	GetRawGameObject()->AddComponent<SpriteColorReference>(m_GroundPlaneColor,m_GroundPlaneSpriteWPtr);

	m_GroundDetailSpriteWPtr = GetRawGameObject()->AddComponent<SpriteRenderer>("GroundDetail",-90,Renderer::RenderMode::Screen);
	GetRawGameObject()->AddComponent<SpriteColorReference>(m_GroundDetailColor, m_GroundDetailSpriteWPtr);

	// Spawn Sky Plane
	const auto skyPlaneGameObjectWPtr { GameObject::AddGameObject ("SkyPlane", GetTransform())};
	skyPlaneGameObjectWPtr.lock()->GetRawTransform()->SetPosition(0, 0, GlobalSettings::GROUND_PLANE_DISTANCE);
	skyPlaneGameObjectWPtr.lock()->GetRawTransform()->SetScale(1000000, 1000000, 1);
	m_SkyPlaneWPtr = skyPlaneGameObjectWPtr.lock()->AddComponent<SpriteRenderer>("SkyPlane", -60,Renderer::RenderMode::World);
	skyPlaneGameObjectWPtr.lock()->AddComponent<SpriteColorReference>(m_SkyColor, m_SkyPlaneWPtr);



	// Spawn Ship Deck
	m_ShipDeckGoWPtr = GameObject::AddGameObject("ShipDeck", GetTransform(), Tag::None, false);
	m_ShipDeckGoWPtr.lock()->AddComponent<SpriteRenderer>("ShipDeck", -1, Renderer::RenderMode::World);

	SwitchLevelState(LevelState::OnDeck);
}

void LevelManager::Update()
{
	MoveAllLevelObjects();
	HandleLevelSpawning();
}


void LevelManager::SpawnGroundDetails()
{
	// Don't spawn if all are false
	if(!m_ActiveStage.spawnBush && !m_ActiveStage.spawnDisk && !m_ActiveStage.spawnRock)
		return;

	const float spawnWidth{ m_ActiveStage.bonusStage ? BONUS_SPAWN_WITH : SPAWN_WITH };
	const auto levelObjectGoWPtr{ GameObject::AddGameObject("LevelObject",GetTransform(),Tag::None,true, Vector3f{Jul::RandomRange(-spawnWidth, spawnWidth), 0, GlobalSettings::DETAIL_SPAWN_DISTANCE}) };
	const auto spriteRendererWPtr{ levelObjectGoWPtr.lock()->AddComponent<SpriteRenderer>() };
	spriteRendererWPtr.lock()->SetRenderLayer(-9);
	AddToWorld(levelObjectGoWPtr);

	std::string sprite{};

	switch (Jul::GetRandomItemFromList({ m_ActiveStage.spawnBush,m_ActiveStage.spawnDisk,m_ActiveStage.spawnRock }))
	{
	case 0:
		sprite = "Bush";
		levelObjectGoWPtr.lock()->AddComponent<SpriteColorReference>(m_BushColor, spriteRendererWPtr);
		break;
	case 1:
		sprite = "Disk";
		levelObjectGoWPtr.lock()->AddComponent<SpriteColorReference>(m_DiskColor, spriteRendererWPtr);
		break;
	case 2:
		sprite = "Rock";
		levelObjectGoWPtr.lock()->AddComponent<SpriteColorReference>(m_RockColor, spriteRendererWPtr);
		break;
	}

	spriteRendererWPtr.lock()->SetSprite(sprite);
}

void LevelManager::HandleLevelSpawning()
{
	if(m_ActiveLevelState == LevelState::OnDeck)
		return;

	m_SpawnTimer += Time::GetDeltaTime();

	const float flySpeed{ m_GameManager.lock()->GetFlySpeed() };

	// Make sure the player is moving 
	if(!(flySpeed > 0))
		return;

	if (m_SpawnTimer >= GameManager::MAX_FLY_SPEED / flySpeed * SPAWN_INTERVAL)
	{
		SpawnGroundDetails();

		// No need for precise timer or -= delay. Some variation is not bad
		m_SpawnTimer = 0.0f;
	}
	
}

void LevelManager::MoveAllLevelObjects()
{
	const float flySpeed{ std::max(0.0f,m_GameManager.lock()->GetFlySpeed())};


	if(m_ActiveLevelState != LevelState::OnDeck)
	{
		m_DistanceTravelled += flySpeed * Time::GetDeltaTime();
		IEvents::Invoke(&IEvents::OnDistanceTravelledChange, m_DistanceTravelled);
	}

	for (auto iterator = m_LevelObjectWPtrs.begin(); iterator != m_LevelObjectWPtrs.end();)
	{
		const std::weak_ptr gameObject{ (*iterator).gameObjectWPtr };

		if (gameObject.expired())
		{
			iterator = m_LevelObjectWPtrs.erase(iterator);
			continue;
		}

		Transform* transform{ gameObject.lock()->GetRawTransform() };

		const std::weak_ptr playerGoWPtr{ GameObject::FindWithTag(Tag::Player) };

		std::weak_ptr<PlayerController> playerControllerWPtr{ };
		if (!playerGoWPtr.expired())
			playerControllerWPtr = playerGoWPtr.lock()->GetComponent<PlayerController>();

		float sidewaysFlyEffect{ playerControllerWPtr.expired() ? 0.0f : Jul::Lerp(SIDEWAYS_FLY_SPEED, -SIDEWAYS_FLY_SPEED, playerControllerWPtr.lock()->GetAimLocation().x) };

		if (m_ActiveStage.bonusStage)
			sidewaysFlyEffect = 0.0f;

		transform->Translate(Vector3f{  sidewaysFlyEffect * (Time::GetDeltaTime() * (*iterator).effectMultiplier),0,-flySpeed * (Time::GetDeltaTime() * (*iterator).effectMultiplier) });

		if ((*iterator).destroyBehindCamera)
		{
			if (transform->GetPosition().z < -GlobalSettings::MAIN_GAME_ZOOM)
			{
				GameObject::Destroy(gameObject);
				iterator = m_LevelObjectWPtrs.erase(iterator);
				continue;
			}
		}

		++iterator;
	}
}


void LevelManager::AddToWorld(const std::weak_ptr<GameObject>& gameObject, float effectMultiplier, bool destroyBehindCamera)
{
	m_LevelObjectWPtrs.push_back(LevelObject{gameObject,effectMultiplier,destroyBehindCamera });
}

void LevelManager::SwitchLevelState(LevelState nextState)
{
	switch (nextState)
	{
		case LevelState::OnDeck:
			m_ShipDeckGoWPtr.lock()->GetRawTransform()->SetPosition(0, 0.4f, 0);
			m_ShipDeckGoWPtr.lock()->SetActive(true);
			break;


		case LevelState::InGame:
			m_ShipDeckGoWPtr.lock()->SetActive(false);

			break;

	case LevelState::Landing:

		m_ShipDeckGoWPtr.lock()->GetRawTransform()->SetPosition(0, 0, 40.0f);
		m_ShipDeckGoWPtr.lock()->SetActive(true);


		Time::StartLerpTimer(6.0f, GetSelfPtr(), [&](float alpha)
			{
				m_ShipDeckGoWPtr.lock()->GetRawTransform()->SetPosition(Jul::SmoothEndLerp( Vector3f{ 0, 0, 160.0f },Vector3f{ 0, 0, 10 }, alpha));
			});

			break;
	}

	m_ActiveLevelState = nextState;
}



void LevelManager::OnPlaneAirborne()
{
	if (m_ActiveLevelState == LevelState::OnDeck)
	{
		SwitchLevelState(LevelState::InGame);
		LoadStage(m_ActiveStageIndex);
	}
}

void LevelManager::OnStageFinishedRunning()
{
	Log(TextColor::BrightWhite, BackgroundColor::BrightRed) << "Finished Stage: " << m_ActiveStageIndex;

	if(m_ActiveStageIndex >= STAGE_COUNT - 1)
	{
		const bool gameWon{ true };
		IEvents::Invoke(&IEvents::OnGameOver, gameWon);
	}
	else
	{
		LoadStage(m_ActiveStageIndex + 1);
	}

}

void LevelManager::OnPlaneLanding()
{
	SwitchLevelState(LevelState::Landing);
}


void LevelManager::SpawnEnemy(EnemyType enemyType, Vector2f offset) const
{
	if(enemyType == EnemyType::None)
		return;

	EnemySpawnConfig config{};

	// Setup enemy config
	switch (enemyType)
	{
		case EnemyType::CloseCurveSideLeft:
		case EnemyType::CloseCurveSideRight:
		{
			config.spriteName = (m_ActiveStageIndex == 1 || m_ActiveStageIndex == 9) ? "EnemyClose_Orange" : "EnemyClose_Blue";
			config.moveDuration = 3.5f;
			config.curveScale = Vector2f{ enemyType == EnemyType::CloseCurveSideLeft ? -1.0f : 1.0f,1 };
			config.curve = CURVE_AWAY_SIDE;
			config.animationDelay = 1.2f;
			config.animationDuration = 1.2f;
			config.animationName = enemyType == EnemyType::CloseCurveSideLeft ? "SwingRight" : "SwingLeft";

		}break;

		case EnemyType::CloseCurvePastLeft:
		case EnemyType::CloseCurvePastRight:
		{
			config.spriteName = (m_ActiveStageIndex == 1 || m_ActiveStageIndex == 9) ? "EnemyClose_Orange" : "EnemyClose_Blue";
			config.moveDuration = 3.5f;
			config.curveScale = Vector2f{ enemyType == EnemyType::CloseCurvePastLeft ? -1.0f : 1.0f ,1 };
			config.curve = CURVE_AWAY_FRONT;
			config.animationDelay = 0.5f;
			config.animationDuration = 1.1f;
			config.animationName = enemyType == EnemyType::CloseCurvePastLeft ? "SwingLeft" : "SwingRight";
					
		}break;

		case EnemyType::CloseCurvePastBarrleLeft:
		case EnemyType::CloseCurvePastBarrleRight:
		{
			config.spriteName = (m_ActiveStageIndex == 1 || m_ActiveStageIndex == 9) ? "EnemyClose_Orange" : "EnemyClose_Blue";
			config.moveDuration = 3.5f;
			config.curveScale = Vector2f{ enemyType == EnemyType::CloseCurvePastBarrleLeft ? -1.0f : 1.0f,1 };
			config.curve = CURVE_AWAY_SIDE_ROLL;
			config.animationDelay = 0.5f;
			config.animationDuration = 3.0f;
			config.animationName = enemyType == EnemyType::CloseCurvePastBarrleLeft ? "RollLeft" : "RollRight";
					
		}break;

		case EnemyType::GrayFarStraight:
		case EnemyType::RedFarStraight:
		case EnemyType::BlueFarStraight:
		{
			switch (enemyType)
			{
				case EnemyType::GrayFarStraight: 	config.spriteName = "Enemy_00"; break;
				case EnemyType::RedFarStraight:		config.spriteName = "Enemy_01"; break;
				case EnemyType::BlueFarStraight:	config.spriteName = "Enemy_02"; break;
			}

			config.startDelay = 2.5f;
			config.moveDuration = 2.0f;
			config.curveScale = Vector2f{ 1,1 };
			config.curve = CURVE_TOWARDS_STRAIGHT_DOWN;
			config.rocketDelay = 0.3f;
		}break;

		case EnemyType::GrayFarStraightDive:
		case EnemyType::RedFarStraightDive:
		case EnemyType::BlueFarStraightDive:
		{
			switch (enemyType)
			{
				case EnemyType::GrayFarStraightDive:config.spriteName = "Enemy_00"; break;
				case EnemyType::RedFarStraightDive:		config.spriteName = "Enemy_01"; break;
				case EnemyType::BlueFarStraightDive:	config.spriteName = "Enemy_02"; break;
			}

			config.startDelay = 3.0f;
			config.moveDuration = 2.5f;
			config.curveScale = Vector2f{ 1,1 };
			config.curve = CURVE_TOWARDS_STRAIGHT_DIVE;
			config.rocketDelay = 0.5f;

		}break;

		case EnemyType::Pillar:
		{
			const float scale{ 2.5f };

			config.spriteName = "Pillar";
			config.colliderSize = Vector3f{ 1.0f,3.78125f,0.5f } * scale;
			config.colliderCenter = Vector3f{ 0.0f,3.78125f / 2.0f,0.0f }* scale;
			config.scale = Vector3f::One() * scale;

			config.enemyClass = EnemyClass::StaticEnemy;
			config.tag = Tag::BlockEnemy;
			config.canBeDestroyed = false;

		}break;

		case EnemyType::Tower:
		{
			config.spriteName = "Tower";
			config.colliderSize = Vector3f{ 3.0f,7.0,0.5f };
			config.colliderCenter = Vector3f{ 0.0f,3.5f,0.0f };
			config.enemyClass = EnemyClass::StaticEnemy;
			config.tag = Tag::StaticEnemy;
			config.canBeDestroyed = true;
			config.scale = Vector3f::One() * 1.50f;

		}break;

		case EnemyType::Tents:
		{
			config.spriteName = "Tents";
			config.colliderSize = Vector3f{ 4.0f,2.0,0.5f };
			config.colliderCenter = Vector3f{ 0.0f,1.0f,0.0f };
			config.enemyClass = EnemyClass::StaticEnemy;
			config.tag = Tag::StaticEnemy;
			config.canBeDestroyed = true;

		}break;

		case EnemyType::Truck: 
		{
			config.spriteName = "Truck";
			config.colliderSize = Vector3f{ 4.0f,2.0,0.5f };
			config.colliderCenter = Vector3f{ 0.0f,1.0f,0.0f };
			config.enemyClass = EnemyClass::StaticEnemy;
			config.tag = Tag::StaticEnemy;
			config.canBeDestroyed = true;

		}break;

		case EnemyType::Hangar:
		{
			config.spriteName = "Hangar";
			config.colliderSize = Vector3f{ 4.0f,2.0,0.5f };
			config.colliderCenter = Vector3f{ 0.0f,1.0f,0.0f };
			config.enemyClass = EnemyClass::StaticEnemy;
			config.tag = Tag::StaticEnemy;
			config.canBeDestroyed = true;

		}break;
	}


	// Spawn enemy using config
	const auto enemyGoWPtr{ GameObject::AddGameObject("Enemy",GetTransform(),config.tag,true,Vector3f{0,0,-100},Vector3f::Zero(),config.scale) };
	enemyGoWPtr.lock()->AddComponent<SpriteRenderer>(config.spriteName, 0, Renderer::RenderMode::World);
	enemyGoWPtr.lock()->AddComponent<Collider>(config.colliderSize, config.colliderCenter);

	if(config.enemyClass == EnemyClass::PathEnemy)
		enemyGoWPtr.lock()->AddComponent<PathEnemy>
		(
			offset,
			config.startDelay, config.moveDuration, config.curveCenter, config.curveScale, config.curve,
			config.rocketDelay,
			config.animationDelay, config.animationDuration, config.animationName
		);
	else
		enemyGoWPtr.lock()->AddComponent<StaticEnemy>(config.canBeDestroyed,offset);

	if(Jul::RandomValue() <= THE_ENEMY_SCREAM_CHANCE)
		AudioManager::PlaySoundEffect({ "TheEnemy", "BeCareful" });
}

void LevelManager::LoadStage(int nextStageIndex)
{
	nextStageIndex = Jul::Clamp(nextStageIndex, 0, STAGE_COUNT);

	AudioManager::PlaySoundEffect("LevelComplete", 0.5f);

	const Stage nextStage{ STAGES[nextStageIndex] };

	const Color4f fromSkyColor{ m_SkyColor };
	const Color4f fromGroundDetailColor{ m_GroundDetailColor };
	const Color4f fromGroundPlaneColor{ m_GroundPlaneColor };

	const Color4f fromBushColor{ m_BushColor };
	const Color4f fromDiskColor{ m_DiskColor };
	const Color4f fromRockColor{ m_RockColor };

	// Requires a copy
	Time::StartLerpTimer(COLOR_CHANGE_TIME, GetSelfPtr(), [=](float alpha)
		{
			m_SkyColor = Jul::SmoothLerp(fromSkyColor, nextStage.skyColor, alpha);
			m_GroundDetailColor = Jul::SmoothLerp(fromGroundDetailColor, nextStage.groundDetailColor, alpha);
			m_GroundPlaneColor = Jul::SmoothLerp(fromGroundPlaneColor, nextStage.groundPlaneColor, alpha);

			m_BushColor = Jul::SmoothLerp(fromBushColor, nextStage.bushColor, alpha);
			m_DiskColor = Jul::SmoothLerp(fromDiskColor, nextStage.diskColor, alpha);
			m_RockColor = Jul::SmoothLerp(fromRockColor, nextStage.rockColor, alpha);
		}
	);

	// Change active stage half way
	Time::StartTimer(COLOR_CHANGE_TIME / 3.0f, false, GetSelfPtr(), [&](int index)
	{
		m_ActiveStageIndex = index;
		m_ActiveStage = STAGES[index];
		IEvents::Invoke(&IEvents::OnStageChange, index, m_ActiveStage.bonusStage);
		RunActiveStage();
	}, nextStageIndex);
}

void LevelManager::RunActiveStage()
{
	Log(TextColor::BrightWhite, BackgroundColor::BrightGreen) << "Running Stage: " << m_ActiveStageIndex;

	m_ActiveSetIndex = 0;
	m_WaitingForFirstDelay = true;

	if (m_ActiveStage.refuel)
		IEvents::Invoke(&IEvents::OnPlaneRefuel);

	AudioManager::PlayMusic(STAGE_MUSIC[m_ActiveStageIndex]);


	if(m_ActiveStage.spawnSets.empty())
	{
		Log(LogType::Warning) << "Stage: " << m_ActiveStageIndex << " enemys not setup";
		OnStageFinishedRunning();
		return;
	}


	Time::StartTimer(0.0f, true, GetSelfPtr(), [&](Time::Timer* timer)
		{

			/// NOT EFFICIENT FINDING EVERY FRAME
			const auto& playerGoWPtr{ GameObject::FindWithTag(Tag::Player) };
			if(!playerGoWPtr.expired())
				if(playerGoWPtr.lock()->GetComponent<PlayerController>().lock()->GetActiveState() != PlayerController::PlayerState::Flying)
					return;

			const SpawnSet spawnSet{ m_ActiveStage.spawnSets[m_ActiveSetIndex] };

			if (m_WaitingForFirstDelay)
			{
				// Wait for before delay
				if (timer->GetElapsedTime() >= spawnSet.delayBefore)
				{
					// Spawn Enemys
					for (const EnemySpawnSettings enemySpawnSettings : spawnSet.spawnSettingsVector)
					{
						Time::StartTimer(enemySpawnSettings.delay, false, GetSelfPtr(), [&](EnemySpawnSettings enemySpawnSettings)
							{
								SpawnEnemy(enemySpawnSettings.enemyType, enemySpawnSettings.offset);
							}, enemySpawnSettings);
					}

					m_WaitingForFirstDelay = false;
					timer->Reset();

				}
			}
			else
			{
				// Wait for after delay
				if (timer->GetElapsedTime() >= spawnSet.delayAfter)
				{
					m_ActiveSetIndex++;
					m_WaitingForFirstDelay = true;
					timer->Reset();

					if (m_ActiveSetIndex >= m_ActiveStage.spawnSets.size())
					{
						timer->Cancel();
						OnStageFinishedRunning();
					}
				}
			}

		});
}


