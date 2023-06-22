#pragma once
#include "Behaviour.h"
#include "IEvents.h"
#include "TextRenderer.h"

class SpriteRenderer;

class GuiManager final : public Behaviour, IEvents
{
protected:
	void Awake() override;

private:

	void OnPlayerAimTargetChange(Vector2f playerAimTarget) override;
	void OnHitsChange(int hits) override;
	void OnStageChange(int stage, bool bonusStage) override;
	void OnScoreChange(int score) override;
	void OnAmmoChange(int newAmmo) override;
	void OnPlayerLivesChanged(int newLives) override;
	void OnGamePaused(bool paused) override;
	void OnGameOver(bool won) override;

	const Rectf AIM_LOCATION_DRAW_RECT{ 262, 188, 16, 12 };
	const Vector2f AMMO_DRAW_LOCATION{ 201,8 };
	const int AMMO_LARGE_BULLET_SPACING{ 7 };
	const int AMMO_SMALL_BULLET_SPACING{ 5 };

	const Color4f YELLOW_TEXT_COLOR{ 0.93f, 0.9f, 0.49f, 1.0f };
	const Color4f NEXT_STAGE_TEXT_COLOR{ 0.79f, 0.59f, 0.78f, 1.0f };

	const static int MAX_LARGE_BULLET_DISPLAYS{9};
	const static int MAX_SMALL_BULLET_DISPLAYS{9};

	std::weak_ptr<TextRenderer> m_ScoreTextWPtr{};
	std::weak_ptr<TextRenderer> m_HitTextWPtr{};
	std::weak_ptr<TextRenderer> m_StageTextWPtr{};
	std::weak_ptr<TextRenderer> m_GameOverTimerTextWPtr{};
	std::weak_ptr<GameObject>   m_GameOverScreenGoWPtr{};
	std::weak_ptr<TextRenderer> m_NextStageTextWPtr{};
	std::weak_ptr<TextRenderer> m_GamePausedTextWPtr{};

	std::weak_ptr<SpriteRenderer> m_AimLocationSpriteWPtr{};
	std::weak_ptr<SpriteRenderer> m_PlaneLivesSpriteWPtr{};

	std::weak_ptr<SpriteRenderer> m_LargeBulletIconWPtrArray[MAX_LARGE_BULLET_DISPLAYS]{};
	std::weak_ptr<SpriteRenderer> m_SmallBulletIconWPtrArray[MAX_SMALL_BULLET_DISPLAYS]{};
};

