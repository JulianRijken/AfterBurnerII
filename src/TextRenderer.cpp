#include "pch.h"
#include "TextRenderer.h"

#include "AssetManager.h"
#include "Camera.h"
#include "Font.h"
#include "Texture.h"
#include "Transform.h"

TextRenderer::TextRenderer(const std::string& fontName, std::string text, const Vector2f& pivot, int renderLayer, const RenderMode& renderMode, bool visible, const std::string& name) :
	Renderer{ renderLayer, renderMode,visible, name },
	m_FontPtr{ fontName.empty() ? nullptr : AssetManager::GetFont(fontName) },
	m_Color{1.0f, 1.0f, 1.0f, 1.0f},
	m_Text{std::move(text)},
	m_Pivot{pivot},
	m_ShadowDistance{2, -2}, // Should be added to parameters and be false default
	m_DrawShadow{true}, // Should be added to parameters and 0 0
	m_IgnoreCameraRoll{false}
{}

void TextRenderer::SetText(const std::string& text)
{
	m_Text = text;
}

void TextRenderer::SetPivot(const Vector2f& pivot)
{
	m_Pivot = pivot;
}

void TextRenderer::SetDrawShadow(const bool drawShadow)
{
	m_DrawShadow = drawShadow;
}


void TextRenderer::SetShadowDistance(const Vector2f& shadowDistance)
{
	m_ShadowDistance = shadowDistance;
}

void TextRenderer::SetIgnoreCameraRoll(bool ignore)
{
	m_IgnoreCameraRoll = ignore;
}

void TextRenderer::DrawWorldSpace(const Camera& camera) const
{
	throw std::logic_error("Function not yet implemented");
}

void TextRenderer::DrawScreenSpace(const Camera& camera) const
{
	if(!m_FontPtr)
		return;

	glPushMatrix();
	{
		// Move to transform position
		glTranslatef(GetTransform().lock()->GetPosition().x, GetTransform().lock()->GetPosition().y, 0);

		// Move to transform scale
		glScalef(GetRawTransform()->GetScale().x, GetRawTransform()->GetScale().y, GetRawTransform()->GetScale().z);

		// Rotate to transform rotation
		glRotatef(GetRawTransform()->GetRotation().z, 0, 0, 1);

		/// HORRIBLE CODE SHOULD BE REMOVED AND FIXED! THIS IS BECAUSE IT IS REALLY REALLY SLOW
		m_FontPtr->UpdateText(m_Text);

		// Move to pivot
		const Vector2f size{ m_FontPtr->GetSize() };
		glTranslatef(-(size.x * m_Pivot.x), -(size.y * m_Pivot.y), 0);

		if (m_DrawShadow)
		{
			glColor4f(0, 0, 0, 1);
			m_FontPtr->Draw(m_ShadowDistance, m_Text);
		}

		glColor4f(m_Color.r, m_Color.g, m_Color.b, m_Color.a);
		m_FontPtr->Draw(Vector2f{ 0,0 },m_Text);

		glColor4f(1,1,1,1); // Reset color as safe guard

	}
	glPopMatrix();
}

void TextRenderer::DrawViewportSpace(const Camera& camera) const
{
	throw std::logic_error("Function not yet implemented");
}


void TextRenderer::SetColor(const Color4f& color)
{
	m_Color = color;
}

void TextRenderer::SetColor(float r, float g, float b, float a)
{
	SetColor(Color4f(r, g, b, a));
}

