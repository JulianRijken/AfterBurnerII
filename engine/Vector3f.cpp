#include "pch.h"
#include <cmath>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include "Vector3f.h"


//-----------------------------------------------------------------
// Vector3f Constructors
//-----------------------------------------------------------------
Vector3f::Vector3f( )
	:Vector3f{ 0.0f, 0.0f, 0.0f }
{
}

Vector3f::Vector3f( float x, float y, float z) :
	x{ x },
	y{ y },
	z{ z }
{}



// -------------------------
// Methods
// -------------------------
bool Vector3f::Equals(const Vector3f& other, float epsilon) const
{
	return ( abs(x - other.x) < epsilon ) && ( abs(y - other.y) < epsilon ) && (abs(z - other.z) < epsilon);
}

float Vector3f::DotProduct(const Vector3f& other) const
{
	return x * other.x + y * other.y + z * other.z;
}

Vector3f Vector3f::CrossProduct(const Vector3f& other) const
{
	return Vector3f
	{
		y * other.z - z * other.y,
		x * other.z + z * other.x,
		x * other.y - y * other.x,
	};
}

std::string Vector3f::ToString() const
{
	std::stringstream buffer;

	buffer << std::fixed;
	buffer << std::setprecision( 2 );
	buffer << "Vector3f(" <<  x  << ", " <<  y  << ", " << z << ")";
	return buffer.str();
}

float Vector3f::Norm() const
{
	return Length();
}

float Vector3f::Length() const
{
	return sqrt( x * x + y * y + z * z);
}

float Vector3f::SquaredLength() const
{
	return x * x + y * y + z * z;
}

//float Vector3f::AngleWith(const Vector3f& other) const
//{
//	//https://stackoverflow.com/questions/21483999/using-atan2-to-find-angle-between-two-vectors
//	// not calling dot and cross functions to prevent the overhead
//	return atan2(x * other.y - other.x * y, x * other.x + y * other.y);
//}

Vector3f Vector3f::Normalized(float epsilon) const
{
	float length{ Length( ) };
	if ( length < epsilon )
	{
		return Vector3f{ 0, 0, 0 };
	}
	else
	{
		return Vector3f{ x / length, y / length, z / length };
	}
}

//Vector3f Vector3f::Orthogonal() const
//{
//	return Vector3f{ -y, x };
//}

//Vector3f Vector3f::Reflect( const Vector3f& surfaceNormal ) const
//{
//	return (*this) - 2 * ( this->DotProduct( surfaceNormal ) * surfaceNormal );
//}

void Vector3f::Set(float newX, float newY, float newZ)
{
	x = newX;
	y = newY;
	z = newZ;
}

// -------------------------
// Member operators
// -------------------------
Vector3f Vector3f::operator-( ) const
{
	return Vector3f{ -x, -y, -z};
}
Vector3f Vector3f::operator+ ( ) const
{
	return Vector3f{ x, y, z};
}

Vector3f& Vector3f::operator*=(float rhs)
{
	x *= rhs;
	y *= rhs;
	z *= rhs;
	return *this;
}

Vector3f& Vector3f::operator/=(float rhs)
{
	*this *= 1 / rhs;
	return *this;
}

Vector3f& Vector3f::operator+=(const Vector3f& rhs)
{
	x += rhs.x;
	y += rhs.y;
	z += rhs.z;
	return *this;
}

Vector3f& Vector3f::operator-=(const Vector3f& rhs)
{
	*this += -rhs;
	return *this;
}

Vector3f Vector3f::Forward()
{
	return Vector3f{ 0,0,1 };
}

Vector3f Vector3f::Right()
{
	return Vector3f{ 1,0,0 };
}

Vector3f Vector3f::Up()
{
	return Vector3f{ 0,1,0 };
}


Vector3f Vector3f::One()
{
	return Vector3f{ 1,1,1 };
}

Vector3f Vector3f::Zero()
{
	return Vector3f{};
}

// -------------------------
// Non-member operators
// -------------------------
Vector3f operator*( float lhs, Vector3f rhs )
{
	return rhs *= lhs;
}

Vector3f operator*( Vector3f lhs, float rhs )
{
	return lhs *= rhs;
}

Vector3f operator/( Vector3f lhs, float rhs )
{
	return lhs *= (1 / rhs);
}

Vector3f operator+( Vector3f lhs, const Vector3f& rhs )
{
	return lhs += rhs;
}

Vector3f operator-( Vector3f lhs, const Vector3f& rhs )
{
	return lhs += -rhs;
}

bool operator==( const Vector3f& lhs, const Vector3f& rhs )
{
	return ( lhs.Equals( rhs ) );
}

bool operator!=( const  Vector3f& lhs, const Vector3f& rhs )
{
	return !( lhs == rhs );
}

std::ostream& operator<< ( std::ostream& lhs, const Vector3f& rhs )
{
	lhs << rhs.ToString( );
	return lhs;
}
