#pragma once
#include "BaseGame.h"

class Game final : public BaseGame
{
public:

	explicit Game( const Window& window );

	Game( Game&& other) = delete;
	Game(const Game& other) = delete;
	Game& operator=(Game&& other) = delete;
	Game& operator=(const Game& other) = delete;

	~Game() override;

	void Update( float elapsedSec ) override;
	void Draw( ) const override;

	// IEvent handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e ) override;
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e ) override;
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e ) override;
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e ) override;

private:

	// FUNCTIONS
	void Initialize() const;
	static void Cleanup( );
	void ClearScreen( ) const;
};