#include "Digit/FeatureGraph.h"
#include <assert.h>
#include "Digit/Math.h"



FeatureGraph::FeatureGraph()
{

}
FeatureGraph::~FeatureGraph()
{
	for (unsigned int i = 0; i < _nodes.size(); i++)
		delete _nodes[i];
	for (unsigned int i = 0; i < _edges.size(); i++)
		delete _edges[i];
}

FeatureGraph::Node* FeatureGraph::addNode(Vector point, size_t n_pixels)
{
	assert(n_pixels > 0);
	Node *n = new Node();
	n->_point = point;
	n->_n_pixels = n_pixels;
	_nodes.push_back(n);
	return n;
}
FeatureGraph::Edge* FeatureGraph::connect(Node* n1, Node *n2,float n_width, float n2_width)
{
	assert(n1->_point != n2->_point); //"Can't connect same nodes."
	Edge *e = new Edge();
	e->_node = n1;
	e->_connected = n2;
	e->_w_node = n_width;
	e->_w_con = n2_width;
	e->_index = _edges.size();
	e->_len = (n1->_point - n2->_point).length();
	_edges.push_back(e);
	

	n1->_edges.push_back(e);
	n2->_edges.push_back(e);

	return e;
}

float FeatureGraph::Edge::getWidth()
{ 
	return (_w_node + _w_con) / 2.f; 
}
float FeatureGraph::Edge::getPixels()
{
	float n = _node->_n_pixels / (float)_node->_edges.size();
	n += _connected->_n_pixels / (float)_connected->_edges.size();
	return n;
}

Vector FeatureGraph::Edge::dir()
{
	Vector dir = _connected->_point - _node->_point;
	dir.normalize();
	return dir;
}

void FeatureGraph::Edge::changeDir()
{
	std::swap(_node, _connected);
	std::swap(_w_node, _w_con);
}

// Find the edge with min node, returns if a node where lowest.
bool checkLower(FeatureGraph::Node *n, FeatureGraph::Edge *e, float &Y, FeatureGraph::Edge *& param)
{
	if (n->_point.Y < Y)
	{
		Y = n->_point.Y;
		param = e;
		return true;
	}
	return false;
}
bool FeatureGraph::findSortStart(Edge *&out)
{
	Edge *lowest = NULL;
	Edge *no_edge = NULL;
	float lowY = FLT_MAX;
	float low_edge_Y = FLT_MAX;
	for (Edge *e : _edges)
	{
		if (!e->_isTraversed)
		{
			Node *n = e->_node;
			//Find a 'lowest' node:
			checkLower(e->_node, e, lowY, lowest);
			if (checkLower(e->_connected, e, lowY, lowest))
				e->changeDir();
			//Find lowest unconnected edge:
			if (e->_node->_edges.size() == 1)
				checkLower(e->_node, e, low_edge_Y, no_edge);
			if (e->_connected->_edges.size() == 1 && checkLower(e->_connected, e, low_edge_Y, no_edge))
				e->changeDir(); //Reverse order of edge
		}
	}
	//No subgraphs left to sort
	if (lowest == NULL) return false;

	if (no_edge == NULL)
		out = lowest;
	else
		out = no_edge;
	//A subgraph is sorted
	return true;
}

FeatureGraph::Edge* traverse_ccw(FeatureGraph::Edge *e)
{
	Vector dir = e->dir();
	float angle_diff = FLT_MAX;
	FeatureGraph::Edge *n_edge = NULL;
	for (FeatureGraph::Edge* next : e->_connected->_edges)
	{
		//Verify not traversed:
		if (next->_isTraversed)
			continue;
		//Sort edge order:
		if (next->_node != e->_connected)
			next->changeDir();

		//Find the rightmost edge
		float a = angleDiff(dir, next->dir());
		if (a < angle_diff)
		{
			angle_diff = a;
			n_edge = next;
		}
	}
	return n_edge;
}


void FeatureGraph::reOrder(Edge *e, int index)
{
	if (e->_index == index) return;
	Edge *other = _edges[index];
	std::swap(_edges[index], _edges[e->_index]);
	std::swap(other->_index, e->_index);
}

int FeatureGraph::featureSortSubgraph(Edge *e, int index)
{
	e->_isTraversed = true;
	Edge *next;
	while ((next = traverse_ccw(e)) != NULL)
	{
		reOrder(next, index++);

		//Recursively sort connected edges
		index = featureSortSubgraph(next, index);
	}
	return index;
}

void FeatureGraph::feaureSort()
{
	for (Edge *e : _edges)
		e->_isTraversed = false;
	Edge *e;
	int e_index = 0;
	//Until all subgraphs are sorted:
	while(findSortStart(e))
	{
		reOrder(e, e_index++);
		e_index = featureSortSubgraph(e, e_index);
	}
}