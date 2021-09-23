#include "neuron.h"
#include <cmath>

float Neuron::sigmoid(float in) {
	return 1./(1. + expf(-in));
}

float Neuron::getOutput() {
	return output;
}

void Neuron::Compute() {
	output = 0;
	for(unsigned int i = 0; i < inputCount; i++) {
		output += inputs[i].getOutput() * weights[i];
	}
	output += bias;
	output = Activate(output);
}

Neuron::Neuron(Neuron* inputs, unsigned int input_length) {
	Activate = sigmoid;
	this->inputs = inputs;
	inputCount = input_length;
	bias = 0;
}

float Neuron::getInput(unsigned int n) {
	return (inputs+n)->getOutput();
}

float Neuron::getWeight(unsigned int n) {
	return weights[n];
}

float* Neuron::getWeights() {
	return weights;
}

void Neuron::setWeights(float *weights) {
	this->weights = weights;
}

void Neuron::setWeight(unsigned int n, float w) {
	weights[n] = w;
}

float Neuron::getBias() {
	return bias;
}

void Neuron::setBias(float b) {
	bias = b;
}
