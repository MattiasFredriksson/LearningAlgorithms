#pragma once
#include "Point.h"

struct Vector
{
	float X, Y;
	Vector() : X(0), Y(0) {}
	Vector(float X, float Y) : X(X), Y(Y) {}
	Vector(Point p) : X((float)p.X), Y((float)p.Y) {}
	Vector(float XY) : X(XY), Y(XY) {}
	Vector operator-() { return{ -X, -Y }; }
	Vector operator+() { return{ X, Y }; }
	Vector operator-(Vector p)
	{
		return Vector(X - p.X, Y - p.Y);
	}
	Vector operator+(Vector p)
	{
		return Vector(X + p.X, Y + p.Y);
	}
	Vector operator+(Point p)
	{
		return Vector(X + p.X, Y + p.Y);
	}
	Vector operator/(float scalar)
	{
		return Vector(X / scalar, Y / scalar);
	}
	Vector operator*(float scalar)
	{
		return Vector(X * scalar, Y * scalar);
	}
	Vector& operator-=(Vector p)
	{
		X -= p.X; Y -= p.Y;
		return *this;
	}
	Vector& operator+=(Vector p)
	{
		X += p.X; Y += p.Y;
		return *this;
	}
	Vector& operator+=(Point p)
	{
		X += p.X; Y += p.Y;
		return *this;
	}
	Vector& operator/=(float scalar)
	{
		X /= scalar; Y /= scalar;
		return *this;
	}
	Vector& operator*=(float scalar)
	{
		X *= scalar; Y *= scalar;
		return *this;
	}
	bool operator==(Vector other) const
	{
		return X == other.X && Y == other.Y;
	}
	bool operator!=(Vector other) const
	{
		return X != other.X || Y != other.Y;
	}
	float dot(Vector& other)
	{
		return X * other.X + Y  * other.Y;
	}
	float length() { return std::sqrt(length_sq()); }
	/* Length square. */
	float length_sq() { return X*X + Y*Y; }
	void normalize() { float len = length(); X /= len; Y /= len; }
	/* Rotate counter clockwise 90 degrees. */
	inline Vector rotateCCW90() {
		return Vector(-Y, X);
	}
};