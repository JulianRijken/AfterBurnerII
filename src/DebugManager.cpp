#include "pch.h"
#include "DebugManager.h"

#include "AudioManager.h"
#include "Coin.h"
#include "GameManager.h"
#include "GlobalSettings.h"
#include "SpriteRenderer.h"
#include "GameObject.h"
#include "Input.h"
#include "PlayerController.h"
#include "TextRenderer.h"
#include "GameTime.h"
#include "Transform.h"
#include "Prefabs.h"


DebugManager::DebugManager() :
	Behaviour("DebugManager"),
	m_FrameTimer{ 0 },
	m_FrameCount{ 0 },
	m_EditingCurve{ false },
	m_ShowCurveSides{ false },
	m_SelectedPoint{ 0 },
	m_CurvePreviewTimer1{ 0.0f },
	m_CurvePreviewTimer2{ 1.0f },
	m_CurvePrintTimer{ 0.0f },
	m_CurveCenter{ 0.0f,0.0f,0.0f },
	m_PreviewCurve
	{
		Vector3f{0.0f,0.0f,-GlobalSettings::MAIN_GAME_ZOOM},
		Vector3f{0.0f,0.0f,-GlobalSettings::MAIN_GAME_ZOOM + (GlobalSettings::MAIN_GAME_ZOOM + GlobalSettings::MAX_ENEMY_SPAWN_DISTANCE) / 3.0f },
		Vector3f{0.0f,0.0f,-GlobalSettings::MAIN_GAME_ZOOM + (GlobalSettings::MAIN_GAME_ZOOM + GlobalSettings::MAX_ENEMY_SPAWN_DISTANCE) / 3.0f * 2.0f },
		Vector3f{0.0f,0.0f, GlobalSettings::MAX_ENEMY_SPAWN_DISTANCE},
	}
{
	// Quick Overwrite
	m_PreviewCurve =
	{
		Vector3f{-10, 2, -GlobalSettings::MAIN_GAME_ZOOM},
		Vector3f{-10, 6, 10},
		Vector3f{ 10, 5, 20},
		Vector3f{-14, 5, 30}
	};

	 //m_PreviewCurve =
	 //{
	 //	Vector3f{ -9.5f, -1.9f, -21.1f },
	 //	Vector3f{ 5.0f, 11.3f, 19.4f },
	 //	Vector3f{ 5.5f, 4.2f, 33.7f },
	 //	Vector3f{ -22.7f, 2.5f, 73.6f }
	 //};



	// m_PreviewCurve =
	//  {
	// 	Vector3f{ 0.0f, -0.1f, 160.0f },
	// 	Vector3f{ 0.0f, -2.0f, 11.0f },
	// 	Vector3f{ 0.0f, -5.9f, 35.0f },
	// 	Vector3f{ 0.0f, -5.1f, -20.0f }
	// };



}

void DebugManager::PrintControls()
{
	std::cout << std::endl;
	Log(TextColor::Black, BackgroundColor::BrightWhite)		<< "- CONTROLS -";
	Log(TextColor::Red, BackgroundColor::BrightWhite)		<< "FIRE     :  space / x";
	Log(TextColor::Red, BackgroundColor::BrightWhite)		<< "MOVE     :  wasd / arrows";
	Log(TextColor::Red, BackgroundColor::BrightWhite)		<< "PAUSE    :  escape / return";
	Log(TextColor::Red, BackgroundColor::BrightWhite)		<< "SELECT   :  space / return";
	Log(TextColor::BrightBlack, BackgroundColor::Black) << "*UI selection (same as move) is done with arrows <->";

	std::cout << std::endl;
	Log(TextColor::Black, BackgroundColor::BrightWhite)		<< "- DEBUG -";
	Log(TextColor::Magenta, BackgroundColor::BrightWhite)	<< "TAB                 :  show fps";
	Log(TextColor::Magenta, BackgroundColor::BrightWhite)	<< "RESTART             :  pause + r ";
	Log(TextColor::Magenta, BackgroundColor::BrightWhite)	<< "PRINT HIERARCHY     :  h";
	Log(TextColor::Magenta, BackgroundColor::BrightWhite)	<< "PRINT COMPONENTS    :  j";
	Log(TextColor::Magenta, BackgroundColor::BrightWhite)	<< "SHOW GIZMOS         :  g";
	Log(TextColor::Magenta, BackgroundColor::BrightWhite)	<< "SHOW UNIT GRID      :  u";
	Log(TextColor::Magenta, BackgroundColor::BrightWhite)	<< "PIXELATE CAMERA     :  p";
	Log(TextColor::Magenta, BackgroundColor::BrightWhite)	<< "INVINCIBILITY       :  o";
	Log(TextColor::Magenta, BackgroundColor::BrightWhite)	<< "DECREASE TIME SCALE :  [";
	Log(TextColor::Magenta, BackgroundColor::BrightWhite)	<< "INCREASE TIME SCALE :  ]";

	std::cout << std::endl;
	Log(TextColor::Black, BackgroundColor::BrightWhite) << "- CURVE EDITOR -";
	Log(TextColor::Blue, BackgroundColor::BrightWhite) << "Left as an exercise for the user :)";

	std::cout << std::endl;
	Log(TextColor::Black, BackgroundColor::BrightWhite) << "- DEBUG -";
	Log(LogType::Warning) << "This is a warning message";
	Log(LogType::Error) <<   "This is a error message";

}

void DebugManager::Awake()
{
	std::cout << std::endl;
	Log(TextColor::Black, BackgroundColor::BrightWhite) << "- PRESS I FOR HELP -";
	std::cout << std::endl;



	m_FramesTextGoWPtr = GameObject::AddGameObject("ScoreText", GetTransform()).lock()->AddComponent<TextRenderer>
		("AfterBurner", "0", Vector2f{ 1.0f,1.0f }, 10000, Renderer::RenderMode::Screen);
	m_FramesTextGoWPtr.lock()->GetRawTransform()->SetPosition(320 - 3, 224 - 3);
	m_FramesTextGoWPtr.lock()->GetRawTransform()->SetScale(0.5f);
	m_FramesTextGoWPtr.lock()->SetColor(1,1,1,0.7f);
	m_FramesTextGoWPtr.lock()->SetVisible(false);


	// Spawn Grid Cells
	for (int x = -GRID_SIZE; x < GRID_SIZE; ++x)
	{
		for (int y = -GRID_SIZE; y < GRID_SIZE; ++y)
		{
			const auto gridCellGOPtr{ GameObject::AddGameObject("GridCell",Tag::None,false)};
			gridCellGOPtr.lock()->AddComponent<SpriteRenderer>("gridCell");
			m_GridCellGoWPtrs.push_back(gridCellGOPtr);
		}
	}


	m_PreviewEnemy1= GameObject::AddGameObject("PreviewEnemy", GetTransform(),Tag::None,false);
	m_PreviewEnemy1.lock()->AddComponent<SpriteRenderer>("EnemyClose_Blue");

	m_PreviewEnemy2 = GameObject::AddGameObject("PreviewEnemy", GetTransform(), Tag::None, false);
	m_PreviewEnemy2.lock()->AddComponent<SpriteRenderer>("Enemy_00");

	// // EXAMPLE CAMERA
	// const auto camera{ GameObject::AddGameObject("Camera", Tag::None,true) };
	// camera.lock()->GetRawTransform()->SetRotation(0, 2, 0);
	// camera.lock()->GetRawTransform()->SetPosition(-10, 6, -10);
	// camera.lock()->AddComponent<Camera>(5.0f, GlobalSettings::MAIN_GAME_ORTHO_SIZE, false);
	// GameObject::FindWithTag(Tag::MainCamera).lock()->SetActive(false);

	// // EXAMPLE CAMERA
	// const auto camera{ GameObject::AddGameObject("Camera", Tag::None,true) };
	// camera.lock()->GetRawTransform()->SetRotation(180.0f, 0, 0);
	// camera.lock()->GetRawTransform()->SetPosition(0, 600, -40);
	// camera.lock()->AddComponent<Camera>(20.0f, 60.0f, false);
	// 
	// GameObject::FindWithTag(Tag::MainCamera).lock()->SetActive(false);
}

void DebugManager::Update()
{
	m_FrameCount++;


	m_FrameTimer += Time::GetUnScaledDeltaTime();
	if(m_FrameTimer >= FRAME_UPDATE_TIME)
	{
		const std::string framesText{std::to_string(int(std::roundf(float(m_FrameCount) / FRAME_UPDATE_TIME))) };

		m_FramesTextGoWPtr.lock()->SetText(framesText);
		m_FrameTimer -= FRAME_UPDATE_TIME;
		m_FrameCount = 0;
	}

	if (m_ShowUnitGrid)
	{
		const auto cameraTransform{ GameObject::FindWithTag(Tag::MainCamera).lock()->GetTransform() };
		for (int x = -GRID_SIZE; x < GRID_SIZE; ++x)
		{
			for (int y = -GRID_SIZE; y < GRID_SIZE; ++y)
			{
				const int index{ (x + GRID_SIZE) + (GRID_SIZE * 2) * (y + GRID_SIZE) };

				const int offsetX = x + int(std::roundf(cameraTransform.lock()->GetPosition().x));
				const int offsetY = y + int(std::roundf(cameraTransform.lock()->GetPosition().y));

				m_GridCellGoWPtrs[index].lock()->GetRawTransform()->SetPosition(float(offsetX), float(offsetY));
			}
		}
	}


	if(m_EditingCurve)
		HandleCurveEditing();

}


void DebugManager::OnDrawGizmos()
{
	// Draw Game Plane
	const Vector3f P0{ -15, 0, 0};
	const Vector3f P1{  15, 0, 0};
	const Vector3f P2{  15, 0, GlobalSettings::MAX_ENEMY_SPAWN_DISTANCE };
	const Vector3f P3{ -15, 0, GlobalSettings::MAX_ENEMY_SPAWN_DISTANCE };

	GizmosSetColor(0, 1, 0, 1);
	GizmosDrawCube(Vector3f::One() * 0.4f, P0);
	GizmosDrawCube(Vector3f::One() * 0.4f, P1);
	GizmosDrawCube(Vector3f::One() * 0.4f, P2);
	GizmosDrawCube(Vector3f::One() * 0.4f, P3);

	GizmosSetColor(0,1,0,0.5f);
	GizmosDrawLine(P0, P1);
	GizmosDrawLine(P1, P2);
	GizmosDrawLine(P2, P3);
	GizmosDrawLine(P3, P0);


	if (m_EditingCurve)
	{
		if (m_ShowCurveSides)
		{
			for (int i = 0; i < 4; ++i)
			{
				Jul::CubicCurve showCurve{ m_PreviewCurve };

				if (i == 1 || i == 2)
					showCurve = Jul::ScaleCurve(showCurve, m_CurveCenter, Vector2f{ -1.0f,1.0f });

				if (i == 2 || i == 3)
					showCurve = Jul::ScaleCurve(showCurve, m_CurveCenter, Vector2f{ 1.0f,-1.0f });

				if (i == 0)
					GizmosDrawCubicBezier(showCurve, 0.5f);
				else
					GizmosDrawCubicBezier(showCurve, 0.0f);
			}
		}
		else
		{
			GizmosDrawCubicBezier(m_PreviewCurve, 0.5f);
		}

		GizmosDrawCubicBezier2D(m_PreviewCurve, 0.1f,100,2,m_SelectedPoint);

	}

}

void DebugManager::OnKeyDown(const SDL_KeyboardEvent& e)
{

	if(Input::Compare("info",e))
		PrintControls();
	


	if (Input::Compare("debugHierarchy", e))
	{
		std::cout << std::endl;
		GameObject::PrintHierarchy(false);
	}

	if (Input::Compare("debugHierarchyWithComponents", e))
	{
		std::cout << std::endl;
		GameObject::PrintHierarchy(true);
	}

	if (Input::Compare("fps", e))
	{
		m_FramesTextGoWPtr.lock()->SetVisible(!m_FramesTextGoWPtr.lock()->IsVisible());
	}

	if (Input::Compare("unitGrid", e))
	{
		m_ShowUnitGrid = !m_ShowUnitGrid;

		for (const auto & gridCellGoWPtr : m_GridCellGoWPtrs)
			gridCellGoWPtr.lock()->SetActive(m_ShowUnitGrid);
	}

	if (Input::Compare("toggleGizmos", e))
	{
		GlobalSettings::DrawGizmos = !GlobalSettings::DrawGizmos;
	}

#ifndef __EMSCRIPTEN__
	if (Input::Compare("pixelateCamera", e))
	{
		GlobalSettings::AllowPixelate = !GlobalSettings::AllowPixelate;
	}
#endif

	if (Input::Compare("toggleCurveEditor", e) && GameManager::Instance.lock()->IsGamePaused())
	{
		EditCurve(!m_EditingCurve);
	}

	if (Input::Compare("toggleCurveSides", e) && GameManager::Instance.lock()->IsGamePaused() && m_EditingCurve)
	{
		m_ShowCurveSides = !m_ShowCurveSides;
	}

	if (Input::Compare("increaseTimeScale", e))
	{
		if (GlobalSettings::DefaultTimeScale < 1.0f)
			GlobalSettings::DefaultTimeScale += 0.1f;
		else
			GlobalSettings::DefaultTimeScale += 1.0f;


		Log() << "Time Scale: " << GlobalSettings::DefaultTimeScale;
	}

	if (Input::Compare("decreaseTimeScale", e))
	{
		if (GlobalSettings::DefaultTimeScale <= 1.0f)
			GlobalSettings::DefaultTimeScale -= 0.1f;
		else
			GlobalSettings::DefaultTimeScale -= 1.0f;

		GlobalSettings::DefaultTimeScale = std::max(0.1f, GlobalSettings::DefaultTimeScale);
		Log() << "Time Scale: " << GlobalSettings::DefaultTimeScale;
	}

	if (Input::Compare("toggleInvincible", e))
	{
		const auto playerGoWPtr{ GameObject::FindWithTag(Tag::Player) };
		if (!playerGoWPtr.expired())
		{

			const auto& playerController{ playerGoWPtr.lock()->GetComponent<PlayerController>() };

			const bool invincible{ !playerController.lock()->IsInvincible() };

			playerController.lock()->SetInvincible(invincible);

			if(invincible)
			{
				Prefabs::SpawnCoin(playerGoWPtr.lock()->GetRawTransform()->GetPosition() + Vector3f::Forward() * 0.1f);
			}
			else
			{
				AudioManager::PlaySoundEffect("CoinBack", 0.3f);
			}
		}
	}

	if (Input::Compare("flipCurve", e) && GameManager::Instance.lock()->IsGamePaused() && m_EditingCurve)
	{
		const Jul::CubicCurve newCurve
		{
			m_PreviewCurve.P3,
			m_PreviewCurve.P2,
			m_PreviewCurve.P1,
			m_PreviewCurve.P0,
		};

		m_PreviewCurve = newCurve;
	}

	if (m_EditingCurve)
	{
		if (Input::Compare("curveEditorP0", e))
			m_SelectedPoint = 0;
		if (Input::Compare("curveEditorP1", e))
			m_SelectedPoint = 1;
		if (Input::Compare("curveEditorP2", e))
			m_SelectedPoint = 2;
		if (Input::Compare("curveEditorP3", e))
			m_SelectedPoint = 3;
	}
}


void DebugManager::OnGamePaused(bool paused)
{
	if (!paused)
		EditCurve(false);
}


void DebugManager::EditCurve(bool edit)
{
	m_EditingCurve = edit;

	m_PreviewEnemy1.lock()->SetActive(m_EditingCurve);
	m_PreviewEnemy2.lock()->SetActive(m_EditingCurve);

	if (m_EditingCurve)
		GlobalSettings::DrawGizmos = true;
}

void DebugManager::HandleCurveEditing()
{
	const Vector2f planeMovement{ Input::GetDirection("curveEditorPlaneMovement") };
	const float verticalMovement{ Input::GetAxis("curveEditorVerticalMovement") };

	Vector3f addedMovement{ (Vector3f{ planeMovement.x * 20.0f,verticalMovement * 20.0f, planeMovement.y * 40.0f } *Time::GetUnScaledDeltaTime()) };

	if (Input::GetKey("moveCurveCenter"))
	{
		addedMovement.x = 0.0f;
		addedMovement.z = 0.0f;

		m_CurveCenter += addedMovement;

		if (Input::GetKey("moveWholeCurve"))
		{
			m_PreviewCurve.P0 = m_PreviewCurve.P0 + addedMovement;
			m_PreviewCurve.P1 = m_PreviewCurve.P1 + addedMovement;
			m_PreviewCurve.P2 = m_PreviewCurve.P2 + addedMovement;
			m_PreviewCurve.P3 = m_PreviewCurve.P3 + addedMovement;
		}
	}
	else
	{
		if (Input::GetKey("moveWholeCurve"))
		{
			m_PreviewCurve.P0 = m_PreviewCurve.P0 + addedMovement;
			m_PreviewCurve.P1 = m_PreviewCurve.P1 + addedMovement;
			m_PreviewCurve.P2 = m_PreviewCurve.P2 + addedMovement;
			m_PreviewCurve.P3 = m_PreviewCurve.P3 + addedMovement;
		}
		else
		{
			switch (m_SelectedPoint)
			{
				case 0: m_PreviewCurve.P0 = m_PreviewCurve.P0 + addedMovement; break;
				case 1: m_PreviewCurve.P1 = m_PreviewCurve.P1 + addedMovement; break;
				case 2: m_PreviewCurve.P2 = m_PreviewCurve.P2 + addedMovement; break;
				case 3: m_PreviewCurve.P3 = m_PreviewCurve.P3 + addedMovement; break;
			}
		}
	}

	m_CurvePreviewTimer1 += Time::GetUnScaledDeltaTime() * 0.25f;
	if (m_CurvePreviewTimer1 >= 1.0f)
		m_CurvePreviewTimer1 -= 1.0f;

	m_CurvePreviewTimer2 -= Time::GetUnScaledDeltaTime() * 0.15f;
	if (m_CurvePreviewTimer2 <= 0.0f)
		m_CurvePreviewTimer2 += 1.0f;

	const Vector3f curvePos1{ Jul::EvaluateCubicBezier(m_PreviewCurve,m_CurvePreviewTimer1) };
	m_PreviewEnemy1.lock()->GetRawTransform()->SetPosition(curvePos1);

	const Vector3f curvePos2{ Jul::EvaluateCubicBezier(m_PreviewCurve,m_CurvePreviewTimer2) };
	m_PreviewEnemy2.lock()->GetRawTransform()->SetPosition(curvePos2);


#ifndef __EMSCRIPTEN__

	m_CurvePrintTimer += Time::GetUnScaledDeltaTime();

	if (m_CurvePrintTimer >= CURVE_PRINT_INTERVAL)
	{
		m_CurvePrintTimer -= CURVE_PRINT_INTERVAL;

		Log() << "Editing Curve";


		const std::string formattedText = std::format
		(
			"Jul::CubicCurve curve\n"
			"{{\n"
			"\tVector3f{{ {:.1f}f, {:.1f}f, {:.1f}f }},\n"
			"\tVector3f{{ {:.1f}f, {:.1f}f, {:.1f}f }},\n"
			"\tVector3f{{ {:.1f}f, {:.1f}f, {:.1f}f }},\n"
			"\tVector3f{{ {:.1f}f, {:.1f}f, {:.1f}f }}\n"
			"}};\n",
			m_PreviewCurve.P0.x, m_PreviewCurve.P0.y, m_PreviewCurve.P0.z,
			m_PreviewCurve.P1.x, m_PreviewCurve.P1.y, m_PreviewCurve.P1.z,
			m_PreviewCurve.P2.x, m_PreviewCurve.P2.y, m_PreviewCurve.P2.z,
			m_PreviewCurve.P3.x, m_PreviewCurve.P3.y, m_PreviewCurve.P3.z
		);

		std::cout << formattedText << std::endl;
		std::cout << "Curve Center: " << m_CurveCenter << std::endl;
	}

#endif

}
