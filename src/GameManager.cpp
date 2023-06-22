#include "pch.h"
#include "GameManager.h"

#include "GameObject.h"
#include "GlobalSettings.h"
#include "Input.h"
#include "GameTime.h"

#include "LevelManager.h"
#include "SpriteRenderer.h"
#include "PlayerController.h"
#include "GUIManager.h"
#include "UIManager.h"
#include "Camera.h"
#include "Jul.h"
#include "Transform.h"
#include "Collider.h"


GameManager::GameManager() = default;


void GameManager::Awake()
{
	// Create Singleton for Game Manger
	if (Instance.expired())
	{
		Instance = GetSelfPtr<GameManager>();
	}
	else
	{
		Log(LogType::Warning) << "GameManager Already Exist. New GameManager will be deleted";
		GameObject::Destroy(GetGameObject());
	}

	const auto mainCameraGoWPtr{ GameObject::AddGameObject("Camera",Tag::MainCamera, true, Vector3f{ 0,0,-GlobalSettings::MAIN_GAME_ZOOM }) };
	mainCameraGoWPtr.lock()->AddComponent<Camera>(GlobalSettings::MAIN_GAME_ZOOM, GlobalSettings::MAIN_GAME_ORTHO_SIZE, GlobalSettings::MAIN_CAMERA_PIXELATE);

	m_UiManagerGoWPtr = GameObject::AddGameObject("UIManager");
	m_UiManagerGoWPtr.lock()->AddComponent<UiManager>();
}

void GameManager::Update()
{
	// Handles time scale for game pausing
	m_PausedTimeScaleAlpha += Time::GetUnScaledDeltaTime() * (m_IsGamePaused ? -PAUSE_SPEED : UN_PAUSE_SPEED);
	m_PausedTimeScaleAlpha = Jul::Clamp01(m_PausedTimeScaleAlpha);

	if(m_IsGamePaused)
		Time::SetTimeScale(Jul::SmoothStartLerp(0.0f, GlobalSettings::DefaultTimeScale, m_PausedTimeScaleAlpha));
	else
		Time::SetTimeScale(Jul::SmoothEndLerp(0.0f, GlobalSettings::DefaultTimeScale, m_PausedTimeScaleAlpha));
}

void GameManager::OnKeyDown(const SDL_KeyboardEvent& e)
{
	if(Input::Compare("forceRestart", e) && m_IsGamePaused)
	{
		const auto playerGoWPtr{ GameObject::FindWithTag(Tag::Player)};
		if(playerGoWPtr.expired())
			return;

		const auto playerControllerWPtr{ playerGoWPtr.lock()->GetComponent<PlayerController>() };
		if(playerControllerWPtr.expired())
			return;

		if (playerControllerWPtr.lock()->GetActiveState() != PlayerController::PlayerState::Flying)
			return;


		if (!m_MainGameSceneWPtr.expired())
		{
			const bool showWinScreen{ false };
			IEvents::Invoke(&IEvents::OnReturnToMainMenu, showWinScreen);
		}
	}

	if (Input::Compare("pause", e))
	{
		if (m_InGame)
			PauseGame(!m_IsGamePaused);
	}
}

void GameManager::OnEnemyHit()
{
	m_Hits++;
	IEvents::Invoke(&IEvents::OnHitsChange, m_Hits);
}


void GameManager::SetFlySpeed(const float flySpeed)
{
	m_FlySpeed = flySpeed;
}

void GameManager::PauseGame(bool paused)
{
	m_IsGamePaused = paused;
	IEvents::Invoke(&IEvents::OnGamePaused, m_IsGamePaused);
}


void GameManager::OnPlayerDestroyed()
{
	m_LivesLeft--;
	IEvents::Invoke(&IEvents::OnPlayerLivesChanged, m_LivesLeft);

	if(m_LivesLeft < 0)
	{
		m_LivesLeft = 0;
		bool gameWon{ false };
		IEvents::Invoke(&IEvents::OnGameOver, gameWon);
	}
	else
	{
		IEvents::Invoke(&IEvents::OnPlayerRespawn);
	}


	IEvents::Invoke(&IEvents::OnPlayerLivesChanged, m_LivesLeft);
}

void GameManager::OnGameStart()
{
	// Make sure the game scene is removed
	if(!m_MainGameSceneWPtr.expired())
		return;

	m_UiManagerGoWPtr.lock()->SetActive(false);

	m_FlySpeed = MAX_FLY_SPEED;
	m_Hits = 0;
	m_LivesLeft = MAX_LIVES;

	AudioManager::PlayMusic(AudioManager::MusicOptions::FinalTakeOff,true,false);

	// This should all be destroyed or kept or somthing else lol

	m_MainGameSceneWPtr = GameObject::AddGameObject("MainGameScene");
	{
		const auto guiManagerGoWPtr{ GameObject::AddGameObject("GUIManager") };
		guiManagerGoWPtr.lock()->AddComponent<GuiManager>();
		guiManagerGoWPtr.lock()->GetRawTransform()->SetParent(m_MainGameSceneWPtr.lock()->GetTransform());

		const auto levelManagerGoWPtr = GameObject::AddGameObject("LevelManager");
		levelManagerGoWPtr.lock()->AddComponent<LevelManager>();
		levelManagerGoWPtr.lock()->GetRawTransform()->SetParent(m_MainGameSceneWPtr.lock()->GetTransform());

		const auto playerGoWPtr = GameObject::AddGameObject("Player", Tag::Player);
		playerGoWPtr.lock()->AddComponent<SpriteRenderer>("player");
		m_PlayerController = playerGoWPtr.lock()->AddComponent<PlayerController>();
		playerGoWPtr.lock()->AddComponent<Collider>(Vector3f{ 0.8f,0.8f,0.0f });
		playerGoWPtr.lock()->GetRawTransform()->SetParent(m_MainGameSceneWPtr.lock()->GetTransform());
	}
	

	m_InGame = true;

	// Runs a frame later because the pause button is the same
	Time::StartTimer(0.0f, false, GetSelfPtr(), [&]()
		{
			PauseGame(false);
		});
}

void GameManager::OnReturnToMainMenu(bool showWinScreen)
{
	// Destroy game
	if (!m_MainGameSceneWPtr.expired())
		GameObject::Destroy(m_MainGameSceneWPtr);

	m_InGame = false;
	m_UiManagerGoWPtr.lock()->SetActive(true);
	PauseGame(false);
}

void GameManager::OnGameOver(bool won)
{
	// Parent timer to main game scene
	Time::StartTimer(won ? 16.0f : 11.0f, false, m_MainGameSceneWPtr, [&](bool gameWon)
		{
			IEvents::Invoke(&IEvents::OnReturnToMainMenu, gameWon);
		}, won);
}

void GameManager::OnDistanceTravelledChange(float distance)
{
	m_DistanceScore = static_cast<int>(distance);
	m_TotalScore = m_Score + m_DistanceScore;

	IEvents::Invoke(&IEvents::OnScoreChange, m_TotalScore);
}


std::weak_ptr<Transform> GameManager::GetMainSceneTransform() const
{
	if (m_MainGameSceneWPtr.expired())
	{
		//Log(LogType::Warning) << "Main scene transform does not exist";
		return {};
	}

	return m_MainGameSceneWPtr.lock()->GetTransform();
}

std::weak_ptr<PlayerController> GameManager::GetPlayerControllerWPtr() const
{
	return m_PlayerController;
}
