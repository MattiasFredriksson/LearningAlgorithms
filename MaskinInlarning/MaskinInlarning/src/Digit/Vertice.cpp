#include "Digit/Vertice.h"
#include "Digit/Math.h"
#include "Digit/Graph.h"
#include <iostream>
#include <assert.h>

Vertice::Vertice(std::vector<Point> &pixels)
	: _parent(NULL), _residual_error(0), _pixels(std::move(pixels))
{
	_sum = sum(_pixels);
	_m = _pixels.size();
	_mc = _sum / (float)_m;
}


void Vertice::init(Graph *parent, unsigned int index)
{
	_index = index;
	_parent = parent;
}

Line Vertice::calcGraphAxis()
{
	std::vector<Vector> points;
	concat(points, _pixels);
	for (unsigned int i = 0; i < _edges.size(); i++)
		concat(points, _edges[i]->_pixels);

	Vector axis_p = _mc; // calcLocalMC();
	subtract(points, axis_p);

	Vector axis = partial_SVD(points);
	if (!std::isfinite(axis.X) || !std::isfinite(axis.Y))
	{
		std::cout << "Axis was infinite\n";
		axis = Vector(0, 0);
	}
	if (axis.X != 0 || axis.Y != 0)
		axis.normalize();
	else
		std::cout << "Axis was 0\n";
	return Line(axis, axis_p);
}
Line Vertice::calcGraphAxis_approx()
{
	float m;
	Vector mc = calcLocalMC(m);
	std::vector<Vector> xy(_edges.size() + 1);
	for (unsigned int i = 0; i < _edges.size(); i++)
		//
		xy[i] = (_edges[i]->_mc - mc) * (float)_edges[i]->_m / m ;
	xy.back() = (_mc - mc) * (_m / m);
	//if (least_square_univariate(xy, _axis) != -1)
	//	_axis = Vector(1, _axis.Y);
	Vector axis = partial_SVD(xy);
	if (!std::isfinite(axis.X) || !std::isfinite(axis.Y))
	{
		std::cout << "Axis was infinite\n";
		axis = Vector(0, 0);
	}
	if (axis.X != 0 || axis.Y != 0)
		axis.normalize();
	else
		std::cout << "Axis was 0\n";
	Vector axis_p = mc;
	return Line(axis, axis_p);
}

AxisWidth Vertice::calcWidth()
{
	Line l = getOAxis();
	AxisWidth res;
	res.p_max = -FLT_MAX;
	res.p_min = FLT_MAX;
	res.n_max = -FLT_MAX;
	res.n_min = FLT_MAX;
	Vector ccw = l._axis.rotateCCW90();
	for (unsigned int i = 0; i < _pixels.size(); i++)
	{
		Vector p = _pixels[i];
		p -= l._p;
		float d = ccw.dot(p);
		if (0 >= l._axis.dot(p))
		{
			res.p_min = std::fminf(res.p_min, d);
			res.p_max = std::fmaxf(res.p_max, d);
		}
		else {
			res.n_min = std::fminf(res.p_min, d);
			res.n_max = std::fmaxf(res.p_max, d);
		}
	}
	if (std::isinf(res.p_max) || std::isinf(res.p_min) || std::isinf(res.n_max) || std::isinf(res.n_min))
		throw std::exception();
	return res;
}
void Vertice::calcRect(Vector &p0, Vector &p1, float &len, float &width)
{
	assert(_ograph.size() > 0);
	Line l = getOAxis();
	float x_min = -0.5f, x_max = 0.5f; //init to min pixel dim.
	float y_min = -0.5f, y_max = 0.5f;
	Vector x_axis = l._axis; 
	Vector y_axis = x_axis.rotateCCW90();
	for (unsigned int i = 0; i < _pixels.size(); i++)
	{
		Vector p = _pixels[i];
		p -= l._p;
		float x = x_axis.dot(p);
		float y = y_axis.dot(p);

		x_min = std::fminf(x, x_min);
		x_max = std::fmaxf(x, x_max);

		y_min = std::fminf(y, y_min);
		y_max = std::fmaxf(y, y_max);
	}

	width = x_max - x_min;
	float height = y_max - y_min;
	Vector mc;
	mc = l._p;
	mc += x_axis * (x_max - width / 2);
	mc += y_axis * (y_max - height / 2);

	if (width > height)
	{
		std::swap(width, height);
		std::swap(x_axis, y_axis);
	}
	//Ret
	len = height;
	p0 = mc - y_axis * (height / 2);
	p1 = mc + y_axis * (height / 2);
}
float Vertice::distSumOGraph_sq(Vector point)
{
	float sum = 0;
	for (unsigned int i = 0; i < _ograph.size(); i++)
	{
		// Calc. length of variance vector, constructed from the distance to the axis of each connected node.
		Line l = _ograph[i];
		float var = l.dist(point);
		sum += var*var;
	}
	return sum;
}

float Vertice::calcResidual(Vector point)
{
	// Calc. length of residual vector, constructed from the distance to the axis of each connected node.
	float sum = distSumOGraph_sq(point);
	for (unsigned int i = 0; i < _edges.size(); i++)
		sum += _edges[i]->distSumOGraph_sq(point);
	return std::sqrt(sum);
}
/* Calc. condition value */
void Vertice::calcResidual()
{
	_residual_error = calcResidual(_mc);
}

Line Vertice::getOAxis()
{
	Vector v(_ograph[0]._axis);
	for (unsigned int i = 1; i < _ograph.size(); i++)
	{
		float sign = std::copysignf(1, _ograph[i]._axis.dot(_ograph[0]._axis));
		v += _ograph[i]._axis * sign;
	}
	v.normalize();
	return Line(v, _mc);
}

float Vertice::findOAxisSpan(Vector &p0, Vector &p1)
{
	p0 = _ograph[0]._p, p1 = _ograph[0]._p;
	float dist = 0;
	for (unsigned int i = 0; i < _ograph.size(); i++)
	{
		Vector p = _ograph[i]._p;
		for (unsigned int ii = i+1; ii < _ograph.size(); ii++)
		{
			float d = (_ograph[ii]._p-p).length_sq();
			if (dist < d)
			{
				dist = d;
				p0 = p;
				p1 = _ograph[ii]._p;
			}
		}
	}
	return std::sqrt(dist);
}

void Vertice::createOAxis()
{
	_ograph.clear();
	_ograph.push_back(calcGraphAxis());
	calcResidual();
	//Segments pointers.
	_con_seg.resize(_edges.size());
	for (unsigned int i = 0; i < _edges.size(); i++)
	{
		_con_seg[i]._seg = _edges[i];
		_con_seg[i]._p = _mc;
	}
}
void Vertice::resructureOAxis()
{
	if (_ograph.size() == 1)
		return;
	Vector v0, v1;
	float d = findOAxisSpan(v0, v1);
	if (d < 5)
		createOAxis(); // Recreate oaxis
}

void Vertice::merge(Vertice * v2)
{
	_sum += v2->_sum;
	_m += v2->_m;
	_mc = _sum / (float)_m;

	//Concat vectors:
	for (unsigned int i = 0; i < v2->_pixels.size(); i++)
		_pixels.push_back(v2->_pixels[i]);

	//Connect
	for (unsigned int i = 0; i < v2->_edges.size(); i++)
	{
		Vertice *con = v2->_edges[i];
		con->separate(v2);
		connect(con);
	}


}

void  Vertice::swap_seg(Vertice * swap_out, Vertice *with, bool merged)
{
	for (unsigned int i = 0; i < _con_seg.size(); i++)
	{
		if (_con_seg[i]._seg == swap_out)
		{
			_con_seg[i]._seg = with;
			if(merged) //Only the merged segment gets a new point
				_con_seg[i]._p = swap_out->getOSegmentPoint(with);
			return;
		}
	}
	throw std::exception();
}
void  Vertice::ograph_merge(Vertice * v2)
{
	for (unsigned int i = 0; i < v2->_ograph.size(); i++)
		_ograph.push_back(v2->_ograph[i]);

	Vertice *other = v2->getOtherCon(this);
	swap_seg(v2, other, true);
	other->swap_seg(v2, this, false);

	merge(v2);
	//Re-calc
	calcResidual();
	for (unsigned int i = 0; i < _edges.size(); i++)
		_edges[i]->calcResidual();

}
Vector Vertice::getOSegmentPoint(Vertice *con)
{
	for (unsigned int i = 0; i < _con_seg.size(); i++)
	{
		if (_con_seg[i]._seg == con)
			return _con_seg[i]._p;
	}
	throw std::exception();
}
void Vertice::getOSegmentPoint(Vertice *con, Vector &close, Vector &other)
{
	close = getOSegmentPoint(con);
	if(_con_seg.size() == 2)
	{
		if (_con_seg[0]._seg == con)
			other = _con_seg[1]._p;
		else
			other = _con_seg[0]._p;
	}
	else
	{ 
		// Find it instead:
		float max_d = 0;
		for (unsigned int i = 0; i < _ograph.size(); i++)
		{
			float d = (_ograph[i]._p - close).length();
			if (d > max_d)
			{
				max_d = d;
				other = _ograph[i]._p;
			}
		}
	}
}


Vertice* Vertice::getOtherCon(Vertice *v)
{
	for (Vertice *v2 : _edges)
	{
		if (v != v2)
			return v2;
	}
	return NULL;
}
bool Vertice::isConnectedTo(Vertice *other)
{
	for (unsigned int i = 0; i < _edges.size(); i++)
	{
		if (_edges[i] == other)
			return true;
	}
	return false;
}

/* Find if this node is connected to the other or if there exists a connection that does.*/
Vertice* Vertice::shareConnection(Vertice *other, Vertice *exclude)
{
	for (unsigned int i = 0; i < _edges.size(); i++)
	{
		if (_edges[i] == exclude)
			continue;
		else if (_edges[i] == other)
			return other;
		else if (_edges[i]->isConnectedTo(other))
			return _edges[i];
	}
	return NULL;
}

std::vector<Vertice*> Vertice::getLoop()
{
	for (unsigned int i = 0; i < _edges.size(); i++)
	{
		for (unsigned int ii = i+1; ii < _edges.size(); ii++)
		{
			Vertice *v = _edges[i]->shareConnection(_edges[ii], this);
			if (v != NULL)
			{
				std::vector<Vertice*> loop;
				loop.push_back(this);
				loop.push_back(_edges[i]);
				if (_edges[ii] != v)
					loop.push_back(v);
				loop.push_back(_edges[ii]);
				return loop;
			}
		}
	}
	return std::vector<Vertice*>();
}
std::vector<std::vector<Vertice*>> Vertice::getAllLoops()
{
	std::vector<std::vector<Vertice*>> loops;
	for (unsigned int i = 0; i < _edges.size(); i++)
	{
		for (unsigned int ii = i + 1; ii < _edges.size(); ii++)
		{
			Vertice *v = _edges[i]->shareConnection(_edges[ii], this);
			if (v != NULL)
			{
				std::vector<Vertice*> loop;
				loop.push_back(this);
				loop.push_back(_edges[i]);
				if (_edges[ii] != v)
					loop.push_back(v);
				loop.push_back(_edges[ii]);
				//Append loop list
				loops.push_back(std::move(loop));
			}
		}
	}
	return loops;
}

Vector Vertice::calcLocalMC(float &m)
{
	Vector tot = _sum;
	m = (float)_m;
	for (unsigned int i = 0; i < _edges.size(); i++)
	{
		tot += _edges[i]->_sum;
		m += _edges[i]->_m;
	}
	return tot / m;
}
Vector Vertice::calcLocalMC()
{
	float m;
	return calcLocalMC(m);
}
void Vertice::connect(Vertice *vertice)
{
	//Verify unconnected.
	if (vertice == this || isConnectedTo(vertice))
		return;
	_edges.push_back(vertice);
	vertice->connect(this);
}

/* Separate connection between the two nodes.
*/
void Vertice::separate(Vertice *vertice)
{
	for (unsigned int i = 0; i < _edges.size(); i++)
	{
		if (_edges[i] == vertice)
		{
			_edges.erase(_edges.begin() + i);
			return;
		}
	}
}


Vertice* Vertice::findMinEdge()
{
	Vertice *ret = _edges[0];
	float min_dist = (ret->_mc - this->_mc).length();
	for (unsigned int i = 1; i < _edges.size(); i++)
	{
		Vertice *a = _edges[i];
		float d = (a->_mc - this->_mc).length();
		if (d < min_dist)
		{
			min_dist = d;
			ret = a;
		}
	}
	return ret;
}