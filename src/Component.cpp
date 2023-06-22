#include "pch.h"
#include "Component.h"

#include "GameObject.h"
#include "GlobalSettings.h"
#include "Log.h"


Component::Component(const std::string& name) :
	Object{ name }
{
	if(GlobalSettings::DEBUG_OBJECT_SYSTEM)
		Log(TextColor::Black, BackgroundColor::Green) << "(COMP) Constructor Called For: " << GetName();
}

Component::~Component()
{
	if (GlobalSettings::DEBUG_OBJECT_SYSTEM)
		Log(TextColor::Black, BackgroundColor::Yellow) << "(COMP) Destructor Called For: " << GetName();
}

std::weak_ptr<Transform> Component::GetTransform() const
{
	return m_AttachedGameObjectPtr.lock()->m_TransformPtr;
}

Transform* Component::GetRawTransform() const
{
	return m_AttachedGameObjectPtr.lock()->m_TransformPtr.lock().get();
}

std::weak_ptr<GameObject> Component::GetGameObject() const
{
	return m_AttachedGameObjectPtr;
}

GameObject* Component::GetRawGameObject() const
{
	return m_AttachedGameObjectPtr.lock().get();
}

