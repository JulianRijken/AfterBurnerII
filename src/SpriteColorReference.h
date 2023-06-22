#pragma once
#include "Behaviour.h"

class SpriteRenderer;

/**
 * \brief Quick simple component that allows the sprite to use a reference color
 */
class SpriteColorReference final: public Behaviour
{

public:

	SpriteColorReference(Color4f& color, std::weak_ptr<SpriteRenderer> spriteRenderer = {});

private:

	void Update() override;
	void Awake() override;

	Color4f& m_Color;
	std::weak_ptr<SpriteRenderer> m_SpriteRenderer;
};

