#include "pch.h"
#include "SpriteColorReference.h"

#include <utility>

#include "GameObject.h"
#include "SpriteRenderer.h"

SpriteColorReference::SpriteColorReference(Color4f& color, std::weak_ptr<SpriteRenderer> spriteRenderer) :
	m_Color{ color },
	m_SpriteRenderer{std::move(spriteRenderer)}
{}

void SpriteColorReference::Update()
{
	if(not m_SpriteRenderer.expired())
		m_SpriteRenderer.lock()->SetColor(m_Color);
}

void SpriteColorReference::Awake()
{
	if(m_SpriteRenderer.expired())
		m_SpriteRenderer = GetRawGameObject()->GetComponent<SpriteRenderer>();

	SpriteColorReference::Update();
}
