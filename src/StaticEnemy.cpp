#include "pch.h"
#include "StaticEnemy.h"

#include "Collider.h"
#include "GameManager.h"
#include "GameObject.h"
#include "GlobalSettings.h"
#include "Jul.h"
#include "PlayerController.h"
#include "Prefabs.h"
#include "GameTime.h"
#include "Transform.h"

StaticEnemy::StaticEnemy(bool canBeDestroyed, Vector2f offset) :
Behaviour{"Static Enemy"},
IDamageable{1.0f},
OFFSET{offset},
m_CanBeDestroyed{canBeDestroyed}
{}

void StaticEnemy::OnDeath()
{
	if (m_CanBeDestroyed)
	{
		GameObject::Destroy(GetGameObject());
		IEvents::Invoke(&IEvents::OnEnemyHit);

		for (int i = 0; i < 3; ++i)
		{
			const Vector3f offset
			{
				Jul::RandomRange(-1.0f,1.0f),
				Jul::RandomRange(3.0f,5.0f) ,
				Jul::RandomRange(-1.0f,1.0f),
			};

			Prefabs::SpawnBigExplosion(GetRawTransform()->GetPosition() + offset * 0.4f,1.0f);


			const auto playerControllerWPtr{ GameManager::Instance.lock()->GetPlayerControllerWPtr() };
			if (!playerControllerWPtr.expired())
			{
				if (playerControllerWPtr.lock()->IsInvincible())
				{
					Prefabs::SpawnCoin(GetRawTransform()->GetPosition() + Vector3f::Forward() * 0.1f);
				}
			}
		}

	}
}

void StaticEnemy::Awake()
{
	GetRawTransform()->SetPosition(OFFSET.x, OFFSET.y, GlobalSettings::STATIC_ENEMY_SPAWN_DISTANCE);

	m_StartScale = GetRawTransform()->GetScale();
}

void StaticEnemy::Update()
{
	const float scaleAlpha{Jul::MapValueInRangeClamped(GetRawTransform()->GetPosition().z,SCALE_START_DISTANCE,-GlobalSettings::MAIN_GAME_ZOOM)};

	const Vector3f nextPosition{ (Vector3f::Forward() * Time::GetDeltaTime() * -GameManager::Instance.lock()->GetFlySpeed()) + GetRawTransform()->GetPosition()};

	//GetRawTransform()->SetScale(Jul::Lerp(m_StartScale, MIN_SCALE,scaleAlpha));
	GetRawTransform()->SetPosition(nextPosition);

	if (GetRawTransform()->GetPosition().z < -GlobalSettings::MAIN_GAME_ZOOM)
		GameObject::Destroy(GetGameObject());
}


