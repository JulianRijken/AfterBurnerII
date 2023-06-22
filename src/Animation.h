#pragma once

#include <vector>
#include "GlobalSettings.h"

class Animation final
{
public:

	Animation(std::vector<GlobalSettings::Cell> cells, float framesPerSecond = 0.0f);

	[[nodiscard]] const GlobalSettings::Cell& GetCellFromNormalizedTime(float time) const;

	const std::vector<GlobalSettings::Cell>FRAMES;
	const int FRAME_COUNT;
	const float FRAMES_PER_SECOND;
};