#include "Digit/KernelFunction.h"
#include "DisjointSet.h"
#include "Vector.h"
#include "Digit/Contour.h"
#include <iostream>
#include <sstream>
#include "Log.h"
#include "Digit/Math.h"

/* For positive nums with sum less then UINT_MAX (and 0)*/
unsigned int div_ceil(unsigned int nume, unsigned int denom) { return (nume + denom - 1) / denom; }

/* Generate point set in img space from kernel indice sets. */
std::vector<std::vector<Point>> toPoints(KernelByte &kernel, DigitImageByte *img, std::vector<std::vector<int>> &indices);

void appendOutput(std::stringstream &ss, std::vector<std::vector<Point>> &points);
void appendOutput(std::stringstream &ss, std::vector<Point> &points);
void appendOutput(std::stringstream &ss, std::vector<Vector> &points);
void appendEdge(std::stringstream &ss, Vector a, Vector b);
void appendRect(std::stringstream &ss, Vector a, Vector b, Vector c, Vector d);

#pragma region Graph

void connectGraph(int dimX, int dimY, std::vector<std::vector<Vertice*>> &verts, std::vector<KernelByte> &kernels, std::vector<DisjointSets> &sets, unsigned char threshold)
{
	for(int y = 0; y < dimY; y++)
	{
		for (int x = 0; x < dimX; x++)
		{
			int k0 = x + y*dimY;
			KernelByte &kernel = kernels[k0];
			DisjointSets &set = sets[k0];
			std::vector<Vertice*> &nodes = verts[k0];

			//Connect right
			if (x < dimX - 1)
			{
				int k2 = k0 + 1;
				//Right kernel
				KernelByte &kernel2 = kernels[k2];
				DisjointSets &set2 = sets[k2];
				std::vector<Vertice*> &nodes2 = verts[k2];

				for (unsigned int i2 = 0; i2 < kernel.size(); i2 += kernel._dimX)
				{
					int i0 = i2 + kernel._dimX - 1;
					if (ispixel(kernel[i0], threshold) && ispixel(kernel2[i2], threshold))
					{
						//Connect
						int e0 = set.getSetIndex(i0);
						int e2 = set2.getSetIndex(i2);
						nodes[e0]->connect(nodes2[e2]);
					}
				}
			}

			//Connect down
			if (y < dimY - 1)
			{
				int k2 = k0 + dimX;
				//Right kernel
				KernelByte &kernel2 = kernels[k2];
				DisjointSets &set2 = sets[k2];
				std::vector<Vertice*> &nodes2 = verts[k2];

				for (unsigned int i2 = 0; i2 < kernel._dimX; i2++)
				{
					int i0 = i2 + (kernel._dimY - 1)*kernel._dimX;
					if (ispixel(kernel[i0], threshold) && ispixel(kernel2[i2], threshold))
					{
						//Connect
						int e0 = set.getSetIndex(i0);
						int e2 = set2.getSetIndex(i2);
						nodes[e0]->connect(nodes2[e2]);
					}
				}
			}
		}
	}
}


/* Function generating a disjoint set graph from the image for the 4x4 kernel.
*/
std::unique_ptr<Graph> generateGraph4x4(DigitImageByte *img, unsigned char threshold)
{
	std::vector<KernelByte> kernels = img->genKernels(4);

	// Kernel related verts.
	std::vector<std::vector<Vertice*>> verts;
	std::vector<DisjointSets> sets;
	sets.resize(kernels.size());
	verts.resize(kernels.size());
	//Graph
	std::vector<Vertice*> graph;

	for (size_t k = 0; k < kernels.size(); k++)
	{
		KernelByte &kernel = kernels[k];
		std::unique_ptr<unsigned char> edge = generateEdgeCut4x4(kernel, threshold);
		sets[k] = kernel.disjointSets(threshold, edge.get());
		DisjointSets &disjoint = sets[k];
		//Gen. points
		std::vector<std::vector<int>> indices = kernel.segmentPositive(threshold, disjoint);
		std::vector<std::vector<Point>> points = toPoints(kernel, img, indices);

		// Convert to graph nodes
		std::vector<Vertice*> &nodes = verts[k];
		for (unsigned int i = 0; i < points.size(); i++)
		{
			Vertice *vert = new Vertice(points[i]);
			nodes.push_back(vert);
			graph.push_back(vert);
		}

		// Connect within kernel:
		for (int i = 1; i < 16; i+=4)
		{
			if (!hasflag(edge.get()[i], EdgeBit::EDGE_R) && ispixel(kernel[i], threshold) && ispixel(kernel[i+1], threshold))
			{
				//Connect
				int i0 = disjoint.getSetIndex(i);
				int i2 = disjoint.getSetIndex(i+1);
				nodes[i0]->connect(nodes[i2]);
			}
		}
		for (int i = 4; i < 8; i++)
		{
			if (!hasflag(edge.get()[i], EdgeBit::EDGE_DOWN) && ispixel(kernel[i], threshold) && ispixel(kernel[i+4], threshold))
			{
				//Connect
				int i0 = disjoint.getSetIndex(i);
				int i2 = disjoint.getSetIndex(i + 4);
				nodes[i0]->connect(nodes[i2]);
			}
		}
	}
	int dim = div_ceil(img->_width, 4);
	connectGraph(dim, dim, verts, kernels, sets, threshold);
	return std::unique_ptr<Graph>(new Graph(new BinaryImage(img, threshold, true), graph));
}
std::unique_ptr<Graph> generateGraph(DigitImageByte *img, int kernel_dim, unsigned char threshold)
{
	std::vector<KernelByte> kernels = img->genKernels(kernel_dim);

	// Kernel related verts.
	std::vector<std::vector<Vertice*>> verts;
	std::vector<DisjointSets> sets;
	sets.resize(kernels.size());
	verts.resize(kernels.size());
	//Graph
	std::vector<Vertice*> graph;

	for (size_t k = 0; k < kernels.size(); k++)
	{
		KernelByte &kernel = kernels[k];
		sets[k] = kernel.disjointSets(threshold);
		DisjointSets &disjoint = sets[k];
		//Gen. points
		std::vector<std::vector<int>> indices = kernel.segmentPositive(threshold, disjoint);
		std::vector<std::vector<Point>> points = toPoints(kernel, img, indices);

		// Convert to graph nodes
		std::vector<Vertice*> &nodes = verts[k];
		for (unsigned int i = 0; i < points.size(); i++)
		{
			Vertice *vert = new Vertice(points[i]);
			nodes.push_back(vert);
			graph.push_back(vert);
		}
	}
	int dim = img->_width / kernel_dim;
	connectGraph(dim, dim, verts, kernels, sets, threshold);
	return std::unique_ptr<Graph>(new Graph(new BinaryImage(img, threshold, true), graph));
}

bool outputFeatures(FeatureGraph *graph, unsigned int max_nodes, Log &log)
{
	if (graph->_edges.size() == 0)
		return false;

	std::stringstream feature;

	//Print first feature:
	FeatureGraph::Edge *e = graph->_edges[0];
	Vector dir = e->dir();
	feature << dir.X << "," << dir.Y << "," << e->_len << "," << e->getWidth();

	//Print Features:
	unsigned int i = 1;
	for (; i < graph->_edges.size() && i < max_nodes; i++)
	{
		FeatureGraph::Edge *e = graph->_edges[i];
		Vector ndir = e->dir();
		float a = angleDiff(dir, ndir);
		float len = e->_len;
		float w = e->getPixels(); //e->getWidth();
		//Assert finite values:
		if (!std::isfinite(a)) a = 0;
		if (!std::isfinite(len)) len = 0;
		if (!std::isfinite(w)) w = 0;
		feature << "," << a << "," << len << "," << w;
		dir = ndir;
	}
	// Append feature vector until complete:
	if (i < max_nodes)
	{
		for (; i < max_nodes; i++)
			feature << ","  << 0 << "," << 0 << "," << 0;
	}

	log.logMsg(feature);
	return true;
}
void outputFeatureGraph(FeatureGraph *graph, std::string &file)
{
	std::stringstream rect_var, edge_var;
	rect_var << "K = [";
	edge_var << "E = [";
	for (size_t i = 0; i < graph->_edges.size(); i++)
	{
		FeatureGraph::Edge *edge = graph->_edges[i];
		// Rect
		Vector p = edge->_node->_point, p1 = edge->_connected->_point;
		Vector v2, v = p1 - p;
		float len = v.length();
		v.normalize();
		if (!std::isfinite(v.X) || !std::isfinite(v.Y))
		{
			v = Vector(1, 0);
			std::cout << "Warning: edge over single point.\n";
		}

		v = v.rotateCCW90();
		
		bool use_square = false;
		if (use_square)
		{
			float w = (edge->_w_con + edge->_w_node) / 4.f; // edge->getPixels() / len;
			v2 = v * w;
			v = v * w;// edge->_w_node;
		}
		else
		{
			v2 = v * (edge->_w_con / 2);
			v = v *  (edge->_w_node / 2);
		}
		
		appendRect(rect_var, p - v, p + v, p1 + v2, p1 - v2);

		// Edges
		appendEdge(edge_var, p, p1);
	}
	edge_var << "];";
	rect_var << "];";

	Log log(file, true, false, false);
	log << "#Edge set:\n";
	log.logMsg(edge_var);
	log << "#Rect set:\n";
	log.logMsg(rect_var);
}


/* Function printing the ograph to a specific file.
*/
void outputGraph(Graph *graph, std::string &file)
{
	std::stringstream pset_var, edge_var, axis_var, mc_var, lmc_var;
	pset_var << "K = {\n";
	edge_var << "E = [\n";
	axis_var << "A = [\n";
	mc_var << "MC = [\n";
	lmc_var << "LMC = [\n";
	for (size_t i = 0; i < graph->_nodes.size(); i++)
	{
		Vertice *node = graph->_nodes[i];
		// Pixel sets
		appendOutput(pset_var, node->_pixels);
		// Edges
		for (size_t ii = 0; ii < node->_edges.size(); ii++)
			appendEdge(edge_var, node->_mc, node->_edges[ii]->_mc);
	}
	// Graph point lists:
	std::vector<Vector> v = graph->getList_MC();
	appendOutput(mc_var, v);
	v = graph->getList_Axis();
	appendOutput(axis_var, v);
	v = graph->getList_LOCALMC();
	appendOutput(lmc_var, v);
	edge_var << "];";
	axis_var << "];";
	mc_var << "];";
	lmc_var << "];";
	pset_var << "};";

	// Output
	Log log(file, true, false, false);
	log << "#Point set:\n";
	log.logMsg(pset_var);
	log << "#Edges:\n";
	log.logMsg(edge_var);
	log << "#Graph axis:\n";
	log.logMsg(axis_var);
	log << "#Graph mass centrum:\n";
	log.logMsg(mc_var);
	log << "#Local mass centrum:\n";
	log.logMsg(lmc_var);
}
#pragma endregion


#pragma region Contour & Segmentation

#pragma region Functions

std::vector<std::vector<Point>> getSegment(KernelByte &kernel, DigitImageByte *img, int threshold)
{
	std::vector<std::vector<int>> arrs = kernel.segmentPositive(threshold);
	std::vector<std::vector<Point>> points(arrs.size());
	for (size_t i = 0; i < arrs.size(); i++)
	{
		std::vector<int> &set = arrs[i];
		points[i].resize(set.size());
		for (size_t ii = 0; ii < set.size(); ii++)
			points[i][ii] = img->invertY(kernel.getPoint(set[ii]));
	}
	return points;
}
/* Generate point set in img space from kernel indice sets. */
std::vector<std::vector<Point>> toPoints(KernelByte &kernel, DigitImageByte *img, std::vector<std::vector<int>> &indices)
{
	std::vector<std::vector<Point>> points(indices.size());
	for (size_t i = 0; i < indices.size(); i++)
	{
		std::vector<int> &set = indices[i];
		points[i].resize(set.size());
		for (size_t ii = 0; ii < set.size(); ii++)
			points[i][ii] = img->invertY(kernel.getPoint(set[ii]));
	}
	return points;
}
std::vector<std::vector<Point>> getSegmentCut4x4(KernelByte &kernel, DigitImageByte *img, int threshold)
{
	std::unique_ptr<unsigned char> edge = generateEdgeCut4x4(kernel, threshold);
	std::vector<std::vector<int>> arrs = kernel.segmentPositive(threshold, edge.get());
	return toPoints(kernel, img, arrs);
}
std::vector<std::vector<Point>> getContour(DigitImageByte *img, int threshold)
{
	std::vector<std::vector<Point>> points = findContour(*img, threshold);
	for (size_t i = 0; i < points.size(); i++)
	{
		std::vector<Point> &set = points[i];
		for (size_t ii = 0; ii < set.size(); ii++)
			set[ii] = img->invertY(set[ii]);
	}
	return points;
}

#pragma endregion

#pragma region Output

void appendEdge(std::stringstream &ss, Vector a, Vector b)
{
	ss << "[" << a.X << "," << b.X << ";" << a.Y << "," << b.Y << "],";
}
void appendRect(std::stringstream &ss, Vector a, Vector b, Vector c, Vector d)
{
	ss << "[" << a.X << "," << b.X << "," << c.X << "," << d.X << ";" << a.Y << "," << b.Y << "," << c.Y << "," << d.Y << "],";
}


void appendOutput(std::stringstream &ss, std::vector<Point> &points)
{
	ss << "\t[";
	for (size_t ii = 0; ii < points.size() - 1; ii++)
		ss << points[ii].X << ", ";
	ss << points.back().X << "; ";
	for (size_t ii = 0; ii < points.size() - 1; ii++)
		ss << points[ii].Y << ", ";
	ss << points.back().Y << "]";
	ss << ",\n";
}

void appendOutput(std::stringstream &ss, std::vector<Vector> &points)
{
	
	ss << "\t["; 
	if (points.size() > 0)
	{
		for (size_t ii = 0; ii < points.size() - 1; ii++)
			ss << points[ii].X << ", ";
		ss << points.back().X << "; ";
		for (size_t ii = 0; ii < points.size() - 1; ii++)
			ss << points[ii].Y << ", ";
		ss << points.back().Y;
	}
	ss << "]";
	ss << ",\n";
}
void appendOutput(std::stringstream &ss, std::vector<std::vector<Point>> &points)
{
	for (size_t i = 0; i < points.size(); i++)
		appendOutput(ss, points[i]);
}



/* Output the disjoint positive segments of each kernel as point sets.
*/
void outputSegmentedKernels(std::ofstream &out, DigitImageByte *img, std::vector<KernelByte> &kernels, int threshold)
{
	std::stringstream kernel_info, ss;
	std::string ID = "K" + std::to_string(kernels[0]._dimX);
	kernel_info << ID <<"_num = [";
	ss << ID << " = {\n";
	int k_count = 0;
	for (size_t k = 0; k < kernels.size(); k++)
	{
		KernelByte &kernel = kernels[k];
		std::vector<std::vector<Point>> points;
		
		if (kernel._dimX == 4 && kernel._dimY == 4)
			points = getSegmentCut4x4(kernels[k], img, threshold);
		else
			points = getSegment(kernels[k], img, threshold);

		appendOutput(ss, points);
		for (size_t i = 0; i < points.size(); i++)
		{
			std::vector<Point> &set = points[i];
			if (k_count++ > 0) kernel_info << ", ";
			kernel_info << '[' << k << ';' << i << ']';
		}
	}
	ss << "};";
	kernel_info << "];";

	// Output
	
	out << kernel_info.rdbuf() << "\n";
	out << ss.rdbuf() << "\n";
}

/* Output the disjoint positive segments of each kernel as point sets.
*/
void outputContourSegments(std::ofstream &out, DigitImageByte *img, int threshold)
{
	std::stringstream ss;
	std::string ID = "C";
	ss << ID << " = {\n";

	std::vector<std::vector<Point>> points = getContour(img, threshold);
	for (size_t i = 0; i < points.size(); i++)
	{
		std::vector<Point> &set = points[i];
		// Print X
		ss << "\t[";
		for (size_t ii = 0; ii < set.size() - 1; ii++)
			ss << set[ii].X << ", ";
		ss << set.back().X << "; ";
		//Print Y
		for (size_t ii = 0; ii < set.size() - 1; ii++)
			ss << set[ii].Y << ", ";
		// End
		ss << set.back().Y << "]";
		ss << "\n";
	}
	ss << "};";
	// Output
	out << ss.rdbuf() << "\n";
}



/* Output the segmented pixel sets created from the Disjoint sets on 4x4 and 7x7 kernels, and the contour point sets.
*/
void outputSegmentedKernels(MNIST &mnist, int img_from, int img_to, int threshold)
{
	for (int i = img_from; i < img_to; i++)
	{
		std::string file = "img/KernelSegments_" + std::to_string(i) + ".m";
		Log log(file, true, false, false);
		log << "#Kernel pixel segments\n";
		log << "#4x4\n";
		DigitImageByte *img = mnist.getImg(i);
		std::vector<KernelByte> kernels = img->genKernels(4);
		outputSegmentedKernels(log.get(), img, kernels, threshold);
		log << "#Contour 4x4\n";
		outputContourSegments(log.get(), img, threshold);

		log << "#7x7\n";
		img->genKernels(7);
		outputSegmentedKernels(log.get(), img, kernels, threshold);
	}
}


/* Function constructing the ographs and outputs their related data to files.
*/
void outputGraph(MNIST &mnist, int dim, int simplify, int img_from, int img_to, int threshold)
{
	for (int i = img_from; i < img_to; i++)
	{

		std::cout << "Processing: " << i << "\n";

		std::string file = "img/Kernel_" + std::to_string(i) + ".m";
		std::string feature_gfile = "img/Feature_" + std::to_string(i) + ".m";
		DigitImageByte *img = mnist.getImg(i);

		//Create graph from image.
		std::unique_ptr<Graph> graph;
		if(dim == 4)
			graph = generateGraph4x4(img, threshold);
		else
			graph = generateGraph(img, dim, threshold);

		// Simplify
		if (simplify > 0)
			graph->simplify_shortest();

		if (simplify > 1)
			graph->clean();

		if (simplify > 2)
		{
			float err = 4.f;
			graph->generateOGraph();
			graph->simplify_candidate(err);
			//Iteratively continue to reduce by increasing error limit.
			while (graph->_nodes.size() > 8)
			{
				//graph->restrutureOGraph();
				if (!graph->simplify_candidate(err += 4))
					break; //Not possible to merge any node with this method
			}
		}
		if (simplify > 3)
		{
			std::unique_ptr<FeatureGraph> feature(graph->toFeatureGraph());
			outputFeatureGraph(feature.get(), feature_gfile);
		}
		// Output
		outputGraph(graph.get(), file);
	}
}


/* Function constructing the feature graphs from the data and writing the feature vectors to the file.
*/
void outputFeatures(MNIST &mnist, const std::string &file, int dim, size_t img_from, size_t img_to, bool out_all)
{
	Log log(file, true, false, false);
	size_t max_node = 0;
	if (img_to <= img_from) img_to = mnist.size();
	for (size_t i = img_from; i < img_to; i++)
	{

		std::cout << "Processing: " << i << "\n";

		DigitImageByte *img = mnist.getImg(i);
		int threshold = (int)img->threshold_Otsu();


		//Create graph from image.
		std::unique_ptr<Graph> graph;
		if (dim == 4)
			graph = generateGraph4x4(img, threshold);
		else
			graph = generateGraph(img, dim, threshold);

		// Simplify
		graph->simplify_shortest();
		graph->clean();
		float err = 4.f;
		graph->generateOGraph();
		graph->simplify_candidate(err);
		//Iteratively continue to reduce by increasing error limit.
		while (graph->_nodes.size() > 8)
		{
			//graph->restructureOGraph();
			if (!graph->simplify_candidate(err += 4))
				break; //Not possible to merge any node with this method
		}
		std::unique_ptr<FeatureGraph> feature(graph->toFeatureGraph());
		feature->feaureSort();
		max_node = feature->_edges.size() > max_node ? feature->_edges.size() :  max_node;
		// Output
		if (out_all)
		{
			std::string file = "img/Kernel_" + std::to_string(i) + ".m";
			std::string feature_gfile = "img/Feature_" + std::to_string(i) + ".m";
			outputGraph(graph.get(), file);
			outputFeatureGraph(feature.get(), feature_gfile);
		}
		if (!outputFeatures(feature.get(), 16, log))
			i--; // Bug somewhere, redo
	}

	std::cout << "Max node: " << max_node << "\n";
}

#pragma endregion

#pragma endregion


#pragma region Print kernels

void transformSet(KernelByte &kernel, int kernel_num)
{
	std::vector<std::vector<int>> set = kernel.segmentPositive(50);
	if (set.size() == 0)
	{
		for (size_t i = 0; i < kernel.size(); i++)
			kernel[i] = ' ';
		return;
	}

	std::cout << "I: " << kernel_num << " -- ";
	for (size_t ii = 0; ii < set.size(); ii++)
	{
		std::vector<int> &s = set[ii];
		for (size_t i = 0; i < s.size(); i++)
		{
			int k_ind = s[i];
			std::cout << k_ind << ", ";
			unsigned char p = kernel[k_ind];
			kernel[k_ind] = (p > 100 ? 'O' : 'o');
		}
	}

	// Clear other
	for (size_t i = 0; i < kernel.size(); i++)
		kernel[i] = kernel[i] == 'O' || kernel[i] == 'o' ? kernel[i] : ' ';
	std::cout << "\n";
}
void transformPrint(KernelByte &kernel, int kernel_num)
{
	int threshold = 50;
	DisjointSets set = kernel.disjointSets(threshold);
	set.generateRecord(0);

	for (size_t row = 0; row < kernel._dimY; row++)
	{
		for (size_t col = 0; col < kernel._dimX; col++)
		{
			int i = col + row*kernel._dimX;
			int i_set = set.getSetIndex(i);
			unsigned char c = (kernel_num % 2 ? 'a' : 'A') + i_set;
			kernel[i] = kernel[i] > threshold ? c : ' ';
		}
	}
}


#pragma endregion


#pragma region Test code

void verifyKernels(DigitImageByte *img)
{

	DigitImageByte img2(*img);
	std::vector<KernelByte> kernels = img->genKernels(4);

	for (size_t i = 0; i < kernels.size(); i++) transformPrint(kernels[i], i);
	img->print(5);


	kernels = img2.genKernels(4);
	for (size_t i = 0; i < kernels.size(); i++) transformPrint(kernels[i], i);
	img2.print(5);

}


void checkKernel(MNIST &mnist, int img_index, int offsetX, int offsetY, int dim)
{
	DigitImageByte *img = mnist._data[img_index];
	KernelByte kernel = img->getKernel(offsetX, offsetY, dim);

	for (int y = 0; y < dim; y++)
	{
		for (int x = 0; x < dim; x++)
		{
			int pixel_ind = offsetX + x + (offsetY + y) * img->_width;
			unsigned char value = img->_pixels[pixel_ind];
			if (kernel[y*dim + x] != value)
				std::cout << "Error 1 at pixel: " << pixel_ind << ". Should be: " << value << " was " << kernel[y*dim + x] << std::endl; //", X: " << x << ", Y: " << y << std::endl;
			if (kernel.get(x, y) != value)
				std::cout << "Error 2 at pixel: " << pixel_ind << ". Should be: " << value << " was " << kernel.get(x, y) << std::endl; //", X: " << x << ", Y: " << y << std::endl;
		}
	}
}
void verifyKernelFunc(MNIST &mnist)
{
	for (int i = 0; i < 10; i++)
	{
		for (int x = 0; x < 24; x++)
		{
			for (int y = 0; y < 24; y++)
				checkKernel(mnist, i, x, y, 4);
		}
	}
}

void checkUnnormalized(MNIST &mnist)
{
	int count = 0;
	for (unsigned int i = 0; i < mnist.size(); i++)
		count += mnist._data[i]->unNormalized();

	std::cout << "MNIST: Unnormalized " << count << " images\n";
}

#pragma endregion