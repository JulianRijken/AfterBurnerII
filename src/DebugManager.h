#pragma once
#include "Behaviour.h"
#include "Gizmos.h"
#include "IEvents.h"


class TextRenderer;

class DebugManager final : public Behaviour, Gizmos, IEvents
{

public:
	DebugManager();

	static void PrintControls();

private:

	void Awake() override;

	void OnDrawGizmos() override;
	void OnKeyDown(const SDL_KeyboardEvent& e) override;
	void Update() override;

	void OnGamePaused(bool paused) override;

	void EditCurve(bool edit);
	void HandleCurveEditing();

	const float FRAME_UPDATE_TIME{1.0f};
	float CURVE_PRINT_INTERVAL{ 1.0f };
	inline static constexpr int GRID_SIZE{ 4 };

	float m_FrameTimer;
	int m_FrameCount;

	std::weak_ptr<TextRenderer> m_FramesTextGoWPtr{};

	bool m_EditingCurve;
	bool m_ShowCurveSides;
	int m_SelectedPoint;
	float m_CurvePreviewTimer1;
	float m_CurvePreviewTimer2;
	float m_CurvePrintTimer;
	Vector3f m_CurveCenter;
	Jul::CubicCurve m_PreviewCurve;
	std::weak_ptr<GameObject> m_PreviewEnemy1;
	std::weak_ptr<GameObject> m_PreviewEnemy2;

	bool m_ShowUnitGrid{false};
	std::vector<std::weak_ptr<GameObject>> m_GridCellGoWPtrs{};
};

