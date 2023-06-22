#include "pch.h"
#include "PathEnemy.h"

#include "Animator.h"
#include "GameManager.h"
#include "GameObject.h"
#include "IEvents.h"
#include "Jul.h"
#include "PlayerController.h"
#include "Prefabs.h"
#include "Rocket.h"
#include "GameTime.h"
#include "Transform.h"


PathEnemy::PathEnemy
(
	Vector2f offset,
	float startDelay, float moveDuration, Vector3f curveCenter, Vector2f curveScale, Jul::CubicCurve curve,
	float rocketDelay,
	float animationDelay, float animationDuration, std::string animationName
) :
	Behaviour{"Path Close"},
	ANIMATION_DELAY{ animationDelay },
	ANIMATION_DURATION{ animationDuration },
	ANIMATION_NAME{std::move(animationName)},
	START_DELAY{ startDelay },
	MOVE_DURATION{ moveDuration },
	CURVE_CENTER{ curveCenter },
	CURVE_SCALE{curveScale},
	CURVE{ Jul::ScaleCurve(curve,curveCenter,curveScale) },
	OFFSET{ offset },
	ROCKET_DELAY{ rocketDelay }
{}

void PathEnemy::Awake()
{
	const Vector3f curveLocation = Jul::EvaluateCubicBezier(CURVE, 0.0f);
	GetRawTransform()->SetPosition(curveLocation + Vector3f{ OFFSET.x,OFFSET.y,0.0f });

	Time::StartTimer(START_DELAY, false, GetSelfPtr(), [&]()
		{
			if (ANIMATION_DURATION > 0.0f)
			{
				Time::StartTimer(ANIMATION_DELAY, false, GetSelfPtr(), [&]()
					{
						GetRawGameObject()->AddComponent<Animator>().lock()->PlayAnimation(ANIMATION_NAME, false, 0, 1.0f / ANIMATION_DURATION);
					});
			}

			if (ROCKET_DELAY > 0.0f)
			{
				Time::StartTimer(ROCKET_DELAY, false, GetSelfPtr(), [&]()
					{
						FireRocket();
					});
			}

			Time::StartLerpTimer(MOVE_DURATION, GetSelfPtr(), [&](float alpha)
				{

					GetRawTransform()->SetRotation(0, 0, alpha * 0.0f);

					const Vector3f curveLocation = Jul::EvaluateCubicBezier(CURVE, alpha);
					GetRawTransform()->SetPosition(curveLocation + Vector3f{ OFFSET.x,OFFSET.y,0.0f });

					if (alpha == 1.0f)
						GameObject::Destroy(GetGameObject());
				});
		});
}

void PathEnemy::OnDrawGizmos()
{
	GizmosDrawCubicBezier(CURVE, 0.5f,100,2.0f,-1,Vector3f{ OFFSET.x,OFFSET.y,0.0f});
}

void PathEnemy::OnDeath()
{
	GameObject::Destroy(GetGameObject());
	Prefabs::SpawnBigExplosion(GetRawTransform()->GetPosition());
	IEvents::Invoke(&IEvents::OnEnemyHit);
}

void PathEnemy::FireRocket() const
{
	// Do not fire when the player is gone or not flying
	const auto playerWPtr{ GameObject::FindWithTag(Tag::Player) };
	if (playerWPtr.expired())
		return;

	const auto playerControllerWPtr{ playerWPtr.lock()->GetComponent<PlayerController>() };
	if(playerControllerWPtr.expired())
		return;

	if(playerControllerWPtr.lock()->GetActiveState() != PlayerController::PlayerState::Flying)
		return;

	// Get the main scene as parent
	const auto mainSceneParentTransform{ GameManager::Instance.lock()->GetMainSceneTransform() };
	if(mainSceneParentTransform.expired())
		return;


	const auto rocketGoWPtr{ GameObject::AddGameObject("Rocket",mainSceneParentTransform) };
	rocketGoWPtr.lock()->GetRawTransform()->SetPosition(GetRawTransform()->GetPosition() + Vector3f::Forward() * 0.1f);
	rocketGoWPtr.lock()->GetRawTransform()->SetScale(0.8f);
	rocketGoWPtr.lock()->AddComponent<SpriteRenderer>("Rocket");
	rocketGoWPtr.lock()->AddComponent<Animator>();


	Vector3f targetDirection{ (playerWPtr.lock()->GetRawTransform()->GetPosition() - GetRawTransform()->GetPosition()).Normalized() };
	//targetDirection += Vector3f::Up() * -0.1f;
	//targetDirection += Vector3f::Up() * -0.05f;

	// The rocket settings should all be in a struct and custom for each enemy if possible!
	targetDirection += Vector3f::Up() * 0.07f;

	rocketGoWPtr.lock()->AddComponent<Rocket>(targetDirection, Vector3f{ 0,0,0 }, playerWPtr.lock()->GetTransform(),
		0.1f,	//float dropTime 
		8.0f,	//float dropSpeed 
		100.0f, //float flySpeed
		0.15f,	//float rotateSpeed
		10.0f,	// maxLifeTime = 10.0f,
		100.0f,	// hitDamage = 100.0f,
		0.0f,	// rotateSpeedDistanceAddition = 0.0f
		50.0f	// rotationCancelDistance = 0.0f
		);
}
