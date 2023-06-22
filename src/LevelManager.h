#pragma once
#include "AudioManager.h"
#include "Behaviour.h"
#include "GameObject.h"
#include "IEvents.h"
#include "Jul.h"
#include "Vector2f.h"
#include "Vector3f.h"


enum class Tag;
class GameManager;
class GameObject;
class AudioManager;
class PlayerController;
class SpriteRenderer;


class LevelManager final : public Behaviour, IEvents
{
public:

	enum class LevelState
	{
		OnDeck,
		InGame,
		Landing
	};

	enum class EnemyType
	{
		None,

		CloseCurveSideRight,
		CloseCurveSideLeft,
		CloseCurvePastRight,
		CloseCurvePastLeft,
		CloseCurvePastBarrleRight,
		CloseCurvePastBarrleLeft,

		GrayFarStraight,
		GrayFarStraightDive,

		RedFarStraight,
		RedFarStraightDive,

		BlueFarStraight,
		BlueFarStraightDive,

		Pillar,

		Tents,
		Truck,
		Tower,
		Hangar,
	};

	enum class EnemyClass
	{
		StaticEnemy,
		PathEnemy
	};

	struct LevelObject
	{
		std::weak_ptr<GameObject> gameObjectWPtr{};
		float effectMultiplier{ 1.0f };
		bool destroyBehindCamera{};
	};

	// The spawn delay, offset and enemy type
	struct EnemySpawnSettings
	{
		float delay{ 0.0f };
		EnemyType enemyType{};
		Vector2f offset{};
	};



	struct EnemySpawnConfig
	{
		std::string spriteName{ "EnemyClose_Blue" };
		Vector3f colliderSize{ 2.2f,2.2f,0.5f };
		Vector3f colliderCenter{ 0.0f,0.0f,0.0f };
		Tag tag{ Tag::TargetEnemy };

		Vector3f scale{ 1.0f,1.0f,1.0f };

		bool canBeDestroyed{ true };

		EnemyClass enemyClass{ EnemyClass::PathEnemy };

		// Dynamic enemy
		float startDelay{ 0.0f };
		float moveDuration{ 3.5f };
		Vector3f curveCenter{ Vector3f{0,0,0} };
		Vector2f curveScale{ 0.0f,0.0f };
		Jul::CubicCurve curve{};
		float rocketDelay{ 0.0f };
		float animationDelay{ 0.0f };
		float animationDuration{ 0.0f };
		std::string animationName{ "" };
	};

	struct SpawnSet
	{
		float delayBefore{ 0.0f };
		std::vector<EnemySpawnSettings> spawnSettingsVector{};
		float delayAfter{ 0.0f };
	};

	struct Stage
	{
		Color4f skyColor{};
		Color4f groundPlaneColor{};
		Color4f groundDetailColor{};

		Color4f bushColor{};
		bool spawnBush{};

		Color4f diskColor{};
		bool spawnDisk{};

		Color4f rockColor{};
		bool spawnRock{};

		bool refuel{};
		bool bonusStage{};


		std::vector<SpawnSet> spawnSets{};
	};


	LevelManager(const std::string& name = "LevelManager");


	static void AddToWorld(const std::weak_ptr<GameObject>& gameObject, float effectMultiplier = 1.0f, bool destroyBehindCamera = true);

	inline static const int STAGE_COUNT{ 8 };

private:

	void Awake() override;
	void Update() override;


	void SpawnGroundDetails();
	void HandleLevelSpawning();
	void MoveAllLevelObjects();


	void SwitchLevelState(LevelState nextState);

	void OnPlaneAirborne() override;
	void OnPlaneLanding() override;
	void OnStageFinishedRunning();

	void SpawnEnemy(EnemyType enemyType, Vector2f offset) const;

	void LoadStage(int nextStageIndex);
	void RunActiveStage();


	const float COLOR_CHANGE_TIME{ 3.0f };
	const float SPAWN_WITH{ 15.0f };
	const float BONUS_SPAWN_WITH{ 25.0f };
	const float SPAWN_INTERVAL{ 0.05f };
	const float SIDEWAYS_FLY_SPEED{ 15.0f };

	const float THE_ENEMY_SCREAM_CHANCE{ 0.07f };

	float m_SpawnTimer;
	float m_DistanceTravelled;

	// If the level is waiting ro the first delay or the second delay
	bool m_WaitingForFirstDelay{};


	LevelState m_ActiveLevelState;


	int m_ActiveSetIndex;
	int m_ActiveStageIndex;
	Stage m_ActiveStage;

	Color4f m_SkyColor;
	Color4f m_GroundPlaneColor;
	Color4f m_GroundDetailColor;

	Color4f m_BushColor;
	Color4f m_RockColor;
	Color4f m_DiskColor;


	std::weak_ptr<GameManager> m_GameManager{};

	std::weak_ptr<SpriteRenderer> m_GroundPlaneSpriteWPtr{};
	std::weak_ptr<SpriteRenderer> m_GroundDetailSpriteWPtr{};
	std::weak_ptr<SpriteRenderer> m_SkyPlaneWPtr{};
	std::weak_ptr<GameObject> m_ShipDeckGoWPtr{};


	inline static std::vector<LevelObject> m_LevelObjectWPtrs{};

	inline static AudioManager::MusicOptions STAGE_MUSIC[STAGE_COUNT]
	{
		AudioManager::MusicOptions::FinalTakeOff,
		AudioManager::MusicOptions::FinalTakeOff,
		AudioManager::MusicOptions::FinalTakeOff,
		AudioManager::MusicOptions::AfterBurner,
		AudioManager::MusicOptions::AfterBurner,
		AudioManager::MusicOptions::RedOut,
		AudioManager::MusicOptions::SuperStripe,
		AudioManager::MusicOptions::FinalTakeOff,
	};


	inline static const Stage STAGES[STAGE_COUNT]
	{
		Stage // 1 Water
		{
			Color4f{0.22745f, 0.63922f, 0.95294f,1},
			Color4f{ 0.00000f, 0.04314f, 0.75686f,1 },
			Color4f{0.00000f, 0.23137f, 0.80392f,1},

			Color4f{ 0.05882f, 0.70980f, 0.40392f,1},
			false,

			Color4f{1,1,1,1},
			true,

			Color4f{1,1,1,1},
			false,

			false,

			false,

			{
				SpawnSet // 1
				{
					2.0f,
					{
						{0.0f,EnemyType::CloseCurvePastRight,Vector2f{}},
						{0.5f,EnemyType::CloseCurvePastRight,Vector2f{}},
						{1.0f,EnemyType::CloseCurvePastLeft ,Vector2f{}},
						{1.5f,EnemyType::CloseCurvePastRight,Vector2f{}},
						{2.0f,EnemyType::CloseCurvePastRight,Vector2f{}},
					},
					5.0f
				},
				SpawnSet // 2
				{
					0.0f,
					{
						{0.0f,EnemyType::CloseCurveSideLeft ,Vector2f{}},
						{0.5f,EnemyType::CloseCurvePastRight,Vector2f{}},
						{1.0f,EnemyType::CloseCurvePastLeft ,Vector2f{}},
						{1.5f,EnemyType::CloseCurvePastRight,Vector2f{}},
					},
					2.0f
				},
				SpawnSet
				{
					0.0f,
					{
						{0.0f,EnemyType::GrayFarStraight,Vector2f{-7, 6.5f}},
					},
					2.5f
				},
				SpawnSet
				{
					0.0f,
					{
						{0.0f,EnemyType::GrayFarStraight,Vector2f{7, 6.5f}},
					},
					2.5f
				},
				SpawnSet
				{
					0.0f,
					{
						{0.0f,EnemyType::GrayFarStraight,Vector2f{-10, 6.5f}},
						{0.1f,EnemyType::GrayFarStraight,Vector2f{ -6, 6.5f}},
						{0.2f,EnemyType::GrayFarStraight,Vector2f{ -2, 6.5f}},
						{0.3f,EnemyType::GrayFarStraight,Vector2f{  2, 6.5f}},
						{0.4f,EnemyType::GrayFarStraight,Vector2f{  6, 6.5f}},
						{0.5f,EnemyType::GrayFarStraight,Vector2f{ 10, 6.5f}},
					},
					7.0f
				},
				SpawnSet
				{
					2.0f,
					{
						{0.0f,EnemyType::CloseCurvePastBarrleLeft	,Vector2f{}},
						{0.5f,EnemyType::CloseCurvePastBarrleRight	,Vector2f{}},
						{1.0f,EnemyType::CloseCurvePastBarrleLeft	,Vector2f{}},
						{1.5f,EnemyType::CloseCurveSideLeft			,Vector2f{}},
						{1.5f,EnemyType::CloseCurveSideRight		,Vector2f{}},
						{2.0f,EnemyType::CloseCurvePastBarrleRight  ,Vector2f{}},
						{2.5f,EnemyType::CloseCurvePastBarrleLeft	,Vector2f{}},
						{3.0f,EnemyType::CloseCurvePastBarrleRight	,Vector2f{}},
						{4.0f,EnemyType::CloseCurvePastRight		,Vector2f{}},
						{5.0f,EnemyType::CloseCurvePastLeft			,Vector2f{}},
					},
					8.0f
				},
			}

		},
		Stage // 2 Desert
		{
			Color4f{0.96471f, 0.65490f, 0.00000f, 1},
			Color4f{0.94902f, 0.78431f, 0.39608f, 1},
			Color4f{0.90588f, 0.68627f, 0.34118f  , 1.0f},

			Color4f{0.97255f, 0.64314f, 0.06275f,1},
			true,

			Color4f{1,1,1,1},
			false,

			Color4f{1,1,1,1},
			false,

			false,

			false,

			{
				SpawnSet
				{
					0.0f,
					{
						{0.0f,EnemyType::CloseCurvePastRight      ,Vector2f{}},
						{0.5f,EnemyType::CloseCurvePastRight      ,Vector2f{}},
						{1.0f,EnemyType::CloseCurvePastLeft       ,Vector2f{}},
						{1.5f,EnemyType::CloseCurvePastRight      ,Vector2f{}},
						{2.0f,EnemyType::CloseCurvePastRight      ,Vector2f{}},
						{3.0f,EnemyType::CloseCurvePastBarrleLeft ,Vector2f{}},
						{4.0f,EnemyType::CloseCurvePastBarrleRight,Vector2f{}},
					},
					5.0f
				},
				SpawnSet
				{
					2.0f,
					{
						{0.5f,EnemyType::GrayFarStraight	, Vector2f{-6.0f, 5.0f} },
						{0.7f,EnemyType::GrayFarStraight	, Vector2f{6.0f, 5.0f} },
						{0.9f,EnemyType::GrayFarStraight	, Vector2f{ 6.0f, 10.0f} },
						{1.1f,EnemyType::GrayFarStraight	, Vector2f{ -6.0f, 10.0f} },


						{0.0f,EnemyType::GrayFarStraightDive, Vector2f{-3.5f, 7.5f} },
						{0.4f,EnemyType::GrayFarStraightDive, Vector2f{ 0.0f, 7.5f} },
						{0.8f,EnemyType::GrayFarStraightDive, Vector2f{ 3.5f, 7.5f} },
					},
					5.0f
				},
				SpawnSet
				{
					0.0f,
					{
						{0.0f,EnemyType::GrayFarStraightDive, Vector2f{ -16.0f, 8.0f} },
						{0.2f,EnemyType::GrayFarStraightDive, Vector2f{ -12.0f, 8.0f} },
						{0.0f,EnemyType::GrayFarStraightDive, Vector2f{ -8.0f,  8.0f} },
					},
					3.0f
				},
				SpawnSet
				{
					0.0f,
					{
						{0.0f,EnemyType::GrayFarStraightDive, Vector2f{ 16.0f, 13.0f} },
						{0.2f,EnemyType::GrayFarStraightDive, Vector2f{ 12.0f, 13.0f} },
						{0.0f,EnemyType::GrayFarStraightDive, Vector2f{ 8.0f,  13.0f} },

						{1.5f,EnemyType::GrayFarStraightDive, Vector2f{ 6.0f,  13.0f} },
						{1.5f,EnemyType::GrayFarStraightDive, Vector2f{ 8.0f,  13.0f} },
					},
					3.0f
				},
				SpawnSet
				{
					0.0f,
					{
						{0.0f,EnemyType::GrayFarStraight, Vector2f{ -9, 5.0f} },
						{0.0f,EnemyType::GrayFarStraight, Vector2f{ -3, 6.0f} },
						{0.0f,EnemyType::GrayFarStraight, Vector2f{  3, 8.0f} },
						{0.0f,EnemyType::GrayFarStraight, Vector2f{  9, 10.0f} },
					},
					3.0f
				},
				SpawnSet
				{
					2.0f,
					{
						{0.5f,EnemyType::GrayFarStraight	, Vector2f{-6.0f, 5.0f} },
						{0.7f,EnemyType::GrayFarStraight	, Vector2f{6.0f, 5.0f} },
						{0.9f,EnemyType::GrayFarStraight	, Vector2f{ 6.0f, 10.0f} },
						{1.1f,EnemyType::GrayFarStraight	, Vector2f{ -6.0f, 10.0f} },


						{0.0f,EnemyType::GrayFarStraightDive, Vector2f{-3.5f, 7.5f} },
						{0.4f,EnemyType::GrayFarStraightDive, Vector2f{ 0.0f, 7.5f} },
						{0.8f,EnemyType::GrayFarStraightDive, Vector2f{ 3.5f, 7.5f} },
					},
					5.0f
				},
				SpawnSet
				{
					0.0f,
					{
						{0.0f,EnemyType::GrayFarStraightDive, Vector2f{ -16.0f, 8.0f} },
						{0.0f,EnemyType::GrayFarStraightDive, Vector2f{ -12.0f, 11.0f} },
						{0.0f,EnemyType::GrayFarStraightDive, Vector2f{ -8.0f,  8.0f} },
					},
					3.0f
				},
				SpawnSet
				{
					0.0f,
					{
						{0.0f,EnemyType::GrayFarStraightDive, Vector2f{ 16.0f, 8.0f} },
						{0.0f,EnemyType::GrayFarStraightDive, Vector2f{ 12.0f, 11.0f} },
						{0.0f,EnemyType::GrayFarStraightDive, Vector2f{ 8.0f,  8.0f} },
					},
					3.0f
				},

			}
		},
		Stage // 3 Dark
		{
			Color4f{0.0f, 0.0f, 0.0f, 1.0f},
			Color4f{0.00392157f, 0.01568627f, 0.2627451f, 1.0f},
			Color4f{0.02745098f, 0.04901961f, 0.55294118f, 1.0f},

			Color4f{0.06667f, 0.08627f, 0.95294f ,1},
			true,

			Color4f{1,1,1,1},
			false,

			Color4f{1,1,1,1},
			false,

			true,

			false,

			{
				SpawnSet
				{
					5.0f,
					{
						{0.0f,EnemyType::BlueFarStraight,Vector2f{-3.5f * 3.0f, (0.0f * 1.6f) + 3.5f}},
						{0.1f,EnemyType::BlueFarStraight,Vector2f{-3.0f * 3.0f, (1.3f * 1.6f) + 3.5f}},
						{0.2f,EnemyType::BlueFarStraight,Vector2f{-1.5f * 3.0f, (2.0f * 1.6f) + 3.5f}},
						{0.3f,EnemyType::BlueFarStraight,Vector2f{-0.4f * 3.0f, (2.2f * 1.6f) + 3.5f}},
						{0.4f,EnemyType::BlueFarStraight,Vector2f{ 0.6f * 3.0f, (2.2f * 1.6f) + 3.5f}},
						{0.5f,EnemyType::BlueFarStraight,Vector2f{ 1.5f * 3.0f, (2.0f * 1.6f) + 3.5f}},
						{0.6f,EnemyType::BlueFarStraight,Vector2f{ 3.0f * 3.0f, (1.3f * 1.6f) + 3.5f}},
						{0.7f,EnemyType::BlueFarStraight,Vector2f{ 3.5f * 3.0f, (0.0f * 1.6f) + 3.5f}},
					},
					7.0f
				},
				SpawnSet
				{
					0.0f,
					{
						{0.0f,EnemyType::RedFarStraight,Vector2f{-1.0f * 7.0f, (0.0f * 0.5f) + 6.5f}},
						{0.1f,EnemyType::RedFarStraight,Vector2f{ 0.0f * 7.0f, (1.0f * 0.5f) + 6.5f}},
						{0.2f,EnemyType::RedFarStraight,Vector2f{ 1.0f * 7.0f, (2.0f * 0.5f) + 6.5f}},
					},
					5.0f
				},
				SpawnSet
				{
					0.0f,
					{
						{0.0f,EnemyType::RedFarStraight,Vector2f{ 1.0f * 7.0f, (0.0f * 0.5f) + 6.5f}},
						{0.1f,EnemyType::RedFarStraight,Vector2f{ 0.0f * 7.0f, (1.0f * 0.5f) + 6.5f}},
						{0.2f,EnemyType::RedFarStraight,Vector2f{-1.0f * 7.0f, (2.0f * 0.5f) + 6.5f}},
					},
					7.0f
				},
				SpawnSet
				{
					0.0f,
					{
						{0.0f,EnemyType::RedFarStraight,Vector2f{ 3.0f * 7.0f, (0.0f * 1.0f) + 6.5f}},
						{0.1f,EnemyType::BlueFarStraight,Vector2f{ 2.0f * 7.0f, (1.0f * 1.0f) + 6.5f}},
						{0.2f,EnemyType::RedFarStraight,Vector2f{ 1.0f * 7.0f, (0.0f * 1.0f) + 6.5f}},
						{0.0f,EnemyType::BlueFarStraight,Vector2f{ 0.0f * 7.0f, (1.0f * 1.0f) + 6.5f}},
						{0.1f,EnemyType::RedFarStraight,Vector2f{-1.0f * 7.0f, (0.0f * 1.0f) + 6.5f}},
						{0.2f,EnemyType::BlueFarStraight,Vector2f{-2.0f * 7.0f, (1.0f * 1.0f) + 6.5f}},
						{0.2f,EnemyType::RedFarStraight,Vector2f{-3.0f * 7.0f, (0.0f * 1.0f) + 6.5f}},
					},
					7.0f
				},
			}

		},
		Stage // 4 Green
		{
			Color4f{0.0f, 0.73f, 0.81f, 1.0f},
			Color4f{0.0f, 0.46f,  0.0f, 1.0f},
			Color4f{0.0f, 0.58f, 0.05f, 1.0f},

			Color4f{0.53f, 0.76f, 0.33f, 1.0f},
			true,

			Color4f{1,1,1,1},
			true,

			Color4f{1,1,1,1},
			false,

			false,

			false,
			{
				SpawnSet
				{
					0.0f,
					{
						{0.0f,EnemyType::GrayFarStraight,Vector2f{ 1.0f * 7.0f, (0.0f * 0.5f) + 6.5f}},
						{0.1f,EnemyType::GrayFarStraight,Vector2f{ 0.0f * 7.0f, (1.0f * 0.5f) + 6.5f}},
						{0.2f,EnemyType::GrayFarStraight,Vector2f{-1.0f * 7.0f, (2.0f * 0.5f) + 6.5f}},
					},
					6.0f
				},
				SpawnSet
				{
					5.0f,
					{
						{0.0f,EnemyType::BlueFarStraight,Vector2f{-3.5f * 3.0f, (0.0f * 1.6f) + 3.5f}},
						{0.1f,EnemyType::BlueFarStraight,Vector2f{-3.0f * 3.0f, (1.3f * 1.6f) + 3.5f}},
						{0.2f,EnemyType::BlueFarStraight,Vector2f{-1.5f * 3.0f, (2.0f * 1.6f) + 3.5f}},
						{0.3f,EnemyType::BlueFarStraight,Vector2f{-0.4f * 3.0f, (2.2f * 1.6f) + 3.5f}},
						{0.4f,EnemyType::BlueFarStraight,Vector2f{ 0.6f * 3.0f, (2.2f * 1.6f) + 3.5f}},
						{0.5f,EnemyType::BlueFarStraight,Vector2f{ 1.5f * 3.0f, (2.0f * 1.6f) + 3.5f}},
						{0.6f,EnemyType::BlueFarStraight,Vector2f{ 3.0f * 3.0f, (1.3f * 1.6f) + 3.5f}},
						{0.7f,EnemyType::BlueFarStraight,Vector2f{ 3.5f * 3.0f, (0.0f * 1.6f) + 3.5f}},
					},
					7.0f
				},
			}
		},
		Stage // 5 Snow
		{
			Color4f(0.0f, 0.16f, 0.97f, 1.0f),
			Color4f{0.93f, 0.93f, 0.93f, 1.0f},
			Color4f{0.93f, 0.93f, 0.93f, 1.0f},

			Color4f{1,1,1,1},
			false,

			Color4f{0.93f, 0.93f, 0.93f, 1.0f},
			true,

			Color4f{1,1,1,1},
			false,

			false,

			false,
			{
				SpawnSet
				{
					0.0f,
					{
						{0.0f,EnemyType::GrayFarStraight,Vector2f{ 1.0f * 7.0f, (0.0f * 0.5f) + 6.5f}},
						{0.1f,EnemyType::GrayFarStraight,Vector2f{ 0.0f * 7.0f, (1.0f * 0.5f) + 6.5f}},
						{0.2f,EnemyType::GrayFarStraight,Vector2f{-1.0f * 7.0f, (2.0f * 0.5f) + 6.5f}},
					},
					6.0f
				},
				SpawnSet
				{
					5.0f,
					{
						{0.0f,EnemyType::BlueFarStraight,Vector2f{-3.5f * 3.0f, (0.0f * 1.6f) + 3.5f}},
						{0.1f,EnemyType::BlueFarStraight,Vector2f{-3.0f * 3.0f, (1.3f * 1.6f) + 3.5f}},
						{0.2f,EnemyType::BlueFarStraight,Vector2f{-1.5f * 3.0f, (2.0f * 1.6f) + 3.5f}},
						{0.3f,EnemyType::BlueFarStraight,Vector2f{-0.4f * 3.0f, (2.2f * 1.6f) + 3.5f}},
						{0.4f,EnemyType::BlueFarStraight,Vector2f{ 0.6f * 3.0f, (2.2f * 1.6f) + 3.5f}},
						{0.5f,EnemyType::BlueFarStraight,Vector2f{ 1.5f * 3.0f, (2.0f * 1.6f) + 3.5f}},
						{0.6f,EnemyType::BlueFarStraight,Vector2f{ 3.0f * 3.0f, (1.3f * 1.6f) + 3.5f}},
						{0.7f,EnemyType::BlueFarStraight,Vector2f{ 3.5f * 3.0f, (0.0f * 1.6f) + 3.5f}},
					},
					7.0f
				},
			}
		},
		Stage // 8 Bonus Stage
		{
			Color4f{0.93f - 0.07f, 0.91f - 0.07f, 0.51f - 0.07f, 1.0f},
			Color4f{ 0.84f, 0.53f, 0.0f, 1.0f},
			Color4f{0.96f, 0.66f, 0.1f, 1.0f},

			Color4f{0.82f, 0.66f, 0.38f, 1.0f},
			true,

			Color4f{0,0,0,0},
			false,

			Color4f{0.82f, 0.66f, 0.38f, 1.0f},
			true,

			false,

			true,
			{
				SpawnSet
				{
					4.0f,
					{
						{ 0.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(0.0f / 2.5f) * 6.0f + 7, 0.0f} },
						{ 1.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(1.0f / 2.5f) * 6.0f + 7, 0.0f} },
						{ 2.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(2.0f / 2.5f) * 6.0f + 7, 0.0f} },
						{ 3.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(3.0f / 2.5f) * 6.0f + 7, 0.0f} },
						{ 4.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(4.0f / 2.5f) * 6.0f + 7, 0.0f} },
						{ 5.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(5.0f / 2.5f) * 6.0f + 7, 0.0f} },
						{ 6.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(6.0f / 2.5f) * 6.0f + 7, 0.0f} },
						{ 7.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(7.0f / 2.5f) * 6.0f + 7, 0.0f} },
						{ 8.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(8.0f / 2.5f) * 6.0f + 7, 0.0f} },
						{ 9.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(9.0f / 2.5f) * 6.0f + 7, 0.0f} },
						{10.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(10.0f / 2.5f) * 6.0f + 7, 0.0f} },
						{11.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(11.0f / 2.5f) * 6.0f + 7, 0.0f} },
						{12.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(12.0f / 2.5f) * 6.0f + 7, 0.0f} },
						{13.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(13.0f / 2.5f) * 6.0f + 7, 0.0f} },
						{14.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(14.0f / 2.5f) * 6.0f + 7, 0.0f} },
						{15.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(15.0f / 2.5f) * 6.0f + 7, 0.0f} },
						{16.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(16.0f / 2.5f) * 6.0f + 7, 0.0f} },
						{17.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(17.0f / 2.5f) * 6.0f + 7, 0.0f} },
						{ 0.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(0.0f / 2.5f) * 6.0f - 7 ,0.0f} },
						{ 1.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(1.0f / 2.5f) * 6.0f - 7 ,0.0f} },
						{ 2.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(2.0f / 2.5f) * 6.0f - 7 ,0.0f} },
						{ 3.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(3.0f / 2.5f) * 6.0f - 7 ,0.0f} },
						{ 4.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(4.0f / 2.5f) * 6.0f - 7 ,0.0f} },
						{ 5.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(5.0f / 2.5f) * 6.0f - 7 ,0.0f} },
						{ 6.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(6.0f / 2.5f) * 6.0f - 7 ,0.0f} },
						{ 7.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(7.0f / 2.5f) * 6.0f - 7 ,0.0f} },
						{ 8.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(8.0f / 2.5f) * 6.0f - 7 ,0.0f} },
						{ 9.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(9.0f / 2.5f) * 6.0f - 7 ,0.0f} },
						{10.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(10.0f / 2.5f) * 6.0f - 7 ,0.0f} },
						{11.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(11.0f / 2.5f) * 6.0f - 7 ,0.0f} },
						{12.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(12.0f / 2.5f) * 6.0f - 7 ,0.0f} },
						{13.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(13.0f / 2.5f) * 6.0f - 7 ,0.0f} },
						{14.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(14.0f / 2.5f) * 6.0f - 7 ,0.0f} },
						{15.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(15.0f / 2.5f) * 6.0f - 7 ,0.0f} },
						{16.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(16.0f / 2.5f) * 6.0f - 7 ,0.0f} },
						{17.0f * 0.8f ,EnemyType::Pillar,Vector2f{sinf(17.0f / 2.5f) * 6.0f - 7 ,0.0f} },


						{ 1.0f * 0.8f ,EnemyType::Tower,Vector2f{sinf(1.0f / 2.5f) * 6.0f, 0.0f} },
						{ 2.0f * 0.8f ,EnemyType::Tents,Vector2f{sinf(2.0f / 2.5f) * 6.0f, 0.0f} },
						{ 5.0f * 0.8f ,EnemyType::Tents,Vector2f{sinf(5.0f / 2.5f) * 6.0f, 0.0f} },
						{ 7.0f * 0.8f ,EnemyType::Tower,Vector2f{sinf(7.0f / 2.5f) * 6.0f, 0.0f} },
						{ 8.0f * 0.8f ,EnemyType::Truck,Vector2f{sinf(8.0f / 2.5f) * 6.0f, 0.0f} },
						{13.0f * 0.8f ,EnemyType::Tents,Vector2f{sinf(13.0f / 2.5f) * 6.0f, 0.0f} },
						{14.0f * 0.8f ,EnemyType::Truck,Vector2f{sinf(14.0f / 2.5f) * 6.0f, 0.0f} },
						{15.0f * 0.8f ,EnemyType::Tents,Vector2f{sinf(15.0f / 2.5f) * 6.0f, 0.0f} },
						{ 1.5f * 0.8f ,EnemyType::Tower,Vector2f{sinf(1.0f / 2.5f) * 6.0f ,0.0f} },
						{ 2.5f * 0.8f ,EnemyType::Truck,Vector2f{sinf(2.0f / 2.5f) * 6.0f ,0.0f} },
						{ 3.5f * 0.8f ,EnemyType::Tents,Vector2f{sinf(3.0f / 2.5f) * 6.0f ,0.0f} },
						{ 6.5f * 0.8f ,EnemyType::Tents,Vector2f{sinf(6.0f / 2.5f) * 6.0f ,0.0f} },
						{ 7.5f * 0.8f ,EnemyType::Tents,Vector2f{sinf(7.0f / 2.5f) * 6.0f ,0.0f} },
						{10.5f * 0.8f ,EnemyType::Tower,Vector2f{sinf(10.0f / 2.5f) * 6.0f ,0.0f} },
						{11.5f * 0.8f ,EnemyType::Truck,Vector2f{sinf(11.0f / 2.5f) * 6.0f ,0.0f} },
						{14.5f * 0.8f ,EnemyType::Truck,Vector2f{sinf(14.0f / 2.5f) * 6.0f ,0.0f} },
						{15.5f * 0.8f ,EnemyType::Tower,Vector2f{sinf(15.0f / 2.5f) * 6.0f ,0.0f} },
					},
					18.0f
				},
				SpawnSet
				{
					0.0f,
					{
						{ 0.0f ,EnemyType::Tower, Vector2f{0.0f, 0.0f} },
						{ 0.0f ,EnemyType::Hangar,Vector2f{3.0f, 0.0f} },
						{ 0.3f ,EnemyType::Hangar,Vector2f{0.0f, 0.0f} },
						{ 0.2f ,EnemyType::Hangar,Vector2f{-4.0f, 0.0f} },
						{ 1.2f ,EnemyType::Tower, Vector2f{3.0f, 0.0f} },
						{ 1.3f ,EnemyType::Tower, Vector2f{0.0f, 0.0f} },
						{ 3.0f ,EnemyType::Tower, Vector2f{4.0f, 0.0f} },
						{ 2.0f ,EnemyType::Hangar,Vector2f{6.0f, 0.0f} },
						{ 3.4f ,EnemyType::Hangar,Vector2f{0.0f, 0.0f} },
						{ 1.2f ,EnemyType::Hangar,Vector2f{-5.0f, 0.0f} },
						{ 1.2f ,EnemyType::Tower, Vector2f{1.0f, 0.0f} },
					},
					4.0f,
				},
				SpawnSet
				{
					0.0f,
					{
						{ 0.0f ,EnemyType::Tower, Vector2f{0.0f, 0.0f} },
						{ 0.0f ,EnemyType::Hangar,Vector2f{3.0f, 0.0f} },
						{ 0.3f ,EnemyType::Hangar,Vector2f{0.0f, 0.0f} },
						{ 0.2f ,EnemyType::Hangar,Vector2f{-4.0f, 0.0f} },
						{ 1.2f ,EnemyType::Tower, Vector2f{3.0f, 0.0f} },
						{ 1.3f ,EnemyType::Tower, Vector2f{-2.3f, 0.0f} },
						{ 3.2f ,EnemyType::Tower, Vector2f{4.5f, 0.0f} },
						{ 2.0f ,EnemyType::Hangar,Vector2f{6.0f, 0.0f} },
						{ 3.4f ,EnemyType::Hangar,Vector2f{0.0f, 0.0f} },
						{ 1.2f ,EnemyType::Hangar,Vector2f{-5.0f, 0.0f} },
						{ 1.2f ,EnemyType::Tower, Vector2f{1.0f, 0.0f} },
					},
					4.0f,
				}
			}
		},
		Stage // 22
		{
			Color4f{0.0f, 0.16f, 0.97f, 1.0f },
			Color4f{0.77f, 0.75f, 0.36f, 1.0f},
			Color4f{0.82f, 0.8f, 0.45f, 1.0f},

			Color4f{0.07f, 0.59f, 0.32f, 1.0f},
			true,

			Color4f{0.93f, 0.93f, 0.93f, 1.0f},
			false,

			Color4f{1,1,1,1},
			false,

			false,

			false,
			{
				SpawnSet
				{
					0.0f,
					{
						{0.0f,EnemyType::GrayFarStraight,Vector2f{ 1.0f * 7.0f, (0.0f * 0.5f) + 6.5f}},
						{0.1f,EnemyType::GrayFarStraight,Vector2f{ 0.0f * 7.0f, (1.0f * 0.5f) + 6.5f}},
						{0.2f,EnemyType::GrayFarStraight,Vector2f{-1.0f * 7.0f, (2.0f * 0.5f) + 6.5f}},
					},
					6.0f
				},
				SpawnSet
				{
					0.0f,
					{
						{0.0f,EnemyType::RedFarStraight,Vector2f{-1.0f * 7.0f, (0.0f * 0.5f) + 6.5f}},
						{0.1f,EnemyType::RedFarStraight,Vector2f{ 0.0f * 7.0f, (1.0f * 0.5f) + 6.5f}},
						{0.2f,EnemyType::RedFarStraight,Vector2f{ 1.0f * 7.0f, (2.0f * 0.5f) + 6.5f}},
					},
					5.0f
				},
				SpawnSet
				{
					0.0f,
					{
						{0.0f,EnemyType::RedFarStraight,Vector2f{ 1.0f * 7.0f, (0.0f * 0.5f) + 6.5f}},
						{0.1f,EnemyType::RedFarStraight,Vector2f{ 0.0f * 7.0f, (1.0f * 0.5f) + 6.5f}},
						{0.2f,EnemyType::RedFarStraight,Vector2f{-1.0f * 7.0f, (2.0f * 0.5f) + 6.5f}},
					},
					7.0f
				},
			}
		},
		Stage // FinalStage
		{
			Color4f{0.22745f, 0.63922f, 0.95294f,1},
			Color4f{ 0.00000f, 0.04314f, 0.75686f,1 },
			Color4f{0.00000f, 0.23137f, 0.80392f,1},

			Color4f{ 0.05882f, 0.70980f, 0.40392f,1},
			false,

			Color4f{1,1,1,1},
			true,

			Color4f{1,1,1,1},
			false,

			false,

			false,
			{}

		}
	};


	const Jul::CubicCurve CURVE_AWAY_SIDE
	{
		Vector3f{-10, 2, -20},
		Vector3f{-10, 6, 10},
		Vector3f{ 10, 5, 20},
		Vector3f{-14, 5, 30}
	};


	const Jul::CubicCurve CURVE_AWAY_FRONT
	{
		Vector3f{ -3.4f, 2.0f, -16.0f },
		Vector3f{ -7.9f, 7.2f, 30.6f },
		Vector3f{ 6.3f, 2.5f, -1.9f },
		Vector3f{ 24.3f, 4.8f, 58.5f }
	};

	const Jul::CubicCurve CURVE_TOWARDS_STRAIGHT_DOWN
	{
		Vector3f{0,0,160},
		Vector3f{0,0,160},
		Vector3f{0,0,-20},
		Vector3f{0,0,-20}
	};

	const Jul::CubicCurve CURVE_AWAY_SIDE_ROLL
	{
		Vector3f{ -9.5f, -1.9f, -21.1f },
		Vector3f{ 5.0f, 13.0f, 19.4f },
		Vector3f{ 12.2f, 2.9f, 33.7f },
		Vector3f{ -22.7f, 2.9f, 73.6f }
	};

	const Jul::CubicCurve CURVE_TOWARDS_STRAIGHT_DIVE
	{
		Vector3f{0.0f,  0.0f,  160.0f},
		Vector3f{0.0f, -3.1f,  16.0f },
		Vector3f{0.0f, -6.8f,  102.2f},
		Vector3f{0.0f, -5.0f, -22.0f }
	};





};

