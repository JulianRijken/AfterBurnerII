#include "pch.h"
#include "Game.h"

#include "AudioManager.h"
#include "Behaviour.h"
#include "Camera.h"
#include "GameObject.h"
#include "Log.h"
#include "AssetManager.h"
#include "GameTime.h"

#include "DebugManager.h"
#include "GameManager.h"


Game::Game( const Window& window ) :
BaseGame{ window }
{
	Initialize();
}

Game::~Game( )
{
	Cleanup();
}


void Game::Initialize() const
{
	AssetManager::Initialize();
	AudioManager::Initialize();

	const auto gameManagerGoWPtr{ GameObject::AddGameObject("GameManager") };
	gameManagerGoWPtr.lock()->AddComponent<GameManager>();

	const auto debugManagerGoWPtr{ GameObject::AddGameObject("DebugManager") };
	debugManagerGoWPtr.lock()->AddComponent<DebugManager>();
}


void Game::Cleanup( )
{
	AssetManager::Cleanup();
	AudioManager::Cleanup();
	GameObject::DestroyAll();
	Time::CleanTimers();

	Log(TextColor::BrightWhite, BackgroundColor::Blue) << "End Of Cleanup";
}

void Game::Update( float elapsedSec )
{
	Time::SetDeltaTime(elapsedSec);
	Time::AddToFrameCount();
	Time::UpdateTimers();

	Behaviour::UpdateAll();
	Behaviour::LateUpdateAll();

	GameObject::DestroyAllInQueue();
}

void Game::Draw() const
{
	ClearScreen();
	Camera::RenderAll();
}

void Game::ClearScreen() const
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}


void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	IEvents::Invoke(&IEvents::OnKeyDown, e);
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	IEvents::Invoke(&IEvents::OnKeyUp, e);
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	IEvents::Invoke(&IEvents::OnMouseMotion, e);
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
	IEvents::Invoke(&IEvents::OnMouseDown, e);
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	IEvents::Invoke(&IEvents::OnMouseUp, e);
}

