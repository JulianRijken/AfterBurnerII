#pragma once
#include <list>
#include <map>

#include "Vector2f.h"

class Input
{
public:

	static bool Compare(const std::string& name, const SDL_KeyboardEvent& e);
	static bool Compare(SDL_Scancode key, const SDL_KeyboardEvent& e);

	static float GetAxis(const std::string& name);
	static Vector2f GetDirection(const std::string& name);


	static bool GetKey(SDL_Scancode key);
	static bool GetKey(const std::string& name);
	static bool GetKeyDown(const std::string& name);
	static bool GetKeyUp(const std::string& name);


private:

	struct Axis
	{
		std::list<SDL_Scancode> Negative{};
		std::list<SDL_Scancode> Positive{};
	};

	struct Direction
	{
		Axis Horizontal{};
		Axis Vertical{};
	};


	inline static std::map<std::string, bool> m_KeyDownStates{};
	inline static std::map<std::string, bool> m_KeyUpStates{};


	inline static const std::map<std::string, std::list<SDL_Scancode>> INPUT_ACTION
	{
		{"fireRocket",			{SDL_SCANCODE_SPACE,SDL_SCANCODE_X}},
		{"uiSelect",			{SDL_SCANCODE_SPACE,SDL_SCANCODE_RETURN}},
		{"uiUp",				{SDL_SCANCODE_UP,SDL_SCANCODE_W}},
		{"uiDown",				{SDL_SCANCODE_DOWN,SDL_SCANCODE_S}},
		{"uiLeft",				{SDL_SCANCODE_LEFT,SDL_SCANCODE_A}},
		{"uiRight",				{SDL_SCANCODE_RIGHT,SDL_SCANCODE_D}},
		{"pause",				{SDL_SCANCODE_ESCAPE, SDL_SCANCODE_RETURN}},
		{"info",				{SDL_SCANCODE_I}},
		{"fps",					{SDL_SCANCODE_TAB}},

		{"unitGrid",			{SDL_SCANCODE_U}},
		{"forceRestart",		{SDL_SCANCODE_R}},
		{"pixelateCamera",		{SDL_SCANCODE_P}},
		{"toggleGizmos",		{SDL_SCANCODE_G}},

		{"toggleCurveEditor",	{SDL_SCANCODE_C}},
		{"toggleCurveSides",	{SDL_SCANCODE_V}},
		{"flipCurve",			{SDL_SCANCODE_5}},
		{"moveWholeCurve",		{SDL_SCANCODE_LSHIFT}},
		{"moveCurveCenter",		{SDL_SCANCODE_LALT}},

		{"toggleInvincible",	{SDL_SCANCODE_O}},

		{"increaseTimeScale",	{SDL_SCANCODE_RIGHTBRACKET}},
		{"decreaseTimeScale",	{SDL_SCANCODE_LEFTBRACKET}},

		{"curveEditorP0",		{SDL_SCANCODE_1}},
		{"curveEditorP1",		{SDL_SCANCODE_2}},
		{"curveEditorP2",		{SDL_SCANCODE_3}},
		{"curveEditorP3",		{SDL_SCANCODE_4}},

		{"debugHierarchy",				{SDL_SCANCODE_H}},
		{"debugHierarchyWithComponents",{SDL_SCANCODE_J}},
	};

	inline static const std::map<std::string, Axis> INPUT_AXIS
	{
		{"horizontal",Axis						{{SDL_SCANCODE_A, SDL_SCANCODE_LEFT},{SDL_SCANCODE_D, SDL_SCANCODE_RIGHT}}},
		{"vertical",Axis						{{SDL_SCANCODE_S, SDL_SCANCODE_DOWN},{SDL_SCANCODE_W, SDL_SCANCODE_UP}}},
		{"curveEditorVerticalMovement",Axis		{{SDL_SCANCODE_Q, SDL_SCANCODE_DOWN},{SDL_SCANCODE_E, SDL_SCANCODE_UP}}},
	};

	inline static const std::map<std::string, Direction> INPUT_DIRECTION
	{
		{
			"playerMovement", Direction
			{
				Axis{{SDL_SCANCODE_A, SDL_SCANCODE_LEFT},{SDL_SCANCODE_D, SDL_SCANCODE_RIGHT}},
				Axis{{SDL_SCANCODE_S, SDL_SCANCODE_DOWN},{SDL_SCANCODE_W, SDL_SCANCODE_UP}}
			}
		},
		{
			"curveEditorPlaneMovement", Direction
			{
				Axis{{SDL_SCANCODE_A, SDL_SCANCODE_LEFT},{SDL_SCANCODE_D, SDL_SCANCODE_RIGHT}},
				Axis{{SDL_SCANCODE_S, SDL_SCANCODE_DOWN},{SDL_SCANCODE_W, SDL_SCANCODE_UP}}
			}
		},
	};
};

