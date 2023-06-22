#include "pch.h"
#include "IDamageable.h"

IDamageable::IDamageable(float maxHealth, float startHealth,bool invincible) :
	m_MaxHealth{ maxHealth },
	m_Health{ startHealth <= 0 ? m_MaxHealth : startHealth },
	m_Invincible{invincible}
{
}

void IDamageable::ChangeHealth(float healthDelta)
{
	m_Health += healthDelta;

	if(m_Health <= 0.0f)
	{
		m_Health = 0;
		OnDeath();
	}
}

void IDamageable::SetHealth(float health)
{
	const float healthChange = health - m_Health;
	OnHealthChange(healthChange);
}

void IDamageable::SetInvincible(bool invincible)
{
	m_Invincible = invincible;
}
