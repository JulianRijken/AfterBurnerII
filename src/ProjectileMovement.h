#pragma once
#include "Behaviour.h"
#include "Vector3f.h"


class ProjectileMovement final : public Behaviour
{

public:

	ProjectileMovement(const Vector3f& velocity, const std::string& name = "ProjectileMovement");

private:
	void Update() override;

	Vector3f m_Velocity;
};

