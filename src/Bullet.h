#pragma once
#include "Behaviour.h"
#include "GameObject.h"
#include "Vector3f.h"


enum class Tag;

class Bullet final : public Behaviour
{

public:
	explicit Bullet(const Vector3f& velocity);

private:

	void Update() override;

	const float SPAWN_CHANCE{0.8f};
	const float HIT_DAMAGE{40.0f};

	const Tag ENEMY_TAG[3]{Tag::TargetEnemy,Tag::StaticEnemy, Tag::BlockEnemy};
	const Vector3f VELOCITY;
};

