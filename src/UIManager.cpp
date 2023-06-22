#include "pch.h"
#include "UIManager.h"

#include "Animator.h"
#include "AudioManager.h"
#include "DebugManager.h"
#include "GameObject.h"
#include "Input.h"
#include "Jul.h"
#include "LevelManager.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"
#include "GameTime.h"
#include "Transform.h"

void UiManager::Awake()
{
	// Intro Screen
	m_IntroScreenGoWPtr = GameObject::AddGameObject("IntroScreen", GetTransform(), Tag::None, true);
	{
		const auto segaLogoWPtr = GameObject::AddGameObject("SegaLogo", m_IntroScreenGoWPtr.lock()->GetTransform());
		segaLogoWPtr.lock()->AddComponent<SpriteRenderer>("SegaLogo", 1000, Renderer::RenderMode::Screen);
		segaLogoWPtr.lock()->GetRawTransform()->SetPosition(GlobalSettings::RENDER_WIDTH / 2.0f, GlobalSettings::RENDER_HEIGHT / 2.0f);
		segaLogoWPtr.lock()->AddComponent<Animator>().lock()->PlayAnimation("Shine", false);

		AudioManager::PlaySoundEffect("Bling");
	}


	// Main Screen
	m_MainScreenGoWPtr = GameObject::AddGameObject("MainScreen", GetTransform(),Tag::None,false);
	{
		GameObject::AddGameObject("MainMenuLogo", m_MainScreenGoWPtr.lock()->GetTransform()).lock()->
			AddComponent<SpriteRenderer>("MainMenuLogo", 1000, Renderer::RenderMode::Screen);


		m_PushStartButtonTextRendererWPtr = GameObject::AddGameObject("NextStageText", m_MainScreenGoWPtr.lock()->GetTransform()).lock()->
			AddComponent<TextRenderer>("AfterBurner", "PUSH START BUTTON", Vector2f{ 0.5f,0.5f }, 1000, Renderer::RenderMode::Screen);
		m_PushStartButtonTextRendererWPtr.lock()->GetRawTransform()->SetPosition(GlobalSettings::RENDER_WIDTH / 2.0f, PUSH_START_TEXT_HEIGHT);
		m_PushStartButtonTextRendererWPtr.lock()->GetRawTransform()->SetScale(0.5f);
		m_PushStartButtonTextRendererWPtr.lock()->SetColor(PRESS_START_BUTTON_COLOR);
		m_PushStartButtonTextRendererWPtr.lock()->SetVisible(false);

		m_UiBackgroundSpriteWPtr = GameObject::AddGameObject("BackgroundMain", m_MainScreenGoWPtr.lock()->GetTransform()).lock()->
			AddComponent<SpriteRenderer>("Empty", 900, Renderer::RenderMode::Viewport);
		m_UiBackgroundSpriteWPtr.lock()->SetColor(0,0,0,1);


		Time::StartTimer(START_TEXT_FLIKKER_SPEED, true, GetSelfPtr(), [&](const Time::Timer* timer)
			{
				if (!IsActiveAndEnabled())
					return;

				const bool visible{ timer->GetTimesLooped() % 2 == 0 };
				m_PushStartButtonTextRendererWPtr.lock()->SetVisible(visible);
			});


		for (const auto& button : MAIN_SCREEN_BUTTONS)
		{
			const auto buttonTextWPtr = GameObject::AddGameObject("OptionText", m_MainScreenGoWPtr.lock()->GetTransform()).lock()->
				AddComponent<TextRenderer>("AfterBurner", button.name, Vector2f{ 0.5f,0.5f }, 1000, Renderer::RenderMode::Screen);
			buttonTextWPtr.lock()->GetRawTransform()->SetPosition(button.position);
			buttonTextWPtr.lock()->GetRawTransform()->SetScale(0.5f);
			buttonTextWPtr.lock()->SetColor(BUTTON_TEXT_COLOR);
		}
	}


	// OptionsScreen
	m_OptionsScreenGoWPtr = GameObject::AddGameObject("OptionsScreen", GetTransform(), Tag::None, false);
	{
		const auto backgroundSpriteWPtr = GameObject::AddGameObject("Background", m_OptionsScreenGoWPtr.lock()->GetTransform()).lock()->
			AddComponent<SpriteRenderer>("Empty", -1000, Renderer::RenderMode::Viewport);
		backgroundSpriteWPtr.lock()->SetColor(OPTIONS_SCREEN_BACKGROUND_COLOR);


		const auto m_OptionMenuTextRendererWPtr = GameObject::AddGameObject("NextStageText", m_OptionsScreenGoWPtr.lock()->GetTransform()).lock()->
			AddComponent<TextRenderer>("AfterBurner", "== OPTION MENU ==", Vector2f{ 0.5f,0.5f }, 1000, Renderer::RenderMode::Screen);
		m_OptionMenuTextRendererWPtr.lock()->GetRawTransform()->SetPosition(GlobalSettings::RENDER_WIDTH / 2.0f, OPTION_MENU_TEXT_HEIGHT);
		m_OptionMenuTextRendererWPtr.lock()->GetRawTransform()->SetScale(0.5f);
		m_OptionMenuTextRendererWPtr.lock()->SetColor(PRESS_START_BUTTON_COLOR);

		for (const auto & button : OPTION_SCREEN_BUTTONS)
		{
			const auto buttonTextWPtr = GameObject::AddGameObject("OptionText", m_OptionsScreenGoWPtr.lock()->GetTransform()).lock()->
				AddComponent<TextRenderer>("AfterBurner", button.name, Vector2f{ 0.0f,0.5f }, 1000, Renderer::RenderMode::Screen);
			buttonTextWPtr.lock()->GetRawTransform()->SetPosition(button.position);
			buttonTextWPtr.lock()->GetRawTransform()->SetScale(0.5f);
			buttonTextWPtr.lock()->SetColor(BUTTON_TEXT_COLOR);
		}

		// Selected Music
		m_SelectedMusicTextRendererWPtr = GameObject::AddGameObject("OptionText", m_OptionsScreenGoWPtr.lock()->GetTransform()).lock()->
			AddComponent<TextRenderer>("AfterBurner", "ERROR", Vector2f{ 0.0f,0.5f }, 1000, Renderer::RenderMode::Screen);
		m_SelectedMusicTextRendererWPtr.lock()->GetRawTransform()->SetPosition(170, OPTION_SCREEN_BUTTONS[0].position.y);
		m_SelectedMusicTextRendererWPtr.lock()->GetRawTransform()->SetScale(0.5f);

		// Selected Music
		m_SelectedVolumeTextRendererWPtr = GameObject::AddGameObject("OptionText", m_OptionsScreenGoWPtr.lock()->GetTransform()).lock()->
			AddComponent<TextRenderer>("AfterBurner", "ERROR", Vector2f{ 0.0f,0.5f }, 1000, Renderer::RenderMode::Screen);
		m_SelectedVolumeTextRendererWPtr.lock()->GetRawTransform()->SetPosition(170, OPTION_SCREEN_BUTTONS[1].position.y);
		m_SelectedVolumeTextRendererWPtr.lock()->GetRawTransform()->SetScale(0.5f);
		m_SelectedVolumeTextRendererWPtr.lock()->SetColor(BUTTON_TEXT_COLOR);

		// Update selected volume based on audio manager
		m_SelectedVolume = int(AudioManager::GetGlobalVolume() * MAX_VOLUME_NUMBER);

		// Selected Level
		m_SelectedLevelTextRendererWPtr = GameObject::AddGameObject("OptionText", m_OptionsScreenGoWPtr.lock()->GetTransform()).lock()->
			AddComponent<TextRenderer>("AfterBurner", "ERROR", Vector2f{ 0.0f,0.5f }, 1000, Renderer::RenderMode::Screen);
		m_SelectedLevelTextRendererWPtr.lock()->GetRawTransform()->SetPosition(170, OPTION_SCREEN_BUTTONS[2].position.y);
		m_SelectedLevelTextRendererWPtr.lock()->GetRawTransform()->SetScale(0.5f);
		m_SelectedLevelTextRendererWPtr.lock()->SetColor(BUTTON_TEXT_COLOR);
	}


	// Game Win Screen
	m_GameWonScreenGoWPtr = GameObject::AddGameObject("IntroScreen", GetTransform(), Tag::None, false);
	{
		const auto winScreen = GameObject::AddGameObject("Win Screen", m_GameWonScreenGoWPtr.lock()->GetTransform());
		winScreen.lock()->AddComponent<SpriteRenderer>("MissionComplete", 10000, Renderer::RenderMode::Screen);
		m_WinScreenAnimatorWPtr = winScreen.lock()->AddComponent<Animator>();
	}



	m_CursorGoWPtr = GameObject::AddGameObject("MainMenuLogo", GetTransform());
	m_CursorGoWPtr.lock()->AddComponent<SpriteRenderer>("Cursor", 1000, Renderer::RenderMode::Screen);
	m_CursorGoWPtr.lock()->SetActive(false);


	Time::StartTimer(LOGO_LOAD_TIME, false, GetSelfPtr(), [&]()
		{
			Time::StartTimer(LOGO_LOAD_TIME_BLACK, false, GetSelfPtr(), [&]()
				{
					ShowScreen(Screens::MainMenu);
					m_CursorGoWPtr.lock()->SetActive(true);

				});
			m_IntroScreenGoWPtr.lock()->SetActive(false);
		});
}

void UiManager::Update()
{

	switch (m_ActiveScreen)
	{
		case Screens::MainMenu:

			// Move cursor
			m_CursorGoWPtr.lock()->GetRawTransform()->SetPosition(100, MAIN_SCREEN_BUTTONS[m_CursorSelection].position.y);

			break;
		case Screens::Options:

			// Move cursor
			const Button selectedButton{ OPTION_SCREEN_BUTTONS[m_CursorSelection] };
			m_CursorGoWPtr.lock()->GetRawTransform()->SetPosition(selectedButton.position.x - 15, selectedButton.position.y);

			// Update Music Preview
			if (AudioManager::MUSIC_MAP.contains(static_cast<AudioManager::MusicOptions>(m_SelectedMusicIndex)))
			{
				std::string musicName{ AudioManager::MUSIC_MAP.at(static_cast<AudioManager::MusicOptions>(m_SelectedMusicIndex)) };

				std::ranges::transform(musicName, musicName.begin(), ::toupper);

				m_SelectedMusicTextRendererWPtr.lock()->SetText(musicName);
				m_SelectedMusicTextRendererWPtr.lock()->SetColor(BUTTON_TEXT_COLOR);
			}
			else
			{
				m_SelectedMusicTextRendererWPtr.lock()->SetText("- BGN STOP -");
				m_SelectedMusicTextRendererWPtr.lock()->SetColor(0.0f, 0.54f, 0.94f, 1.0f);
			}

			// Update Volume Preview
			m_SelectedVolumeTextRendererWPtr.lock()->SetText(std::to_string(m_SelectedVolume));

			// Update Level Preview
			m_SelectedLevelTextRendererWPtr.lock()->SetText(std::to_string(GlobalSettings::StartingStage + 1));

			break;
	}
}

void UiManager::ShowScreen(Screens screen)
{
	m_ActiveScreen = screen;

	m_IntroScreenGoWPtr.lock()->SetActive(false);
	m_MainScreenGoWPtr.lock()->SetActive(false);
	m_OptionsScreenGoWPtr.lock()->SetActive(false);
	m_GameWonScreenGoWPtr.lock()->SetActive(false);

	switch (screen)
	{
		case Screens::Intro:
			m_IntroScreenGoWPtr.lock()->SetActive(true);
			break;
		case Screens::MainMenu:
			m_MainScreenGoWPtr.lock()->SetActive(true);
			break;
		case Screens::Options:
			m_OptionsScreenGoWPtr.lock()->SetActive(true);
			break;
		case Screens::GameWon:
			m_GameWonScreenGoWPtr.lock()->SetActive(true);
	}
}

void UiManager::OnKeyDown(const SDL_KeyboardEvent& e)
{
	if(!IsActiveAndEnabled())
		return;



	if(m_ActiveScreen == Screens::Intro || m_ActiveScreen == Screens::GameWon)
		return;

	if (Input::Compare("uiSelect", e))
	{

		switch (m_ActiveScreen)
		{
		case Screens::MainMenu:

			switch (m_CursorSelection)
			{
				case 0:
					Invoke(&IEvents::OnGameStart);
					break;
				case 1:
					m_CursorSelection = 0;
					ShowScreen(Screens::Options);
					break;

			}

			break;
		case Screens::Options:


			switch (m_CursorSelection)
			{
				case 0:
				{
						const AudioManager::MusicOptions selectedMusic{ static_cast<AudioManager::MusicOptions>(m_SelectedMusicIndex) };
						AudioManager::SetSelectedMusic(selectedMusic);
						AudioManager::PlayerSelectedMusic();
				}
				break;

			case 3:
				DebugManager::PrintControls();

				break;
			case 4:
				m_CursorSelection = 0;
				ShowScreen(Screens::MainMenu);
				break;
			}

			break;
		}

	}


	if (Input::Compare("uiUp", e))
	{
		m_CursorSelection--;
		AudioManager::PlaySoundEffect("MenuSelect2", UI_SELECT_SOUND_VOLUME);
	}

	if (Input::Compare("uiDown", e))
	{
		m_CursorSelection++;
		AudioManager::PlaySoundEffect("MenuSelect2", UI_SELECT_SOUND_VOLUME);
	}

	if (Input::Compare("uiLeft", e))
	{
		if (m_ActiveScreen == Screens::Options)
		{

			if (m_CursorSelection == 0)
			{
				m_SelectedMusicIndex--;
			}

			if (m_CursorSelection == 1)
			{
				m_SelectedVolume--;
				m_SelectedVolume = Jul::ClampLoop(m_SelectedVolume, 0, MAX_VOLUME_NUMBER);
				AudioManager::SetGlobalVolume(float(m_SelectedVolume) / float(MAX_VOLUME_NUMBER));
			}

			if (m_CursorSelection == 2)
			{
				if (GlobalSettings::StartingStage >= 1)
					GlobalSettings::StartingStage--;
			}
		}

		AudioManager::PlaySoundEffect("MenuSelect", UI_SELECT_SOUND_VOLUME);
	}

	if (Input::Compare("uiRight", e))
	{
		if (m_ActiveScreen == Screens::Options)
		{
			if (m_CursorSelection == 0)
			{
				m_SelectedMusicIndex++;
			}

			if (m_CursorSelection == 1)
			{
				m_SelectedVolume++;
				m_SelectedVolume = Jul::ClampLoop(m_SelectedVolume, 0, MAX_VOLUME_NUMBER);
				AudioManager::SetGlobalVolume(float(m_SelectedVolume) / float(MAX_VOLUME_NUMBER));
			}
			 
			if (m_CursorSelection == 2)
			{
				if(GlobalSettings::StartingStage < LevelManager::STAGE_COUNT - 1)
					GlobalSettings::StartingStage++;
			}
		}

		AudioManager::PlaySoundEffect("MenuSelect", UI_SELECT_SOUND_VOLUME);
	}


	
	int max{0};

	switch (m_ActiveScreen)
	{ 
	case Screens::MainMenu:
		max = int(MAIN_SCREEN_BUTTONS.size());
		break;
	case Screens::Options:
		max = int(OPTION_SCREEN_BUTTONS.size());
		break;
	}

	m_CursorSelection = Jul::ClampLoop(m_CursorSelection, 0, max - 1);


	m_SelectedMusicIndex = Jul::ClampLoop(m_SelectedMusicIndex, 0,6);

}

void UiManager::OnReturnToMainMenu(bool showWinScreen)
{
	if (showWinScreen)
	{
		ShowScreen(Screens::GameWon);

		AudioManager::PlayMusic(AudioManager::MusicOptions::City202);
		m_WinScreenAnimatorWPtr.lock()->PlayAnimation("Won");

		Time::StartTimer(12.0f, false, GetSelfPtr(), [&]()
			{
				ShowScreen(Screens::MainMenu);
			});
	}
}
