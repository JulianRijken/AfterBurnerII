#include "pch.h"
#include "Input.h"
#include <iostream>


bool Input::Compare(const std::string& name, const SDL_KeyboardEvent& e)
{
	// If action exists
	if(INPUT_ACTION.count(name))
	{
		// Loop all keys in action
		for (const SDL_Scancode key: INPUT_ACTION.at(name))
		{
			if (key == e.keysym.scancode)
				return true;
		}
	}
	else
	{
		std::cout << "Input action: " << name << " does not exist!";
	}

	return false;
}

bool Input::Compare(SDL_Scancode key, const SDL_KeyboardEvent& e)
{
	return key == e.keysym.scancode;
}

float Input::GetAxis(const std::string& name)
{
	const Uint8* statesPtr = SDL_GetKeyboardState(nullptr);

	float output{0};

	if (INPUT_AXIS.contains(name))
	{
		const auto& [Negative, Positive]{ INPUT_AXIS.at(name) };

		// Loop all negative buttons
		for (const SDL_Scancode& key : Negative)
		{
			if (statesPtr[key])
			{
				output--;
				break;
			}
		}

		// Loop all positive buttons
		for (const SDL_Scancode& key : Positive)
		{
			if (statesPtr[key])
			{
				output++;
				break;
			}
		}
	}

	return output;
}

Vector2f Input::GetDirection(const std::string& name)
{
	const Uint8* statesPtr = SDL_GetKeyboardState(nullptr);

	Vector2f output{0,0};

	if (INPUT_DIRECTION.contains(name))
	{
		const auto& [Horizontal, Vertical]{ INPUT_DIRECTION.at(name) };

		for (const SDL_Scancode& key : Horizontal.Positive)
		{
			if (statesPtr[key])
			{
				output.x++;
				break;
			}
		}

		for (const SDL_Scancode& key : Horizontal.Negative)
		{
			if (statesPtr[key])
			{
				output.x--;
				break;
			}
		}

		for (const SDL_Scancode& key : Vertical.Positive)
		{
			if (statesPtr[key])
			{
				output.y++;
				break;
			}
		}

		for (const SDL_Scancode& key : Vertical.Negative)
		{
			if (statesPtr[key])
			{
				output.y--;
				break;
			}
		}
	}


	return output.Normalized();
}

bool Input::GetKey(SDL_Scancode key)
{
	const Uint8* pStates = SDL_GetKeyboardState(nullptr);
	return pStates[key];
}

bool Input::GetKey(const std::string& name)
{
	const Uint8* pStates = SDL_GetKeyboardState( nullptr );

	if (INPUT_ACTION.contains(name))
	{
		const std::list<SDL_Scancode>& keys{ INPUT_ACTION.at(name) };
		for (const SDL_Scancode& key : keys)
		{
			if (pStates[key])
				return true;
		}
	}

	return false;
}

bool Input::GetKeyDown(const std::string& name)
{
	if (!m_KeyDownStates.contains(name))
		m_KeyDownStates.insert({ name,false });

	// If key is down
	if(GetKey(name))
	{
		// Check if key is not already down
		if (m_KeyDownStates[name] == false)
		{
			m_KeyDownStates[name] = true;
			return true;
		}
	}
	else
	{
		m_KeyDownStates[name] = false;
	}

	return false;
}


bool Input::GetKeyUp(const std::string& name)
{
	if (!m_KeyUpStates.contains(name))
		m_KeyUpStates.insert({ name,false });

	// If key is up
	if (!GetKey(name))
	{
		// Check if key is not already up
		if (m_KeyUpStates[name] == true)
		{
			m_KeyUpStates[name] = false;
			return true;
		}
	}
	else
	{
		m_KeyUpStates[name] = true;
	}

	return false;
}
