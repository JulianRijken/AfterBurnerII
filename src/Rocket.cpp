#include "pch.h"
#include "Rocket.h"

#include "SpriteRenderer.h"
#include "Animator.h"
#include "Collider.h"
#include "GameObject.h"
#include "IDamageable.h"
#include "Prefabs.h"
#include "GameTime.h"
#include "Transform.h"

Rocket::Rocket(const Vector3f& startDirection, const Vector3f& defaultTarget, std::weak_ptr<Transform> target,
               float dropTime, float dropSpeed, float flySpeed,
               float rotateSpeed, float maxLifeTime, float hitDamage,
               float rotateSpeedDistanceAddition, float rotationCancelDistance) :

	DROP_TIME{dropTime},
	DROP_SPEED{dropSpeed},
	FLY_SPEED{flySpeed},
	ROTATE_SPEED{rotateSpeed},
	MAX_LIFE_TIME{maxLifeTime},
	HIT_DAMAGE{hitDamage},
	ROTATE_SPEED_DISTANCE_ADDITION{rotateSpeedDistanceAddition},
	CANCEL_ROTATION_DISTANCE{ rotationCancelDistance },

	m_Dropping(true),
	m_TargetHit(false),

	m_FlyDirection{startDirection.Normalized()},
	m_StaticTargetPosition{defaultTarget},
	m_DynamicTargetTransform{std::move(target)}
{}


void Rocket::Awake()
{
	m_RocketSpriteRenderer = GetRawGameObject()->GetComponent<SpriteRenderer>();
	m_RocketAnimator = GetRawGameObject()->GetComponent<Animator>();

	m_RocketAnimator.lock()->PlayAnimation("Dropping");

	GameObject::Destroy(GetGameObject(), MAX_LIFE_TIME);


	m_StartPosition = GetRawTransform()->GetPosition();

	if (!m_DynamicTargetTransform.expired())
	{
		m_TargetCollider = m_DynamicTargetTransform.lock()->GetRawGameObject()->GetComponent<Collider>();

		if (m_TargetCollider.expired())
			Log(LogType::Warning) << "Rocket target transform has no collider! " << m_DynamicTargetTransform.lock()->GetRawGameObject()->GetName();

		m_StaticTargetPosition = m_DynamicTargetTransform.lock()->GetRawTransform()->GetPosition();
	}


	Time::StartTimer(DROP_TIME, false, GetSelfPtr(), [&]()
	{
			m_Dropping = false;

			m_RocketAnimator.lock()->PlayAnimation(m_FlyDirection.z < 0 ? "FlyingTowards" : "FlyingAway", true);

			Time::StartTimer(CLOUD_SPAWN_INTERVAL, true, GetSelfPtr(), [&]()
			{
				if (m_FlyDirection.z > 0)
				Prefabs::SpawnCloud(0.3f, GetRawTransform()->GetPosition());
				else
					Prefabs::SpawnCloud(1.0f, GetRawTransform()->GetPosition());
			});
	});
}

void Rocket::Update()
{

	if(m_TargetHit)
		return;

	if(m_RocketSpriteRenderer.expired() || m_RocketAnimator.expired())
		return;

	if(m_Dropping)
	{
		GetRawTransform()->Translate(0, -DROP_SPEED * Time::GetDeltaTime(), 0.0f);
		return;
	}

	HandleRotation();


	// Out of bounds check
	if (GetRawTransform()->GetPosition().z > GlobalSettings::MAX_ENEMY_SPAWN_DISTANCE)
	{
		Prefabs::SpawnSmallExplosion(GetRawTransform()->GetPosition());
		GameObject::Destroy(GetGameObject());
		return;
	}

	// Get rocket out of camera
	if (GetRawTransform()->GetPosition().z < -10.0f)
	{
		GameObject::Destroy(GetGameObject());
		return;
	}

	// Destroy when hitting ground
	if (GetRawTransform()->GetPosition().y < 0)
	{
		Prefabs::SpawnSmallExplosion(GetRawTransform()->GetPosition());
		GameObject::Destroy(GetGameObject());
		return;
	}


	// Handle rocket collision
	const Vector3f nextPosition{ GetRawTransform()->GetPosition() + (m_FlyDirection * Time::GetDeltaTime() * FLY_SPEED) };
	if (!m_DynamicTargetTransform.expired() && !m_TargetCollider.expired())
	{
		if (m_TargetCollider.lock()->HasCollisionPredictivePlaner(GetRawTransform()->GetPosition(), nextPosition))
		{
			OnHitTarget();
			return;
		}
	}


	// Apply Movement
	GetRawTransform()->SetPosition(nextPosition);
}


void Rocket::OnHitTarget() const
{
	// If rocket has a dynamic target
	if(m_DynamicTargetTransform.expired())
		return;

	const auto& damageableWPtr{ m_DynamicTargetTransform.lock()->GetRawGameObject()->GetComponent<IDamageable>() };

	// If dynamic target has a damageable interface
	if(damageableWPtr.expired())
		return;

	// Deal damage
	damageableWPtr.lock()->ChangeHealth(-HIT_DAMAGE);

	GameObject::Destroy(GetGameObject());
}

void Rocket::HandleRotation()
{
	Vector3f rotateTarget{ m_StaticTargetPosition};

	if (!m_DynamicTargetTransform.expired())
	{
		// Checks if rocket is past dynamic target
		const bool canRotate
		{
			(m_StaticTargetPosition.z - m_StartPosition.z) < 0.0f ?
			(GetRawTransform()->GetPosition().z > m_DynamicTargetTransform.lock()->GetPosition().z) :
			(GetRawTransform()->GetPosition().z < m_DynamicTargetTransform.lock()->GetPosition().z)
		};

		if(!canRotate)
			return;

		rotateTarget = m_DynamicTargetTransform.lock()->GetPosition();
	}

	
	const float distance{ (rotateTarget - GetRawTransform()->GetPosition()).Length() };

	if (distance <= CANCEL_ROTATION_DISTANCE)
	{
		if(GlobalSettings::DrawGizmos)
			m_RocketSpriteRenderer.lock()->SetColor(0.0f, 1.0f, 0.0f, 1.0f);
		return;
	}
	else
	{
		if (GlobalSettings::DrawGizmos)
			m_RocketSpriteRenderer.lock()->SetColor(1.0f, 0.0f, 0.0f, 1.0f);
	}
	


	const float startDistance{ (m_StaticTargetPosition - m_StartPosition).Length() };
	const float distancePower{ distance / startDistance };

	const float distanceExtraRotateSpeed{Jul::SmoothStartLerp(1.0f,0.0f,distancePower) * ROTATE_SPEED_DISTANCE_ADDITION };

	const float finalRotateSpeed{ std::max(0.0f,(ROTATE_SPEED + distanceExtraRotateSpeed)) };


	const Vector3f targetVector{ (rotateTarget - GetRawTransform()->GetPosition()) };
	m_FlyDirection = Jul::MoveTowards(m_FlyDirection, targetVector.Normalized(), Time::GetDeltaTime() * finalRotateSpeed);
	m_FlyDirection = m_FlyDirection.Normalized();
}
