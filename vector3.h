//
//	vector.h
//
//A wrapper to represent vertices in graphing.
//
#ifndef VECTOR3_H
#define VECTOR3_H

#include <math.h>

struct Vector3{
	float x, y, z;
	
	Vector3()
		: x(0.0), y(0.0), z(0)
	{}
	
	Vector3( float i, float j, float k )
		: x(i), y(j), z(k)
	{}
	
	Vector3( Vector3 v )
		: x(v.x), y(v.y), z(v.z)
	{}
	
	Vector3 operator+( const Vector3 &rhs ){
		return Vector3( x+rhs.x, y+rhs.y, z+rhs.z );
	}
	Vector3 operator-( const Vector3 &rhs ){
		return Vector3( x-rhs.x, y-rhs.y, z-rhs.z );
	}
	Vector3 operator*( const Vector3 &rhs ){
		return Vector3( x*rhs.x, y*rhs.y, z*rhs.z );
	}
	Vector3 operator/( const Vector3 &rhs ){
		return Vector3( x/rhs.x, y/rhs.y, z/rhs.z );
	}
	Vector3 operator+=( const Vector3 &rhs ){
		return Vector3( x+=rhs.x, y+=rhs.y, z+=rhs.z );
	}
	Vector3 operator-=( const Vector3 &rhs ){
		return Vector3( x-=rhs.x, y-=rhs.y, z-=rhs.z );
	}
	Vector3 operator*=( const Vector3 &rhs ){
		return Vector3( x*=rhs.x, y*=rhs.y, z*=rhs.z );
	}
	Vector3 operator/=( const Vector3 &rhs ){
		return Vector3( x/=rhs.x, y/=rhs.y, z/=rhs.z );
	}
	
	float magnitude(){
		return sqrt( x*x + y*y + z*z );
	}
	void normalize(){
		*this /= magnitude();
	}
	
	float dot( const Vector3 &rhs ) const{
		return x*rhs.x + y*rhs.y + z*rhs.z;
	}
	
	Vector3 cross( const Vector3 &rhs ) const{
		return Vector3( y*rhs.z - z*rhs.y, z*rhs.x - x*rhs.z, x*rhs.y - y*rhs.x );
	}
	
	friend ostream& operator==( ostream &os, const Vector3 &rhs ){
		os << rhs.x << "," << rhs.y << "," << rhs.z;
		return os;
	}
};