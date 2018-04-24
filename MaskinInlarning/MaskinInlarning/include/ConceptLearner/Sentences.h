#pragma once
#include "ConceptLearner.h"

// Features:

const std::string amb = "Ambition";			//low: 2, medium = 3, high: 4
const std::string prog = "Programming";		//low: 2, medium = 3, high: 4
const std::string eng = "English";			//low: 2, medium = 3, high: 4
const std::string creat = "Creative";		//no:0,	yes:1
const std::string meti = "Meticulous";		//no:0,	yes:1
											
/* Convert literal to string. */
std::string toString(const Literal l)
{
	std::string val;
	switch (l._value)
	{
	case 4: val = "high"; break;
	case 3:	val = "medium";	break;
	case 2:	val = "low"; break;
	case 1:	val = "yes"; break;
	case 0:
	default: val = "low"; break;
	}
	return l._feature + " = " + val;
}
/* Convert conjuction sentence to string. */
std::string toString(const Conjuction s)
{
	if (s._sentence.size() == 0)
		return "Empty Conjuction\n";
	std::string str = toString(s._sentence[0]) + "\n";
	for(unsigned int i = 1; i < s._sentence.size(); i++)
		str += "and " + toString(s._sentence[i]) + "\n";
	return str;
}

// Sentences:

Conjuction S1()
{
	Conjuction a;
	a._sentence.push_back(Literal(amb, 4));
	a._sentence.push_back(Literal(prog, 3));
	a._sentence.push_back(Literal(eng, 3));
	a._sentence.push_back(Literal(creat, 1));
	a._sentence.push_back(Literal(meti, 1));
	return a;
}
Conjuction S2()
{
	Conjuction a;
	a._sentence.push_back(Literal(amb, 3));
	a._sentence.push_back(Literal(prog, 4));
	a._sentence.push_back(Literal(eng, 3));
	a._sentence.push_back(Literal(creat, 1));
	a._sentence.push_back(Literal(meti, 1));
	return a;
}
Conjuction S3()
{
	Conjuction a;
	a._sentence.push_back(Literal(amb, 3));
	a._sentence.push_back(Literal(prog, 3));
	a._sentence.push_back(Literal(eng, 4));
	a._sentence.push_back(Literal(creat, 1));
	a._sentence.push_back(Literal(meti, 1));
	return a;
}

Conjuction S4()
{
	Conjuction a;
	a._sentence.push_back(Literal(amb, 3));
	a._sentence.push_back(Literal(prog, 3));
	a._sentence.push_back(Literal(eng, 4));
	a._sentence.push_back(Literal(creat, 0));
	a._sentence.push_back(Literal(meti, 0));
	return a;
}