#pragma once


class GlobalSettings
{
public:

	struct Cell
	{
		int row{};
		int col{};
	};


	inline static bool DrawGizmos{ false };
	inline static float DefaultTimeScale{ 1.0f };
	inline static int StartingStage{ 0 };

#ifdef __EMSCRIPTEN__
	inline static bool AllowPixelate{ false };
#else
	inline static bool AllowPixelate{ true };
#endif

	inline static constexpr bool USE_VSYNC{ true };
	inline static constexpr char WINDOW_TITLE[] = "AfterBurner II - Julian Rijken - 1DAE12E";
	inline static constexpr bool MAIN_CAMERA_PIXELATE{true};
	inline static constexpr bool DEBUG_OBJECT_SYSTEM{false};

	// Genesis version 
	// https://www.youtube.com/watch?v=Bn51XT5FLAo

	// Emulator: https://www.retrogames.cz/play_1517-Genesis.php?language=EN
	// Same resolution as this video of the genesis version


	inline static constexpr int RENDER_WIDTH{ 320 };
	inline static constexpr int RENDER_HEIGHT{ 224 };


#ifdef __EMSCRIPTEN__
	inline static float WINDOW_SCALE{ 1.0f };
#else
	inline static float WINDOW_SCALE{ 4.0f };
#endif

	inline static float WINDOW_POSITION_X{ 1.0f };
	inline static float WINDOW_POSITION_Y{ 1.0f };
	

	inline static constexpr int MAX_ENEMY_SPAWN_DISTANCE{ 160 };
	inline static constexpr int STATIC_ENEMY_SPAWN_DISTANCE{ 140 };
	inline static constexpr int DETAIL_SPAWN_DISTANCE{ 120 };
	inline static constexpr int GROUND_PLANE_DISTANCE{ 150 };

	inline static constexpr float PLAYER_MOVE_SPEED_X{3.0f};
	inline static constexpr float PLAYER_MOVE_SPEED_Y{3.0f};


	inline static constexpr float MAIN_GAME_ZOOM{ 22 };

	inline static constexpr int PIXELS_PER_UNIT{ 32 };
	inline static constexpr float MAIN_GAME_ORTHO_SIZE{ RENDER_HEIGHT / PIXELS_PER_UNIT / 2.0f}; // 7 Units in height // 32 pixels per unit


	inline static float DEBUG_VAL_1{0};
	inline static float DEBUG_VAL_2{0};
	inline static float DEBUG_VAL_3{0};


	static void SetRenderOrthographic(const float orthoSize);
	static void SetRenderScreen();
	static void SetRenderViewport();
};

