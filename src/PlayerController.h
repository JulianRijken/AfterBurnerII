#pragma once

#include "Behaviour.h"
#include "Gizmos.h"
#include "GlobalSettings.h"
#include "IDamageable.h"
#include "IEvents.h"
#include "TargetVisual.h"
#include "GameTime.h"
#include "Vector2f.h"
#include "Vector3f.h"

class Animator;
class Camera;
class SpriteRenderer;
class GameManager;


class PlayerController final : public Behaviour, public IDamageable, IEvents
{

public:

	struct PlayerMovementSettings
	{
		PositionExtends CrosshairPositionExtends;
		PositionExtends PlanePositionExtends;
		PositionExtends CameraPositionExtends;
		RotationExtends CameraRotationExtends;

		// Alpha 0 - 1
		float PlayerMoveRange;

		float SmoothAmount;
		float MoveSpeed;

		float HoldTimeBeforeBarrelRoll;
		float TimeWindowsForBarrelRoll;
		bool CanBarrelRoll;
	};

	enum class PlayerState
	{
		Flying,
		Crashing,
		Respawning,
		Launching,
		Landing,
		ReFueling,
	};


	PlayerController(const bool enabled = true, const std::string& name = "Player");

	[[nodiscard]] const Vector2f& GetAimLocation() const { return m_AimLocation; }
	[[nodiscard]] PlayerState GetActiveState() const;


private:

	void Update() override;
	void Awake() override;

	void UpdateAimTargetBasedOnInput();
	void ApplyMovementBasedOnAimTarget() const;

	void Respawn();
	void BlinkPlayer();

	void HandleGunFire();
	void FireBullet(bool side) const;
	void FireRocket();
	void UpdateTargets();

	void AddRocket();

	void HandleCollision();

	void DestroyPlane();
	void LaunchPlane();
	void LandPlane();
	void RefuelPlane();

	void OnDeath() override;
	void OnPlaneRefuel() override;
	void OnPlayerRespawn() override;
	void OnGameOver(bool won) override;
	void OnStageChange(int stage, bool bonusStage) override;

	[[nodiscard]] Vector3f GetAimTargetWorldPosition() const;


	const PlayerMovementSettings GROUND_MOVEMENT_SETTINGS
	{
		{ -1.2f,-1.3f,1.2f,0.3f },
		{ -3,-2.0f,3,0.0f },
		{ -5,8.0f,5,8.0f},
		{ 1.9f,1.3f,0,0,0.0f,0.0f},

		0.0f,
		14,
		0.8f,

		0.0f,
		0.0f,
		false
	};

	const PlayerMovementSettings SKY_MOVEMENT_SETTINGS
	{
		{ -2.2f,0.5f,2.2f,0.5f },
		{ -2,-1.3f,2,1.5f },
		{ -1,5.5f,1,2},
		{ 1.5f,-2.3f,1,-1,-50,50},

		0.2f,
		12,
		0.8f,

		0.3f,
		0.15f,
		true
	};


	const float BULLET_FIRE_SPEED{ 300.0f };
	const float BULLET_FIRE_HEIGHT_OFFSET{ -0.2f };

	const float AIM_TARGET_WORLD_DISTANCE{ GlobalSettings::MAX_ENEMY_SPAWN_DISTANCE };
	const float MIN_DISTANCE_FOR_LOCK{ 0.75f};

	const float ROCKET_FIRE_DELAY{ 0.15f };
	const int	MAX_AMMO{99};

	const float BULLET_FIRE_DELAY{ 0.08f };
	const float BULLET_BURST_DELAY{ 0.2f };
	const float BULLET_BURST_BULLET_COUNT{5};
	const float INITIAL_BULLET_SPREAD{ 0.4f };

	const float MOVEMENT_MODE_SWITCH_TIME{ 3.0f };

	const float DEFAULT_FLY_SPEED{ 100.0f };
	const Vector2f DEFAULT_AIM_LOCATION{ 0.5f,0.45f };


	PlayerMovementSettings m_ActiveMovementSettings{};
	PlayerMovementSettings m_LastActiveMovementSettings{};
	PlayerMovementSettings m_TargetMovementSettings{};

	PlayerState m_ActivePlayerState{};

	std::weak_ptr<Camera> m_MainCameraWPtr{};
	std::weak_ptr<GameManager> m_GameManagerWPtr{};
	std::weak_ptr<GameObject> m_CrosshairWPtr{};
	std::weak_ptr<SpriteRenderer> m_PlayerSpriteComponentWPtr{};

	std::deque<std::weak_ptr<TargetVisual>> m_Targets{};

	Vector2f m_AimLocation{};
	Vector2f m_SmoothAimLocation{};
	Vector2f m_PlayerAimLocation{};

	std::shared_ptr<Time::Timer> m_RocketFireTimerPtr;
	int m_RocketAmmoCount;
	float m_FireTimer;
	float m_FireCount;
	float m_BarrleRollAngle;
	float m_TimeHoldingSide;
	bool m_RocketFireSide;

	int m_InBarrleRoll;

	bool m_MoveToAimTarget;
	bool m_ManualCameraControl;



	// Player dying animation
	int m_DyingState{0};
	Vector3f m_LastPositionBeforeDeath{};

	const Vector2f DEATH_ANIMATION_AIM_LOCATION_1{ 0.7f,0.7f };
	const Vector2f DEATH_ANIMATION_AIM_LOCATION_2{ 0.5f,0.4f };
	const Vector2f DEATH_ANIMATION_AIM_LOCATION_3{ 0.5f,0.2f };
	const Vector2f DEATH_ANIMATION_AIM_LOCATION_4{ 0.5f,0.0f };


	// Player launch animation
	const float LAUNCH_DURATION{3.5f };
	const Vector3f LAUNCH_ANIMATION_START_LOCATION{0,2.4f,18 };
	const Vector3f LAUNCH_ANIMATION_END_LOCATION{0,17.0f,60 };
	const int LAUNCH_ANIMATION_CLOUD_COUNT{12 };

	// Player ReFuel animation
	int m_RefuelingState{};
	int m_RocketsRefueled{};
	Vector3f m_LastLocationRefuel;
	Vector3f m_StartLocationRefuel;
	const float REFUEL_PLANE_PRECISION{ 1000.0f};
	const float REFUEL_ROCKET_DELAY{0.067f};
	const int REFUEL_ROCKET_COUNT{50};
	const Vector2f REFUEL_ANIMATION_AIM_LOCATION_1{ 0.5f,0.4f };
	const Vector3f REFUEL_ANIMATION_LOCATION_2{ 0.54f, 3.5f, -0.00000000001f };
	const Vector3f REFUEL_ANIMATION_LOCATION_3{ 0.5775f,4.28f, -0.00000000001f };
	std::weak_ptr<GameObject> m_RefuelPlaneGoWPtr{};

	// Game over
	int m_LandingState{};
	Vector3f m_LastLocationBeforeLanding;
	Vector3f m_LandAnimationLastCameraRotation{};
	Vector3f m_LandAnimationLastCameraPosition{};
	const Vector2f LAND_ANIMATION_AIM_LOCATION_1{ 0.5f,0.5f };
	const Vector2f GAME_OVER_AIM_LOCATION{ 0.5f,0.8f };
	const Vector2f LAND_ANIMATION_AIM_LOCATION_2{ 0.5f,0.0f };
	const Vector3f LAND_ANIMATION_DOWN_CAMERA_ROTATION{10,0,0 };
	const Vector3f LAND_ANIMATION_DOWN_CAMERA_POSITION{0,9,-15.0f};
	const Vector3f LAND_ANIMATION_STRAIGHT_CAMERA_ROTATION{0.5f,0,0 };
	const Vector3f LAND_ANIMATION_STRAIGHT_CAMERA_POSITION{ 0,5,-25.0f };

	const Vector3f LAND_ANIMATION_PLANE_LOCATION{ 0,2.4f,18 };
};

