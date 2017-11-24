#include "ConceptLearner.h"
#include "Sentences.h"
#include <iostream>

Conjuction LGG_Conj(const Conjuction& x, const Conjuction& y)
{
	Conjuction intersect; //New sentene.
	for (int i = 0; i < x._sentence.size(); i++) {
		const Literal& l = x._sentence[i];
		//Find if l exist in y:
		for (int ii = 0; ii < y._sentence.size(); ii++) {
			if (l == y._sentence[ii]) {
				// Append literal to our new sentence:
				intersect._sentence.push_back(l);
				break;
			}
		}
	}
	return intersect;
}

Conjuction LGG_Set(Conjuction* D, int len)
{
	Conjuction H = D[0];
	for (int i = 1; i < len; i++)
		H = LGG_Conj(H, D[i]);
	return H;
}

void evaluate()
{	
	Conjuction D[3] = { S1(), S2(), S3() };
	Conjuction h = LGG_Set(D, 3);
	std::cout << " Students that most likely will pass DV2542 \n";
	std::cout << "LGG hypothesis:\n";
	std::cout << toString(h);
}

/* Test function: */
void test()
{
	std::cout << "Test hypothesis:\n\n";
	Conjuction D[4] = { S1(), S2(), S3(), S4() };
	Conjuction h = LGG_Set(D, 4);
	std::cout << "S1 + S2 + S3 + S4:\n";
	std::cout << toString(h) << "\n\n";

	h = LGG_Set(D, 2);
	std::cout << "S1 + S2:\n";
	std::cout << toString(h) << "\n\n";

	D[1] = S3();
	h = LGG_Set(D, 2);
	std::cout << "S1 + S3:\n";
	std::cout << toString(h) << "\n\n";

	D[0] = S2();
	h = LGG_Set(D, 2);
	std::cout << "S2 + S3:\n";
	std::cout << toString(h) << "\n\n";
}