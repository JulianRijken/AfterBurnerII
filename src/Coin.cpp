#include "pch.h"
#include "Coin.h"

#include "GameObject.h"
#include "Jul.h"
#include "SpriteRenderer.h"
#include "GameTime.h"
#include "Transform.h"


void Coin::Awake()
{
	m_StartPosition = GetRawTransform()->GetPosition();

	Time::StartLerpTimer(0.8f, false, GetSelfPtr(), [&](float alpha)
		{
			const float positionOffset{ Jul::SmoothEndLerp(0.0f,2.0f,alpha) };
			const float smoothStartAlpha{ Jul::SmoothStartLerp(0.0f,1.0f,Jul::MapValueInRangeClamped(alpha,0.5f,1.0f,0.0f,1.0f)) };

			GetRawTransform()->SetPosition(m_StartPosition + Vector3f::Up() * positionOffset);
			const auto spriteRendererWPtr{ GetRawGameObject()->GetComponent<SpriteRenderer>() };

			spriteRendererWPtr.lock()->SetColor(Jul::Lerp(Color4f{ 1,1,1,1 }, Color4f{ 1,1,1,0 }, smoothStartAlpha));

		});
}
