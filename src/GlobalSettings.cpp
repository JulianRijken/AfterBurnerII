#include "pch.h"
#include "GlobalSettings.h"

#include "Vector2f.h"

void GlobalSettings::SetRenderOrthographic(const float orthoSize)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	const Vector2f orthoSizeBox{ orthoSize * GlobalSettings::RENDER_WIDTH / GlobalSettings::RENDER_HEIGHT, orthoSize };
	glOrtho(GLdouble(-orthoSizeBox.x), GLdouble(orthoSizeBox.x), GLdouble(-orthoSizeBox.y), GLdouble(orthoSizeBox.y), -1, 1);
}

void GlobalSettings::SetRenderScreen()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, GlobalSettings::RENDER_WIDTH, 0, GlobalSettings::RENDER_HEIGHT, -1, 1);

}

void GlobalSettings::SetRenderViewport()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(0, 1, 0, 1, -1, 1);
}
