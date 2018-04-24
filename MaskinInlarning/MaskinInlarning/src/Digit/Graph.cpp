#include "Digit/Graph.h"
#include <queue>
#include <assert.h>
#include <iostream>

Graph::Graph(BinaryImage *img, std::vector<Vertice*> &nodes)
	: _img(img),_nodes(std::move(nodes)) 
{
	init();
}
Graph::~Graph()
{
	for (unsigned int i = 0; i < _nodes.size(); i++)
		delete _nodes[i];
	delete _img;
}


void Graph::remove(Vertice *v)
{
	int index = v->_index;
	_nodes.erase(_nodes.begin() + v->_index);

	for (unsigned int i = index; i < _nodes.size(); i++)
		_nodes[i]->_index = i;
}

void Graph::join(Vertice *v, Vertice *v2)
{

	v->merge(v2);
	remove(v2);
}
void Graph::ograph_join(Vertice *v, Vertice *v2)
{

	v->ograph_merge(v2);
	remove(v2);
}

#pragma region Best candidate
/*
Vertice* findBestCandidate(std::vector<Vertice*> &queue, unsigned int &ind)
{
	Vertice *v = queue[0];
	ind = 0;
	for (unsigned int i = 1; i < queue.size(); i++)
	{
		// Find candidates
		if (queue[i]->_residual_error < v->_residual_error)
		{
			v = queue[i];
			ind = i;
		}
	}
	return v;
}
float findBestJoin(std::vector<Vertice*> &loop, Vertice *&out_a, Vertice *&out_b)
{
	float min_err = FLT_MAX;
	for (unsigned int i = 0; i < loop.size(); i++)
	{
		Vertice *a = loop[i];
		for (unsigned int ii = i+1; ii < loop.size(); ii++)
		{
			Vertice *b = loop[ii];
			Vector mc = (a->_sum + b->_sum) / (float)(a->_m + b->_m);
			float res_a = a->calcResidual(mc), res_b = b->calcResidual(mc);
			float err = (res_a + res_b);
			if (err < min_err)
			{
				out_a = a;
				out_b = b;
				min_err = err;
			}
		}
	}
	return min_err;
}
float findBestJoin(std::vector<std::vector<Vertice*>> &loop, Vertice *&out_a, Vertice *&out_b)
{
	float min_err = FLT_MAX;
	Vertice *a, *b;
	for (unsigned int i = 0; i < loop.size(); i++)
	{
		float err = findBestJoin(loop[i], a, b);
		if (err < min_err)
		{
			min_err = err;
			out_a = a;
			out_b = b;
		}
	}
	return min_err;
}
void Graph::simplify_candidate()
{
	std::vector<Vertice*> queue;
	for (unsigned int i = 0; i < _nodes.size(); i++)
	{
		// Find candidates
		if (_nodes[i]->_edges.size() > 1)
			queue.push_back(_nodes[i]);
	}

	while (queue.size() > 0)
	{
		unsigned int queue_ind;
		Vertice *v = findBestCandidate(queue, queue_ind);
		int i = indexOf(v);
		std::vector<std::vector<Vertice*>> loops;
		if (i >= 0 && _nodes[i]->_edges.size() > 1)
			loops = v->getAllLoops();
		if (loops.size() > 0)
		{
			//Find best remove:
			Vertice *a, *b;
			findBestJoin(loops, a, b);
			join(a, b);
		}
		else // Remove from candidates.
			queue.erase(queue.begin() + queue_ind);
	}
}
*/
#pragma endregion

#pragma region Simplify lines

float residualError(Vertice *a, Vertice *b)
{
	float min_err = FLT_MAX;
	Vector mc = (a->_sum + b->_sum) / (float)(a->_m + b->_m);
	float res_a = a->calcResidual(b->_mc), res_b = a->calcResidual(a->_mc);
	float err = (res_a + res_b);
	return err;
}
/* Find the edge with least total distance between the center points and the graph direction.
*/
float findEdgeCandidate(std::vector<Vertice*> &nodes, Vertice *&a, Vertice *&b)
{
	float min_err = FLT_MAX;
	for (unsigned int i = 0; i < nodes.size(); i++)
	{
		Vertice *v = nodes[i];
		if(v->_edges.size() > 2)
			continue; //Don't append to a vertice that is connected to multiple vertices.
		for (Vertice* e : v->_edges)
		{
			if (e->_edges.size() != 2)
				continue; // Don't join remove an edge vertice (or multi-connected).
			float err = residualError(v, e);
			if (err < min_err)
			{
				min_err = err;
				a = v;
				b = e;
			}
		}
	}
	return min_err;
}

bool Graph::simplify_candidate(float err_max)
{
	Vertice *a, *b;
	float err = findEdgeCandidate(_nodes, a, b);
	int num = 0;
	while(err < err_max)
	{
		if (b->_edges.size() == 1)
			std::swap(a, b); // Join b with a, if b is end
		ograph_join(a, b);
		err = findEdgeCandidate(_nodes, a, b);
		num++;
	}
	std::cout << "Num removed: " << num << ", Err: " << err << std::endl;
	return err < FLT_MAX;
}

#pragma endregion

#pragma region Shortest Edge

bool check2x2(BinaryImage *img, Vector point)
{
	Point p((int)point.X, (int)point.Y);
	return img->pixel(p.X, p.Y) < 1 || img->pixel(p.X+1, p.Y) < 1 || img->pixel(p.X, p.Y+1) < 1 || img->pixel(p.X+1, p.Y+1) < 1;
}
bool check1x1(BinaryImage *img, Vector point)
{
	return img->pixel((int)point.X, (int)point.Y) < 1;
}

/* Find the shortest vector between two vertices in the loop.
*/
float findMinEdge(BinaryImage *img, std::vector<Vertice*> &loop, Vertice *&out_a, Vertice *&out_b)
{
	float min_dist = FLT_MAX;
	for (unsigned int i = 0; i < loop.size(); i++)
	{
		Vertice *a = loop[i];
		for (unsigned int ii = i + 1; ii < loop.size(); ii++)
		{
			Vertice *b = loop[ii];
			//Vector c = (a->_sum + b->_sum) / (float)(a->_m + b->_m);
			//if (check2x2(img, c))
			//	continue; // Verify mc point is not empty.

			float d = (a->_mc - b->_mc).length();
			if (d < min_dist)
			{
				out_a = a;
				out_b = b;
				min_dist = d;
			}
		}
	}
	return min_dist;
}
/* Find the shortest distance vector between vertices within a loop in the loop list
*/
float findMinEdge(BinaryImage *img, std::vector<std::vector<Vertice*>> &loop, Vertice *&out_a, Vertice *&out_b)
{
	float min_err = FLT_MAX;
	Vertice *a, *b;
	for (unsigned int i = 0; i < loop.size(); i++)
	{
		float err = findMinEdge(img, loop[i], a, b);
		if (err < min_err)
		{
			min_err = err;
			out_a = a;
			out_b = b;
		}
	}
	return min_err;
}

void Graph::simplify_shortest()
{
	/* Find the smallest edge and remove:
	*/
	Vertice *join_a, *join_b;
	do {
		float min_err = FLT_MAX;
		join_a = NULL; join_b = NULL;

		for (unsigned int i = 0; i < _nodes.size(); i++)
		{
			// Find any loops in the node
			std::vector<std::vector<Vertice*>> loops;
			if (_nodes[i]->_edges.size() > 1)
				loops = _nodes[i]->getAllLoops();
			// Find the two vertices within a loop with 
			// shortest distance between eachother
			if (loops.size() > 0)
			{
				Vertice *a, *b;
				float err = findMinEdge(_img, loops, a, b);
				if (err < min_err)
				{
					min_err = err;
					join_a = a;
					join_b = b;
				}
			}
		}
		// Merge the two vertices removing or reducing the face
		if(join_a != NULL && join_b != NULL)
			join(join_a, join_b);
	} while (join_a != NULL);
}

#pragma endregion


bool join_middle(Vertice *v, Vertice *v2, float dot)
{
	const float max_diff = std::cos(3.14f / 6.f);
	return v->_ograph.size() > 1 && std::abs(dot) > max_diff;
}

#pragma region Feature Graph construction

struct Pair
{
	FeatureGraph::Node *first, *last;
	Pair() : first(NULL), last(NULL) {}
	Pair(FeatureGraph::Node *first, FeatureGraph::Node *last) : first(first), last(last) {}
};

Pair gen_node(FeatureGraph *g, FeatureGraph::Node** created, Vertice *v, float width, float next_width, Vertice *edge);
FeatureGraph::Node* gen_next(FeatureGraph *g, FeatureGraph::Node** created, Vertice *v, Vertice *edge, float edge_width)
{
	//If created return it:
	if (created[v->_index] != NULL)
		return created[v->_index];

	AxisWidth w = edge->calcWidth();
	float next_width = w.width();

	Vertice *next = edge->getOtherCon(v);
	Pair pair;
	// Generate an edge segment:
	if (next == NULL)
	{
		Vector p = edge->getOSegmentPoint(v);
		pair = gen_node(g, created, v, edge_width, edge_width, edge);
		Pair sec = gen_node(g, created, edge, next_width, next_width, v);
		g->connect(pair.last, sec.last, edge_width, next_width);
		return pair.first;
	}
	else
	{
		pair = gen_node(g, created, v, edge_width, next_width, edge);

		//Recurse
		FeatureGraph::Node *n = gen_next(g, created, edge, next, next_width);
		g->connect(pair.last, n, edge_width, next_width);

		// Return the first node created in this iteration
		return pair.first;
	}
}

Pair gen_node(FeatureGraph *g, FeatureGraph::Node** created, Vertice *v, float width, float next_width,  Vertice *edge)
{

	FeatureGraph::Node *n, *n2;

	// Create an extra node (as first node of next segment) if next is two segment node
	if (v->_ograph.size() > 1)
	{
		Vector p, p2;
		v->getOSegmentPoint(edge, p2, p);

		//Create 2 nodes from a single vertice.
		size_t n_pixel = v->_pixels.size() / 2;
		n = g->addNode(p, n_pixel);
		n2 = g->addNode(p2, n_pixel);

		g->connect(n, n2, width, width);
		created[v->_index] = n;
		
		return Pair(n, n2);
	}
	else {
		size_t n_pixel = v->_pixels.size();
		n = g->addNode(v->_mc, n_pixel);
		created[v->_index] = n;
		return Pair(n, n);
	}
}


FeatureGraph* Graph::toFeatureGraph()
{
	// List storing the first node created for a single vertex.
	// Merged segments generate 2 vertices, the first one created in the recursive direction 
	// the graph is traversed is inserted, so that recursion reaching the segment connects to the 
	// initial segment (and not the other).
	FeatureGraph::Node** created = new FeatureGraph::Node*[_nodes.size()];
	std::memset(created, 0, _nodes.size() * sizeof(void*));
	FeatureGraph *graph = new FeatureGraph();

	for (Vertice* node : _nodes)
	{
		if (node->_edges.size() > 2)
		{
			// Create multi node.
			Vector p;
			created[node->_index] = graph->addNode(node->_mc, node->_pixels.size());
		}
	}
	//Connect multi nodes to eachother
	for (Vertice* node : _nodes)
	{
		if (node->_edges.size() > 2)
		{
			for (Vertice *e : node->_edges)
			{
				if (e->_edges.size() > 2 && e->_index > node->_index)
				{
					float w = node->calcWidth().width();
					float w2 = e->calcWidth().width();
					FeatureGraph::Node *n = created[node->_index];
					FeatureGraph::Node *n2 = created[e->_index];
					graph->connect(n, n2, w, w2);
				}
			}
		}
	}

	// Create all single/double connected nodes (and edges):
	// Process iterates in both directions from a single node until a created segment is found
	// or a node only has 1 edge.
	for (Vertice* node : _nodes)
	{
		if (created[node->_index] == NULL)
		{
			AxisWidth w = node->calcWidth();
			float width = w.width();
			//Create
			if (node->_edges.size() == 0)
			{ 
				//Single element
				Vector p0, p1;
				float len, width;
				node->calcRect(p0, p1, len, width);
				if (len * width > 16)
				{
					size_t n_pixel = node->_pixels.size() / 2;
					FeatureGraph::Node *n = graph->addNode(p0, n_pixel);
					FeatureGraph::Node *n2 = graph->addNode(p1, n_pixel);
					graph->connect(n, n2, width, width);
					created[node->_index] = n;
					std::cout << "Created single edge.\n";
				}
				else
					std::cout << "Culled feature segment of area: " << len*width << "\n";

			}
			else if (node->_edges.size() == 1)
			{
				FeatureGraph::Node *n;
				n = gen_next(graph, created, node, node->_edges[0], width);
			}
			else if (node->_edges.size() == 2)
			{
				Vertice *next = node->_edges[0], *prev = node->_edges[1];
				FeatureGraph::Node *n2, *n;

				AxisWidth w_prev = next->calcWidth();
				float width_prev = w_prev.width();

				bool was_created = created[prev->_index] != NULL;
				n = gen_next(graph, created, node, node->_edges[0], width);

				// Create in other direction if not already iterated
				if (created[prev->_index] == NULL)
				{
					Vertice *prevx2 = prev->getOtherCon(node);
					// Generate an edge segment:
					if (prevx2 == NULL)
					{
						Vector p = prev->getOSegmentPoint(node);
						Pair pair = gen_node(graph, created, prev, width_prev, width, node);
						n2 = pair.last;
					}
					else
						n2 = gen_next(graph, created, prev, prevx2, width);
					graph->connect(n, n2, width, width_prev);
				}
				else if (was_created) //If it was a multi connected  node
				{
					n2 = created[prev->_index];
					graph->connect(n, n2, width, width_prev);
				}
			}
		}
	}
	return graph;
}

#pragma endregion

void Graph::clean()
{
	for (unsigned int i = 0; i < _nodes.size(); i++)
	{
		Vertice *v = _nodes[i];
		if (_nodes[i]->_m < 5)
		{
			if (v->_edges.size() == 0)
			{
				remove(v);
				i--;
				continue;
			}
			else if (v->_edges.size() == 1)
			{
				Vertice *edge = v->findMinEdge();
				join(edge, v);
				i--; //Account for index change
			}
		}
	}
}

void Graph::init()
{
	for (unsigned int i = 0; i < _nodes.size(); i++)
	{
		_nodes[i]->init(this, i);
	}
}
void Graph::generateOGraph()
{
	for (unsigned int i = 0; i < _nodes.size(); i++)
		_nodes[i]->createOAxis();
}
void Graph::restructureOGraph()
{
	for (unsigned int i = 0; i < _nodes.size(); i++)
		_nodes[i]->resructureOAxis();
}


#pragma region List funcs

std::vector<Vector> Graph::getList_Axis()
{
	std::vector<Vector> axis;
	for (Vertice *node : _nodes)
	{
		for(unsigned int ii = 0; ii < node->_ograph.size(); ii++)
			axis.push_back(node->_ograph[ii]._axis);
	}
	return axis;
}


std::vector<Vector> Graph::getList_MC()
{
	std::vector<Vector> mc(_nodes.size());
	for (unsigned int i = 0; i < mc.size(); i++)
		mc[i] = _nodes[i]->_mc;
	return mc;
}

std::vector<Vector> Graph::getList_LOCALMC()
{
	std::vector<Vector> mc;
	for (Vertice *node : _nodes)
	{
		for (unsigned int ii = 0; ii < node->_ograph.size(); ii++)
			mc.push_back(node->_ograph[ii]._p);
	}
	return mc;
}
#pragma endregion