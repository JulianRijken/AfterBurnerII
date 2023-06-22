#pragma once
#include "GlobalSettings.h"
#include "Renderer.h"

class Sprite;

class SpriteRenderer final : public Renderer
{
public:

	SpriteRenderer(const std::string& spriteName = {}, int renderLayer = 0, const RenderMode& renderMode = RenderMode::World, bool visible = true, const std::string& name = "SpriteRenderer");

	void SetDrawCell(int row, int col);
	void SetDrawCell(GlobalSettings::Cell cell);
	void SetColor(const Color4f& color);
	void SetColor(float r,float g, float b, float a = 1);
	void SetIgnoreCameraRoll(bool ignore);
	void SetSprite(const std::string& spriteName);

	[[nodiscard]] Sprite* GetSprite() const;

private:

	void DrawWorldSpace(const Camera& camera) const override;
	void DrawScreenSpace(const Camera& camera) const override;
	void DrawViewportSpace(const Camera& camera) const override;

	
	int m_DrawRow;
	int m_DrawCol;
	bool m_IgnoreCameraRoll;
	Sprite* m_SpritePtr;
	Color4f m_Color;
};

