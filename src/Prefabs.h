#pragma once
#include <memory>

#include "Vector3f.h"


class TargetVisual;
class Transform;

class Prefabs
{
public:

	static void SpawnBigExplosion(Vector3f position = Vector3f{ 0,0,0 }, float drag = 0.7f);
	static void SpawnSmallExplosion(Vector3f position = Vector3f{ 0,0,0 }, float drag = 0.5f);

	static void SpawnCloud(float drag, Vector3f position = Vector3f{ 0,0,0 }, Vector3f scale = Vector3f{ 1,1,1 });
	static void SpawnCoin(Vector3f position);

	static std::weak_ptr<TargetVisual> SpawnTargetVisual(const std::weak_ptr<Transform>& targetTransform);
};

