#include "pch.h"
#include "Collider.h"

#include "GameObject.h"
#include "Transform.h"


Collider::Collider(Vector3f size, Vector3f center , const std::string& name) :
	Component{ name },
	SIZE{size},
	HALF_SIZE{SIZE / 2.0f},
	CENTER{center}
{
	m_ColliderPtrs.insert(this);
}

Collider::~Collider()
{
	m_ColliderPtrs.erase(this);
}


bool Collider::HasCollisionPredictivePlanerGlobal(const Vector3f& currentPosition, const Vector3f& nextPosition, CollisionInfo& hitInfo, Tag ignoreTag)
{
	// Rest hit info
	hitInfo = {};

	// Swap in case current is lower so it works both ways
	float lowerDistance{ currentPosition.z };
	float higherDistance{ nextPosition.z };

	if (currentPosition.z > nextPosition.z)
		std::swap(lowerDistance, higherDistance);

	// Collision size is compared against the center of the collision for accuracy
	const Vector3f inBetweenPosition{ Jul::Lerp(currentPosition,nextPosition,0.5f) };

	for (Collider* colliderPtr : m_ColliderPtrs)
	{
		if (ignoreTag != Tag::None)
		{
			if (colliderPtr->GetRawGameObject()->CompareTag(ignoreTag))
				continue;
		}

		const Vector3f& colliderPosition{ colliderPtr->GetRawTransform()->GetPosition() + colliderPtr->CENTER};

		// If bullet is in between
		if (lowerDistance <= colliderPosition.z && higherDistance >= colliderPosition.z)
		{
			const Vector3f& colliderSize{ colliderPtr->SIZE };

			const Rectf collisionRect
			{
				colliderPosition.x - colliderSize.x / 2.0f,
				colliderPosition.y - colliderSize.y / 2.0f,
				colliderSize.x,
				colliderSize.y,
			};

			if (Jul::HasCollisionRectOnVector2D(collisionRect, Vector2f{ inBetweenPosition.x,inBetweenPosition.y }))
			{
				hitInfo.hitCollider = colliderPtr;
				hitInfo.isHit = true;
				return true;
			}
		}
	}

	return false;
}

bool Collider::HasCollisionPredictivePlaner(const Vector3f& currentPosition, const Vector3f& nextPosition) const
{
	// Swap in case current is lower so it works both ways
	float lowerDistance{ currentPosition.z };
	float higherDistance{ nextPosition.z };
	if (currentPosition.z > nextPosition.z)
		std::swap(lowerDistance, higherDistance);

	// Collision size is compared against the center of the collision for accuracy
	const Vector3f inBetweenPosition{ Jul::Lerp(currentPosition,nextPosition,0.5f) };


	const Vector3f& colliderPosition{ GetRawTransform()->GetPosition() + CENTER };

	// If bullet is in between
	if (lowerDistance <= colliderPosition.z && higherDistance >= colliderPosition.z)
	{
		const Vector3f& colliderSize{ SIZE };

		const Rectf collisionRect
		{
			colliderPosition.x - colliderSize.x / 2.0f,
			colliderPosition.y - colliderSize.y / 2.0f,
			colliderSize.x,
			colliderSize.y,
		};

		if (Jul::HasCollisionRectOnVector2D(collisionRect, Vector2f{ inBetweenPosition.x,inBetweenPosition.y }))
			return true;
	}

	return false;

}

void Collider::OnDrawGizmos()
{
	GizmosSetColor(0.58f, 0.99f, 0.529f, 0.5f);
	GizmosDrawCube(SIZE, CENTER + GetRawTransform()->GetPosition());

	GizmosSetColor(1.0f, 1.0f, 1.0f, 1.0f);
	GizmosDrawWireCube(SIZE, CENTER + GetRawTransform()->GetPosition());
}