#include "Digit/KernelByte.h"
#include "DisjointSet.h"
#include "Digit/DigitImageByte.h"
#include "Digit/Contour.h"
#include <iostream>
#include <algorithm>
#include<assert.h>
#include<memory>



KernelByte::KernelByte()
	: _kernel(NULL), _dimX(0), _dimY(0), _source_X(0), _source_Y(0), _source_cols(0)
{}
KernelByte::KernelByte(unsigned char * source, unsigned int X, unsigned int Y, int img_width, int dim)
	: _kernel(&source[Y*img_width + X]), _dimX(dim), _dimY(dim), _source_X(X), _source_Y(Y), _source_cols(img_width)
{}

unsigned char& KernelByte::operator[](size_t index)
{
	std::ldiv_t d = std::div(index, (long)_dimX);
	return _kernel[d.rem + d.quot * _source_cols];
}
unsigned char& KernelByte::get(size_t x, size_t y)
{
	assert(x < _dimX);
	assert(y < _dimY);
	return _kernel[x + y * _source_cols];
}

bool KernelByte::get(int x, int y, unsigned char*& value)
{
	if (x < 0 || y < 0 || x >= (int)_dimX || y >= (int)_dimY)
		return false;
	value = &get(x, y);
	return true;
}


int KernelByte::getKernelIndex(int x, int y)
{
	return x + y * _dimX;
}
int KernelByte::getKernelIndex(Point p) { return getKernelIndex(p.X, p.Y); };

/* Get the pixels point in the image. */
Point KernelByte::getPoint(size_t index)
{
	std::ldiv_t d = std::div(index, (long)_dimX);
	return Point(_source_X + d.rem, _source_Y + d.quot);
}
/* Get image index*/
int KernelByte::toImgIndex(int index)
{
	std::div_t d = std::div(index, _dimX);
	return (_source_X + d.rem) + (_source_Y + d.quot) * _source_cols;
}
/* Translate the kernel point indices to pixels point in the image. */
Point KernelByte::getPoint(Point kernelIndices)
{
	return Point(_source_X + kernelIndices.X, _source_Y + kernelIndices.Y);
}

DisjointSets KernelByte::disjointSets(unsigned char threshold)
{
	DisjointSets set(size());
	for (size_t y = 0; y < _dimY; y++)
	{
		for (size_t x = 0; x < _dimX; x++)
		{
			int p0 = y * _dimX + x;
			int p1 = y * _dimX + x + 1;
			int p2 = (y + 1) * _dimX + x;

			int type = operator[](p0)  > threshold;

			if (x < _dimX - 1 && ((operator[](p1) > threshold) == type)) // Horizontal edges
				set.connect(p0, p1);

			if (y < _dimY - 1 && ((operator[](p2) > threshold) == type)) // Vertical edges
				set.connect(p0, p2);
		}
	}
	return set;
}
DisjointSets KernelByte::disjointSets(unsigned char threshold, unsigned char *edges)
{
	DisjointSets set(size());
	for (size_t y = 0; y < _dimY; y++)
	{
		for (size_t x = 0; x < _dimX; x++)
		{
			int p0 = y * _dimX + x;
			int type = operator[](p0)  > threshold;
			// Check horizontal edge
			if (hasflag(edges[p0], EdgeBit::EDGE_R))
			{
				int p1 = y * _dimX + x + 1;
				if (x < _dimX - 1 && ((operator[](p1) > threshold) == type))
					set.connect(p0, p1);
			}
			// Check vertical edges
			if (hasflag(edges[p0], EdgeBit::EDGE_DOWN))
			{
				int p2 = (y + 1) * _dimX + x;
				if (y < _dimY - 1 && ((operator[](p2) > threshold) == type)) 
					set.connect(p0, p2);
			}
		}
	}
	return set;
}
std::vector<std::vector<int>> KernelByte::segmentPositive(unsigned char threshold, DisjointSets &disjoint)
{
	std::vector<std::vector<int>> indices = disjoint.getSets(0);
	std::vector<std::vector<int>> set;

	for (size_t i = 0; i < indices.size(); i++)
	{
		int indice = indices[i][0];
		int val = operator[](indice);
		if (val > threshold) //Verify positive set
		{
			disjoint.renameIndex(indice, set.size());
			set.push_back(std::move(indices[i]));
		}
	}
	return set;

}
std::vector<std::vector<int>> KernelByte::segmentPositive(int threshold)
{
	DisjointSets disjoint = disjointSets(threshold);
	return segmentPositive(threshold, disjoint);
}
std::vector<std::vector<int>> KernelByte::segmentPositive(int threshold, unsigned char *edges)
{
	DisjointSets disjoint = disjointSets(threshold, edges);
	return segmentPositive(threshold, disjoint);
}


const int NUM_EDGE = 12;
const int EDGE_IND[NUM_EDGE] = { 0,1,2,3,4,7,8,11,12,13,14,15 };



void KernelByte::binary(char *set, unsigned char threshold)
{
	for (unsigned int i = 0; i < size(); i++)
		set[i] = ispixel(operator[](i), threshold);
}




/*
edge		<< Edge evaluated.
other		<< value of other 2x2 edge
con_adj		<< value of the other edge for the weak edge.
opposite	<< value of the edge between the opposite 2x2 squares.
*/
bool mergeEdge(char num_pxl, char edge, char other, char con_adj, char opposite)
{
	// Do not merge if not connected!
	if (edge == 0)
		return false;
	// Merge all 'strong' connections
	if (edge == 2)
		return true;
	// If single pixel.
	if (num_pxl == 1)
		return true;
	// 2 Connect: If only 2 related
	if (con_adj == 0 && other == 0)
		return true;
	// 2 Connect: Merge 2 weak if the opposite side is strong.
	if (other == 0 && con_adj == 1 && opposite == 2 )
		return true;
	// 3 connect: If weak connected x2 and all other strong
	if (con_adj == 2 && opposite == 2 && other == 1)
		return true;
	// 3 connect: If other axis has only strong connections.
	if (con_adj == 2 && other == 2 && opposite == 1)
		return true;
	// 4 connect - If all other are strong:
	if (con_adj == 2 && opposite == 2 && other == 2)
		return true;
	// 4 connect - If all 4 connected (not 2x2 square empty use: num_pixel < 12):
	//if (opposite > 0 && con_adj > 0 && other > 0)
	//	return true;
	return false;
}

char count2x2(char*binary, int ind)
{
	return binary[ind] + binary[ind+1] + binary[ind+4] + binary[ind+5];
}
/* Generate a edge cut, setting all edges cut to 0
*/
std::unique_ptr<unsigned char> generateEdgeCut4x4(KernelByte &kernel, unsigned char threshold)
{
	char binary[16];
	kernel.binary(binary, threshold);

	// Find connections along center axis:
	int num_0 = count2x2(binary, 0), num_1 = count2x2(binary, 2), num_2 = count2x2(binary, 8), num_3 = count2x2(binary, 10);
	bool cx[4], cy[4];
	for (int i = 0; i < 4; i++)
		cx[i] = binary[i + 4] && binary[i + 8] ? 1 : 0;
	for (int i = 0; i < 4; i++)
		cy[i] = (binary[i * 4 + 1] && binary[i * 4 + 2] ? 1 : 0);
	int val0 = cx[0] + cx[1];
	int val2 = cy[0] + cy[1];

	//If 2x2 connected
	char cx0 = cx[0] + cx[1], cx1 = cx[2] + cx[3];
	char cy0 = cy[0] + cy[1], cy1 = cy[2] + cy[3];

	// Find connected entities:
	bool m01, m02, m13, m23;
	//m01 || m10
	m01 = mergeEdge(num_0, cy0, cx0, cx1, cy1) || mergeEdge(num_1, cy0, cx1, cx0, cy1);
	//m02 || m20
	m02 = mergeEdge(num_0, cx0, cy0, cy1, cx1) || mergeEdge(num_2, cx0, cy1, cy0, cx1);
	//m13 || m31
	m13 = mergeEdge(num_1, cx1, cy0, cy1, cx0) || mergeEdge(num_3, cx1, cy1, cy0, cx0);
	//m23 || m32
	m23 = mergeEdge(num_2, cy1, cx0, cx1, cy0) || mergeEdge(num_3, cy1, cx1, cx0, cy0);

	unsigned char* edge = new unsigned char[16];
	memset(edge, UINT8_MAX, 16);

	if (!m01)
	{
		clearflag_r(edge[1], EdgeBit::EDGE_R);
		clearflag_r(edge[2], EdgeBit::EDGE_L);
		clearflag_r(edge[5], EdgeBit::EDGE_R);
		clearflag_r(edge[6], EdgeBit::EDGE_L);
	}if (!m23)
	{
		clearflag_r(edge[9], EdgeBit::EDGE_R);
		clearflag_r(edge[10], EdgeBit::EDGE_L);
		clearflag_r(edge[13], EdgeBit::EDGE_R);
		clearflag_r(edge[14], EdgeBit::EDGE_L);
	}
	if (!m02)
	{
		clearflag_r(edge[4], EdgeBit::EDGE_DOWN);
		clearflag_r(edge[8], EdgeBit::EDGE_UP);
		clearflag_r(edge[5], EdgeBit::EDGE_DOWN);
		clearflag_r(edge[9], EdgeBit::EDGE_UP);
	}
	if (!m13)
	{
		clearflag_r(edge[6], EdgeBit::EDGE_DOWN);
		clearflag_r(edge[10], EdgeBit::EDGE_UP);
		clearflag_r(edge[7], EdgeBit::EDGE_DOWN);
		clearflag_r(edge[11], EdgeBit::EDGE_UP);
	}
	return std::unique_ptr<unsigned char>(edge);
}