#pragma once
#include <unordered_set>

#include "Component.h"
#include "GameObject.h"
#include "Gizmos.h"
#include "Transform.h"
#include "Vector3f.h"


enum class Tag;

class Collider final : public Component, Gizmos
{
public:

	Collider(const Vector3f size = Vector3f{ 1,1,1 }, const Vector3f center = Vector3f{0,0,0}, const std::string& name = "ColliderComponent");
	~Collider() override;

	struct CollisionInfo
	{
		bool isHit{false};
		Collider* hitCollider{nullptr};
	};


	[[nodiscard]] const Vector3f& GetPositionCenter() const { return CENTER + GetRawTransform()->GetPosition(); }
	[[nodiscard]] const Vector3f& GetSize() const { return SIZE; }
	[[nodiscard]] const Vector3f& GetHalfSize() const { return SIZE; }
	[[nodiscard]] const Vector3f& GetCenter() const { return CENTER; }

	[[nodiscard]] static const std::unordered_set<Collider*>& GetColliderPtrs() { return m_ColliderPtrs;  }

	[[nodiscard]] static bool HasCollisionPredictivePlanerGlobal(const Vector3f& currentPosition, const Vector3f& nextPosition, CollisionInfo& hitInfo, Tag ignoreTag = Tag::None);
	[[nodiscard]] bool HasCollisionPredictivePlaner(const Vector3f& currentPosition, const Vector3f& nextPosition) const;


private:

	void OnDrawGizmos() override;

	const Vector3f SIZE;
	const Vector3f HALF_SIZE;
	const Vector3f CENTER;

	inline static std::unordered_set<Collider*> m_ColliderPtrs{};
};

