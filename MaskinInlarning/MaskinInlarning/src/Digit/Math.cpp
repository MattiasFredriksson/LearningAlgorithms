#include "Digit/Math.h"
#include <assert.h>
#include <iostream>

#pragma region Print

void printM(std::vector<float> &M, int dim)
{
	for (int i = 0; i < dim; i++)
	{
		for (int j = 0; j < dim; j++)
			std::cout << M[i*dim + j] << ", ";
		std::cout << "\n";
	}
}
void printV(std::vector<float> &v)
{
	for (unsigned int i = 0; i < v.size(); i++)
		std::cout << v[i] << ", ";
	std::cout << "\n";
}

#pragma endregion

#pragma region Matrix & Vec funcs

float square(float x) { return x*x; }
Vector sum(std::vector<Point> &points)
{
	Vector v(0);
	for (unsigned int i = 0; i < points.size(); i++)
		v += points[i];
	return v;
}
Vector sum(std::vector<Vector> &points)
{
	Vector v(0);
	for (unsigned int i = 0; i < points.size(); i++)
		v += points[i];
	return v;
}
float sum(std::vector<float> &v)
{
	float s = 0;
	for (unsigned int i = 0; i < v.size(); i++)
		s += v[i];
	return s;
}
Vector avg(std::vector<Vector> &points)
{
	Vector v(0);
	for (unsigned int i = 0; i < points.size(); i++)
		v += points[i];
	return v / (float)points.size();
}
float dot(float *v, float *v2, unsigned int n)
{
	float s = 0;
	for (unsigned int i = 0; i < n; i++)
		s += v[i] * v2[i];
	return s;
}
/* Calc. dot product of two vectors. */
float dot(std::vector<float> &v, std::vector<float> &v2)
{
	assert(v.size() == v2.size());
	float d = 0;
	size_t n = v.size();
	for (unsigned int i = 0; i < n; i++)
		d += v[i] * v2[i];
	return d;
}
float norm(std::vector<float> &v)
{
	float d = dot(v, v);
	return d > 0 ? 1 / std::sqrt(d) : 0;
}
/* Normalize a vector. */
void normalize(std::vector<float> &v)
{
	float n = norm(v);
	// If not 0 vector
	if (n < EPSILON) return;
	for (unsigned int j = 0; j < v.size(); j++)
		v[j] *= n;
}
/* Subtract a point from each variable in the vector.
*/
void subtract(std::vector<Vector> &points, Vector sub)
{
	for (unsigned int i = 0; i < points.size(); i++)
		points[i] -= sub;
}
/* Mult a scalar with the vector.
*/
void mult(std::vector<float> &v, float scalar)
{
	for (unsigned int i = 0; i < v.size(); i++)
		v[i] *= scalar;
}
/* Add the 'from' list to the 'to' list.
*/
void concat(std::vector<Vector>& to, std::vector<Point> &from)
{
	for (unsigned int i = 0; i < from.size(); i++)
		to.push_back(from[i]);
}
std::vector<float> matrixMult(std::vector<Vector> &M2xN, std::vector<Vector> &MNx2)
{
	assert(M2xN.size() == MNx2.size());
	unsigned int n = M2xN.size();
	std::vector<float> M(n * n);

	for (unsigned int j = 0; j < n; j++)
	{
		Vector v = MNx2[j];
		for (unsigned int i = 0; i < n; i++)
			M[i*n + j] = v.dot(M2xN[i]);
	}
	return M;
}
/* Mult. NxN matrix with Nx1 vector.
*/
std::vector<float> mult_Mxv(std::vector<float> &M, std::vector<float> &v)
{
	unsigned int n = v.size();
	std::vector<float> r(n);
	for (unsigned int j = 0; j < n; j++)
	{
		float sum = 0;
		for (unsigned int i = 0; i < n; i++)
			sum += v[i] * M[i*n + j];
		r[j] = sum;
	}
	return r;
}
/* Mult. transposed vector x Matrix x vector == transposed(v)*M*v */
double mult_v_TxMxv(std::vector<float> &M, std::vector<float> &v)
{
	unsigned int n = v.size();
	assert(n*n == M.size());
	// Sum of v_T * x where x = M*v
	double sum = 0;
	for (unsigned int i = 0; i < n; i++)
	{
		// Dot(row, v)
		float x = dot(&M[n*i], v.data(), n);
		// Calc. v_T[i]*x[i]
		sum += v[i] * x;
	}
	return sum;
}
/* Mult. a 2xN matrix by a column vector of Nx1.*/
Vector mult_2xN_v(std::vector<Vector> &M, std::vector<float> &v)
{
	assert(v.size() == M.size());

	Vector r(0);
	unsigned int n = v.size();
	for (unsigned int i = 0; i < n; i++)
	{
		r.X += M[i].X * v[i];
		r.Y += M[i].Y * v[i];
	}
	return r;
}

double potens_dif(std::vector<float> &x0, std::vector<float> &x, float l)
{
	l = copysignf(1.0, l); //func: sign()
	double sum = 0;
	for (unsigned int i = 0; i < x0.size(); i++)
		sum += square(x0[i] - l * x[i]);
	return std::sqrt(sum);
}
std::vector<float> initial_vec(unsigned int n)
{
	// Create an initial unit vector
	std::vector<float> v = std::vector<float>(n);
	float div = 1.f / n;
	for (unsigned int i = 0; i < n; i++)
		v[i] = div;
	return v;
}
std::vector<float> unitAxis(unsigned int n, int axis_ind)
{
	// Create an initial unit vector
	std::vector<float> v = std::vector<float>(n);
	std::memset(v.data(), 0, v.size() * sizeof(float));
	v[axis_ind] = 1;
	return v;
}

#pragma endregion

/* Power iteration method (potens metoden) for finding the dominant eigen value (in absolut value) and related eigen vector.
A		<<	NxN matrix
n		<<	N == matrix dimension
eigen_v	<>	Output eigen vector and used as input of initial search vector (if size() == 0 [1,0,..,0] is used).
eigen	>>	Output eigen value.
eps		<<	Tolerated error value 
iters	<<	Number of search iterations.
*/
int potens(std::vector<float>& A, unsigned int n, std::vector<float>& eigen_v, float &eigen, float eps, int iters)
{
	// Find a suitable initial vector:
	std::vector<float> x, x0 = eigen_v.size() == 0 ? initial_vec(n) : eigen_v;
	float d;
	for(unsigned int i = 1; i < n + 1; i++){
		x = mult_Mxv(A, x0);
		d = dot(x, x);
		if (d > EPSILON)
			break;
		x0 = std::move(unitAxis(n, i-1));
		if (i == n) return -2;
	}
	//Perform first iter:
	double dif, l;
	mult(x0, 1 / std::sqrt(d));
	l = mult_v_TxMxv(A, x0);
	dif = potens_dif(x0, x, l);
	x0 = std::move(x);
	
	while (iters-- >= 0 && dif > eps) {// Iterate
		x = mult_Mxv(A, x0);
		normalize(x);
		l = mult_v_TxMxv(A, x);
		dif = potens_dif(x0, x, l);
		x0 = std::move(x);
	}
	eigen = l;
	eigen_v = std::move(x0);
	return iters; //Ret. number of iters
}


#pragma region Least square related

/* Find the first normalized eigen vector. */
Vector partial_SVD(std::vector<Vector> &Nx2)
{
	if (Nx2.size() < 2)
	{
		std::cout << "Warning: Nx2 was dimension: " << Nx2.size() << "\n";
		return Nx2.size() == 1 ? Nx2[0] : Vector(1, 0); //(1,0) is garbage.
	}
	// Create the NxN matrix by transpose(Nx2) * Nx2
	std::vector<float> M = matrixMult(Nx2, Nx2);
	std::vector<float> eigen_v;
	float eigen;
	if (potens(M, Nx2.size(), eigen_v, eigen) < 0)
	{
		std::cout << "Warning: No eigen pair found.\n";
		return Vector(1, 0); // Garbage value, should not be returned.
	}
	Vector  v = mult_2xN_v(Nx2, eigen_v);

	Vector r =  v / std::sqrt(eigen);
	if (!std::isfinite(r.X) || !std::isfinite(r.Y))
	{
		eigen_v = std::vector<float>();
		std::cout << "Warning: Eigen vector was 0.\n";
		return Vector(1, 0); // Garbage value, should not be returned.
	}
	return r;
}

/* Calc. an slope on the form f(x) = ky that intersects origo (0 vector), using min. square with a single variable column.
 * Returns the result as a direction vector.
*/
Vector slope(std::vector<Vector> ab)
{
	if (ab.size() == 0) return Vector(0);
	// Variance
	float a = 0, b = 0;
	for (unsigned int i = 0; i < ab.size(); i++)
	{
		a += ab[i].X * ab[i].X;
		b += ab[i].X * ab[i].Y;
	}
	a /= ab.size();
	b /= ab.size();

	//If a is 0 it's aligned with y axis
	const float e = 0.00001f;
	if (e > a && a < -e)
		return Vector(0, b >= 0 ? 1.f : -1.f);

	return Vector(1, b / a);
}
/* Univariate linear regression
*/
int least_square_univariate(std::vector<Vector> ab, Vector &result)
{
	if (ab.size() == 0) return 0;
	// Mean x
	Vector mean = sum(ab) / (float)ab.size();


	float var_xx = 0, var_xy = 0;
	for (size_t i = 0; i < ab.size(); i++)
	{
		float x = ab[i].X - mean.X;
		float y = ab[i].Y - mean.Y;
		var_xx += x*x;
		var_xy += x*y;
	}

	//If a is 0 it's aligned with y axis
	const float e = 0.00001f;
	if (e > var_xx && var_xx < -e)
	{
		result = Vector(mean.X, mean.Y >= 0 ? 1.f : -1.f);
		return -1;
	}
	else
	{
		// Calc. line function f(x) = a + b*x
		float b = var_xy / var_xx;
		float a = mean.Y - b * mean.X;
		result = Vector(a, b);
		return 1;
	}
}
/*Total least square function. */
Vector deming_regression(std::vector<Vector> ab)
{
	if (ab.size() < 2)
		return ab.size() == 1 ? ab[0] : Vector(1, 0);
	// Mean
	float n = (float)ab.size();
	Vector mean = sum(ab) / n;


	float var_xx = 0, var_yy = 0, var_xy = 0;
	for (size_t i = 0; i < ab.size(); i++)
	{
		float x = ab[i].X - mean.X;
		float y = ab[i].Y - mean.Y;
		var_xx += x*x;
		var_yy += y*y;
		var_xy += x*y;
	}
	var_xx /= (n - 1);
	var_xy /= (n - 1);
	var_yy /= (n - 1);
	float d = 1;
	float p = var_yy + d * var_xx;
	float b1 = (var_yy - d*var_xx + std::sqrt(p*p + 4 * d*var_xy*var_xy)) / (2 * var_xy);
	float b0 = mean.Y - b1*mean.X;

	//If a is 0 it's aligned with y axis
	const float e = 0.00001f;
	if (e > var_xy && var_xy < -e)
		return Vector(mean.X, mean.Y >= 0 ? 1.f : -1.f);
	else
	{
		Vector p0 = ab[0], p1 = ab[1];
		p0.X = p0.X + b1 / (b1*b1 + d)*(p0.Y - b0 - b1*p0.X);
		p1.X = p1.X + b1 / (b1*b1 + d)*(p1.Y - b0 - b1*p1.X);
		p0.Y = b0 + b1* p0.Y;
		p1.Y = b0 + b1* p1.Y;
		// Output 'true' function
		return Vector(b0, b1);
	}
}

#pragma endregion


float angleDiff(Vector v, Vector v1)
{
	//We assume normalized vectors
	float a = std::acosf(v.dot(v1));
	//Use the y axis to determine + or - angle
	float pn = v.rotateCCW90().dot(v1);
	return a * (pn >= 0 ? 1 : -1);
}