#include "pch.h"
#include "Animation.h"

#include <utility>

#include "Jul.h"

Animation::Animation(std::vector<GlobalSettings::Cell> cells, float framesPerSecond):
	FRAMES{std::move(cells)},
	FRAME_COUNT{ int(FRAMES.size())},
	FRAMES_PER_SECOND(framesPerSecond)
{}

const GlobalSettings::Cell& Animation::GetCellFromNormalizedTime(float time) const
{
	if (time <= 0)
		return FRAMES.front();

	if (time >= 1)
		return FRAMES.back();

	return FRAMES[static_cast<int>(static_cast<float>(FRAME_COUNT) * time)];
}