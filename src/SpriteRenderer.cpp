#include "pch.h"
#include "SpriteRenderer.h"

#include "Camera.h"
#include "AssetManager.h"
#include "Sprite.h"
#include "Transform.h"
#include "Vector3f.h"


SpriteRenderer::SpriteRenderer(const std::string& spriteName, int renderLayer, const RenderMode& renderMode, bool visible, const std::string& name) :
	Renderer{ renderLayer, renderMode,visible, name },
	m_DrawRow{ 0 },
	m_DrawCol{ 0 },
	m_IgnoreCameraRoll{false},
	m_SpritePtr{spriteName.empty() ? nullptr : AssetManager::GetSprite(spriteName)},
	m_Color{1.0f, 1.0f, 1.0f, 1.0f}
{}


void SpriteRenderer::SetDrawCell(int row, int col)
{
	m_DrawRow = row;
	m_DrawCol = col;
}

void SpriteRenderer::SetDrawCell(GlobalSettings::Cell cell)
{
	m_DrawRow = cell.row;
	m_DrawCol = cell.col;
}

void SpriteRenderer::SetColor(const Color4f& color)
{
	m_Color = color;
}

void SpriteRenderer::SetColor(float r, float g, float b, float a)
{
	SetColor(Color4f(r, g, b,a));
}

void SpriteRenderer::SetIgnoreCameraRoll(bool ignore)
{
	m_IgnoreCameraRoll = ignore;
}

void SpriteRenderer::SetSprite(const std::string& spriteName)
{
	m_SpritePtr = AssetManager::GetSprite(spriteName);
}

Sprite* SpriteRenderer::GetSprite() const
{
	return m_SpritePtr;
}

void SpriteRenderer::DrawWorldSpace(const Camera& camera) const
{
	if(m_SpritePtr == nullptr)
		return;

	glPushMatrix();
	{
		// Scale to horizon
		const float scale = camera.DistanceToFlatScale(GetRawTransform()->GetPosition().z);
		glScalef(scale, scale, 1);

		// Move horizon
		const Vector3f cameraPos{ camera.GetRawTransform()->GetPosition() };
		glTranslatef(-cameraPos.x, -cameraPos.y, 0);


		// Move to transform position
		glTranslatef(GetRawTransform()->GetPosition().x, GetRawTransform()->GetPosition().y, 0);
		// Move to transform scale
		glScalef(GetRawTransform()->GetScale().x, GetRawTransform()->GetScale().y, GetRawTransform()->GetScale().z);

		// Rotate to transform rotaiton
		if(m_IgnoreCameraRoll)
			glRotatef(-camera.GetRawTransform()->GetRotation().z, 0, 0, 1);
		else
			glRotatef(GetRawTransform()->GetRotation().z,0,0,1);


		// Scale to unit space
		glScalef(1.0f / m_SpritePtr->PIXLES_PER_UNIT, 1.0f / m_SpritePtr->PIXLES_PER_UNIT, 1);

		// Move to pivot
		glTranslatef(-m_SpritePtr->PIVOT.x, -m_SpritePtr->PIVOT.y, 0);

		glColor4f(m_Color.r, m_Color.g, m_Color.b, m_Color.a);
		m_SpritePtr->Draw(Rectf{ 0,0 ,m_SpritePtr->CELL_SIZE.x  , m_SpritePtr->CELL_SIZE.y },m_DrawCol, m_DrawRow);
		glColor4f(1, 1, 1, 1); // Reset color as safe guard

	}
	glPopMatrix();
}

void SpriteRenderer::DrawScreenSpace(const Camera& camera) const
{
	if (m_SpritePtr == nullptr)
		return;

	glPushMatrix();
	{
		// Move to transform position
		glTranslatef(GetRawTransform()->GetPosition().x, GetRawTransform()->GetPosition().y, 0);

		// Move to transform scale
		glScalef(GetRawTransform()->GetScale().x, GetRawTransform()->GetScale().y, GetRawTransform()->GetScale().z);

		// Rotate to transform rotation
		glRotatef(GetRawTransform()->GetRotation().z, 0, 0, 1);

		// Move to pivot
		glTranslatef(-m_SpritePtr->PIVOT.x, -m_SpritePtr->PIVOT.y, 0);

		glColor4f(m_Color.r, m_Color.g, m_Color.b, m_Color.a);
		m_SpritePtr->Draw(Rectf{ 0,0 ,m_SpritePtr->CELL_SIZE.x  , m_SpritePtr->CELL_SIZE.y }, m_DrawCol, m_DrawRow);
		glColor4f(1, 1, 1, 1); // Reset color as safe guard

	}
	glPopMatrix();

}

void SpriteRenderer::DrawViewportSpace(const Camera& camera) const
{
	if (m_SpritePtr == nullptr)
		return;

	glPushMatrix();
	{
		// Move to transform position
		glTranslatef(GetRawTransform()->GetPosition().x, GetRawTransform()->GetPosition().y, 0);

		// Move to transform scale
		glScalef(GetRawTransform()->GetScale().x, GetRawTransform()->GetScale().y, GetRawTransform()->GetScale().z);

		// Rotate to transform rotation
		glRotatef(GetRawTransform()->GetRotation().z, 0, 0, 1);

		// Move to pivot
		glTranslatef(-m_SpritePtr->PIVOT.x / m_SpritePtr->CELL_SIZE.x, -m_SpritePtr->PIVOT.y / m_SpritePtr->CELL_SIZE.y, 0);

		glColor4f(m_Color.r, m_Color.g, m_Color.b, m_Color.a);
		m_SpritePtr->Draw(Rectf{ 0,0 ,1  , 1 }, m_DrawCol, m_DrawRow);
		glColor4f(1, 1, 1, 1); // Reset color as safe guard
	}
	glPopMatrix();
}
