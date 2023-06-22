#include "pch.h"
#include "GUIManager.h"

#include "GameObject.h"
#include "Jul.h"
#include "Renderer.h"
#include "SpriteRenderer.h"
#include "TextRenderer.h"
#include "GameTime.h"
#include "Transform.h"


class SpriteRenderer;

void GuiManager::Awake()
{
	GetRawGameObject()->AddComponent<SpriteRenderer>("BaseUI", 0, Renderer::RenderMode::Screen);

	m_ScoreTextWPtr = GameObject::AddGameObject("ScoreText", GetTransform()).lock()->AddComponent<TextRenderer>
		("AfterBurner", "0", Vector2f{ 0.0f,0.0f }, 1000, Renderer::RenderMode::Screen);
	m_ScoreTextWPtr.lock()->GetRawTransform()->SetPosition(72,205);
	m_ScoreTextWPtr.lock()->GetRawTransform()->SetScale(0.5f);
	m_ScoreTextWPtr.lock()->SetColor(YELLOW_TEXT_COLOR);


	m_HitTextWPtr = GameObject::AddGameObject("HitText", GetTransform()).lock()->AddComponent<TextRenderer>
		("AfterBurner", "0", Vector2f{ 0.0f,0.0f }, 1000, Renderer::RenderMode::Screen);
	m_HitTextWPtr.lock()->GetRawTransform()->SetPosition(208, 205);
	m_HitTextWPtr.lock()->GetRawTransform()->SetScale(0.5f);
	m_HitTextWPtr.lock()->SetColor(YELLOW_TEXT_COLOR);


	m_StageTextWPtr = GameObject::AddGameObject("StageText", GetTransform()).lock()->AddComponent<TextRenderer>
		("AfterBurner", "0", Vector2f{ 0.0f,0.0f }, 1000, Renderer::RenderMode::Screen);
	m_StageTextWPtr.lock()->GetRawTransform()->SetPosition(180, 16);
	m_StageTextWPtr.lock()->GetRawTransform()->SetScale(0.5f);
	m_StageTextWPtr.lock()->SetColor(YELLOW_TEXT_COLOR);


	// Game over screen
	m_GameOverScreenGoWPtr = GameObject::AddGameObject("GameOverScreen", GetTransform(),Tag::None,false);
	m_GameOverScreenGoWPtr.lock()->AddComponent<SpriteRenderer>("GameOverScreen", 500, Renderer::RenderMode::Screen);

	const auto gameOverTextGoWPtr = GameObject::AddGameObject("GameOverScreen", m_GameOverScreenGoWPtr.lock()->GetTransform(), Tag::None, true);
	m_GameOverTimerTextWPtr = gameOverTextGoWPtr.lock()->AddComponent<TextRenderer>
		("AfterBurner", "0", Vector2f{ 0.5f,0.5f }, 1000, Renderer::RenderMode::Screen);
	m_GameOverTimerTextWPtr.lock()->GetRawTransform()->SetPosition(GlobalSettings::RENDER_WIDTH / 2.0f, 78);
	m_GameOverTimerTextWPtr.lock()->GetRawTransform()->SetScale(0.5f);
	m_GameOverTimerTextWPtr.lock()->SetColor(YELLOW_TEXT_COLOR);



	// Next stage text
	m_NextStageTextWPtr = GameObject::AddGameObject("NextStageText", GetTransform()).lock()->AddComponent<TextRenderer>
		("AfterBurner", "0", Vector2f{ 0.5f,0.0f }, 1000, Renderer::RenderMode::Screen);
	m_NextStageTextWPtr.lock()->GetRawTransform()->SetPosition(160, 32);
	m_NextStageTextWPtr.lock()->GetRawTransform()->SetScale(0.5f);
	m_NextStageTextWPtr.lock()->SetColor(NEXT_STAGE_TEXT_COLOR);
	m_NextStageTextWPtr.lock()->SetVisible(false);


	m_AimLocationSpriteWPtr = GameObject::AddGameObject("AimLocation", GetTransform()).lock()->AddComponent<SpriteRenderer>
		("AimLocation", 1000,Renderer::RenderMode::Screen);


	m_PlaneLivesSpriteWPtr = GameObject::AddGameObject("PlaneLives", GetTransform()).lock()->AddComponent<SpriteRenderer>
		("PlaneLives", 1000, Renderer::RenderMode::Screen);
	m_PlaneLivesSpriteWPtr.lock()->GetRawTransform()->SetPosition(8, 7);


	for (int i = 0; i < MAX_LARGE_BULLET_DISPLAYS; ++i)
	{
		const auto newBulletWPtr{ GameObject::AddGameObject("BulletSprite", GetTransform()).lock()->AddComponent<SpriteRenderer>
			("AmmoIcon", 1000, Renderer::RenderMode::Screen) };
		newBulletWPtr.lock()->SetDrawCell(0, 0);
		newBulletWPtr.lock()->SetVisible(false);

		m_LargeBulletIconWPtrArray[i] = newBulletWPtr;
	}

	for (int i = 0; i < MAX_SMALL_BULLET_DISPLAYS; ++i)
	{
		const auto newBulletWPtr{ GameObject::AddGameObject("AimLocation", GetTransform()).lock()->AddComponent<SpriteRenderer>
			("AmmoIcon", 1000, Renderer::RenderMode::Screen) };
		newBulletWPtr.lock()->SetDrawCell(0, 1);
		newBulletWPtr.lock()->SetVisible(false);

		m_SmallBulletIconWPtrArray[i] = newBulletWPtr;
	}

	m_GamePausedTextWPtr = GameObject::AddGameObject("PausedText", GetTransform()).lock()->AddComponent<TextRenderer>
		("AfterBurner", "0", Vector2f{ 0.5f,0.5f }, 1000, Renderer::RenderMode::Screen);
	m_GamePausedTextWPtr.lock()->GetRawTransform()->SetPosition(GlobalSettings::RENDER_WIDTH / 2.0f, 170);
	m_GamePausedTextWPtr.lock()->GetRawTransform()->SetScale(0.5f);
	m_GamePausedTextWPtr.lock()->SetColor(YELLOW_TEXT_COLOR);
	m_GamePausedTextWPtr.lock()->SetText("PAUSE !!");
	m_GamePausedTextWPtr.lock()->SetVisible(false);
}


void GuiManager::OnPlayerAimTargetChange(Vector2f playerAimTarget)
{
	const Vector2f drawLocation
	{
		Jul::Lerp(AIM_LOCATION_DRAW_RECT.left,AIM_LOCATION_DRAW_RECT.left + AIM_LOCATION_DRAW_RECT.width,playerAimTarget.x),
		Jul::Lerp(AIM_LOCATION_DRAW_RECT.bottom,AIM_LOCATION_DRAW_RECT.bottom + AIM_LOCATION_DRAW_RECT.height,playerAimTarget.y),
	};

	m_AimLocationSpriteWPtr.lock()->GetRawTransform()->SetPosition(drawLocation);
}

void GuiManager::OnHitsChange(int hits)
{
	m_HitTextWPtr.lock()->SetText(std::to_string(hits));
}

void GuiManager::OnStageChange(int stage, bool bonusStage)
{
	// Go from index to actual number
	stage += 1;


	m_StageTextWPtr.lock()->SetText(std::to_string(stage));


	m_NextStageTextWPtr.lock()->SetText("STAGE " + std::to_string(stage));

	Time::StartTimer(0.3f, true, GetSelfPtr(), [=](Time::Timer* timer)
		{
			if (timer->GetTimesLooped() >= 8)
			{
				timer->Cancel();
				return;
			}

			m_NextStageTextWPtr.lock()->SetVisible(Jul::IsEven(timer->GetTimesLooped()));
		});
}

void GuiManager::OnScoreChange(int score)
{
	m_ScoreTextWPtr.lock()->SetText(std::to_string(score));
}

void GuiManager::OnAmmoChange(int newAmmo)
{
	const int displayLargeBulletCount{ int(float(newAmmo) / float(MAX_LARGE_BULLET_DISPLAYS + 1)) };
	const int displaySmallBulletCount{ newAmmo % (MAX_SMALL_BULLET_DISPLAYS + 1) };


	int largeBulletIndex{0};
	for (std::weak_ptr<SpriteRenderer>& largeBullet : m_LargeBulletIconWPtrArray)
	{
		if(largeBulletIndex < displayLargeBulletCount)
		{

			largeBullet.lock()->SetVisible(true);
			largeBullet.lock()->GetRawTransform()->SetPosition
			(
				AMMO_DRAW_LOCATION.x + largeBulletIndex * AMMO_LARGE_BULLET_SPACING, 
				AMMO_DRAW_LOCATION.y
			);

			largeBulletIndex++;
		}
		else
			largeBullet.lock()->SetVisible(false);
	}

	int smallBulletIndex{ 0 };
	for (std::weak_ptr<SpriteRenderer>& smallBullet : m_SmallBulletIconWPtrArray)
	{
		if (smallBulletIndex < displaySmallBulletCount)
		{

			smallBullet.lock()->SetVisible(true);
			smallBullet.lock()->GetRawTransform()->SetPosition
			(
				AMMO_DRAW_LOCATION.x + largeBulletIndex * AMMO_LARGE_BULLET_SPACING + smallBulletIndex * AMMO_SMALL_BULLET_SPACING,
				AMMO_DRAW_LOCATION.y
			);

			smallBulletIndex++;
		}
		else
			smallBullet.lock()->SetVisible(false);
	}
}

void GuiManager::OnPlayerLivesChanged(int newLives)
{
	int drawCell{};

	switch (newLives)
	{
	case 0:
		drawCell = 2;
		break;
	case 1:
		drawCell = 1;
		break;
	case 2:
		drawCell = 0;
		break;
	}

	m_PlaneLivesSpriteWPtr.lock()->SetDrawCell(0, drawCell);
}

void GuiManager::OnGamePaused(bool paused)
{
	m_GamePausedTextWPtr.lock()->SetVisible(paused);
}

void GuiManager::OnGameOver(bool won)
{
	if (!won)
	{
		m_GameOverScreenGoWPtr.lock()->SetActive(true);
		m_GameOverTimerTextWPtr.lock()->SetText("9");

		Time::StartTimer(1.0f, true, GetSelfPtr(), [&](Time::Timer* timer)
			{
				m_GameOverTimerTextWPtr.lock()->SetText(std::to_string(abs(9 - timer->GetTimesLooped())));

		if (timer->GetTimesLooped() >= 10)
		{
			m_GameOverScreenGoWPtr.lock()->SetActive(false);
			timer->Cancel();
		}
			});
	}
}
