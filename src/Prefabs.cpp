#include "pch.h"
#include "Prefabs.h"

#include "Animator.h"
#include "AudioManager.h"
#include "Coin.h"
#include "GameManager.h"
#include "GameObject.h"
#include "LevelManager.h"
#include "SpriteRenderer.h"
#include "Transform.h"
#include "Jul.h"
#include "TargetVisual.h"


void Prefabs::SpawnBigExplosion(Vector3f position, float drag)
{
	const float destroyTime{0.4f};
	const auto mainSceneParentTransform{ GameManager::Instance.lock()->GetMainSceneTransform() };
	const auto explosionGoWPtr{ GameObject::AddGameObject("Explosion", mainSceneParentTransform, Tag::None,true,position,Vector3f::Zero(),Vector3f::One() * 0.75f)};
	explosionGoWPtr.lock()->AddComponent<SpriteRenderer>("Explosion", 0, Renderer::RenderMode::World);
	explosionGoWPtr.lock()->AddComponent<Animator>().lock()->PlayAnimation("Explode", false, 0, 1.0f / destroyTime);

	AudioManager::PlaySoundEffect("Explosion_00", 0.8f);
	LevelManager::AddToWorld(explosionGoWPtr, drag);

	GameObject::Destroy(explosionGoWPtr, destroyTime);
}

void Prefabs::SpawnSmallExplosion(Vector3f position, float drag)
{
	const float destroyTime{ 0.4f };
	const auto mainSceneParentTransform{ GameManager::Instance.lock()->GetMainSceneTransform() };
	const auto explosionGoWPtr{ GameObject::AddGameObject("Explosion", mainSceneParentTransform, Tag::None,true,position,Vector3f::Zero(),Vector3f::One() * 0.5f) };
	explosionGoWPtr.lock()->AddComponent<SpriteRenderer>("Explosion", 0, Renderer::RenderMode::World);
	explosionGoWPtr.lock()->AddComponent<Animator>().lock()->PlayAnimation("Explode", false, 0, 1.0f / destroyTime);

	AudioManager::PlaySoundEffect("Explosion_00", 0.8f);
	LevelManager::AddToWorld(explosionGoWPtr, drag);

	GameObject::Destroy(explosionGoWPtr, destroyTime);
}


void Prefabs::SpawnCloud(float drag, Vector3f position, Vector3f scale)
{
	drag = Jul::Clamp01(drag);

	const float destroyTime{ 0.3f };

	const auto mainSceneParentTransform{ GameManager::Instance.lock()->GetMainSceneTransform() };
	const auto cloudGoWPtr{ GameObject::AddGameObject("Cloud", mainSceneParentTransform, Tag::None,true,position,Vector3f{0,0,0},scale) };
	cloudGoWPtr.lock()->AddComponent<SpriteRenderer>("Cloud", 0, Renderer::RenderMode::World);
	cloudGoWPtr.lock()->AddComponent<Animator>().lock()->PlayAnimation("Idle", false, 0, 1.0f / destroyTime);

	LevelManager::AddToWorld(cloudGoWPtr,1.0f - drag);

	GameObject::Destroy(cloudGoWPtr, destroyTime);
}

void Prefabs::SpawnCoin(Vector3f position)
{
	AudioManager::PlaySoundEffect("Pickup_Coin", 0.2f);
	const auto mainSceneParentTransform{ GameManager::Instance.lock()->GetMainSceneTransform() };
	const auto coinGoWPtr = GameObject::AddGameObject("Coin", mainSceneParentTransform);
	coinGoWPtr.lock()->GetRawTransform()->SetPosition(position);
	coinGoWPtr.lock()->AddComponent<SpriteRenderer>("Coin");
	coinGoWPtr.lock()->AddComponent<Animator>().lock()->PlayAnimation("Rotate", true);
	coinGoWPtr.lock()->AddComponent<Coin>();
}


std::weak_ptr<TargetVisual> Prefabs::SpawnTargetVisual(const std::weak_ptr<Transform>& targetTransform)
{
	const auto mainSceneParentTransform{ GameManager::Instance.lock()->GetMainSceneTransform() };
	const auto targetGoWPtr{ GameObject::AddGameObject("TargetVisual",mainSceneParentTransform) };
	const auto spriteRendererWPtr{ targetGoWPtr.lock()->AddComponent<SpriteRenderer>("TargetVisual",40) };
	spriteRendererWPtr.lock()->SetIgnoreCameraRoll(true);
	
	return targetGoWPtr.lock()->AddComponent<TargetVisual>(targetTransform);
}
