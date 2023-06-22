#pragma once
#include <unordered_set>
#include <vector>

#include "Component.h"

class Camera;

class Renderer : public Component
{
	friend Camera;

public:

	enum class RenderMode
	{
		World,
		Screen,
		Viewport
	};

	Renderer(int renderLayer, const RenderMode& renderMode, bool visible = true, const std::string& name = "Base Renderer");
	~Renderer() override;

	Renderer(Renderer&&) = delete;
	Renderer(const Renderer&) = delete;
	Renderer& operator=(Renderer&&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	[[nodiscard]] int GetRenderLayer() const { return m_RenderLayer; }
	void SetRenderLayer(int renderLayer) { m_RenderLayer = renderLayer; }

	void SetVisible(bool visible) { m_Visible = visible; }
	[[nodiscard]] bool IsVisible() const { return m_Visible; }

protected:

	// World space using orthographic size
	virtual void DrawWorldSpace(const Camera& camera) const {}

	// Screen space 0 to render size
	virtual void DrawScreenSpace(const Camera& camera) const{}

	// Viewport space 0 to 1
	virtual void DrawViewportSpace(const Camera& camera) const{}


private:

	static std::vector<Renderer*> GetAllRenderers();

	bool m_Visible;
	int m_RenderLayer{};
	const RenderMode RENDER_MODE;

	inline static std::unordered_set<Renderer*> m_RendererPtrs{};

};

