#pragma once
#include "Point.h"
class DigitImageByte;

class BinaryImage
{
private:

public:
	char *_pixels;
	size_t _width, _height;

	BinaryImage();
	BinaryImage(DigitImageByte *img, unsigned char threshold, bool invertY);
	BinaryImage(const BinaryImage &copy);
	~BinaryImage();

	size_t size() const { return _width * _height; }
	int index(int x, int y) { return x + y * _width; };
	int index(Point p) { return p.X + p.Y * _width; };
	bool inBounds(int x, int y) { return x >= 0 && y >= 0 && x < (int)_width && y < (int)_height; }
	char pixel(int x, int y);
};