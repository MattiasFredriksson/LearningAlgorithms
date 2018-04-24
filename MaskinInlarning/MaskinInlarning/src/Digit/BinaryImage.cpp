#include "Digit/BinaryImage.h"
#include "Digit/DigitImageByte.h"



BinaryImage::BinaryImage()
	: _pixels(NULL), _width(0), _height(0) {}
BinaryImage::BinaryImage(DigitImageByte *img, unsigned char threshold, bool invertY)
	: _pixels(new char[img->size()]), _width(img->_width), _height(img->_height)
{
	if (invertY)
	{
		for (unsigned int y = 0; y < _height; y++)
		{
			for (unsigned int x = 0; x < _width; x++)
			{
				int i = x + y *_width;
				int ii = x + (_height - y - 1) * _width;
				_pixels[i] = ispixel(img->_pixels[ii], threshold);
			}
		}
	}
	else {
		for (unsigned int i = 0; i < size(); i++)
			_pixels[i] = ispixel(img->_pixels[i], threshold);
	}
}
BinaryImage::~BinaryImage() { delete[] _pixels;  }

BinaryImage::BinaryImage(const BinaryImage &copy)
	: _pixels(new char[copy.size()]), _width(copy._width), _height(copy._height)
{
	for (size_t i = 0; i < copy.size(); i++)
		_pixels[i] = copy._pixels[i];
}

char BinaryImage::pixel(int x, int y)
{
	if (x < 0 || y < 0 || x >= (int)_width || y >= (int)_height)
		return 0;
	return _pixels[index(x, y)];
}
