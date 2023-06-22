#pragma once
#include <memory>

#include "Log.h"
#include "Object.h"

class GameObject;
class Transform;


class Component : public Object
{
	friend GameObject;

public:

	Component(const std::string& name = "Base Component");

	virtual ~Component();

	Component(Component&&) = delete;
	Component(const Component&) = delete;
	Component& operator=(Component&&) = delete;
	Component& operator=(const Component&) = delete;


	[[nodiscard]] std::weak_ptr<Transform> GetTransform() const;
	[[nodiscard]] Transform* GetRawTransform() const;
	[[nodiscard]] std::weak_ptr<GameObject> GetGameObject() const;
	[[nodiscard]] GameObject* GetRawGameObject() const;


	[[nodiscard]] std::weak_ptr<Component> GetSelfPtr() const
	{
		return m_SelfPtr;
	}

	template<typename  ComponentType>
	[[nodiscard]] std::weak_ptr<ComponentType> GetSelfPtr() const
	{
		if (m_SelfPtr.expired())
			Log(LogType::Warning) << "GetSelfPtr() Called Before Awake!";

		return std::dynamic_pointer_cast<ComponentType>(m_SelfPtr.lock());
	}


private:

	std::weak_ptr<Component> m_SelfPtr;
	std::weak_ptr<GameObject> m_AttachedGameObjectPtr;
};

