#include "pch.h"
#include "ProjectileMovement.h"

#include "GameTime.h"
#include "Transform.h"


ProjectileMovement::ProjectileMovement(const Vector3f& velocity, const std::string& name) :
Behaviour{name},
m_Velocity{velocity}
{
}

void ProjectileMovement::Update()
{
	const Vector3f deltaVelocity{ m_Velocity * Time::GetDeltaTime() };
	GetRawTransform()->Translate(deltaVelocity);
}
