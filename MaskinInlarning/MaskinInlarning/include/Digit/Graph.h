#pragma once
#include "Vertice.h"
#include "DigitImageByte.h"
#include "BinaryImage.h"
#include "FeatureGraph.h"

class Graph
{
public:
	BinaryImage *_img;
	Graph(BinaryImage *img, std::vector<Vertice*> &nodes);
	~Graph();
	std::vector<Vertice*> _nodes;

	/* Init. vertices to graph. */
	void init();

	/* Convert an ograph to a feature graph.
	*/
	FeatureGraph* toFeatureGraph();

	/* Reduce the graph by removing 'faces', contracting the shortest edge that is part of a circular path of 3 or 4 vertices.
	*/
	void simplify_shortest();
	/* Reduce the graph by contracting edges that has an error of less then the value.
	*/
	bool simplify_candidate(float err_max = 4.f);
	/* Clean the graph, removing vertices that are redundant to a specific limit.
	*/
	void clean();

	/* Contract the edge between the two vertices.
	*/
	void join(Vertice *v, Vertice *v2);
	/* Remove a vertice from the graph.
	*/
	void remove(Vertice *v);

	/* Calc. graph axis in every node. */
	void generateOGraph();
	/* Restructures the ograph, creating a new ograph from the existing vertices. */
	void restructureOGraph();
	/* Contract edge when in 'ograph mode'.
	*/
	void ograph_join(Vertice *v, Vertice *v2);

	/* Output point lists related to the vertices of the graph.
	*/

	std::vector<Vector> getList_Axis();
	std::vector<Vector> getList_MC();
	std::vector<Vector> getList_LOCALMC();
};