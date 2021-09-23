#include "inputneuron.h"
#include <iostream>

InputNeuron::InputNeuron() : Neuron(nullptr, (unsigned int)NULL){
	output = 0;
	//uh nothing here for now i guess
}

void InputNeuron::setOutput(float in) {
	output = in;
}

//thats all folks
