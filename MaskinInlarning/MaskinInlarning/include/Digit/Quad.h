#pragma once
#include "Vector.h"


struct Quad
{
public:
	Vector _p0, _p1, _p2, _p3;
	/* Normalized major axis. */
	Vector _major;
	/* Length of the major axis. */
	float _mlen;

	Quad(Vector p0, Vector p1, Vector p2, Vector p3);


};