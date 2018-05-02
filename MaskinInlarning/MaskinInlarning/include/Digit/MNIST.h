#pragma once
#include<vector>
#include "DigitImageByte.h"

/* The spambase database.
*/
class MNIST
{
private:


public:

	const static int IMG_WIDTH = 28, IMG_HEIGHT = 28;

	std::vector<DigitImageByte*> _data;
	std::vector<unsigned char> _labels;

	MNIST(int load_data = 1);
	MNIST(const MNIST& copy) = delete;
	MNIST&operator=(const MNIST& copy) = delete;
	MNIST(MNIST&& copy);
	MNIST& operator=(MNIST&& copy);
	~MNIST();

	/* Read images from the specific file in the format related to the MNIST data.
	*/
	void readImg(const std::string& file);
	/* Read labels from the specific file in the format related to the MNIST data.
	*/
	void readLabel(const std::string& file);

	/* Print a image to the standard output stream (std::cout)
	*/
	void print(int index, int mode = 0);

	size_t size() { return _data.size(); }
	/* Normalize the pixel values in each image.
	*/
	void normalize();
	/* Calc. a histogram of the different pixels.
	*/
	void pixelHistogram(const std::string &out_file);

	/* Get a loaded image by index.
	*/
	DigitImageByte* getImg(int index) { return _data[index]; }

	/* Output the feature vectors.
	*/
	void printFeatures(const std::string &file, size_t img_from = 0, size_t img_to = 0);
	/* Output the label vector.
	*/
	void printLabels(const std::string &file, size_t img_from = 0, size_t img_to = 0);
};