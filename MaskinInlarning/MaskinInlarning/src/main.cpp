#include <iostream>
#include "ConceptLearner/ConceptLearner.h"
#include "RandomGenerator.h"
#include "Digit/MNIST.h"
#include "Digit/KernelFunction.h"
#include "Neural/Neural.h"
#include <memory>
#include "Test.h"

#define LOG_IMPLEMENTATION
#include "Log.h"


int main()
{
	mf::RandomGenerator rnd;

	//test();

	MNIST data(1);

	int ind = 0;
	if (ind >= 0)
	{
		data.print(ind);
		data.print(ind, 2);
		data.print(ind, 1);
	}
	std::string label = "label.txt";
	std::string feat = "features.txt";
	//data.printLabels(label);
	//data.printFeatures(feat, 50);

	//outputGraph(data, 2, 4, 0, 100, 50);
	//outputSegmentedKernels(data, 0, 100, 50);

	//data.pixelHistogram("PixelDist.txt");
	
	std::cout << "Done\n";
	std::getchar();
}
