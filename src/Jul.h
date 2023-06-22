#pragma once
#include "utils.h"
#include "Vector3f.h"


class Jul
{

public:

	static bool IsEven(const int number)
	{
		return number % 2 == 0;
	}

	template <typename DataType>
	static DataType Clamp(const DataType value, DataType min, DataType max)
	{
		if (min > max)
		{
			//std::swap(min, max);
			const DataType tempMin{ min };
			min = max;
			max = tempMin;
		}


		return std::max(std::min(max, value), min);
	}


	template <typename DataType>
	static DataType Clamp01(const DataType value)
	{
		return 	std::max(std::min(1.0f, value), 0.0f);
	}

	static void ClampLoop01(float& value)
	{
		if(value > 1 || value < 0)
			value -= floorf(value);
	}

	static float ClampLoop01(const float value)
	{
		if (value > 1 || value < 0)
			 return value - floorf(value);

		return value;
	}

	static int ClampLoop(const int value, int min, int max)
	{
		if (min > max)
		{
			const int tempMax = max;
			max = min;
			min = tempMax;
		}

		int result{ value };


		int range{ max - min };

		while (result > max)
			result -= range + 1;

		while (result < min)
			result += range + 1;


		return result;
	}


	static float OneMinus(const float& value)
	{
		return 1.0f - value;
	}

	static float MapWave(const float time, const float min, const float max, const float repeatTime, const float startTime)
	{
		const float amplitude{ (max - min) / 2.0f };
		const float intercept{ min + amplitude };
		const float pulsation{ 2 * float(M_PI) / repeatTime };
		const float phase{ 2 * float(M_PI) * startTime };

		return amplitude * sinf(pulsation * time + phase) + intercept;
	}


	static float Lerp(const float a, const float b, float t)
	{
		t = Clamp01(t);
		return  a + (b - a) * t;
	}

	static Color4f Lerp(const Color4f a, const Color4f b, float t)
	{
		t = Clamp01(t);
		return Color4f
		{
			Lerp(a.r,b.r,t),
			Lerp(a.g,b.g,t),
			Lerp(a.b,b.b,t),
			Lerp(a.a,b.a,t),
		};
	}

	static Vector2f Lerp(const Vector2f a, const Vector2f b, float t)
	{
		t = Clamp01(t);
		return Vector2f
		{
			Lerp(a.x,b.x,t),
			Lerp(a.y,b.y,t),
		};
	}

	static Vector3f Lerp(const Vector3f a, const Vector3f b, float t)
	{
		t = Clamp01(t);
		return Vector3f
		{
			Lerp(a.x,b.x,t),
			Lerp(a.y,b.y,t),
			Lerp(a.z,b.z,t),
		};
	}


	struct QuadraticCurve
	{
		Vector3f P0;
		Vector3f P1;
		Vector3f P2;
	};

	static Vector3f EvaluateQuadraticBezier(const Vector3f P0, const Vector3f P1, const Vector3f P2, float t)
	{
		return EvaluateCubicBezier({ P0,P1,P2 }, t);
	}

	static Vector3f EvaluateQuadraticBezier(const QuadraticCurve curve, float t)
	{
		//! Un optimized version
		const Vector3f a{ Lerp(curve.P0,curve.P1,t) };
		const Vector3f b{ Lerp(curve.P1,curve.P2,t) };

		return Lerp(a, b, t);
	}


	struct CubicCurve
	{
		Vector3f P0;
		Vector3f P1;
		Vector3f P2;
		Vector3f P3;
	};

	static Vector3f EvaluateCubicBezier(const Vector3f P0, const Vector3f P1, const Vector3f P2, const Vector3f P3, float t)
	{
		return EvaluateCubicBezier({ P0,P1,P2,P3}, t);
	}

	static CubicCurve ScaleCurve(CubicCurve curve, Vector3f center,Vector2f scale = Vector2f{1,1})
	{
		if (scale.x != 1.0f)
		{
			curve.P0.x -= center.x;
			curve.P1.x -= center.x;
			curve.P2.x -= center.x;
			curve.P3.x -= center.x;

			curve.P0.x *= scale.x;
			curve.P1.x *= scale.x;
			curve.P2.x *= scale.x;
			curve.P3.x *= scale.x;

			curve.P0.x += center.x;
			curve.P1.x += center.x;
			curve.P2.x += center.x;
			curve.P3.x += center.x;
		}

		if (scale.y != 1.0f)
		{
			curve.P0.y -= center.y;
			curve.P1.y -= center.y;
			curve.P2.y -= center.y;
			curve.P3.y -= center.y;

			curve.P0.y *= scale.y;
			curve.P1.y *= scale.y;
			curve.P2.y *= scale.y;
			curve.P3.y *= scale.y;

			curve.P0.y += center.y;
			curve.P1.y += center.y;
			curve.P2.y += center.y;
			curve.P3.y += center.y;
		}

		return curve;
	}

	static Vector3f EvaluateCubicBezier(const CubicCurve curve, float t)
	{
		// Un optimized version
		const Vector3f a{ Lerp(curve.P0,curve.P1,t) };
		const Vector3f b{ Lerp(curve.P1,curve.P2,t) };
		const Vector3f c{ Lerp(curve.P2,curve.P3,t) };
		 
		const Vector3f d{ Lerp(a,b,t) };
		const Vector3f e{ Lerp(b,c,t) };

		return Lerp(d,e,t);
	}


	static float SmoothLerp(const float a, const float b, float t)
	{
		t = Clamp01(t);
		return Lerp(a,b, MapWave(t, 0, 1, 2, -0.25f));
	}

	static Color4f SmoothLerp(const Color4f a, const Color4f b, float t)
	{
		t = Clamp01(t);
		return Color4f
		{
			SmoothLerp(a.r,b.r,t),
			SmoothLerp(a.g,b.g,t),
			SmoothLerp(a.b,b.b,t),
			SmoothLerp(a.a,b.a,t),
		};
	}

	static Vector3f SmoothLerp(const Vector3f a, const Vector3f b, float t)
	{
		return Vector3f
		{
			SmoothLerp(a.x,b.x,t),
			SmoothLerp(a.y,b.y,t),
			SmoothLerp(a.z,b.z,t),
		};
	}

	static PositionExtends SmoothLerp(const PositionExtends a, const PositionExtends b, float t)
	{
		return PositionExtends
		{
			Lerp(a.left  ,b.left, t),
			Lerp(a.bottom,b.bottom,t),
			Lerp(a.right ,b.right, t),
			Lerp(a.top   ,b.top,   t),
		};
	}

	static RotationExtends SmoothLerp(const RotationExtends a, const RotationExtends b, float t)
	{
		return RotationExtends
		{
			SmoothLerp(a.minPitch,b.minPitch,t),
			SmoothLerp(a.maxPitch,b.maxPitch,t),
			SmoothLerp(a.minYaw  ,b.minYaw,t),
			SmoothLerp(a.maxYaw  ,b.maxYaw,t),
			SmoothLerp(a.minRoll ,b.minRoll,t),
			SmoothLerp(a.maxRoll ,b.maxRoll,t),
		};
	}



	static float SmoothEndLerp(const float a, const float b, float t)
	{
		t = Clamp01(t);
		return Lerp(a, b, MapWave(t, -1, 1, 4, 0));
	}

	static Vector3f SmoothEndLerp(const Vector3f a, const Vector3f b, float t)
	{
		return Vector3f
		{
			SmoothEndLerp(a.x,b.x,t),
			SmoothEndLerp(a.y,b.y,t),
			SmoothEndLerp(a.z,b.z,t),
		};
	}

	static float SmoothStartLerp(const float a, const float b, float t)
	{
		t = Clamp01(t);
		return Lerp(a, b, MapWave(t, 0, 2, 4, -0.25f));
	}

	static Vector3f SmoothStartLerp(const Vector3f a, const Vector3f b, float t)
	{
		return Vector3f
		{
			SmoothStartLerp(a.x,b.x,t),
			SmoothStartLerp(a.y,b.y,t),
			SmoothStartLerp(a.z,b.z,t),
		};
	}


	static float MoveTowards(const float& current, const float& target, const float& maxDelta)
	{
		
		float direction{ target - current };
		if (direction > maxDelta)
			direction = maxDelta;

		return current + direction;
	}

	static Vector2f MoveTowards(const Vector2f& current, const Vector2f& target, const float& maxDelta)
	{
		Vector2f direction{ target - current };
		if (direction.Length() > maxDelta)
			direction = direction.Normalized() * maxDelta;

		return current + direction;
	}

	static Vector3f MoveTowards(const Vector3f& current, const Vector3f& target, const float& maxDelta)
	{
		Vector3f direction{ target - current };
		if(direction.Length() > maxDelta)
			direction = direction.Normalized() * maxDelta;

		return current + direction;
	}

	static Color4f MoveTowards(const Color4f& current, const Color4f& target, float maxDelta)
	{
		Color4f result = current;

		const float deltaR = target.r - current.r;
		const float deltaG = target.g - current.g;
		const float deltaB = target.b - current.b;
		const float deltaA = target.a - current.a;

		const float magnitude = sqrtf(deltaR * deltaR + deltaG * deltaG + deltaB * deltaB + deltaA * deltaA);

		if (magnitude == 0 || maxDelta >= magnitude)
		{
			result.r = target.r;
			result.g = target.g;
			result.b = target.b;
			result.a = target.a;
		}
		else
		{
			float scaleFactor = maxDelta / magnitude;
			result.r += deltaR * scaleFactor;
			result.g += deltaG * scaleFactor;
			result.b += deltaB * scaleFactor;
			result.a += deltaA * scaleFactor;
		}

		return result;
	}


	static float MapValueInRange(float value, float inRangeMin, float inRangeMax, float outRangeMin = 0.0f, float outRangeMax = 1.0f)
	{
		return (value - inRangeMin) * (outRangeMax - outRangeMin) / (inRangeMax - inRangeMin) + outRangeMin;
	}

	static float MapValueInRangeClamped(float value, float inRangeMin, float inRangeMax, float outRangeMin = 0.0f, float outRangeMax = 1.0f)
	{
		return Clamp((value - inRangeMin) * (outRangeMax - outRangeMin) / (inRangeMax - inRangeMin) + outRangeMin, outRangeMin, outRangeMax);
	}


	static	float RandomRange(float min, float max)
	{
		return float(RandomRange(double(min), double(max)));
	}

	static double RandomRange(double min, double max)
	{
		// Swap
		if (min > max)
		{
			const double tempMax = max;
			max = min;
			min = tempMax;
		}

		const double randomAlpha{ double(rand()) / RAND_MAX };
		const double range{ max - min };

		return randomAlpha * range + min;
	}

	static int RandomRange(int min, int max)
	{
		// Swap
		if (min > max)
		{
			const int tempMax = max;
			max = min;
			min = tempMax;
		}

		return rand() % (max - min + 1) + min;
	}

	static float RandomValue()
	{
		return rand() % RAND_MAX / float(RAND_MAX);
	}

	static int GetRandomItemFromList(const std::initializer_list<bool>& boolList)
	{
		// Get list of filtered items
		std::vector<int> filteredItems;
		for (int index = 0; index < boolList.size(); ++index)
		{
			if (*(boolList.begin() + index))
				filteredItems.push_back(index);
		}

		// If no true values found, return -1
		if (filteredItems.empty()) 
			return -1;
		
		// Return the random item
		return filteredItems[std::rand() % filteredItems.size()];
	}


	static float RadToDeg(float radians)
	{
		return radians / float(M_PI) * 180.0f;
	}

	static float DegToRad(float degrees)
	{
		return degrees / 180.0f * float(M_PI);
	}


	static bool HasCollisionBoxOnBox(const Vector3f& center1, const Vector3f& size1, const Vector3f& center2,const Vector3f& size2)
	{
		/// Code is calculated in if statement to allow for early cancle but sacraficing readability
		/* 
		// Calculate the half-extents of each box
		Vector3f half_extents1 = { size1.x / 2, size1.y / 2, size1.z / 2 };
		Vector3f half_extents2 = { size2.x / 2, size2.y / 2, size2.z / 2 };

		// Calculate the distance between the centers of the two boxes
		Vector3f distance = { center1.x - center2.x, center1.y - center2.y, center1.z - center2.z };

		// Calculate the minimum distance required for the two boxes to not overlap in each dimension
		float min_distance_x = half_extents1.x + half_extents2.x;
		float min_distance_y = half_extents1.y + half_extents2.y;
		float min_distance_z = half_extents1.z + half_extents2.z;

		// Check if the two boxes overlap in each dimension
		bool has_collision_x = std::abs(distance.x) <= min_distance_x;
		bool has_collision_y = std::abs(distance.y) <= min_distance_y;
		bool has_collision_z = std::abs(distance.z) <= min_distance_z;

		// If the two boxes overlap in all three dimensions, they are colliding
		return has_collision_x && has_collision_y && has_collision_z;
		*/

		// Check if the two boxes overlap in each dimension
		if (std::abs(center1.x - center2.x) > size1.x / 2 + size2.x / 2)
			return false;

		if (std::abs(center1.y - center2.y) > size1.y / 2 + size2.y / 2)
			return false;

		if (std::abs(center1.z - center2.z) > size1.z / 2 + size2.z / 2)
			return false;

		return true;
	}

	static float GetDistanceBetweenPoints2D(Vector2f point1, Vector2f point2)
	{
		return sqrtf(powf(point2.x - point1.x, 2.0f) + powf(point2.y - point1.y, 2.0f));
	}

	static float GetDistanceBetweenPoints2D(Point2f point1, Point2f point2)
	{
		return sqrtf(powf(point2.x - point1.x, 2.0f) + powf(point2.y - point1.y, 2.0f));
	}

	static bool HasCollisionRectOnRect2D(const Vector2f& center1, const Vector2f& size1, const Vector2f& center2, const Vector2f& size2)
	{
		if (std::abs(center1.x - center2.x) > size1.x / 2 + size2.x / 2)
			return false;

		if (std::abs(center1.y - center2.y) > size1.y / 2 + size2.y / 2)
			return false;

		return true;
	}

	static bool HasCollisionRectOnRect2D(const Rectf& rect1, const Rectf& rect2)
	{
		if (rect1.left + rect1.width > rect2.left)
		{
			if (rect1.left < rect2.left + rect2.width)
			{
				if (rect1.bottom < rect2.bottom + rect2.height)
				{
					if (rect1.bottom + rect1.height > rect2.bottom)
					{
						return true;
					}
				}
			}
		}

		return false;
	}

	static bool HasCollisionRectOnPoint2D(const Rectf& rect, const Point2f& point)
	{
		if (point.x > rect.left && point.x < rect.left + rect.width)
		{
			if (point.y > rect.bottom && point.y < rect.bottom + rect.height)
			{
				return true;
			}
		}

		return false;
	}

	static bool HasCollisionRectOnVector2D(const Rectf& rect, const Vector2f& vector)
	{
		if (vector.x > rect.left && vector.x < rect.left + rect.width)
		{
			if (vector.y > rect.bottom && vector.y < rect.bottom + rect.height)
			{
				return true;
			}
		}

		return false;
	}

	static bool HasCollisionCircleOnCircle2D(const Point2f& circle1Position, float circle1Radius, const Point2f& circle2Position, float circle2Radius)
	{
		const float combinedRadii{ circle1Radius + circle2Radius };
		const float distanceBetweenCircles{ GetDistanceBetweenPoints2D(circle1Position,circle2Position) };

		if (distanceBetweenCircles < combinedRadii)
			return true;

		return false;
	}

	static bool HasCollisionCircleOnPoint2D(const Point2f& circlePosition, float circleRadius, const Point2f& point)
	{
		const float distanceBetween{ GetDistanceBetweenPoints2D(circlePosition,point) };

		if (distanceBetween < circleRadius)
			return true;

		return false;
	}

	static bool HasCollisionEllipseOnPoint2D(const Point2f& ellipse1Position, const Point2f& ellipse1Radii, const Point2f& point)
	{
		float result{ powf(point.x - ellipse1Position.x,2) / powf(ellipse1Radii.x, 2) + powf(point.y - ellipse1Position.y,2) / powf(ellipse1Radii.y, 2) };

		return result < 1;
	}





};

	//double SineLerp(double value, const double from, const double to)
	//{
	//	value = Clamp01(value);
	//	return from + (to - from) * (0.5 * sin(M_PI * value + (-M_PI / 2.0)) + 0.5);
	//}




	//float GetDistanceBetween(Point2f fromPoint, Point2f toPoint)
	//{
	//	return sqrtf(powf(toPoint.x - fromPoint.x, 2) + powf(toPoint.y - fromPoint.y, 2));
	//}

	//float GetAngleDifference(float fromAngle, float toAngle)
	//{
	//	return atan2f(sinf(toAngle - fromAngle), cosf(toAngle - fromAngle));
	//}

	//float GetAngleToPoint(Point2f fromPoint, Point2f toPoint)
	//{
	//	return atan2(toPoint.y - fromPoint.y, toPoint.x - fromPoint.x);
	//}


