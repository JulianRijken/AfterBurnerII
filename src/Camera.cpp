#include "pch.h"
#include "Camera.h"

#include <cassert>

#include "GameObject.h"
#include "Log.h"
#include "Texture.h"
#include <Matrix2x3.h>

#include "Gizmos.h"
#include "GlobalSettings.h"
#include "Renderer.h"
#include "Transform.h"


Camera::Camera(float zoom, float orthoSize, bool pixelate, const std::string& name) :
	Component{ name },
	m_Zoom{ zoom },
	m_OrthoSize{ orthoSize },
	m_Pixelate{ pixelate },
	m_RenderTexturePtr{new Texture{"Empty.png"}}
{
	assert(zoom >= 1);
	m_CameraPtrs.insert(this);
}

Camera::~Camera()
{
	m_CameraPtrs.erase(this);
	delete m_RenderTexturePtr;
}


void Camera::RenderAll()
{
	for (auto iterator = m_CameraPtrs.begin(); iterator != m_CameraPtrs.end(); ++iterator)
		(*iterator)->Render();
}

void Camera::Render() const
{
	// Has to be a copy as items get removed from the vector
	std::vector<Renderer*> renderers{ Renderer::GetAllRenderers() };

	const auto compareDistance = [](const Renderer* a, const Renderer* b)
	{
		return a->GetRawTransform()->GetPosition().z > b->GetRawTransform()->GetPosition().z;
	};

	const auto compareLayer = [](const Renderer* a, const Renderer* b)
	{
		return a->GetRenderLayer() < b->GetRenderLayer();
	};

	const auto visibleAndActive = std::ranges::remove_if(renderers,[](const Renderer* rendererPtr)
	{
		return !rendererPtr->IsVisible() || !rendererPtr->GetRawGameObject()->IsActiveInHierarchy();
	}).begin();

	renderers.resize(std::distance(renderers.begin(), visibleAndActive));
	std::ranges::sort(renderers, compareDistance);
	std::ranges::stable_sort(renderers, compareLayer);

	if (m_Pixelate && GlobalSettings::AllowPixelate)
	{
		// Set viewport to render size
		glViewport(0, 0, GlobalSettings::RENDER_WIDTH, GlobalSettings::RENDER_HEIGHT);
	}

	for (const Renderer* rendererPtr : renderers)
	{
		switch (rendererPtr->RENDER_MODE)
		{
			case Renderer::RenderMode::World:
			{
				GlobalSettings::SetRenderOrthographic(m_OrthoSize);

				glPushMatrix();
				{
					// Roll World
					glRotatef(GetRawTransform()->GetRotation().z, 0, 0, 1);

					// Move World "Fake rotation"
					glTranslatef(-GetRawTransform()->GetRotation().y, GetRawTransform()->GetRotation().x, 0);

					rendererPtr->DrawWorldSpace(*this);
				}
				glPopMatrix();

				break;
			}
			case Renderer::RenderMode::Screen:
			{
				GlobalSettings::SetRenderScreen();
				rendererPtr->DrawScreenSpace(*this);

				break;
			}
			case Renderer::RenderMode::Viewport:
			{
				GlobalSettings::SetRenderViewport();
				rendererPtr->DrawViewportSpace(*this);

				break;
			}
		}

	}

	if (m_Pixelate && GlobalSettings::AllowPixelate)
	{
		glBindTexture(GL_TEXTURE_2D, m_RenderTexturePtr->GetID());
		glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 0, 0, GlobalSettings::RENDER_WIDTH, GlobalSettings::RENDER_HEIGHT, 0);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glViewport(0, 0, GlobalSettings::WINDOW_WIDTH, GlobalSettings::WINDOW_HEIGHT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, 1, 1, 0, -1, 1);

		m_RenderTexturePtr->Draw(Rectf{ 0,0,1,1 });
	}

	GlobalSettings::SetRenderOrthographic(m_OrthoSize);
	Gizmos::DrawAllGizmos(this);
}


Vector2f Camera::WorldToFlatPoint(const Vector3f& worldPoint) const
{
	Matrix2x3 rollCameraM;
	rollCameraM.SetAsRotate(GetRawTransform()->GetRotation().z);

	Matrix2x3 rotateCameraM;
	rotateCameraM.SetAsTranslate(-GetRawTransform()->GetRotation().y, GetRawTransform()->GetRotation().x);

	Matrix2x3 scaleToHorizonM;
	const float scale = DistanceToFlatScale(worldPoint.z);
	scaleToHorizonM.SetAsScale(scale, scale);

	Matrix2x3 translateHorizonM;
	translateHorizonM.SetAsTranslate(-GetRawTransform()->GetPosition().x, -GetRawTransform()->GetPosition().y);


	const Matrix2x3 transformationM = rollCameraM * rotateCameraM * scaleToHorizonM * translateHorizonM;

	return Vector2f{ transformationM.Transform(Point2f{ worldPoint.x,worldPoint.y }) };
}

Vector3f Camera::FlatToWorldPoint(const Vector2f& flatPoint, float distance) const
{
	Matrix2x3 rollCameraM;
	rollCameraM.SetAsRotate(-GetRawTransform()->GetRotation().z);

	Matrix2x3 rotateCameraM;
	rotateCameraM.SetAsTranslate(GetRawTransform()->GetRotation().y, -GetRawTransform()->GetRotation().x);

	Matrix2x3 translateHorizonM;
	translateHorizonM.SetAsTranslate(GetRawTransform()->GetPosition().x,GetRawTransform()->GetPosition().y);


	const Matrix2x3 transformationM = translateHorizonM * rotateCameraM * rollCameraM;
	const Vector2f translatedFlatPoint{ transformationM.Transform(Point2f{ flatPoint.x,flatPoint.y }) };
	Vector3f translatedWorldPoint{ translatedFlatPoint.x,translatedFlatPoint.y, 0};

	translatedWorldPoint += (translatedWorldPoint - GetRawTransform()->GetPosition()).Normalized() * distance;

	return translatedWorldPoint;
}

float Camera::DistanceToFlatScale(float distance) const
{
	// Lock distance to not go further back than the camera
	if (distance < -GlobalSettings::MAIN_GAME_ZOOM)
		distance = -GlobalSettings::MAIN_GAME_ZOOM;

	return std::max(m_Zoom / (distance - GetRawTransform()->GetPosition().z), 0.0f);
}


void Camera::SetOrthoSize(float orthoSize)
{
	m_OrthoSize = orthoSize;
}

std::weak_ptr<Camera> Camera::GetMainCamera()
{
	if(const std::weak_ptr cameraGoWPtr{ GameObject::FindWithTag(Tag::MainCamera) }; !cameraGoWPtr.expired())
		return cameraGoWPtr.lock()->GetComponent<Camera>();

	Log(LogType::Error) << "No Main Camera Found";
	return {};
}