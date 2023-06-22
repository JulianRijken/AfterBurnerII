#pragma once
#include "Behaviour.h"
#include "IDamageable.h"
#include "Vector2f.h"
#include "Vector3f.h"

struct Vector2f;

class StaticEnemy final : public Behaviour, public IDamageable
{
public:
	explicit StaticEnemy(bool canBeDestroyed,Vector2f offset = Vector2f{0,0});


private:

	void OnDeath() override;
	void Awake() override;
	void Update() override;


	const float MOVE_SPEED{ 100 };
	const Vector2f OFFSET;
	const float SCALE_START_DISTANCE{ 30.0f };
	const Vector3f MIN_SCALE{1.0f,1.0f ,1.0f };
	Vector3f m_StartScale{Vector3f::One()};

	bool m_CanBeDestroyed;

};

