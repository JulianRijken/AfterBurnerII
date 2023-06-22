#include "pch.h"
#include "GameObject.h"

#include "Behaviour.h"
#include "Transform.h"

#include "GlobalSettings.h"
#include "Log.h"
#include "GameTime.h"

GameObject::GameObject(const std::string& name, Tag tag) :
	Object{ name },
	m_Tag{tag},
	m_IsActiveSelf{false},
	m_IsActiveInHierarchy{false}
{
	if (GlobalSettings::DEBUG_OBJECT_SYSTEM)
		Log(TextColor::Black, BackgroundColor::BrightGreen) << "(GAME) Constructor Called For: " << GetName();
}

GameObject::~GameObject()
{

	if (GlobalSettings::DEBUG_OBJECT_SYSTEM)
		Log(TextColor::Black, BackgroundColor::BrightYellow) << "(GAME) Destructor Called For: " << GetName() ;
}


void GameObject::SetActive(bool active)
{
	// Call OnEnable or OnDisable based on if the behaviour is enabled
	for (const std::shared_ptr<Component>& component : m_ComponentSPtrs)
	{
		if (const std::shared_ptr<Behaviour>& castBehaviourPtr{ std::dynamic_pointer_cast<Behaviour>(component) })
		{
			if(castBehaviourPtr->m_Enabled)
			{
				if (m_IsActiveSelf)
					castBehaviourPtr->OnEnable();
				else
					castBehaviourPtr->OnDisable();
			}
		}
	}

	m_IsActiveSelf = active;

	UpdateHierarchy();
}


void GameObject::UpdateHierarchy()
{
	const auto parent{ GetRawTransform()->GetParent() };

	if(parent.expired())
	{
		PropagateHierarchy(m_IsActiveSelf);
	}
	else
	{
		const bool isParentActiveInHierarchy{parent.lock()->GetRawGameObject()->IsActiveInHierarchy() };
		PropagateHierarchy(isParentActiveInHierarchy);
	}
}

void GameObject::PropagateHierarchy(bool isParentActiveInHierarchy)
{
	m_IsActiveInHierarchy = isParentActiveInHierarchy && m_IsActiveSelf;

	const auto children{ GetRawTransform()->GetChildren() };
	for (const auto& child : children)
		child.lock()->GetRawGameObject()->PropagateHierarchy(m_IsActiveInHierarchy);
}


bool GameObject::IsActiveSelf() const
{
	return m_IsActiveSelf;
}

bool GameObject::IsActiveInHierarchy() const
{
	return m_IsActiveInHierarchy;
}

bool GameObject::CompareTag(Tag tag) const
{
	return m_Tag == tag;
}


Tag GameObject::GetTag() const { return m_Tag; }

Transform* GameObject::GetRawTransform() const { return m_TransformPtr.lock().get(); }

std::weak_ptr<Transform> GameObject::GetTransform() const { return m_TransformPtr; }



void GameObject::DestroyAllInQueue()
{
	while (!m_GameObjectToBeDestroyedWPtrs.empty())
	{
		const std::weak_ptr<GameObject>& gameObject{ m_GameObjectToBeDestroyedWPtrs.front() };

		// Last check for multiple of the same in the queue
		if (not gameObject.expired())
			m_GameObjectSPtrs[static_cast<int>(gameObject.lock()->m_Tag)].erase(gameObject.lock());
			
		m_GameObjectToBeDestroyedWPtrs.pop();
	}
}

void GameObject::DestroyAll()
{
	for (int i{ 0 }; i < TAG_COUNT; ++i)
		m_GameObjectSPtrs[i].clear();
}


void GameObject::Destroy(const std::shared_ptr<GameObject>& gameObject)
{
	// The object should remove itself from the parent!!!
	// Currently working like a garbage truck on fire lol
	// Please implement when time sees fit 


	// Destroy all children
	for (const auto& child : gameObject->GetRawTransform()->GetChildren())
	{
		if (!child.expired())
			Destroy(child.lock()->GetGameObject().lock());
	}

	///! Components are automatically destroyed as they go out of scope

	m_GameObjectToBeDestroyedWPtrs.push(gameObject);
}

void GameObject::Destroy(const std::weak_ptr<GameObject>& gameObject)
{
	Destroy(gameObject.lock());
}

void GameObject::Destroy(const std::weak_ptr<GameObject>& gameObject, float delay)
{
	Time::StartTimer(delay, false, gameObject.lock()->m_SelfPtr,[=]()
	{
		Destroy(gameObject.lock());
	});
}




std::weak_ptr<GameObject> GameObject::FindWithTag(Tag tag)
{
	//const auto& set{ m_GameObjectPtrs[static_cast<int>(tag)] };
	const std::unordered_set<std::shared_ptr<GameObject>>& set{ m_GameObjectSPtrs[static_cast<int>(tag)] };

	if (set.empty())
		return {};

	return *set.begin();
}

std::vector<std::weak_ptr<GameObject>> GameObject::FindGameObjectsWithTag(Tag tag)
{
	std::vector<std::weak_ptr<GameObject>> foundObjects{};

	for (std::weak_ptr<GameObject> gameObjectPtr : m_GameObjectSPtrs[static_cast<int>(tag)])
		foundObjects.push_back(gameObjectPtr);

	return foundObjects;
}

std::weak_ptr<GameObject> GameObject::AddGameObject(const std::string& name, Vector3f position, Vector3f rotation,
                                                    Vector3f scale)
{
	return AddGameObject(name, Tag::None,true, position, rotation, scale);
}


std::weak_ptr<GameObject> GameObject::AddGameObject(const std::string& name, Tag tag, bool isActive, Vector3f position,
	Vector3f rotation, Vector3f scale)
{
	return AddGameObject(name, {}, tag, isActive, position, rotation, scale);
}

std::weak_ptr<GameObject> GameObject::AddGameObject(const std::string& name, const std::weak_ptr<Transform>& parent, Tag tag,
	bool isActive, Vector3f position, Vector3f rotation, Vector3f scale)
{
	const std::shared_ptr<GameObject>& sharedPtr{ std::make_shared<GameObject>(name,tag) };
	sharedPtr->m_SelfPtr = sharedPtr;
	m_GameObjectSPtrs[static_cast<int>(tag)].insert(sharedPtr);

	sharedPtr->m_TransformPtr = sharedPtr->AddComponent<Transform>(position, rotation, scale);

	if(!parent.expired())
		sharedPtr->m_TransformPtr.lock()->SetParent(parent);


	sharedPtr->SetActive(isActive);

	// Log(BackgroundColor::BrightGreen, TextColor::Black) << "GameObject Added: " << name << "\n";
	return sharedPtr;

}



void GameObject::PrintHierarchy(bool withComponents)
{
	/// !!!THIS IS A DEBUG FUNCTION IS IS NOT CORRECTY WRITTEN NOR IS IS EFFICIENT!!!
	Log(TextColor::Black, BackgroundColor::BrightWhite) << "- HIERARCHY -";

	for (const std::unordered_set<std::shared_ptr<GameObject>>& gameObjectSPtr : m_GameObjectSPtrs)
	{
		for (const std::shared_ptr<GameObject>& objectSPtr : gameObjectSPtr)
		{
			// Only start printing from objects with no parent so the top ones
			if (objectSPtr->GetRawTransform()->GetParent().expired())
			{
				debugHierarchyPrintCounter = 0;
				PrintHierarchyRecursive(objectSPtr, withComponents);
			}
		}
	}

}

void GameObject::PrintHierarchyRecursive(const std::weak_ptr<GameObject>& gameObjectWPtr, bool withComponents)
{
	/// !!!THIS IS A DEBUG FUNCTION IS IS NOT CORRECTY WRITTEN NOR IS IS EFFICIENT!!!

	BackgroundColor backgroundColor{ BackgroundColor::BrightBlue };
	switch (debugHierarchyPrintCounter)
	{
	case 0:
		backgroundColor = BackgroundColor::Yellow;
		break;
	case 1:
		backgroundColor = BackgroundColor::Red;
		break;
	case 2:
		backgroundColor = BackgroundColor::Green;
		break;
	case 3:
		backgroundColor = BackgroundColor::Cyan;
		break;
	case 4:
		backgroundColor = BackgroundColor::Magenta;
		break;
	}



	Log(TextColor::Black, backgroundColor) << (gameObjectWPtr.lock()->IsActiveInHierarchy() ? "A" : "-") << " - " << debugHierarchyPrintCounter << " - " << gameObjectWPtr.lock()->GetName();
	debugHierarchyPrintCounter++;

	if (withComponents)
	{
		const auto& components{ gameObjectWPtr.lock()->m_ComponentSPtrs };
		for (const std::shared_ptr<Component>& component : components)
		{
			Log(TextColor::BrightBlack, BackgroundColor::Black) << "L " << component->GetName();
		}
	}

	const auto children{ gameObjectWPtr.lock()->GetRawTransform()->GetChildren() };

	if (children.empty())
	{
		debugHierarchyPrintCounter--;
	}


	for (const auto& child : children)
	{
		if (!child.expired())
			PrintHierarchyRecursive(child.lock()->GetGameObject(), withComponents);
	}

}
