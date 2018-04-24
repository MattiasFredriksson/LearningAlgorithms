#pragma once
#include <vector>
#include "Point.h"
#include "Vector.h"
#include "Line.h"

class Graph;
struct AxisWidth 
{ 
	float p_min, p_max, n_min, n_max; 

	float p_diff() { return p_max - p_min; }
	float n_diff() { return n_max - n_min; }
	float width() { return (n_max - n_min + p_max - p_min) / 2.f; }
};

/* Graph vertice. Can represent both an ograph vertice and 'normal' graph vertice.
*/
class Vertice
{
public:

	Graph *_parent;

	/* Mass center - and center point, and our approximated axis.*/
	Vector _mc;
	/*Sum of pixel points. */
	Vector _sum;
	/* N pixels */
	size_t _m;

	unsigned int _index;
	/* The pixels that are associated with the cluster the vertice represent.
	*/
	std::vector<Point> _pixels;
	/* The connected vertices.
	*/
	std::vector<Vertice*> _edges;

	/* Ograph variables. Ograph is a second interpretation of the graph where 
	 * some information of merged vertices are stored to enable reconstruction of
	 * related line segments to those nodes.
	 */

	// Graph orientation vars:
	std::vector<Line> _ograph;
	struct OGraphEnd { Vertice *_seg; Vector _p; };
	std::vector<OGraphEnd> _con_seg;
	float _residual_error;


	Vertice(std::vector<Point> &pixels);

	void init(Graph *parent, unsigned int index);

	/* Calc. the average axis of the graph in the point. */
	Line calcGraphAxis();
	Line calcGraphAxis_approx();

	/* Calc. the mass centre of the graph (MC of vertice and connected).*/
	Vector calcLocalMC();
	/* Calc. local mass centre and return mass: total pixel count.*/
	Vector calcLocalMC(float &m);

	void calcResidual();
	float calcResidual(Vector point);

	/* Init Ograph mode.
	*/
	void createOAxis();
	void resructureOAxis();
	/* Get the approximated direction of the combined graph. */
	Line getOAxis();
	/* Find the span over the oaxis.*/
	float findOAxisSpan(Vector &p0, Vector &p1);

	/* Calc. the furtest point perpendicular to the axis in both directions from the point. */
	AxisWidth calcWidth();
	/* Calc. a rectangle representation of the vertice. */
	void calcRect(Vector &p0, Vector &p1, float &len, float &width);


	/* Return if this node share and edge with the other node. */
	bool isConnectedTo(Vertice *other);
	/* Find if this node is connected to the other or if there exists a connection that does.*/
	Vertice* shareConnection(Vertice *other, Vertice *exclude);
	/* Get the 'other' connection not equal to the parameter. Returns first non-equal node if this is not a segment, NULL if there is no other.*/
	Vertice* getOtherCon(Vertice *v);

	/* Get 3-4 vertices connected to eachother. */
	std::vector<Vertice*> getLoop();
	/* Get all occurances of 3-4 vertices connected to eachother. */
	std::vector<std::vector<Vertice*>> getAllLoops();

	/* Connect the two nodes. Verifies nodes are unconnected.
	*/
	void connect(Vertice *vertice);
	/* Separate connection between the two nodes.
	*/
	void separate(Vertice *vertice);

	/* General merge method for concatenating two vertices data into this.
	*/
	void merge(Vertice * v2);
	/* Merge method when graph is in ograph mode.
	*/
	void ograph_merge(Vertice * v2);

	/* Get the ograph segment point that is related to an edge. */
	Vector getOSegmentPoint(Vertice *con);
	/* Get the cloesest and opposite ograph segment points, related to an connected node. */
	void getOSegmentPoint(Vertice *con, Vector &close, Vector &other);

	Vertice* findMinEdge();

private:

	/* A method for tracking the ograph line segment relationships to the initial graph.
	*/
	void  swap_seg(Vertice * v, Vertice *v2, bool merged);
	float distSumOGraph_sq(Vector point);
};
