#include "Digit/Quad.h"




Quad::Quad(Vector p0, Vector p1, Vector p2, Vector p3)
	: _p0(p0), _p1(p1), _p2(p2), _p3(p3)
{
	// Find greatest axis:
	_major = p1 - p0;
	_mlen = _major.length_sq();
	// p0 axis
	Vector tmp = p2 - p0;
	float len = tmp.length_sq();
	if (len < _mlen) { _major = tmp; _mlen = len; }
	tmp = p3 - p0;  len = tmp.length_sq();
	if (len < _mlen) {	_major = tmp; _mlen = len;	}

	// p1 axis
	tmp = p2 - p1; len = tmp.length_sq();
	if (len < _mlen) { _major = tmp; _mlen = len; }
	tmp = p3 - p1;len = tmp.length_sq();
	if (len < _mlen) { _major = tmp; _mlen = len; }

	_mlen = std::sqrt(_mlen);
	_major /= _mlen;
}
