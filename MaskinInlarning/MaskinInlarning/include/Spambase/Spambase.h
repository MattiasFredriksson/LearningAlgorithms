#pragma once
#include<vector>
#include<string>
#include<memory>

/* Single instance (spam or ham mail)
*/
class Spam
{
public:
	//1 if spam
	bool _isSpam;

	std::vector<double> _features;

	Spam() {}
	~Spam() {}
};

/* Set of instances, subset of our spambase
*/
class InstanceSet
{
public:
	std::vector<Spam*> _data;
	std::vector<int> _features_remain;

	InstanceSet() {}
	InstanceSet(unsigned int num_features) : _features_remain(num_features) {}
	InstanceSet(std::vector<Spam*> data, unsigned int num_features);

	// Separate into two sets (initialized in function).
	void split(int split_feature, double threshold, InstanceSet& set_a, InstanceSet& set_b);
	~InstanceSet() {}


};

/* The spambase database.
*/
class Spambase
{
private:


public:

	static const unsigned int NUM_FEATURES = 57;

	// Container for spambase data.
	std::vector<Spam*> _data;

	Spambase();
	Spambase(const Spambase& copy);
	~Spambase();

	void read(std::string file);

	/* Create a set containing all elements in the spambase. */
	InstanceSet createInstanceSet();

	//Convert features to specific value representation
	void convertX();

};

