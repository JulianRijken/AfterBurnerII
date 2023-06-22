#pragma once
#include "Behaviour.h"

#include "Vector3f.h"


class Animator;
class SpriteRenderer;

class Rocket final : public Behaviour
{
public:

	explicit Rocket(
		const Vector3f& startDirection,
		const Vector3f& defaultTarget,
		std::weak_ptr<Transform> target = {},
		float dropTime = 0.35f,
		float dropSpeed = 5.0f,
		float flySpeed = 150.0f,
		float rotateSpeed = 1.7f,
		float maxLifeTime = 10.0f,
		float hitDamage = 100.0f,
		float rotateSpeedDistanceAddition = 0.0f,
		float rotationCancelDistance = 0.0f
		);


private:
	void Update() override;
	void Awake() override;

	void OnHitTarget() const;
	void HandleRotation();

	const float DROP_TIME;
	const float DROP_SPEED;
	const float FLY_SPEED;
	const float ROTATE_SPEED;
	const float MAX_LIFE_TIME;
	const float HIT_DAMAGE;
	const float ROTATE_SPEED_DISTANCE_ADDITION;
	const float CANCEL_ROTATION_DISTANCE;

	const float CLOUD_SPAWN_INTERVAL{ 0.025f };

	bool m_Dropping;
	bool m_TargetHit;

	Vector3f m_FlyDirection{};
	Vector3f m_StaticTargetPosition{};
	Vector3f m_StartPosition{};

	std::weak_ptr<Transform> m_DynamicTargetTransform{};

	std::weak_ptr<Collider> m_TargetCollider{};

	std::weak_ptr<SpriteRenderer> m_RocketSpriteRenderer{};
	std::weak_ptr<Animator> m_RocketAnimator{};
};

