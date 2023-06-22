#pragma once
#include "Behaviour.h"
#include "Vector3f.h"

class AutoMove final : public Behaviour
{
public:
	explicit AutoMove(Vector3f velocity);

private:

	const Vector3f VELOCITY;

	void Update() override;
};

