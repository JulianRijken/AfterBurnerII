#pragma once
#include "Renderer.h"
#include "Vector2f.h"

class Font;
class Texture;

class TextRenderer : public Renderer
{
public:

	TextRenderer(const std::string& fontName = {}, std::string text = {}, const Vector2f& pivot = {}, int renderLayer = 0, const RenderMode& renderMode = RenderMode::World, bool visible = true, const std::string& name = "SpriteRenderer");

	void SetColor(const Color4f& color);
	void SetColor(float r, float g, float b, float a = 1);
	void SetText(const std::string& text);
	void SetPivot(const Vector2f& pivot);
	void SetDrawShadow(const bool drawShadow);
	void SetShadowDistance(const Vector2f& shadowDistance);
	void SetIgnoreCameraRoll(bool ignore);

private:

	void DrawWorldSpace(const Camera& camera) const override;
	void DrawScreenSpace(const Camera& camera) const override;
	void DrawViewportSpace(const Camera& camera) const override;

	Font* m_FontPtr;
	Color4f m_Color;
	std::string m_Text;
	Vector2f m_Pivot;
	Vector2f m_ShadowDistance;
	bool m_DrawShadow;
	bool m_IgnoreCameraRoll;

};

