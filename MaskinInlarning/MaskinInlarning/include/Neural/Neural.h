#pragma once
#include<vector>
#include<string>
#include "RandomGenerator.h"

class Neuron;
class Layer;
class Neural;

class Neuron {
public:

	double _in;
	double _out;
	int _index;
	/* The weights to the child layer. */
	std::vector<double> _weights;
	double _error;
	Neural& _neural;
	/**
	*  Generate a neuron with random weights
	*/
	Neuron(Neural& neural, int child_layer_size, int index);
	~Neuron(){}
	/**
	* Evaluate input value
	* @param value
	*/
	void evaluate(double value);
	/**
	* Evaluate connected neurons
	* @param con_parent
	*/
	void evaluate(Layer& parent_con);
	/**
	* Evaluate output layer error
	* @param y
	*/
	void error(double y);
	/**
	* Evaluate error of connected neutrons.
	* @param child_con
	*/
	void error(Layer& child_con);
	/**
	*
	* @return
	*/
	double getLossSquare();
	double getLossAbs();
private:
	/**
	* Activation function
	*/
	double g();
	double g_p();

};


class Layer {

public:
	int _size;
	std::vector<Neuron*> _neuron;

	Layer(Neural& neural, int layer_size, int child_layer_size);
	~Layer();
};

class Neural {
private:

	std::vector<Layer*> _layer;
	int _layers;


	void init(int* layers, int length);
public:

	mf::RandomGenerator& _random;
	//Step size
	double _alpha = 1;

	/* Construct a network of certain size.
	*/
	Neural(mf::RandomGenerator& random, int layers, int network_size);
	/* Construct a network of set of layer sizes.
	*/
	Neural(mf::RandomGenerator& random, int* layers, int length);
	~Neural();

	/**
	* Evaluate the input
	* @param f_input
	*/
	void evaluate(double* f_input);
	/**
	* Evaluate the output error
	* @param f_output
	*/
	void learn(double* f_output);
	/**
	* Execute a single back propagation
	* @param f_input
	* @param f_output
	*/
	void backPropLearn(double* f_input, double* f_output);

	void printAvgError();
	/**
	* Get the ouput layer.
	* @return
	*/
	std::vector<double> getOuput();
	/**
	* Get the avg. square loss
	* @return
	*/
	double getAvgSquareLoss();
	/**
	* Get the avg. absolute loss
	* @return
	*/
	double getAvgAbsLoss();
	double getAbsLoss(int index);
	double getSquareLoss(int index);

	/**
	* Read the neural network of created size from file. Must match size...
	* @param file
	*/
	void read(std::string file);
	bool store(std::string file);
};