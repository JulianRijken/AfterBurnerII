#pragma once
#include "Texture.h"
#include "Vector2f.h"

struct Vector2f;

class Font
{
public:

	Font(std::string fontName, std::string fontPath, const int pixelResolution);

	~Font();

	Font(Font&&) = delete;
	Font(const Font&) = delete;
	Font& operator=(Font&&) = delete;
	Font& operator=(const Font&) = delete;

	/// THIS IS A HORRIBLE FIX! The pivot moving runs after the texture gets created
	///	this makes it so that it grabs the texture of the previously used font..
	///	so now update before drawing... Dev looking at this, please fix :)
	///	The cashing has absolutely no use this way!!! all this code should be in text renderer
	///	and only the ttf font should be made an asset! 
	void UpdateText(const std::string& text);

	void Draw(const Vector2f& drawPosition, const std::string& text);
	void Draw(const Rectf& drawRect, const std::string& text);

	[[nodiscard]] Vector2f GetSize() const;

	const int PIXEL_RESOLUTION;
	const std::string FONT_PATH;
	const std::string FONT_NAME;

private:

	// Used for caching
	std::string m_LastDrawnText{};
	Texture* m_Texture;



};

