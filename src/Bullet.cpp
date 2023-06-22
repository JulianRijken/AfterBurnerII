#include "pch.h"
#include "Bullet.h"

#include "AudioManager.h"
#include "Collider.h"
#include "GameObject.h"
#include "Transform.h"
#include "GlobalSettings.h"
#include "IDamageable.h"
#include "Jul.h"
#include "Prefabs.h"
#include "GameTime.h"


Bullet::Bullet(const Vector3f& velocity) :
	Behaviour{ "Bullet" },
	VELOCITY{ velocity }
{}

void Bullet::Update()
{
	// If bullet hit the ground
	if (GetRawTransform()->GetPosition().y < 0)
	{
		if(Jul::RandomValue() > SPAWN_CHANCE)
			Prefabs::SpawnSmallExplosion(GetRawTransform()->GetPosition());

		GameObject::Destroy(GetGameObject());
		return;
	}

	// If bullet goes to far
	if (GetRawTransform()->GetPosition().z > GlobalSettings::MAX_ENEMY_SPAWN_DISTANCE)
	{
		GameObject::Destroy(GetGameObject());
		return;
	}


	// Handle collision with enemys
	const Vector3f currentPosition{ GetRawTransform()->GetPosition() };
	const Vector3f deltaVelocity{ VELOCITY * Time::GetDeltaTime() };
	const Vector3f nextPosition{ deltaVelocity + currentPosition };

	Collider::CollisionInfo collisionInfo;
	if(Collider::HasCollisionPredictivePlanerGlobal(currentPosition,nextPosition, collisionInfo))
	{
		// Check tag
		for (const auto tag : ENEMY_TAG)
		if (collisionInfo.hitCollider->GetRawGameObject()->CompareTag(tag))
		{
			// Check damageable
			const auto& damageableWPtr{ collisionInfo.hitCollider->GetRawGameObject()->GetComponent<IDamageable>() };
			if (!damageableWPtr.expired())
			{
				// Apply Damage
				damageableWPtr.lock()->ChangeHealth(-HIT_DAMAGE);

				AudioManager::PlaySoundEffect({"Hit_00","Hit_00","Hit_00"}, 0.1f);

				GameObject::Destroy(GetGameObject());
				Prefabs::SpawnSmallExplosion(GetRawTransform()->GetPosition());

				return;
			}
		}
	}

	GetRawTransform()->SetPosition(nextPosition);
}

