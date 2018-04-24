#pragma once
#include <vector>
#include "Point.h"
#include "Vector.h"

const float EPSILON = 0.000001f;

/* Calc. the sum of the points.
*/
Vector sum(std::vector<Point> &points);
/* Calc. the sum of the points.
*/
Vector sum(std::vector<Vector> &points);
/* Calc. average of the point set.
*/
Vector avg(std::vector<Vector> &points);
/* Subtract a point from each variable in the vector.
*/
void subtract(std::vector<Vector> &points, Vector sub);
/* Add the 'from' list to the 'to' list.
*/
void concat(std::vector<Vector>& to, std::vector<Point> &from);


/* Calc. the angle difference from [-180, 180]
*/
float angleDiff(Vector v, Vector v1);

/* Power iteration method (potens metoden) for finding the dominant eigen value (in absolut value) and related eigen vector.
A		<<	NxN matrix
n		<<	N == matrix dimension
eigen_v	<>	Output eigen vector and used as input of initial search vector (if size() == 0 [1,0,..,0] is used).
eigen	>>	Output eigen value.
eps		<<	Tolerated error value
iters	<<	Number of search iterations.
*/
int potens(std::vector<float>& A, unsigned int n, std::vector<float>& eigen_v, float &eigen, float eps = EPSILON, int iters = 2000);
/* Find the first vector of U, related to the dominant eigen value. */
Vector partial_SVD(std::vector<Vector> &Nx2);

/* Calc. an slope on the form f(x) = ky that intersects origo (0 vector), using min. square with a single variable column.
* Returns the result as a direction vector.
*/
Vector slope(std::vector<Vector> ab);
/* Univariate linear regression
*/
int least_square_univariate(std::vector<Vector> ab, Vector &result);
/*Total least square function. 
*/
Vector deming_regression(std::vector<Vector> ab);