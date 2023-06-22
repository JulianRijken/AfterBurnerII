#include "pch.h"
#include "Gizmos.h"

#include "Camera.h"
#include "GlobalSettings.h"
#include "utils.h"

void Gizmos::DrawAllGizmos(const Camera* camera)
{
	m_ActiveCamera = camera;

    if(GlobalSettings::DrawGizmos)
		for (Gizmos* gizmosPtr : m_GizmosPtrs)
			gizmosPtr->OnDrawGizmos();
}

void Gizmos::GizmosDrawWireCube(const Vector3f& size, const Vector3f& center, float lineWidth)
{
    if (!m_ActiveCamera)
        return;


    const float halfWidth = size.x / 2.0f;
    const float halfHeight = size.y / 2.0f;
    const float halfDepth = size.z / 2.0f;

    // Calculate the 8 vertices of the cuboid
    const Vector3f vertices[8]
    {
        Vector3f{center.x - halfWidth, center.y - halfHeight, center.z - halfDepth},
        Vector3f{center.x - halfWidth, center.y - halfHeight, center.z + halfDepth},
        Vector3f{center.x - halfWidth, center.y + halfHeight, center.z - halfDepth},
        Vector3f{center.x - halfWidth, center.y + halfHeight, center.z + halfDepth},
        Vector3f{center.x + halfWidth, center.y - halfHeight, center.z - halfDepth},
        Vector3f{center.x + halfWidth, center.y - halfHeight, center.z + halfDepth},
        Vector3f{center.x + halfWidth, center.y + halfHeight, center.z - halfDepth},
        Vector3f{center.x + halfWidth, center.y + halfHeight, center.z + halfDepth}
    };

    // Project each vertex onto a 2D plane
    Vector2f screenPoints[8];
    for (int i = 0; i < 8; i++) 
        screenPoints[i] = m_ActiveCamera->WorldToFlatPoint(vertices[i]);

    glLineWidth(lineWidth);
    glBegin(GL_LINES);

    // Front face
    glVertex2f(screenPoints[0].x, screenPoints[0].y);
    glVertex2f(screenPoints[1].x, screenPoints[1].y);

    glVertex2f(screenPoints[1].x, screenPoints[1].y);
    glVertex2f(screenPoints[3].x, screenPoints[3].y);

    glVertex2f(screenPoints[3].x, screenPoints[3].y);
    glVertex2f(screenPoints[2].x, screenPoints[2].y);

    glVertex2f(screenPoints[2].x, screenPoints[2].y);
    glVertex2f(screenPoints[0].x, screenPoints[0].y);

    // Back face
    glVertex2f(screenPoints[4].x, screenPoints[4].y);
    glVertex2f(screenPoints[5].x, screenPoints[5].y);

    glVertex2f(screenPoints[5].x, screenPoints[5].y);
    glVertex2f(screenPoints[7].x, screenPoints[7].y);

    glVertex2f(screenPoints[7].x, screenPoints[7].y);
    glVertex2f(screenPoints[6].x, screenPoints[6].y);

    glVertex2f(screenPoints[6].x, screenPoints[6].y);
    glVertex2f(screenPoints[4].x, screenPoints[4].y);

    // Connecting edges
    glVertex2f(screenPoints[0].x, screenPoints[0].y);
    glVertex2f(screenPoints[4].x, screenPoints[4].y);

    glVertex2f(screenPoints[1].x, screenPoints[1].y);
    glVertex2f(screenPoints[5].x, screenPoints[5].y);

    glVertex2f(screenPoints[2].x, screenPoints[2].y);
    glVertex2f(screenPoints[6].x, screenPoints[6].y);

    glVertex2f(screenPoints[3].x, screenPoints[3].y);
    glVertex2f(screenPoints[7].x, screenPoints[7].y);

    glEnd();
}

void Gizmos::GizmosDrawCube(const Vector3f& size, const Vector3f& center)
{
    if (!m_ActiveCamera)
        return;

    const float halfWidth = size.x / 2.0f;
    const float halfHeight = size.y / 2.0f;
    const float halfDepth = size.z / 2.0f;

    // Calculate the 8 vertices of the cuboid
    const Vector3f vertices[8]
    {
        Vector3f{center.x - halfWidth, center.y - halfHeight, center.z - halfDepth},
        Vector3f{center.x - halfWidth, center.y - halfHeight, center.z + halfDepth},
        Vector3f{center.x - halfWidth, center.y + halfHeight, center.z - halfDepth},
        Vector3f{center.x - halfWidth, center.y + halfHeight, center.z + halfDepth},
        Vector3f{center.x + halfWidth, center.y - halfHeight, center.z - halfDepth},
        Vector3f{center.x + halfWidth, center.y - halfHeight, center.z + halfDepth},
        Vector3f{center.x + halfWidth, center.y + halfHeight, center.z - halfDepth},
        Vector3f{center.x + halfWidth, center.y + halfHeight, center.z + halfDepth}
    };

    // Project each vertex onto a 2D plane
    Vector2f screenPoints[8];
    for (int i = 0; i < 8; i++)
        screenPoints[i] = m_ActiveCamera->WorldToFlatPoint(vertices[i]);

    glBegin(GL_QUADS);

    // Front face
    glVertex2f(screenPoints[0].x, screenPoints[0].y);
    glVertex2f(screenPoints[1].x, screenPoints[1].y);
    glVertex2f(screenPoints[3].x, screenPoints[3].y);
    glVertex2f(screenPoints[2].x, screenPoints[2].y);

    // Back face
    glVertex2f(screenPoints[4].x, screenPoints[4].y);
    glVertex2f(screenPoints[5].x, screenPoints[5].y);
    glVertex2f(screenPoints[7].x, screenPoints[7].y);
    glVertex2f(screenPoints[6].x, screenPoints[6].y);

    // Top face
    glVertex2f(screenPoints[0].x, screenPoints[0].y);
    glVertex2f(screenPoints[2].x, screenPoints[2].y);
    glVertex2f(screenPoints[6].x, screenPoints[6].y);
    glVertex2f(screenPoints[4].x, screenPoints[4].y);

    // Bottom face
    glVertex2f(screenPoints[1].x, screenPoints[1].y);
    glVertex2f(screenPoints[5].x, screenPoints[5].y);
    glVertex2f(screenPoints[7].x, screenPoints[7].y);
    glVertex2f(screenPoints[3].x, screenPoints[3].y);

    // Left face
    glVertex2f(screenPoints[0].x, screenPoints[0].y);
    glVertex2f(screenPoints[4].x, screenPoints[4].y);
    glVertex2f(screenPoints[5].x, screenPoints[5].y);
    glVertex2f(screenPoints[1].x, screenPoints[1].y);

    // Right face
    glVertex2f(screenPoints[2].x, screenPoints[2].y);
    glVertex2f(screenPoints[3].x, screenPoints[3].y);
    glVertex2f(screenPoints[7].x, screenPoints[7].y);
    glVertex2f(screenPoints[6].x, screenPoints[6].y);

    glEnd();
}

void Gizmos::GizmosDrawLine(const Vector3f& from, const Vector3f& to, float lineWidth)
{
    const Vector2f from2D{ m_ActiveCamera->WorldToFlatPoint(from) };
    const Vector2f to2D{ m_ActiveCamera->WorldToFlatPoint(to) };

    glLineWidth(lineWidth);
    glBegin(GL_LINES);
    {
        glVertex2f(from2D.x, from2D.y);
        glVertex2f(to2D.x, to2D.y);
    }
    glEnd();
}

void Gizmos::GizmosSetColor(const Color4f& color)
{
	glColor4f(color.r, color.g, color.b, color.a);
}

void Gizmos::GizmosSetColor(float r, float g, float b, float a)
{
    glColor4f(r,g,b,a);
}

// Hahaha Horrible code but it's all preview anyways ;) 
void Gizmos::GizmosDrawCubicBezier(Jul::CubicCurve curve, float pointSize, int segments, float width, int selectedPoint, Vector3f offset)
{
    curve.P0 += offset;
    curve.P1 += offset;
    curve.P2 += offset;
    curve.P3 += offset;

    for (int i = 0; i < segments; ++i)
    {
        const float fromAlpha{ float(i) / float(segments) };
        const float toAlpha{ float(i + 1) / float(segments) };

        const Vector3f fromPosition{ Jul::EvaluateCubicBezier(curve, fromAlpha) };
        const Vector3f toPosition{ Jul::EvaluateCubicBezier(curve, toAlpha) };

        GizmosSetColor(Jul::Lerp(Color4f{1.0f,0.0f,0.0f,1.0f}, Color4f{ 0.0f,1.0f,0.0f,1.0f }, fromAlpha));
        GizmosDrawLine(fromPosition, toPosition, width);
    }

    GizmosSetColor(0.5f, 0.5f, 0.5f, 1.0f);
    GizmosDrawLine(curve.P0,curve.P1);
    GizmosDrawLine(curve.P2,curve.P3);

    if (selectedPoint != -1)
    {
        selectedPoint = Jul::Clamp(selectedPoint, 0, 4);
        if (pointSize > 0.0f)
        {
            const Vector3f cubeSize{ Vector3f::One() * pointSize };

            GizmosSetColor(0.1f, 0.1f, 0.1f, 0.9f);
            if(selectedPoint == 0)
                GizmosSetColor(1.0f, 0.0f, 0.0f, 1.0f);

            GizmosDrawCube(cubeSize, curve.P0);

            GizmosSetColor(0.1f, 0.1f, 0.1f, 0.9f);
            if (selectedPoint == 1)
                GizmosSetColor(1.0f, 0.0f, 0.0f, 1.0f);

            GizmosDrawCube(cubeSize, curve.P1);

            GizmosSetColor(0.1f, 0.1f, 0.1f, 0.9f);
            if (selectedPoint == 2)
                GizmosSetColor(1.0f, 0.0f, 0.0f, 1.0f);

            GizmosDrawCube(cubeSize, curve.P2);

            GizmosSetColor(0.1f, 0.1f, 0.1f, 0.9f);
            if (selectedPoint == 3)
                GizmosSetColor(1.0f, 0.0f, 0.0f, 1.0f);

            GizmosDrawCube(cubeSize, curve.P3);
        }
    }
    else
    {
        if (pointSize > 0.0f)
        {
            const Vector3f cubeSize{ Vector3f::One() * pointSize };

            GizmosSetColor(1.0f, 0.0f, 0.0f, 1.0f);
            GizmosDrawCube(cubeSize, curve.P0);
            GizmosSetColor(1.0f, 0.2f, 0.2f, 0.5f);
            GizmosDrawCube(cubeSize, curve.P1);
            GizmosSetColor(0.2f, 1.0f, 0.2f, 0.5f);
            GizmosDrawCube(cubeSize, curve.P2);
            GizmosSetColor(0.0f, 1.0f, 0.0f, 1.0f);
            GizmosDrawCube(cubeSize, curve.P3);
        }
    }
}

void Gizmos::GizmosDrawCubicBezier2D(Jul::CubicCurve curve, float pointSize, int segments, float width, int selectedPoint, float scale)
{
    Jul::CubicCurve flatCurve
    {
        Vector3f{curve.P0.x,curve.P0.z,0},
        Vector3f{curve.P1.x,curve.P1.z,0},
        Vector3f{curve.P2.x,curve.P2.z,0},
        Vector3f{curve.P3.x,curve.P3.z,0},
    };

    const float height{ 4.0f };

    flatCurve = Jul::ScaleCurve(flatCurve, Vector3f{ 0,height,0 }, Vector2f{ scale,scale });

    GizmosSetColor(1, 0, 1, 1);
    GizmosDrawLine(Vector3f{ -10,height,0 }, Vector3f{ 10,height,0 });
    GizmosDrawLine(Vector3f{ 0,-10,0 }, Vector3f{ 0,10,0 });


    GizmosSetColor(0, 1, 1, 1);
    GizmosDrawLine(Vector3f{ 0,-GlobalSettings::MAIN_GAME_ZOOM,0 }, Vector3f{ 0,-GlobalSettings::MAIN_GAME_ZOOM,0 });

    GizmosDrawCubicBezier(flatCurve, pointSize, segments, width, selectedPoint);
}

//void Gizmos::DrawCircle3D(const Vector3f& center, float radius, float lineWidth, int vertexCount)
//{
//	DrawCircle2D(Camera::WorldToFlatPoint(center), Camera::DistanceToFlatScale(center.z) * radius, lineWidth, vertexCount);
//}
//
//
//void Gizmos::DrawCircle2D(const Vector2f& center, float radius, float lineWidth, int vertexCount)
//{
//	DrawCircle2D(center.x, center.y, radius, lineWidth, vertexCount);
//}
//
//void Gizmos::DrawCircle2D(float centerX, float centerY, float radius, float lineWidth, int vertexCount)
//{
//	if (radius > 0 && lineWidth > 0)
//	{
//		const float angleDelta{ utils::g_Pi * 2 / float(vertexCount) };
//
//		glLineWidth(lineWidth);
//		glBegin(GL_LINE_LOOP);
//		{
//			for (float angle{ 0.0f }; angle < 2 * utils::g_Pi; angle += angleDelta)
//				glVertex2f(centerX + radius * cosf(angle), centerY + radius * sinf(angle));
//		}
//		glEnd();
//	}
//}
