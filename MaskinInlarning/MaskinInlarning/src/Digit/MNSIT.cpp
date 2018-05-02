#include "Digit/MNIST.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <algorithm>
#include<assert.h>
#include "Log.h"
#include "Digit/KernelFunction.h"


MNIST::MNIST(int load_data)
{
	if (load_data == 1)
	{
		readImg("train-images.idx3-ubyte");
		readLabel("train-labels.idx1-ubyte");
	}
	else if (load_data == 2)
	{
		readImg("t10k-images.idx3-ubyte");
		readLabel("t10k-labels.idx1-ubyte");
	}
}

MNIST::~MNIST()
{
	for (unsigned int i = 0; i < _data.size(); i++)
		delete _data[i];
}

MNIST::MNIST(MNIST&& copy)
	: _labels(std::move(copy._labels)), _data(std::move(copy._data))
{
	_labels = copy._labels;
	_data = copy._data;
}


MNIST& MNIST::operator=(MNIST&& copy)
{
	if (&copy == this) return *this;
	for (unsigned int i = 0; i < _data.size(); i++)
		delete _data[i];
	_labels = std::move(copy._labels);
	_data = std::move(copy._data);
	return *this;
}



void MNIST::print(int index, int mode)
{
	std::cout << "Image: " << index << "\n";
	std::cout << "Digit: " << (int)_labels[index] << "\n";
	_data[index]->print(mode);
}

void MNIST::readLabel(const std::string& file)
{
	std::ifstream stream(file, std::ifstream::binary | std::ifstream::in);
	if (stream.is_open())
	{
		// Magic number
		unsigned char m0 = stream.get();
		unsigned char m1 = stream.get();
		unsigned char m2 = stream.get();
		unsigned char m3 = stream.get();

		int num_labels = ((int)stream.get() << 24) + ((int)stream.get() << 16) + ((int)stream.get() << 8) + stream.get();

		_labels.resize(num_labels);
		stream.read(reinterpret_cast<char*>(_labels.data()), num_labels);
		assert(stream.peek() == EOF);
		std::cout << "Loaded MNIST: " << num_labels << " labels\n";
	}
	else
		std::cout << "MNIST file: " << file.c_str() << " could not be opened.\n";
}

void MNIST::readImg(const std::string& file)
{

	std::ifstream stream(file, std::ifstream::binary | std::ifstream::in);
	if (stream.is_open())
	{
		// Magic number
		unsigned char m0 = stream.get();
		unsigned char m1 = stream.get();
		unsigned char m2 = stream.get();
		unsigned char m3 = stream.get();

		// Data dimensions
		assert(m3 == 3);
		int dims[3] = { 0,0,0 };
		for (unsigned char i = 0; i < m3; i++)
			dims[i] = ((int)stream.get() << 24) + ((int)stream.get() << 16) + ((int)stream.get() << 8) + stream.get();
 		
		//Img data
		_data.reserve(dims[0]);
		for (int i = 0; i < dims[0]; i++)
		{
			std::unique_ptr<DigitImageByte> img(new DigitImageByte(IMG_WIDTH, IMG_HEIGHT));
			stream.read(reinterpret_cast<char*>(img->_pixels), IMG_WIDTH * IMG_HEIGHT);
			_data.push_back(img.release());
		}
		assert(stream.peek() == EOF);
		std::cout << "Loaded MNIST: " << _data.size() << " images\n";
	}
	else
		std::cout << "MNIST file: " << file.c_str() << " could not be opened.\n";
}

void MNIST::normalize()
{
	for (unsigned int i = 0; i < size(); i++)
		_data[i]->normalize();
}


void MNIST::pixelHistogram(const std::string &file)
{
	unsigned int distr[256];
	float distr_norm[256];
	for (size_t i = 0; i < 256; i++) distr[i] = 0;
	for (size_t i = 0; i < size(); i++)
		_data[i]->histogram(distr);

	Log out(file, true, false, false);
	out << "Distribution:\n";
	for (size_t i = 0; i < 256; i++)
		out << distr[i] << (i < 255 ? ", " : "\n");

	out << "Normalized:\n";
	for (size_t i = 0; i < 256; i++)
	{
		distr_norm[i] = distr[i] / ((float)_data.size()*IMG_WIDTH*IMG_HEIGHT);
		out << distr_norm[i] << (i < 255 ? ", " : "\n");
	}
}


void MNIST::printFeatures(const std::string &file, size_t img_from, size_t img_to)
{
	outputFeatures(*this, file, 2, img_from, img_to);
}
void MNIST::printLabels(const std::string &file, size_t img_from, size_t img_to)
{;
	if (img_to <= img_from) img_to = _labels.size();
	std::stringstream label;
	label << (int)_labels[img_from];
	for (size_t i = img_from + 1; i < _labels.size(); i++)
		label << "," << (int)_labels[i];

	Log l(file, true, false, false);
	l.logMsg(label);
	std::cout << "Generated label file\n";
}