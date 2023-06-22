#pragma once
#include "structs.h"


struct Vector3f final
{
	// -------------------------
	// Constructors 
	// -------------------------
	Vector3f( );
	explicit Vector3f( float x, float y , float z);

	// -------------------------
	// Member operators
	// -------------------------
	Vector3f operator-( ) const;
	Vector3f operator+( ) const;
	Vector3f& operator*=( float rhs);
	Vector3f& operator/=( float rhs);
	Vector3f& operator+=( const Vector3f& rhs);
	Vector3f& operator-=( const Vector3f& rhs);

	// -------------------------
	// Methods
	// -------------------------

	static Vector3f Forward();
	static Vector3f Right();
	static Vector3f Up();
	static Vector3f One();
	static Vector3f Zero();

	// Are two vectors equal within a threshold?				
	// u.Equals(v)
	bool Equals( const Vector3f& other, float epsilon = 0.001f ) const;

	// Convert to String 
	std::string	ToString( ) const;

	// DotProduct
	// float d = u.DotProduct(v);
	float DotProduct( const Vector3f& other ) const;

	// CrossProduct 
	Vector3f CrossProduct( const Vector3f& other ) const;
	
	// Norm of a vector 
	// float l = v.Norm();
	float Norm( ) const;

	// Length of a vector: 
	// float l = v.Length();
	float Length( ) const;

	// Square Length of a vector.
	// Faster alternative for Length, sqrt is not executed. 
	float SquaredLength( ) const;
	
	//// AngleWith returns the smallest angle with another vector within the range [-PI/2, PI/2]. 
	//// A pos angle is counter clockwise from this to the other
	//// float angle = u.AngleWith(v);
	//float AngleWith( const Vector3f& other ) const;


	// Returns normalized form of a vector
	// Vector3f n = v.Normalized();
	Vector3f Normalized( float epsilon = 0.001f ) const;

	//// Returns the orthogonal of the Vector3f
	//// Vector3f w = v.Orthogonal();
	//Vector3f Orthogonal( ) const;

	//// Returns a vector that is the reflection of the Vector3f
	//// surfaceNormal: represents the normal of the surface at the reflection point
	//Vector3f Reflect( const Vector3f& surfaceNormal ) const;

	// Sets the values of x and y
	void Set(float newX, float newY, float newZ);

	// -------------------------
	// Datamembers 
	// -------------------------
	float x;
	float y;
	float z;
};
// -------------------------
// Non member operators
// -------------------------
Vector3f operator*( float lhs, Vector3f rhs );
Vector3f operator*( Vector3f lhs, float rhs );
Vector3f operator/( Vector3f lhs, float rhs );

Vector3f operator+( Vector3f lhs, const Vector3f& rhs );
Vector3f operator-( Vector3f lhs, const Vector3f& rhs );

bool operator==( const Vector3f& lhs, const Vector3f& rhs );
bool operator!=( const  Vector3f& lhs, const Vector3f& rhs );
std::ostream& operator<< ( std::ostream& lhs, const Vector3f& rhs );

