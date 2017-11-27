#include "Spambase.h"
#include <fstream>
#include <iostream>
#include <sstream>



InstanceSet::InstanceSet(std::vector<Spam*> data, unsigned int num_features)
	: _data(data.size()), _features_remain(num_features)
{
	for (int i = 0; i < data.size(); i++)
		_data[i] = data[i];
	for (int i = 0; i < num_features; i++)
		_features_remain[i] = i;
}

void InstanceSet::split(int split_feature, double threshold, InstanceSet& set_a, InstanceSet& set_b)
{
	set_a = InstanceSet(this->_features_remain.size() - 1);
	set_b = InstanceSet(this->_features_remain.size() - 1);

	//Split this set into two:
	for (int i = 0; i < _data.size(); i++)
	{
		if (_data[i]->_features[split_feature] >= threshold)
			set_b._data.push_back(_data[i]);
		else
			set_a._data.push_back(_data[i]);
	}

	// Copy remaining features (ignoring the split feature):
	int index = 0;
	for (int i = 0; i < this->_features_remain.size(); i++)
	{
		if (i != split_feature)
		{
			set_a._features_remain[index] = i;
			set_b._features_remain[index++] = i;
		}
	}
}


#pragma region Spambase


Spambase::Spambase()
{
	read("spambase.data");
}

Spambase::~Spambase()
{
	for (unsigned int i = 0; i < _data.size(); i++)
		delete _data[i];
}


InstanceSet Spambase::createInstanceSet() {
	return InstanceSet(_data, numFeatures());
}


void Spambase::read(std::string file)
{

	std::ifstream stream(file);
	if (stream.is_open())
	{
		std::string line, item;
		std::stringstream ss;

		while (std::getline(stream, line))
		{
			Spam* spam = new Spam();
			ss = std::stringstream(line);

			int i = 0;
			while (std::getline(ss, item, ','))
			{
				double value = std::atof(item.c_str());
				// Get the last value as the spam value
				if (i++ == NUM_FEATURES)
					spam->_isSpam = (value == 1.0);
				else
					spam->_features.push_back(value);
			}

			if (i <= NUM_FEATURES)
			{
				std::cout << "Not enough data for spam item\n";
				continue;
			}

			_data.push_back(spam);
		}
		std::cout << "Loaded spambase.\n";
	}
	else
		std::cout << "Spambase file: " + file + " could not be opened.\n";
}

#pragma endregion