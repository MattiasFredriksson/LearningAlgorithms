#include "Digit/Contour.h"
#include <assert.h>

#pragma region Contour generation

void cwStep(int &x, int &y);
void ccwStep(int &x, int &y);
Point getNextEdge(DigitImageByte &img, char* contour, Point cur, Point diff, int threshold);
Point adjacentNeg(DigitImageByte &img, int x, int y, int threshold);


/* Search from a starting point (which is a pixel belonging to the contour).
*/
std::vector<Point> traceContour(DigitImageByte &img, char *contour, int x, int y, int threshold);

/* Find all separated contours in the kernel.
*/
std::vector<std::vector<Point>> findContour(DigitImageByte &img, int threshold)
{
	std::unique_ptr<char> contour = contourPixels(img, threshold);
	std::vector<std::vector<Point>> contourList;
	std::unique_ptr<bool> visited(new bool[img.size()]);
	memset(visited.get(), 0, img.size());

	//Iterate over all pixels and find starting point(s) for the contours.
	for (unsigned int y = 0; y < img._height; y++)
	{
		for (unsigned int x = 0; x < img._width; x++)
		{
			int index = img.index(x, y);
			if (contour.get()[index] && !visited.get()[index])
			{
				std::vector<Point> edge = traceContour(img, contour.get(), x, y, threshold);
				for (unsigned int i = 0; i < edge.size(); i++)
					visited.get()[img.index(edge[i])] = true;
				contourList.push_back(std::move(edge));
			}
		}
	}

	return contourList;
}

/* Search from a starting point (which is a pixel belonging to the contour).
*/
std::vector<Point> traceContour(DigitImageByte &img, char *contour, int x, int y, int threshold)
{
	Point point(x, y), off = adjacentNeg(img, x, y, threshold);
	std::vector<Point> list;
	list.push_back(point);
	// Search in a direction, decided through start the steps from an adjacent negative or 'empty' pixel point.
	while (true)
	{
		off = getNextEdge(img, contour, point, off, threshold);
		if (off.X == -2) break; //Termination case, occurs when the pixel is not connected.
		point += off;
		off = -off;
		list.push_back(point);
		// Loop backed to our initial pixel.
		if (point == list[0])
		{
			// Verify that it did not loop back to a contour edge of single pixel width.
			// Check is performed that there exists a shared adjacent pixel which is the
			// next in clockwise order (the pixel in ccw order is known to be negative!).
			off = getNextEdge(img, contour, point, off, threshold);
			point += off;
			off = -off;
			if (point == list[1])
				break; // Terminate
			//If it's not the same direction continue!
			list.push_back(point);
		}

		assert(list.size() < img.size());
	}
	return list;
}
Point getNextEdge(DigitImageByte &img, char* contour, Point cur, Point diff, int threshold)
{
	/*	The first pixel 3x3 adjacent to current pixel in cw or ccw order is the next contour pixel.
	*	Order is determined by the alignment of the positive & negative field to the last contour segment.
	*/

	// 3x3 Kernel:
	for (int i = 0; i < 8; i++)
	{
		cwStep(diff.X, diff.Y);
		Point p = cur + diff;
		// If stepping in direction results in pixel out of bounds or not a pixel that is not a contour: terminate.
		if (img.inBounds(p.X, p.Y) && contour[img.index(p)])
		{
			// Verify that the pixel belongs to the same contour.
			// Performed by checking that they share atleast one adjacent pixel.
			if (abs(diff.X) + abs(diff.Y) == 2)
			{
				Point off = diff;
				cwStep(off.X, off.Y);
				if (notpixel(img.pixel(off.X+cur.X, off.Y+cur.Y), threshold))
					continue;
			}
			return diff;
		}
	}
	// The pixel is not connected to another (set of a single element).
	return Point(-2);
}
/* Generate a grid array of bits determining which pixels are contour pixels.
*/
std::unique_ptr<char> contourPixels(DigitImageByte &img, unsigned char threshold)
{

#define ISPIXEL(index_v) ispixel(img._pixels[index_v], threshold)
#define NOTPIXEL(index_v) notpixel(img._pixels[index_v], threshold)
	char* ptr = new char[img.size()];
	std::memset(ptr, 0, img.size());
	//First & last row, if it is a pixel it's also a contour (on edges)!
	int ind = img.size() - img._width;
	// Rows:
	for (int i = 0; i < (int)img._width; i++)
	{
		ptr[i] = ISPIXEL(i); //First
		int ii = i + ind;
		ptr[ii] = ISPIXEL(ii); // Last
	}

	//First & last columns:
	for (int i = img._width; i < (int)img._height - 1; i += img._width)
	{
		ptr[i] = ISPIXEL(i); // First
		int ii = i + img._width - 1;
		ptr[ii] = ISPIXEL(ii); // Last
	}

	//Center
	int last = ind - 1;
	for (int i = img._width + 1; i < last; i += 2)
	{
		int end = i + img._width - 2;
		for (; i < end; i++)
			ptr[i] = ISPIXEL(i) && (NOTPIXEL(i + 1) || NOTPIXEL(i - 1) || NOTPIXEL(i + img._width) || NOTPIXEL(i - img._width));
	}
	return std::unique_ptr<char>(ptr);
}

/*Find a connected negative pixel offset for a contour pixel. */
Point adjacentNeg(DigitImageByte &img, int x, int y, int threshold)
{
	if (notpixel(img.pixel(x + 1, y), threshold))
		return Point(1, 0);
	if (notpixel(img.pixel(x - 1, y), threshold))
		return Point(-1, 0);
	if (notpixel(img.pixel(x, y + 1), threshold))
		return Point(0, 1);
	return Point(0, -1); // Searching a contour pixel so last must be empty!...
}
void cwStep(int &x, int &y)
{
	// Clockwise stepping where -Y is up & X is right
	if (y == -1)
	{
		if (x == 1)
			y++;
		else
			x++;
	}
	else if (y == 0)
		y += x > 0 ? 1 : -1;
	else //y == 1
	{
		if (x == -1)
			y--;
		else
			x--;
	}
}
void ccwStep(int &x, int &y)
{
	// Counter-Clockwise stepping where -Y is up & X is right
	if (y == -1)
	{
		if (x == -1)
			y++;
		else
			x--;
	}
	else if (y == 0)
		y += x > 0 ? -1 : 1;
	else //y == 1
	{
		if (x == 1)
			y--;
		else
			x++;
	}
}

#pragma endregion


#pragma region Contour segmentation


/* Convert a segment point difference to an angle.*/
int toAngle(Point diff)
{
	if (diff.Y == 1)
	{
		if (diff.X == 0)
			return 90;
		else
			return diff.X == -1 ? 120 : 45;
	}
	else if (diff.Y == -1)
	{
		if (diff.X == 0)
			return 270;
		else
			return (diff.X == -1 ? 225: 315);
	}
	else
		return diff.X == 1 ? 0 : 180;
}

int angleDiff(int a0, int a1)
{
	int lap = a1 - (360 - a0);
	return lap < -180 ? lap : a1 - a0;
}
struct ContourSegment
{
	float _X, _Y, _X1, _Y1;
};
/* Segment a contour.
*/
void segmentContour(std::vector<Point> &contour)
{
	if (contour.size() > 2) return;
	int a0 = toAngle(contour[1] - contour[0]);
	int diff = 0;
	int step = 0;
	for (unsigned int i = 2; i < contour.size(); i++)
	{
		int a = toAngle(contour[i] - contour[i - 1]);
		diff += angleDiff(a, a0);
		if (diff != 0)
		{
			if (++step == 2)
			{
				//Create segment point

			}
		}
		else
			step = 0;
	}
}

void segmentContour(DigitImageByte &img, unsigned char threshold)
{
	std::vector<std::vector<Point>> contour = findContour(img, threshold);

	for (unsigned int i = 0; i < contour.size(); i++)
	{
		segmentContour(contour[i]);
	}
}

#pragma endregion