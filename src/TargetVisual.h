#pragma once
#include "Behaviour.h"

class Camera;
class Animator;

class TargetVisual final : public Behaviour
{

public:
	explicit TargetVisual(std::weak_ptr<Transform> targetTransform, float animationDuration = 0.25f, float lifeTime = 2.0f);

	void ResetTime();

	[[nodiscard]] std::weak_ptr<Transform> GetTarget() const { return TARGET_TRANSFORM; }

private:

	void Update() override;
	void Awake() override;

	const std::weak_ptr<Transform> TARGET_TRANSFORM{};
	const float ANIMATION_DURATION;
	const float TARGET_LIFETIME;

	std::weak_ptr<Camera> m_MainCameraWPtr{};
	std::weak_ptr<Animator> m_AnimatorWPtr{};

	float m_TimeAlive;
	bool m_IsGoingAway;
};

