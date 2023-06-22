#include "pch.h"
#include "Behaviour.h"

#include "GameObject.h"


Behaviour::Behaviour(const std::string& name) :
Component{name},
m_Enabled{true}
{
	m_BehaviourPtrs.insert(this);
}

Behaviour::~Behaviour()
{
	m_BehaviourPtrs.erase(this);
};


bool Behaviour::IsActiveAndEnabled() const
{
	return (m_Enabled && GetRawGameObject()->IsActiveInHierarchy());
}

void Behaviour::SetEnabled(bool enabled)
{
	if(enabled == m_Enabled)
		return;

	if(GetRawGameObject()->IsActiveInHierarchy())
	{
		if (enabled)
			OnEnable();
		else
			OnDisable();
	}

	m_Enabled = enabled;
}

void Behaviour::UpdateAll()
{
	for (auto iterator = m_BehaviourPtrs.begin(); iterator != m_BehaviourPtrs.end(); ++iterator)
	{
		if((*iterator)->IsActiveAndEnabled())
			(*iterator)->Update();
	}
}

void Behaviour::LateUpdateAll()
{
	for (auto iterator = m_BehaviourPtrs.begin(); iterator != m_BehaviourPtrs.end(); ++iterator)
	{
		if ((*iterator)->IsActiveAndEnabled())
			(*iterator)->LateUpdate();
	}
}