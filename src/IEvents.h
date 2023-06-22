#pragma once
#include <unordered_set>

#include "Vector2f.h"


class IEvents
{

public:

	IEvents()
	{
		m_EventPtrs.insert(this);
	}

	virtual ~IEvents()
	{
		m_EventPtrs.erase(this);
	}

	static void Invoke(void(IEvents::* method)())
	{
		for (auto iterator = m_EventPtrs.begin(); iterator != m_EventPtrs.end(); ++iterator)
		{
			// Convert local variables to rvalue references explicitly
			((*iterator)->*method)();
		}
	}

	template<typename... Args>
	static void Invoke(void(IEvents::* method)(Args...), Args... args)
	{
		for (auto iterator = m_EventPtrs.begin(); iterator != m_EventPtrs.end(); ++iterator)
		{
			// Convert local variables to rvalue references explicitly
			((*iterator)->*method)(std::forward<Args>(args)...);
		}
	}

	template<typename... Args>
	static void Invoke(void(IEvents::* method)(Args...), Args&&... args)
	{
		for (auto iterator = m_EventPtrs.begin(); iterator != m_EventPtrs.end(); ++iterator)
		{
			// Convert local variables to rvalue references explicitly
			((*iterator)->*method)(std::forward<Args>(args)...);
		}
	}


	virtual void OnKeyDown(const SDL_KeyboardEvent& e) {}
	virtual void OnKeyUp(const SDL_KeyboardEvent& e) {}
	virtual void OnMouseMotion(const SDL_MouseMotionEvent& e) {}
	virtual void OnMouseDown(const SDL_MouseButtonEvent& e) {}
	virtual void OnMouseUp(const SDL_MouseButtonEvent& e) {}

	virtual void OnStageChange(int stage, bool bonusStage) {}
	virtual void OnPlayerDestroyed() {}
	virtual void OnPlayerLivesChanged(int newLives) {}
	virtual void OnPlayerRespawn() {}
	virtual void OnEnemyHit() {}
	virtual void OnHitsChange(int hits) {}
	virtual void OnScoreChange(int score) {}
	virtual void OnDistanceTravelledChange(float distance) {}
	virtual void OnAmmoChange(int newAmmo) {}

	virtual void OnPlayerAimTargetChange(Vector2f playerAimTarget) {}

	virtual void OnGameStart() {}
	virtual void OnGameOver(bool won) {}
	virtual void OnReturnToMainMenu(bool showWinScreen) {}
	virtual void OnGamePaused(bool paused) {}

	virtual void OnPlaneAirborne() {}
	virtual void OnPlaneRefuel() {}
	virtual void OnPlaneLanding() {}

private:

	inline static std::unordered_set<IEvents*> m_EventPtrs{};

};

