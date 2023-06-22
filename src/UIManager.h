#pragma once
#include <array>

#include "Behaviour.h"
#include "IEvents.h"

class Animator;
class TextRenderer;
class SpriteRenderer;

class UiManager final : public Behaviour, IEvents
{
private:

	enum class Screens
	{
		Intro,
		MainMenu,
		Options,
		GameWon,
	};

	struct Button
	{
		Vector2f position{};
		std::string name{};
	};


	void Awake() override;
	void Update() override;

	void OnKeyDown(const SDL_KeyboardEvent& e) override;
	void OnReturnToMainMenu(bool showWinScreen) override;

	void ShowScreen(Screens screen);

	const float UI_SELECT_SOUND_VOLUME{ 5.0f };

	const float PUSH_START_TEXT_HEIGHT{ 45 };
	const float OPTION_MENU_TEXT_HEIGHT{ 180 };

	const float LOGO_LOAD_TIME{ 1.5f };
	const float LOGO_LOAD_TIME_BLACK{ 0.0f };

	const float START_TEXT_FLIKKER_SPEED{ 0.55f };
	const int MAX_VOLUME_NUMBER{ 14 };

	const Color4f OPTIONS_SCREEN_BACKGROUND_COLOR{ 0.0f, 0.48f, 0.54f, 1.0f };
	const Color4f BUTTON_TEXT_COLOR{ 0.89f, 0.87f, 0.49f, 1.0f };
	const Color4f PRESS_START_BUTTON_COLOR{ 1.0f, 0.09f, 0.0f, 1.0f };


	const std::array<Button, 2> MAIN_SCREEN_BUTTONS
	{
		Button
		{
			Vector2f{160,75},
			"- START -",
		},
		Button
		{
			Vector2f{160,60},
			"- OPTION -",
		}

	};

	const std::array<Button,5> OPTION_SCREEN_BUTTONS
	{
		Button
		{
			Vector2f{98,135},
			"MUSIC",
		},
		Button
		{
			Vector2f{98,120},
			"SOUND",
		},
		Button
		{
			Vector2f{98,105},
			"LEVEL",
		},
		Button
		{
			Vector2f{98,90},
			"CONTROL",
		},
		Button
		{
			Vector2f{98,75},
			"EXIT",
		}

	};


	Screens m_ActiveScreen{};
	int m_CursorSelection{};
	int m_SelectedMusicIndex{};
	int m_SelectedVolume{};


	std::weak_ptr<GameObject> m_IntroScreenGoWPtr;
	std::weak_ptr<GameObject> m_MainScreenGoWPtr;
	std::weak_ptr<GameObject> m_OptionsScreenGoWPtr;
	std::weak_ptr<GameObject> m_GameWonScreenGoWPtr;


	std::weak_ptr<TextRenderer> m_PushStartButtonTextRendererWPtr;
	std::weak_ptr<TextRenderer> m_SelectedMusicTextRendererWPtr;
	std::weak_ptr<TextRenderer> m_SelectedVolumeTextRendererWPtr;
	std::weak_ptr<TextRenderer> m_SelectedLevelTextRendererWPtr;
	std::weak_ptr<GameObject> m_CursorGoWPtr;
	std::weak_ptr<SpriteRenderer> m_UiBackgroundSpriteWPtr{};


	std::weak_ptr<Animator> m_WinScreenAnimatorWPtr{};
};

