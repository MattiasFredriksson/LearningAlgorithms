#include"Neural/Neural.h"
#include<math.h>
#include<algorithm>
#include<iostream>
#include<sstream>
#include <fstream>

#pragma region Neuron
	/**
	*  Generate a neuron with random weights
	*/
Neuron::Neuron(Neural& neural, int child_layer_size, int index)
	: _neural(neural), _index(index), _weights(child_layer_size)
{
	for (int i = 0; i < child_layer_size; i++)
		_weights[i] = _neural._random.randomDouble(-1, 1);
}
/**
* Evaluate input value
* @param value
*/
void Neuron::evaluate(double value)
{
	_in = value;
	_out = g();
}
/**
* Evaluate connected neurons
* @param con_parent
*/
void Neuron::evaluate(Layer& parent_con)
{
	_in = 0;
	for (int i = 0; i < parent_con._size; i++)
		_in += parent_con._neuron[i]->_weights[_index] * parent_con._neuron[i]->_out;
	_out = g();
}
/**
* Evaluate output layer error
* @param y
*/
void Neuron::error(double y)
{
	_error = g_p() * (y - _out);
}
/**
*
* @return
*/
double Neuron::getLossSquare()
{
	double loss = _error / g_p();
	return loss*loss;
}
double Neuron::getLossAbs()
{
	return std::abs(_error / g_p());
}
/**
* Evaluate error of connected neutrons.
* @param child_con
*/
void Neuron::error(Layer& child_con)
{
	// Sum the error of this node:
	double acc_err = 0;
	for (int i = 0; i < child_con._size; i++)
		acc_err += _weights[i] * child_con._neuron[i]->_error;
	_error = g_p() * acc_err;
	// Update the weights to the child layer.
	for (int i = 0; i < child_con._size; i++)
		_weights[i] = _weights[i] + _neural._alpha * _out * child_con._neuron[i]->_error;
}
/**
* Activation function
*/
double Neuron::g() {
	return 1 / (1 + std::exp(-_in));
}
double Neuron::g_p() {
	return _out * (1 - _out); // g(in) = _out
}
#pragma endregion


#pragma region Layer


Layer::Layer(Neural& neural, int layer_size, int child_layer_size)
	: _size(layer_size), _neuron(layer_size) {

	for (int i = 0; i < layer_size; i++)
		_neuron[i] = new Neuron(neural, child_layer_size, i);
}

Layer::~Layer()
{
	for (int i = 0; i < _size; i++)
		delete _neuron[i];
}


#pragma endregion


#pragma region Neural

void Neural::init(int* layers, int length)
{
	_layers = length;
	_layer = std::vector<Layer*>(_layers);
	// Initiate neurons
	for (int i = 0; i < _layers - 1; i++)
		_layer[i] = new Layer(*this, layers[i], layers[i + 1]);
	//Output layer (no weights!)
	_layer[_layers - 1] = new Layer(*this, layers[_layers - 1], 0);
}
	/* Construct a network of certain size.
	*/
	Neural::Neural(mf::RandomGenerator& random, int layers, int network_size)
		: _random(random)
	{
		int num_layer = std::max(0, layers) + 2;
		int* arr = new int[num_layer];
		for (int i = 0; i < num_layer; i++)
			arr[i] = network_size;
		init(arr, num_layer);
		delete[] arr;
	}
	Neural::~Neural()
	{
		for (int i = 0; i < _layers; i++)
			delete _layer[i];
	}
	/* Construct a network of certain size.
	*/
	Neural::Neural(mf::RandomGenerator& random, int* layers, int length)
		: _random(random)
	{
		init(layers, length);
	}

	/**
	* Evaluate the input
	* @param f_input
	*/
	void Neural::evaluate(double* f_input)
	{
		// Evaluate input layer
		for (int i = 0; i < _layer[0]->_size; i++)
			_layer[0]->_neuron[i]->evaluate(f_input[i]);
		// Evaluate remaining layers
		for (int i = 1; i < _layers; i++) {
			for (Neuron* n : _layer[i]->_neuron)
				n->evaluate(*_layer[i - 1]);
		}
	}
	/**
	* Evaluate the output error
	* @param f_output
	*/
	void Neural::learn(double* f_output)
	{
		// Evaluate output layer error
		Layer* outp = _layer[_layers - 1];
		for (int i = 0; i < outp->_size; i++)
			outp->_neuron[i]->error(f_output[i]);
		// Evaluate remaining layer errors
		for (int i = _layers - 2; i >= 0; i--) {
			for (Neuron* n : _layer[i]->_neuron)
				n->error(*_layer[i + 1]);
		}
	}
	/**
	* Execute a single back propagation
	* @param f_input
	* @param f_output
	*/
	void Neural::backPropLearn(double* f_input, double* f_output)
	{
		evaluate(f_input);
		learn(f_output);
	}

	void Neural::printAvgError() {
		double error = 0;
		for (Neuron* n : _layer[_layers - 1]->_neuron)
			error += std::abs(n->_error);
		std::cout << "Avg. error " << error << "\n";
	}
	/**
	* Get the ouput layer.
	* @return
	*/
	std::vector<double> Neural::getOuput()
	{
		Layer* outp = _layer[_layers - 1];
		std::vector<double> arr(outp->_size);
		for (int i = 0; i < outp->_size; i++)
			arr[i] = outp->_neuron[i]->_out;
		return arr;
	}
	/**
	* Get the avg. square loss
	* @return
	*/
	double Neural::getAvgSquareLoss() {

		Layer* outp = _layer[_layers - 1];
		double err = 0;
		for (int i = 0; i < outp->_size; i++)
			err += outp->_neuron[i]->getLossSquare();
		return err / outp->_size;
	}
	/**
	* Get the avg. absolute loss
	* @return
	*/
	double Neural::getAvgAbsLoss() {

		Layer* outp = _layer[_layers - 1];
		double err = 0;
		for (int i = 0; i < outp->_size; i++)
			err += outp->_neuron[i]->getLossAbs();
		return err / outp->_size;
	}
	double Neural::getAbsLoss(int index) { return  _layer[_layers - 1]->_neuron[index]->getLossAbs(); }
	double Neural::getSquareLoss(int index) { return  _layer[_layers - 1]->_neuron[index]->getLossSquare(); }

	/**
	* Read the neural network of created size from file. Must match size...
	* @param file
	*/
	void Neural::read(std::string file) {
		std::ifstream stream(file + "-L" + std::to_string(_layers) + ".txt");
		if (stream.is_open())
		{
			// Try to parse initial data from the line:
			try {
				std::string line, value;
				std::stringstream ss;

				for (Layer* lay : _layer) {
					//Iterate neurons in layer
					for (Neuron* n : lay->_neuron) {
						if (!std::getline(stream, line))
							throw std::exception("No line for neuron");
						ss = std::stringstream(line);
						unsigned int i = 0;
						while (std::getline(ss, value, ':'))
							n->_weights[i++] = std::atof(value.c_str());
						//Verify enough values
						if(i < n->_weights.size())
							throw std::exception("Not enough data for neuron");
					}
				}
				std::cout << "Loaded neural network.\n";
			}
			catch (const std::exception& e) {
				std::cout << e.what() << std::endl;
				std::cout << "Could not parse neuron.\n";
			}
		}
		else
			std::cout << "File: " + file + " could not be opened.\n";
	}
	bool Neural::store(std::string file) {
		try {
			std::ofstream stream(file + "-L" + std::to_string(_layers) + ".txt");
			for (Layer* lay : _layer) {
				//Iterate neurons in layer
				for (Neuron* n : lay->_neuron)
				{
					for (unsigned int i = 0; i < n->_weights.size() - 1; i++)
						stream << n->_weights[i] << ":";
					if (n->_weights.size() > 0)
						stream << n->_weights[n->_weights.size() - 1];
					stream << std::endl;
				}
			}
			stream.close();
			return true;
		}
		catch (const std::exception& e) {
			std::cout << e.what() << std::endl;
			return false;
		}
	}


#pragma endregion





