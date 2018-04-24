#pragma once
#include <math.h>
#include <algorithm>
struct Point
{
	int X, Y;
	Point() : X(0), Y(0) {}
	Point(int X, int Y) : X(X), Y(Y) {}
	Point(int XY) : X(XY), Y(XY) {}
	Point operator-() { return{ -X, -Y }; }
	Point operator+() { return{ X, Y }; }
	Point operator-(Point p)
	{
		return Point(X - p.X, Y - p.Y);
	}
	Point operator+(Point p)
	{
		return Point(X + p.X, Y + p.Y);
	}
	Point& operator-=(Point p)
	{
		X -= p.X; Y -= p.Y;
		return *this;
	}
	Point& operator+=(Point p)
	{
		X += p.X; Y += p.Y;
		return *this;
	}
	bool operator==(Point p) { return X == p.X && Y == p.Y; }
}; 
