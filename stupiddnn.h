#ifndef STUPIDDNN_H
#define STUPIDDNN_H
#include "neuron.h"
#include "inputneuron.h"


class StupidDNN {
	public:
		StupidDNN(int input_width, int output_width, int hidden_width, int hidden_layers);
		StupidDNN(StupidDNN& olddnn);
		~StupidDNN();
		float* inputs; //screw double precision, i dont have time to sit and wait for that
		float* outputs;
		static float sigmoid(float sum);
		void SubmitWeightArray(float* weightList);
		void SubmitBiasArray(float* biasList);
		void Shuffle();
		void Compute();
		void setActivate(float (*Activator)(float in));
		unsigned int getWeightCount();
		unsigned int getBiasCount();
		void Mutate(float (*WeightModifier)(float weight), float (*BiasModifier)(float bias));
	protected:
		InputNeuron* inputN;
		Neuron* outputN;
		Neuron* hiddens;
		int inputSize;
		int outputSize;
		int hiddenSize;
		int hiddenLayerCount;
		float* hiddenOutputs;
		float* biasArray;
		float* weightArray;
		void allocateMemory();
		void wire();
};

#endif // STUPIDDNN_H
