#pragma once
#include "KernelByte.h"
#include <vector>


class DigitImageByte
{
private:

	void printContour(unsigned char threshold);
	void printPixels(unsigned char threshold, int binary);
public:
	unsigned char *_pixels;
	size_t _width, _height;

	DigitImageByte();
	DigitImageByte(size_t dimX, size_t dimY);
	DigitImageByte(const DigitImageByte &copy);
	DigitImageByte& operator=(const DigitImageByte &copy);
	~DigitImageByte();

	void print(int mode = 0);

	KernelByte getKernel(int x, int y, int kernel_dim);
	std::vector<KernelByte> genKernels(int dim);

	/* Get number of pixels in the image
	*/
	size_t size() const { return _width * _height; }
	int index(int x, int y) { return x + y * _width; };
	int index(Point p) { return p.X + p.Y * _width; };
	bool inBounds(int x, int y) { return x >= 0 && y >= 0 && x < (int)_width && y < (int)_height; }
	unsigned char pixel(int x, int y);
	
	Point invertY(Point p);

	bool unNormalized();
	void normalize();

	std::shared_ptr<unsigned int> histogram();
	void histogram(unsigned int histo[256]);
	unsigned char threshold_Otsu();
};

