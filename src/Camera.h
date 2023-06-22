#pragma once
#include <unordered_set>

#include "Component.h"
#include "Vector2f.h"
#include "Vector3f.h"


class Texture;

class Camera final : public Component
{

public:

	Camera(float zoom, float orthoSize, bool pixelate = true, const std::string& name = "CameraComponent");
	~Camera() override;

	void Render() const;

	// World is the actual point in the world based on unit space and distance
	// Flat in unit space but is locked to the camera so no roll pitch etc just unit space
	// Screen is form 0,0 to the render resolution in size
	// Viewport is form 0,0 to 1,1

	// I'm not returning by ref on purpose so I can chain the functions! Maybe look on how to do this in the future
	// Anyone reading this in the future... please tell me :) and thanks for reading the code!
	[[nodiscard]] Vector2f WorldToFlatPoint(const Vector3f& worldPoint) const;
	[[nodiscard]] Vector3f FlatToWorldPoint(const Vector2f& flatPoint, float distance = 0) const;
	[[nodiscard]] float DistanceToFlatScale(float distance) const;


	static void RenderAll();

	void SetOrthoSize(float orthoSize);

	// Quick shortcut to get the main camera form any class
	static std::weak_ptr<Camera> GetMainCamera();

private:

	float m_Zoom;
	float m_OrthoSize;
	bool m_Pixelate;

	Texture* m_RenderTexturePtr{};

	inline static std::unordered_set<Camera*> m_CameraPtrs{};
};

