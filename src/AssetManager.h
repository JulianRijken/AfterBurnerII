#pragma once
#include <map>

#include "Animation.h"
#include "Vector2f.h"


class Font;
class Sprite;

class AssetManager final
{
public:

	static void Initialize();

	static void Cleanup();

	static Sprite* GetSprite(const std::string& name);
	static Font* GetFont(const std::string& name);

	static void AddSprite(const std::string& assetName, const std::string& path, float pixelsPerUnit, const Vector2f& pivotAlpha = Vector2f{ 0,0 },
		int colCount = 1, int rowCount = 1, std::map<std::string, Animation> animations = {}
	);

	static void AddFont(const std::string& assetName, const std::string& fontPath, const int pixelResolution);


private:

	inline static std::map<std::string, Sprite*> m_SpritePtrMap{};
	inline static std::map<std::string, Font*> m_FontPtrMap{};
};

