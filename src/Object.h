#pragma once

class Object
{
public:

	Object(Object&&) = delete;
	Object& operator=(const Object&) = delete;
	Object& operator=(Object&&) = delete;

	[[nodiscard]] const std::string& GetName() const { return m_Name; }

protected:

	// Required for smart pointers :/
	Object(const Object&) = default;

	Object(std::string name = "BaseObject");
	~Object() = default;

private:
	std::string m_Name;
};
