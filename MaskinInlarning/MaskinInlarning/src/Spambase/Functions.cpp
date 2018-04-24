#include "Spambase/Functions.h"
#include "RandomGenerator.h"




std::shared_ptr<Neural> constructNet()
{
	mf::RandomGenerator rnd({ 1235152, 123981203, 210487124, 1239823 });
	const int num_layer = 7;
	int layers[num_layer]{ Spambase::NUM_FEATURES, 29, 15, 8, 4, 2, 1 };
	return std::shared_ptr<Neural>(new Neural(rnd, layers, num_layer));
}


void neuralFunction()
{
	Spambase spambase;

	std::shared_ptr<Neural> net = constructNet();
}