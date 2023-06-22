#pragma once
#include "Behaviour.h"
#include "Gizmos.h"
#include "IDamageable.h"


class PathEnemy final : public Behaviour, public IDamageable, Gizmos
{

public:

	PathEnemy
	(
		Vector2f offset,
		float startDelay, float moveDuration,Vector3f curveCenter, Vector2f curveScale, Jul::CubicCurve curve,
		float rocketDelay = 0.0f,
		float animationDelay = 0.0f, float animationDuration = 0.0f, std::string animationName = ""
	);

private:

	void Awake() override;
	void OnDrawGizmos() override;

	void OnDeath() override;

	void FireRocket() const;

	const float ANIMATION_DELAY;
	const float ANIMATION_DURATION;
	const std::string ANIMATION_NAME;

	const float START_DELAY;
	const float MOVE_DURATION;
	const Vector3f CURVE_CENTER;
	const Vector2f CURVE_SCALE;
	const Jul::CubicCurve CURVE;

	const Vector2f OFFSET;

	const float ROCKET_DELAY;
};

