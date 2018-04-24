#pragma once
#include "MNIST.h"
#include "Graph.h"

/* Function constructing the feature graphs from the data and writing the feature vectors to the file.
*/
void outputFeatures(MNIST &mnist, std::string &file, int dim, size_t img_from, size_t img_to, int threshold, bool out_all = false);

/* Output the segmented pixel sets created from the Disjoint sets on 4x4 and 7x7 kernels, and the contour point sets.
 */
void outputSegmentedKernels(MNIST &mnist, int img_from, int img_to, int threshold);

/* Function constructing the ographs and outputs their related data to files.
*/
void outputGraph(MNIST &mnist, int dim, int simplify, int img_from, int img_to, int threshold);

/* Function printing the ograph to a specific file.
*/
void outputGraph(Graph *graph, std::string &file);

/* Function generating a disjoint set graph from the image for the 4x4 kernel.
*/
std::unique_ptr<Graph> generateGraph4x4(DigitImageByte *img, unsigned char threshold);

#pragma region Tests

void verifyKernels(DigitImageByte *img);
void verifyKernelFunc(MNIST &mnist);
/* Check if all images are normalized.
*/
void checkUnnormalized(MNIST &mnist);

#pragma endregion