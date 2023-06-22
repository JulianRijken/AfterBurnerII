#pragma once
#include <map>
#include "Texture.h"
#include "Vector2f.h"

#include "Animation.h"

class Sprite
{

public:

	Sprite(std::string name, const std::string& path,const float pixelsPerUnit,const Vector2f& pivotAlpha = Vector2f{ 0,0 },
		int rowCount = 1, int colCount = 1, std::map<std::string, Animation> animations = {}
	);

	~Sprite();

	Sprite(Sprite&& other) = delete;
	Sprite(const Sprite& other) = delete;
	const Sprite& operator=(Sprite&& other) = delete;
	const Sprite& operator=(const Sprite& other) = delete;


	void Draw(const Rectf& drawRect) const;
	void Draw(const Rectf& drawRect, int column, int row = {}) const;
	void Draw(const Vector2f& drawPosition, int column, int row = {}) const;
	void Draw(const Vector2f& drawPosition) const;

	[[nodiscard]] const Animation* GetAnimation(const std::string& name) const;

	const int ROW_COUNT;
	const int COL_COUNT;
	const float PIXLES_PER_UNIT;
	const Texture* TEXTURE;
	const Vector2f CELL_SIZE;
	const Vector2f PIVOT;
	const std::string SPRITE_NAME;
	const std::map<std::string, Animation> ANIMATIONS;
};