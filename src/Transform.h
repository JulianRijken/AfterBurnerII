#pragma once
#include <vector>

#include "Component.h"
#include "Vector2f.h"
#include "Vector3f.h"


class Transform final : public Component
{

public:

	Transform(const Vector3f& position = Vector3f{ 0,0,0 }, const Vector3f& rotation = Vector3f{ 0,0,0 }, const Vector3f& scale = Vector3f{ 1,1,1 }, const std::string& name = "TransformComponent");


	[[nodiscard]] const Vector3f& GetPosition() const{return m_Position;}
	[[nodiscard]] const Vector3f& GetRotation() const{return m_Rotation;}
	[[nodiscard]] const Vector3f& GetScale() const { return m_Scale; }

	[[nodiscard]] Vector3f Right2D() const;

	[[nodiscard]] Vector3f Up2D() const;


	void SetPosition(const Vector3f& position) { m_Position = position; }
	void SetPosition(const Vector2f& position) { m_Position.x = position.x; m_Position.y = position.y; }
	void SetPosition(const float x, const float y) { m_Position.x = x; m_Position.y = y; }
	void SetPosition(const float x, const float y, const float z) { SetPosition(Vector3f{ x,y,z }); }

	void SetRotation(const Vector3f& rotation) { m_Rotation = rotation; }
	void SetRotation(const float pitch, const float yaw, const float roll) { SetRotation(Vector3f{ pitch,yaw,roll }); }

	void SetScale(const Vector3f& scale) { m_Scale = scale; }
	void SetScale(const float scale) { m_Scale = Vector3f{scale,scale,scale}; }
	void SetScale(const float x, const float y) { m_Scale.x = x; m_Scale.y = y;}
	void SetScale(const float x, const float y, const float z) { SetScale(Vector3f{ x,y,z }); }


	void Translate(const Vector3f& translation) { m_Position += translation; }
	void Translate(const float x, const float y, const float z = 0) { Translate(Vector3f{ x,y,z }); }

	void Rotate(const Vector3f& rotation) { m_Rotation += rotation; }
	void Rotate(const float pitch, const float yaw, const float roll = 0) { Rotate(Vector3f{pitch, yaw, roll }); }


	void SetParent(const std::weak_ptr<Transform>& parentTransformWPtr);
	void AddChild(const std::weak_ptr<Transform>& childTransformWPtr);

	[[nodiscard]] std::vector<std::weak_ptr<Transform>> GetChildren() { return m_ChildWPtrs; }
	[[nodiscard]] std::weak_ptr<Transform> GetParent() { return m_ParentWPtr; }

private:

	Vector3f m_Position;
	Vector3f m_Rotation;
	Vector3f m_Scale;
	std::weak_ptr<Transform> m_ParentWPtr{};
	std::vector<std::weak_ptr<Transform>> m_ChildWPtrs{};
};

