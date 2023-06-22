#include "pch.h"
#include "Transform.h"

#include "GameObject.h"
#include "Jul.h"


Transform::Transform(const Vector3f& position, const Vector3f& rotation, const Vector3f& scale, const std::string& name) :
	Component{ "Transform" },
	m_Position{ position },
	m_Rotation{ rotation },
	m_Scale{ scale },
	m_ParentWPtr{},
	m_ChildWPtrs{}
{

}

Vector3f Transform::Right2D() const
{
	const float rightAngleRad{ Jul::DegToRad(m_Rotation.z) };
	return Vector3f{ cosf(rightAngleRad), sinf(rightAngleRad), 0 };
}

Vector3f Transform::Up2D() const
{
	const float upAngleRad{ Jul::DegToRad(m_Rotation.z + 90.0f) };
	return Vector3f{ cosf(upAngleRad), sinf(upAngleRad), 0 };
}

void Transform::SetParent(const std::weak_ptr<Transform>& parentTransformWPtr)
{
	if(parentTransformWPtr.expired())
		return;

	///! For safty add so that you can't set the parent to itself or a child of itslef
	m_ParentWPtr = parentTransformWPtr;
	parentTransformWPtr.lock()->AddChild(GetTransform());
	GetRawGameObject()->UpdateHierarchy();
}

void Transform::AddChild(const std::weak_ptr<Transform>& childTransformWPtr)
{
	m_ChildWPtrs.emplace_back(childTransformWPtr);
	childTransformWPtr.lock()->GetRawGameObject()->UpdateHierarchy();
}
