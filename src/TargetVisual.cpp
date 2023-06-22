#include "pch.h"
#include "TargetVisual.h"

#include "GameObject.h"
#include "SpriteRenderer.h"
#include "Transform.h"
#include "Animator.h"
#include "Camera.h"
#include "GameTime.h"


TargetVisual::TargetVisual(std::weak_ptr<Transform> targetTransform, float animationDuration, float lifeTime) :
Behaviour("TargetVisual"),
TARGET_TRANSFORM{std::move(targetTransform)},
ANIMATION_DURATION{ animationDuration },
TARGET_LIFETIME{lifeTime},
m_TimeAlive{0.0f},
m_IsGoingAway{false}
{
}

void TargetVisual::ResetTime()
{
	m_TimeAlive = 0.0f;
}

void TargetVisual::Update()
{
	if(m_IsGoingAway)
		return;

	if (m_MainCameraWPtr.expired())
		return;


	if(!TARGET_TRANSFORM.expired() && m_TimeAlive < TARGET_LIFETIME)
	{
		const Vector3f flatSpacePosition{ m_MainCameraWPtr.lock()->FlatToWorldPoint(m_MainCameraWPtr.lock()->WorldToFlatPoint(TARGET_TRANSFORM.lock()->GetRawTransform()->GetPosition())) };
		GetRawTransform()->SetPosition(flatSpacePosition);
	}
	else
	{
		m_AnimatorWPtr.lock()->PlayAnimation("UnLock", false, 0.0f,1.0f / ANIMATION_DURATION);
		GameObject::Destroy(GetGameObject(), ANIMATION_DURATION);
		m_IsGoingAway = true;
	}

	m_TimeAlive += Time::GetDeltaTime();
}

void TargetVisual::Awake()
{
	m_MainCameraWPtr = Camera::GetMainCamera();
	m_AnimatorWPtr = GetRawGameObject()->AddComponent<Animator>();
	m_AnimatorWPtr.lock()->PlayAnimation("Lock", false, 0.0f, 1.0f / ANIMATION_DURATION);
}

