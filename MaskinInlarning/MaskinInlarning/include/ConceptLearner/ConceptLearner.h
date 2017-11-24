#pragma once
#include<vector>
#include<string>

// Structs:

struct Literal {
	std::string _feature;
	int _value;

	Literal()
		: _feature(), _value()
	{}
	Literal(std::string feat, int val)
		: _feature(feat), _value(val)
	{}

	bool operator==(const Literal &other) const
	{
		return other._feature == _feature && other._value == _value;
	}
};

struct Conjuction {
	std::vector<Literal> _sentence;
};

//Functions:

Conjuction LGG_Set(Conjuction* D, int len);

/* Evaluate the hypothesis: */
void evaluate();

/* Test function: */
void test();

