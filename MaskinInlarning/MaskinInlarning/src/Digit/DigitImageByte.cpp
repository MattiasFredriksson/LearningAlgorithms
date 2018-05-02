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
DigitImageByte& DigitImageByte::operator=(const DigitImageByte &copy)
{
	if (this == &copy) return *this;
	delete[] _pixels;
	_width = copy._width;
	_height = copy._height;
	_pixels = new unsigned char[copy.size()];
	for (size_t i = 0; i < copy.size(); i++)
		_pixels[i] = copy._pixels[i];
	return *this;
}

#pragma region Print stuff

void DigitImageByte::print(int mode)
{

	for (size_t col = 0; col < _width; col++)
		std::cout << '_';
	std::cout << '\n';
	int threshold = (int)threshold_Otsu();
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

#pragma endregion

void DigitImageByte::histogram(unsigned int histo[256])
{
	for (size_t ii = 0; ii < size(); ii++)
		histo[_pixels[ii]]++;
}
std::shared_ptr<unsigned int> DigitImageByte::histogram()
{
	unsigned int *histo = new unsigned int[256];
	std::memset(histo, 0, 256);
	histogram(histo);
	return std::shared_ptr<unsigned int>(histo);
}


unsigned char DigitImageByte::threshold_Otsu()
{
	// Maximize between class variance using Otsu's method
	double N = size();
	std::shared_ptr<unsigned int> histo = histogram();

	double w0 = 0, w1 = 0;
	double u0, u1, uk = 0, uT = 0;
	for (unsigned int i = 0; i < 255; i++)
		uT += (i + 1)*histo.get()[i] / N;
	double max = 0;
	unsigned int lvl_threshold = 0;
	// Find max 'between class variance'
	for (unsigned int i = 0; i < 256; i++)
	{
		double pi = histo.get()[i] / N;
		w0 += pi;
		w1 = 1 - w0;
		uk += (i + 1) * pi;
		u0 = uk / w0;
		u1 = (uT - uk) / w1;
		// Calc. and compare 'between' variance
		double var_B = w0 * w1*(u1 - u0)*(u1 - u0);
		if (var_B > max)
		{
			lvl_threshold = i;
			max = var_B;
		}
	}
	// Return the threshold
	return lvl_threshold;
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
