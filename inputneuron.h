#ifndef INPUTNEURON_H
#define INPUTNEURON_H
#include "neuron.h"


class InputNeuron : public Neuron {
	public:
		InputNeuron();
		void setOutput(float in);
};

#endif // INPUTNEURON_H
