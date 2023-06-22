#include "pch.h"
#include "AutoMove.h"

#include "GameTime.h"
#include "Transform.h"

AutoMove::AutoMove(Vector3f velocity) :
Behaviour{"AutoMove"},
VELOCITY{velocity}
{}

void AutoMove::Update()
{
	GetRawTransform()->Translate(VELOCITY * Time::GetDeltaTime());
}
