#pragma once
#include "Behaviour.h"
#include "Vector3f.h"

class Coin final : public Behaviour
{
private:
	void Awake() override;

	Vector3f m_StartPosition{};
};

