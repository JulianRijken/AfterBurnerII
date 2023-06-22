#pragma once
#include "Behaviour.h"
#include "GameObject.h"
#include "IEvents.h"

class PlayerController;

class GameManager final : public Behaviour, IEvents
{
public:

	GameManager();

	inline static std::weak_ptr<GameManager> Instance{};

	void Awake() override;
	void Update() override;

	void SetFlySpeed(const float flySpeed);
	void PauseGame(bool paused);

	void OnKeyDown(const SDL_KeyboardEvent& e) override;

	void OnEnemyHit() override;
	void OnPlayerDestroyed() override;
	void OnGameStart() override;
	void OnReturnToMainMenu(bool showWinScreen) override;
	void OnGameOver(bool won) override;
	void OnDistanceTravelledChange(float distance) override;

	[[nodiscard]] std::weak_ptr<Transform> GetMainSceneTransform() const;
	[[nodiscard]] std::weak_ptr<PlayerController> GetPlayerControllerWPtr() const;
	[[nodiscard]] float GetFlySpeed() const { return m_FlySpeed; };
	[[nodiscard]] bool IsGamePaused() const { return m_IsGamePaused; }

	inline static const float MAX_FLY_SPEED{ 100 };
	float m_FlySpeed{ MAX_FLY_SPEED };

private:

	const int MAX_LIVES{ 2 };


	int m_ActiveStage{};

	int m_TotalScore{};
	int m_Score{};
	int m_DistanceScore{};
	int m_Hits{};
	int m_LivesLeft{ MAX_LIVES };

	bool m_IsGamePaused{ false };
	bool m_InGame{ false };
	float m_PausedTimeScaleAlpha{1.0f};

	const float PAUSE_SPEED{2.0f};
	const float UN_PAUSE_SPEED {0.5f};

	std::weak_ptr<GameObject> m_MainGameSceneWPtr{};
	std::weak_ptr<GameObject> m_UiManagerGoWPtr{};
	std::weak_ptr<PlayerController> m_PlayerController{};
};

