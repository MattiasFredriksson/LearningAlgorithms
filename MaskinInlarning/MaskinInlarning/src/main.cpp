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

	int ind = -1;
	if (ind >= 0) {
		data.print(ind);
		data.print(ind, 2);
		data.print(ind, 1);
	}

	//data.printLabels("label.txt");
	//data.printFeatures("features.txt", 50);
	
	// Output data
	if(true)
	{
		//data.printFeatures("../../Digit-Sckit-Eval/feature/features_train.txt");
		//data.printLabels("../../Digit-Sckit-Eval/feature/label_train.txt");

		data = MNIST(2);
		data.printFeatures("../../Digit-Sckit-Eval/feature/features_test.txt");
		data.printLabels("../../Digit-Sckit-Eval/feature/label_test.txt");
	}
	//outputGraph(data, 2, 4, 0, 100, 50);
	//outputSegmentedKernels(data, 0, 100, 50);

	//data.pixelHistogram("PixelDist.txt");
	
	std::cout << "Done\n";
	std::getchar();
}
