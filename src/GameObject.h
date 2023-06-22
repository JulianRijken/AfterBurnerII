#pragma once
#include "Object.h"

#include <memory>
#include <queue>
#include <unordered_set>

#include "Behaviour.h"
#include "Component.h"
#include "Vector3f.h"


class Component;

enum class Tag
{
	None,
	MainCamera,
	Player,
	PlayerCamera,
	TargetEnemy,
	StaticEnemy,
	BlockEnemy,
	TAG_COUNT
};

class GameObject final : public Object
{
	friend Component;

public:

	GameObject(const std::string& name = "New GameObject", Tag tag = Tag::None);
	~GameObject();

	GameObject(GameObject&&) = delete;
	GameObject(const GameObject&) = delete;
	GameObject& operator=(GameObject&&) = delete;
	GameObject& operator=(const GameObject&) = delete;

	void SetActive(bool active);
	void UpdateHierarchy();
	void PropagateHierarchy(bool isParentActiveInHierarchy);

	
	[[nodiscard]] bool IsActiveSelf() const;
	[[nodiscard]] bool IsActiveInHierarchy() const;
	[[nodiscard]] bool CompareTag(Tag tag) const;
	[[nodiscard]] Tag GetTag() const;
	[[nodiscard]] Transform* GetRawTransform() const;
	[[nodiscard]] std::weak_ptr<Transform> GetTransform() const;


	template<class ComponentType, class... Args>
	std::enable_if_t<std::is_base_of_v<Component, ComponentType>, std::weak_ptr<ComponentType>>
	AddComponent(Args&&... args)
	{
		std::shared_ptr<ComponentType> genericComponentPtr{ std::make_shared<ComponentType>(std::forward<Args>(args)...) };
		m_ComponentSPtrs.push_back(genericComponentPtr);

		Component* castComponentPtr{static_cast<Component*>(genericComponentPtr.get()) };
		castComponentPtr->m_AttachedGameObjectPtr = m_SelfPtr;
		castComponentPtr->m_SelfPtr = genericComponentPtr;

		if (Behaviour * castBehaviourPtr{ dynamic_cast<Behaviour*>(genericComponentPtr.get()) })
			castBehaviourPtr->Awake();


		return genericComponentPtr;
	}


	template <class ComponentType>
	std::weak_ptr<ComponentType> GetComponent()
	{
		for (const std::shared_ptr<Component>& componentPtr : m_ComponentSPtrs)
		{
			if (std::shared_ptr<ComponentType> basePtr = std::dynamic_pointer_cast<ComponentType>(componentPtr))
				return basePtr;
		}

		return {};
	}


	// Debug Functions
	static void PrintHierarchy(bool withComponents);
	static void PrintHierarchyRecursive(const std::weak_ptr<GameObject>& gameObjectWPtr, bool withComponents);

private:

	inline static const int TAG_COUNT{ static_cast<int>(Tag::TAG_COUNT) };

	Tag m_Tag;
	bool m_IsActiveSelf{};
	bool m_IsActiveInHierarchy{};
	bool m_IsDestroyed{};
	std::weak_ptr<Transform> m_TransformPtr;
	std::weak_ptr<GameObject> m_SelfPtr;

	std::vector<std::shared_ptr<Component>> m_ComponentSPtrs{};

	inline static std::queue<std::weak_ptr<GameObject>> m_GameObjectToBeDestroyedWPtrs{};
	inline static std::unordered_set<std::shared_ptr<GameObject>> m_GameObjectSPtrs[TAG_COUNT]{};
	inline static int debugHierarchyPrintCounter{};

public:

	static void DestroyAllInQueue();

	static void DestroyAll();
	static void Destroy(const std::shared_ptr<GameObject>& gameObject);
	static void Destroy(const std::weak_ptr<GameObject>& gameObject);
	static void Destroy(const std::weak_ptr<GameObject>& gameObject, float delay);

	static std::weak_ptr<GameObject> FindWithTag(Tag tag);
	static std::vector<std::weak_ptr<GameObject>> FindGameObjectsWithTag(Tag tag);

	static std::weak_ptr<GameObject> AddGameObject(const std::string& name, Vector3f position, Vector3f rotation = Vector3f{ 0,0,0 }, Vector3f scale = Vector3f{ 1,1,1 });
	static std::weak_ptr<GameObject> AddGameObject(const std::string& name, Tag tag = Tag::None, bool isActive = true, Vector3f position = Vector3f{0,0,0}, Vector3f rotation = Vector3f{0,0,0}, Vector3f scale = Vector3f{ 1,1,1 });
	static std::weak_ptr<GameObject> AddGameObject(const std::string& name, const std::weak_ptr<Transform>& parent, Tag tag = Tag::None, bool isActive = true, Vector3f position = Vector3f{ 0,0,0 }, Vector3f rotation = Vector3f{ 0,0,0 }, Vector3f scale = Vector3f{ 1,1,1 });
};


