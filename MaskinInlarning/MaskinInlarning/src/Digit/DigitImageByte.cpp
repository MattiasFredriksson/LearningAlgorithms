#include "Digit/DigitImageByte.h"
#include "Digit/Contour.h"
#include <iostream>
#include <memory>
#include <algorithm>
#include<assert.h>

/* Generate a grid array of bits determining which pixels are contour pixels.
*/
std::unique_ptr<char> contourPixels(DigitImageByte &img, unsigned char threshold);

DigitImageByte::DigitImageByte() 
	: _pixels(NULL), _width(0), _height(0) {}
DigitImageByte::DigitImageByte(size_t dimX, size_t dimY) 
	: _pixels(new unsigned char[dimX * dimY]), _width(dimX), _height(dimY) {}
DigitImageByte::~DigitImageByte() { delete[] _pixels; }

DigitImageByte::DigitImageByte(const DigitImageByte &copy)
	: _pixels(new unsigned char[copy.size()]), _width(copy._width), _height(copy._height) 
{
	for (size_t i = 0; i < copy.size(); i++)
		_pixels[i] = copy._pixels[i];
}

void DigitImageByte::print(unsigned char threshold, int mode)
{

	for (size_t col = 0; col < _width; col++)
		std::cout << '_';
	std::cout << '\n';

	if (mode == 0)
		printPixels(threshold, true);
	else if (mode == 1)
		printContour(threshold);
	else
		printPixels(threshold, false);

	for (size_t col = 0; col < _width; col++)
		std::cout << '_';
	std::cout << '\n';
}

void DigitImageByte::printContour(unsigned char threshold)
{
	std::unique_ptr<char> contourPx = contourPixels(*this, threshold);
	for (size_t row = 0; row < _height; row++)
	{
		for (size_t col = 0; col < _width; col++)
		{
			char p = contourPx.get()[col + row*_width];
			std::cout << (p ? '@' : ' ');
		}
		std::cout << std::endl;
	}
}

void DigitImageByte::printPixels(unsigned char threshold, int binary)
{
	for (size_t row = 0; row < _height; row++)
	{
		for (size_t col = 0; col < _width; col++)
		{
			unsigned char p = _pixels[col + row*_width];
			if (binary == 1)
				std::cout << (p > threshold ? '@' : (p > threshold >> 1 ? '.' : ' '));
			else
				std::cout << p;
		}
		std::cout << std::endl;
	}
}

Point DigitImageByte::invertY(Point p)
{
	return Point(p.X, _height - p.Y);
}
unsigned char DigitImageByte::pixel(int x, int y)
{
	if (x < 0 || y < 0 || x >= (int)_width || y >= (int)_height)
		return 0;
	return _pixels[index(x, y)];
}

bool DigitImageByte::unNormalized()
{
	int c = 0;
	for (unsigned int i = 0; i < size(); i++)
		c += (int)(_pixels[i] == 255 || _pixels[i] == 0);
	return c == 0; // True if not in interval [0, 255]
}

void DigitImageByte::normalize()
{
	unsigned char min = 255, max = 0;
	for (unsigned int i = 0; i < size(); i++)
	{
		min = std::min(min, _pixels[i]);
		max = std::max(max, _pixels[i]);
	}

	if (min == 0 && max == 255) return;

	float begin = (float)min;
	float end = (float)max;
	for (unsigned int i = 0; i < size(); i++)
	{
		_pixels[i] = (unsigned char)(((float)_pixels[i] - begin) / (end - begin) * 255);
	}
}


KernelByte DigitImageByte::getKernel(int x, int y, int kernel_dim)
{
	return KernelByte(_pixels, x, y, _width, kernel_dim);
}


std::vector<KernelByte> DigitImageByte::genKernels(int dim)
{
	//if (_width % dim > 0 || _height % dim > 0) std::cout << "Warning: Kernels not aligned with image, excluding border.\n";
	
	//Floor div
	int num_w = _width / dim, num_h = _height / dim;
	std::vector<KernelByte> kernels(num_w*num_h);
	
	int i = 0;
	for (int y = 0; y < num_h * dim; y += dim)
	{
		for (int x = 0; x < num_w * dim; x += dim)
			kernels[i++] = getKernel(x, y, dim);
	}
	return kernels;
}

/*
struct ContourPixel
{
public:
	// Contour pixel id and index
	int _ID, _index;
	ContourPixel() : _ID(-1), _index(-1) {}
	ContourPixel(int ID, int ind) : _ID(ID), _index(ind) {}
};
struct ContourImage
{
public:
	std::vector<std::vector<Point>> _lines;
	std::shared_ptr<ContourPixel> _map;
	ContourImage(std::vector<std::vector<Point>> &lines, ContourPixel *contour_map) : _lines(lines), _map(contour_map) {}
};
ContourImage DigitImageByte::genContour(unsigned char threshold)
{
	std::vector<std::vector<Point>> lines = findContour(*this, threshold);
	ContourPixel *img = new ContourPixel[size()];
	memset(img, 0, sizeof(ContourPixel)*size());

	for (unsigned int i = 0; i < lines.size(); i++)
	{
		std::vector<Point> &p = lines[i];
		for (unsigned int ii = 0; ii < lines.size(); ii++)
		{
			int ind = index(p[ii]);
			img[ind] = ContourPixel(i, ii);
		}
	}
}

*/

