#pragma once
#include "DigitImageByte.h"
#include <memory>


std::unique_ptr<char> contourPixels(DigitImageByte &img, unsigned char threshold);
std::vector<std::vector<Point>> findContour(DigitImageByte &img, int threshold);

