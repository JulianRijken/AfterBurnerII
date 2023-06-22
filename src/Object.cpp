#include "pch.h"
#include "Object.h"

Object::Object(std::string name) :
m_Name{std::move(name)}{}
