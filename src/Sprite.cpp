#include "pch.h"

#include <utility>

#include "Animation.h"
#include "Sprite.h"

#include "Log.h"


Sprite::Sprite(std::string name, const std::string& path, const float pixelsPerUnit, const Vector2f& pivotAlpha,
               int rowCount, int colCount, std::map<std::string, Animation> animations) :
	ROW_COUNT{ std::max(1, rowCount) },
	COL_COUNT{std::max(1, colCount)},
	PIXLES_PER_UNIT{pixelsPerUnit},
	TEXTURE{ new Texture{path} },
	CELL_SIZE{ TEXTURE->GetWidth() / float(COL_COUNT),TEXTURE->GetHeight() / float(ROW_COUNT) },
	PIVOT{pivotAlpha.x * CELL_SIZE.x, pivotAlpha.y * CELL_SIZE.y},
	SPRITE_NAME{std::move(name)},
	ANIMATIONS{std::move(animations)}
{
}

Sprite::~Sprite()
{
	delete TEXTURE;
}


void Sprite::Draw(const Rectf& drawRect) const
{
	TEXTURE->Draw(drawRect);
}

void Sprite::Draw(const Rectf& drawRect, int column, int row) const
{
	TEXTURE->Draw(drawRect, Rectf{ CELL_SIZE.x * float(column),CELL_SIZE.y * float(row + 1),CELL_SIZE.x,CELL_SIZE.y });
}

void Sprite::Draw(const Vector2f& drawPosition, int column, int row) const
{
	TEXTURE->Draw(drawPosition.ToPoint2f(), Rectf{ CELL_SIZE.x * float(column), CELL_SIZE.y * float(row + 1),CELL_SIZE.x,CELL_SIZE.y });
}

void Sprite::Draw(const Vector2f& drawPosition) const
{
	TEXTURE->Draw(drawPosition.ToPoint2f());
}

const Animation* Sprite::GetAnimation(const std::string& name) const
{
	if (!ANIMATIONS.contains(name))
	{
		Log(TextColor::Black,BackgroundColor::BrightRed)<< "Animation '" << name << "' does not exist for sprite '" << SPRITE_NAME << "'";
		return nullptr;
	}

	return &ANIMATIONS.at(name);
}
