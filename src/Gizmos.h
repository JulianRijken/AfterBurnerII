#pragma once
#include <unordered_set>

#include "Jul.h"
#include "Vector3f.h"

class Camera;

class Gizmos
{
public:

	Gizmos()
	{
		m_GizmosPtrs.insert(this);
	}

	virtual ~Gizmos()
	{
		m_GizmosPtrs.erase(this);
	}

	static void DrawAllGizmos(const Camera* camera);

protected:

	static void GizmosDrawWireCube(const Vector3f& size, const Vector3f& center, float lineWidth = 1.0f);

	static void GizmosDrawCube(const Vector3f& size, const Vector3f& center);
	static void GizmosDrawLine(const Vector3f& from, const Vector3f& to, float lineWidth = 1.0f);

	static void GizmosSetColor(const Color4f& color);
	static void GizmosSetColor(float r, float g, float b, float a = 1.0f);

	static void GizmosDrawCubicBezier(Jul::CubicCurve curve, float pointSize = 0.5f, int segments = 100, float width = 2.0f,int selectedPoint = -1, Vector3f offset = Vector3f{0,0,0});
	static void GizmosDrawCubicBezier2D(Jul::CubicCurve curve, float pointSize = 0.5f, int segments = 100, float width = 2.0f, int selectedPoint = -1, float scale = 0.05f);

	virtual void OnDrawGizmos() = 0;

private:


	inline static const Camera* m_ActiveCamera{nullptr};

	inline static std::unordered_set<Gizmos*> m_GizmosPtrs{};
};

