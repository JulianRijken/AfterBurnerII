#include "pch.h"
#include "Renderer.h"



Renderer::Renderer(int renderLayer, const RenderMode& renderMode, bool visible, const std::string& name) :
	Component{name},
	m_Visible{visible},
	m_RenderLayer{ renderLayer },
	RENDER_MODE{ renderMode }
{
	m_RendererPtrs.insert(this);

}

Renderer::~Renderer()
{
	m_RendererPtrs.erase(this);
}

std::vector<Renderer*> Renderer::GetAllRenderers()
{
	 return std::vector<Renderer*>(m_RendererPtrs.begin(), m_RendererPtrs.end());
}

