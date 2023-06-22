#pragma once
#include "Behaviour.h"
#include "SpriteRenderer.h"

class Animation;

class Animator final : public Behaviour
{
public:

	Animator(std::weak_ptr<SpriteRenderer> spriteRendererPtr = {}, const std::string& name = "Animator");

	void PlayAnimation(const std::string& name,bool looping = false, float startFrameTime = 0, float speedMultiplier = 1);

	[[nodiscard]] bool GetPaused() const { return m_IsPaused; }
	[[nodiscard]] bool GetIsPlaying() const { return m_IsPlaying; }
	[[nodiscard]] float GetFrameTime() const { return m_FrameTime; }

	void SetPaused(bool isPaused);
	void SetLooping(bool isLooping);
	void SetPlaybackSpeed(float playbackSpeed);

private:

	void Awake() override;
	void Update() override;

	bool m_IsPaused;
	bool m_IsPlaying;
	bool m_IsLooping;
	float m_FrameTime;
	float m_SpeedMultiplier;
	const Animation* m_ActiveAnimation;

	std::weak_ptr<SpriteRenderer> m_SpriteRendererPtr;
};