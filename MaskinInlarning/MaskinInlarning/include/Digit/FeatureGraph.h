#pragma once
#include<vector>
#include "Vector.h"

class FeatureGraph
{
public:
	struct Edge;
	struct Node
	{
		Vector _point;
		std::vector<Edge*> _edges;
		size_t _n_pixels;
	};
	struct Edge
	{
		Node *_node, *_connected;
		float _len, _w_node, _w_con;
		/*Used for sorting.*/
		bool _isTraversed;
		int _index;

		float getWidth();
		float getPixels();
		Vector dir();
		void changeDir();
	};

	FeatureGraph();
	~FeatureGraph();

	std::vector<Node*> _nodes;
	std::vector<Edge*> _edges;

	Node* addNode(Vector point, size_t n_pixels);
	Edge* connect(Node* n1, Node *n2, float n_width, float n2_width);

	int featureSortSubgraph(Edge *e, int index);
	bool findSortStart(Edge*& out);
	void feaureSort();

	void reOrder(Edge *e, int i1);
};