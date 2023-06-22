#include "pch.h"
#include "Animator.h"

#include "GameObject.h"
#include "Jul.h"
#include "Sprite.h"
#include "GameTime.h"

Animator::Animator(std::weak_ptr<SpriteRenderer> spriteRendererPtr, const std::string& name) :
	Behaviour{name},
	m_IsPaused{ true },
	m_IsPlaying{false},
	m_IsLooping{true},
	m_FrameTime{0},
	m_SpeedMultiplier{1},
	m_ActiveAnimation{nullptr},
	m_SpriteRendererPtr{std::move(spriteRendererPtr)}
{
}

void Animator::PlayAnimation(const std::string& name, bool looping, float startFrameTime, float speedMultiplier)
{
	m_IsPlaying = true;
	m_IsLooping = looping;
	m_SpeedMultiplier = speedMultiplier;
	m_FrameTime = startFrameTime;
	m_IsPaused = false;

	m_ActiveAnimation = m_SpriteRendererPtr.lock()->GetSprite()->GetAnimation(name);
}

void Animator::SetPaused(bool isPaused)
{
	m_IsPaused = isPaused;
}

void Animator::SetLooping(bool isLooping)
{
	m_IsLooping = isLooping;
}

void Animator::SetPlaybackSpeed(float playbackSpeed)
{
	m_SpeedMultiplier = playbackSpeed;
}


void Animator::Update()
{
	if(m_SpriteRendererPtr.expired())
		return;

	if(m_ActiveAnimation == nullptr)
		return;

	if (!m_IsPaused)
	{
		const float delta{ Time::GetDeltaTime() * (m_ActiveAnimation->FRAMES_PER_SECOND / m_ActiveAnimation->FRAME_COUNT) * m_SpeedMultiplier };

		if(m_IsLooping)
			m_FrameTime = Jul::ClampLoop01(m_FrameTime + delta);
		else
			m_FrameTime = Jul::Clamp01(m_FrameTime + delta);

		// Has to maybe be looked in to in the future as it may not be a good representation of "playing" when scrubbing 
		if (delta + m_FrameTime > 1 || delta + m_FrameTime < 0)
			m_IsPlaying = false;
	}

	m_SpriteRendererPtr.lock()->SetDrawCell(m_ActiveAnimation->GetCellFromNormalizedTime(m_FrameTime));
}

void Animator::Awake()
{
	// Try finding the sprite renderer if it's not set
	if (m_SpriteRendererPtr.expired())
		m_SpriteRendererPtr = GetRawGameObject()->GetComponent<SpriteRenderer>();
}
