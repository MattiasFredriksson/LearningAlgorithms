#pragma once
#include "DisjointSet.h"
#include "Point.h"
#include <memory>

class ContourImage;

inline bool ispixel(unsigned char val, unsigned char threshold) { return val > threshold; };
inline bool notpixel(unsigned char val, unsigned char threshold) { return val <= threshold; };

inline unsigned char setflag(unsigned char flag, unsigned char index) { return flag |= 1u << index; }
inline void setflag_r(unsigned char& flag, unsigned char index) { flag |= 1u << index; }
inline unsigned char clearflag(unsigned char flag, unsigned char index) { return flag &= ~(1u << index); }
inline void clearflag_r(unsigned char &flag, unsigned char index) { flag &= ~(1u << index); }
inline bool hasflag(unsigned char flag, unsigned char index) { return (flag >> index) & 1u; }

enum EdgeBit 
{
	EDGE_R = 0,
	EDGE_DOWN = 1,
	EDGE_L = 2,
	EDGE_UP = 3
};

struct KernelByte
{
	// Kernel pointer to source data.
	unsigned char *_kernel;
	//Kernel dimension
	unsigned int _dimX, _dimY;
	// Coordinate of the kernel in the source image.
	unsigned int _source_X, _source_Y;
	// Columns in source grid
	int _source_cols;

	KernelByte();
	KernelByte(unsigned char * source, unsigned int X, unsigned int Y, int source_rows, int dim);

	unsigned char& operator[](size_t index);
	unsigned char& get(size_t x, size_t y);
	bool get(int x, int y, unsigned char *&value);
	int getKernelIndex(int x, int y);
	int getKernelIndex(Point p);
	/* Translate the kernel point indices to pixels point in the image. */
	Point getPoint(Point kernelIndices);
	/* Get the pixel coordinate in the image. */
	Point getPoint(size_t index);
	/* Get image index*/
	int toImgIndex(int index);

	size_t size() const { return _dimX * _dimY; }

	/* Segment the pixels in the kernel by the threshold, separating into 'positive' and 'negative' disjoint segments. */
	DisjointSets disjointSets(unsigned char threshold);
	DisjointSets disjointSets(unsigned char threshold, unsigned char *edges);

	/* Get the disjointed pixel sets in the kernel with values over the threshold. */
	std::vector<std::vector<int>> segmentPositive(int threshold);
	std::vector<std::vector<int>> segmentPositive(int threshold, unsigned char *edges);
	std::vector<std::vector<int>> segmentPositive(unsigned char threshold, DisjointSets &disjoint);


	/* Generate a binary representation of the kernel in the pointer. */
	void binary(char *set, unsigned char threshold);

};


/* Generate a edge cut, setting all edges cut to 0
*/
std::unique_ptr<unsigned char> generateEdgeCut4x4(KernelByte &kernel, unsigned char threshold);