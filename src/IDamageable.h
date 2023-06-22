#pragma once
class IDamageable
{
public:

	IDamageable(float maxHealth = 100, float startHealth = 0, bool invincible = false);
	virtual ~IDamageable() = default;

	void ChangeHealth(float healthDelta);
	void SetHealth(float health);
	void SetInvincible(bool invincible);

	[[nodiscard]] float IsInvincible() const { return m_Invincible; }

	virtual void OnHealthChange(float delta){}
	virtual void OnDeath(){}

protected:

	float m_MaxHealth;
	float m_Health;
	bool m_Invincible;
};

