#include "Test.h"
#include <iostream>



void testPartialSVD()
{
	std::vector < Vector> M(8);
	M[0] = Vector(0,1);
	M[1] = Vector(0,2);
	M[2] = Vector(0,3);
	M[3] = Vector(1,4);
	M[4] = Vector(0,-1);
	M[5] = Vector(0,-2);
	M[6] = Vector(0,-3);
	M[7] = Vector(1,-5);

	Vector u = partial_SVD(M);
	std::cout << "U1: " << u.X << "," << u.Y << std::endl;
}