#include "pch.h"
#include "Font.h"

#include "Log.h"
#include "Vector2f.h"


Font::Font(std::string fontName, std::string fontPath, const int pixelResolution) :
	PIXEL_RESOLUTION{pixelResolution},
	FONT_PATH{std::move(fontPath)},
	FONT_NAME{std::move(fontName)},
	m_Texture{nullptr}
{
}

Font::~Font()
{
	delete m_Texture;
}


void Font::UpdateText(const std::string& text)
{
	if (text.empty())
	{
		m_Texture = nullptr;
		return;
	}

	if (text != m_LastDrawnText || m_Texture == nullptr)
	{
		delete m_Texture;
		m_Texture = new Texture{ text,FONT_PATH,PIXEL_RESOLUTION,Color4f{1,1,1,1} };
		m_LastDrawnText = text;
	}
}

void Font::Draw(const Vector2f& drawPosition, const std::string& text)
{
	if(text.empty())
		return;

	UpdateText(text);

	if(m_Texture != nullptr)
		m_Texture->Draw(drawPosition.ToPoint2f());
}

void Font::Draw(const Rectf& drawRect, const std::string& text)
{
	if (text.empty())
		return;

	UpdateText(text);

	if (m_Texture != nullptr)
		m_Texture->Draw(drawRect);
}

Vector2f Font::GetSize() const
{
	return Vector2f{ m_Texture->GetWidth(),m_Texture->GetHeight() };
}
