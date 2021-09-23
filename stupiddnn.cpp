#include "stupiddnn.h"
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <new>
#include <algorithm>

//         G I V E M E Y O U R R A M
void StupidDNN::allocateMemory() {
	//"In fact, you shouldn't use malloc and free. Use new[] and delete[] as they will run constructors and destructors"
	//Frick off y'all, I do what I want, I allocate massive blocks of memory and run constructors separate with placement new
	inputN = (InputNeuron*)malloc(sizeof(InputNeuron)*inputSize);
	hiddens = (Neuron*)malloc(sizeof(Neuron)*hiddenSize*hiddenLayerCount);
	outputN = (Neuron*)malloc(sizeof(Neuron)*outputSize);
	biasArray = (float*)calloc(hiddenSize*hiddenLayerCount+outputSize, sizeof(float));
	weightArray = (float*)calloc(inputSize * hiddenSize + hiddenLayerCount * hiddenSize * hiddenSize, sizeof(float));
	outputs = (float*)calloc(outputSize, sizeof(float)); //segfaulted because i forgot that part

	/*
	 * I SPENT A WHOLE NIGHT IN PAIN
	 * I COUDLNT FRIGNGIN SLEEP ECAUSE OF THIS
	 * AGJAHGDJADSHGKDSAHG
	 */
	inputs = (float*)calloc(inputSize, sizeof(float));
}

void StupidDNN::wire() {
	/*
 * "Don’t use this “placement new” syntax unless you have to," they said.
 * "You are taking sole responsibility that the pointer you pass to the “placement new” operator points to a region of memory
 * that is big enough and is properly aligned for the object type that you’re creating," they said
 * "Neither the compiler nor the run-time system make any attempt to check whether you did this right," they said.
 * "You have been warned," they said.
 *
 * "Frick y'all,"  I said. (it worked btw)
 * */
	//PS: if there's a segfault, it's probably here

	//the easy part
	for(int i = 0; i < inputSize; i ++) {
		InputNeuron* inputNeuron = (inputN + i); // P O I N T E R S C A L I N G S A V E S T H E D A Y
		new(inputNeuron) InputNeuron();
	}

	//the fun part. first hidden layer is irregular
	for(int depth = 0; depth < hiddenSize; depth++) {
		Neuron* neuron = hiddens + depth;
		new(neuron) Neuron(inputN, inputSize);
	}
	for(int layer = 1; layer < hiddenLayerCount; layer++) {
		for(int depth = 0; depth < hiddenSize; depth++) {
			Neuron* boomerneurons = hiddens + (layer - 1) * hiddenSize;
			Neuron* neuron = hiddens + layer * hiddenSize + depth;
			new(neuron) Neuron(boomerneurons, hiddenSize);
		}
	}
	for(int depth = 0; depth < outputSize; depth++) {
		Neuron* boomerneurons = hiddens + (hiddenLayerCount - 1) * hiddenSize;
		Neuron* neuron = outputN + depth;
		new(neuron) Neuron(boomerneurons, hiddenSize);
	}
	//please tell me i'm done wiring up a neural network by hand
}

StupidDNN::StupidDNN(int input_width, int output_width, int hidden_width, int hidden_layers){
	inputSize = input_width;
	outputSize = output_width;
	hiddenSize = hidden_width;
	hiddenLayerCount = hidden_layers;

	allocateMemory();
	wire();
}

StupidDNN::~StupidDNN() {
	free(inputN);
	free(hiddens);
	free(outputN);
	free(biasArray);
	free(weightArray);
	free(outputs);
	free(inputs);
}

unsigned int StupidDNN::getBiasCount() {
	return hiddenSize * hiddenLayerCount + outputSize; //Whiteboard verified
}

unsigned int StupidDNN::getWeightCount() {
	return inputSize * hiddenSize + hiddenLayerCount * hiddenSize * hiddenSize; //Whiteboard verified
}

//ah frick i still have to configure the whole thing huh
void StupidDNN::SubmitBiasArray(float *biasList) { //Whiteboard verified
	if(biasArray != biasList) {
		memcpy(biasArray, biasList, sizeof(float)*getBiasCount());
	}
	for(int layer = 0; layer < hiddenLayerCount; layer++) {
		for(int depth = 0; depth < hiddenSize; depth++) {
			Neuron* neuron = hiddens + layer * hiddenSize + depth;
			float bias = *(biasList + layer * hiddenSize + depth);
			neuron->setBias(bias);
		}
	}
	float* outputBiases = biasList + hiddenSize * hiddenLayerCount;
	for(int i = 0; i < outputSize; i++) {
		outputN[i].setBias(outputBiases[i]);
	}
}

void StupidDNN::SubmitWeightArray(float* weightList) {
	if(weightList != weightArray) {
		memcpy(weightArray, weightList, sizeof(float)*getWeightCount()); //what the frick is wrong with memcpy
		//nothing, turns out I forgot this ^^^^^
	}

	/*
	 * EVERYTHING HERE IS WHITEBOARD VERIFIED AND PASSED ALL TESTS (so far)
	 * HOWEVER, THIS IS PROBABLY THE PART OF THE CODE CAUSING UNPREDICTABLE OUTPUT
	 */
	//First Hidden Neurons weight config - irregular because each neuron has inputSize weights
	for(int i = 0; i < hiddenSize; i++) {
		Neuron* neuron = hiddens + i;
		float* weights = weightArray + i * inputSize;
		neuron->setWeights(weights);
	}

	//Rest of hidden neurons, all have hiddenSize weights
	for(int layer = 1; layer < hiddenLayerCount; layer++) {
		for(int depth = 0; depth < hiddenSize; depth++) {
			Neuron* neuron = hiddens + layer * hiddenSize + depth;
			float* weights = weightArray + inputSize * hiddenSize + (layer - 1) * hiddenSize * hiddenSize + depth * hiddenSize;
			neuron->setWeights(weights);
		}
	}

	//Output neurons, each has hiddenSize weights
	for(int i = 0; i < outputSize; i++) {
		Neuron* neuron = outputN + i;
		float* weights = weightArray + inputSize * hiddenSize + (hiddenLayerCount - 1) * hiddenSize * hiddenSize + i * hiddenSize;
		neuron->setWeights(weights);
	}
}

void StupidDNN::Shuffle() {
	for(unsigned int i = 0; i < getBiasCount(); i++) {
		biasArray[i] = ((float)rand()*2.f / (float)RAND_MAX - 1.f);
	}
	for(unsigned int i = 0; i < getWeightCount(); i++) {
		weightArray[i] = ((float)rand()*2.f / (float)RAND_MAX - 1.f);
	}
	SubmitBiasArray(biasArray);
	SubmitWeightArray(weightArray);
}

float StupidDNN::sigmoid(float sum) {
	return 1./(1+exp(-sum));
}

void StupidDNN::Compute() {
	//nearly forgot to set inputs correctly until i just about ran it
	//yall would laugh at my struggling
	//this is why i dont release source code for my projects
	//lmao probably talking to myself
	for(int i = 0; i < inputSize; i++) {
		inputN[i].setOutput(inputs[i]);
	}
	//oh boy
	for(int layer = 0; layer < hiddenLayerCount; layer++) {
		for(int depth = 0; depth < hiddenSize; depth++) {
			Neuron* neuron = hiddens + layer * hiddenSize + depth;
			neuron->Compute();
		}
	}
	//do do do i Compute() all the neurons and do the following when i am done
	for(int i = 0; i < outputSize; i++) {
		//do do do i am an idiot and forgot to compute the output neurons バアアアアアアカ
		outputN[i].Compute();
		outputs[i] = outputN[i].getOutput();
	}
	//oh that was easy
}

void StupidDNN::setActivate(float (*Activator)(float)) {
	for(int i = 0; i < hiddenSize * hiddenLayerCount; i++) {
		hiddens[i].Activate = Activator;
	}
	for(unsigned int i = hiddenSize * hiddenLayerCount; i < getBiasCount(); i++) {
		outputN[i - hiddenSize * hiddenLayerCount].Activate = Activator;
	}
}

//Ok, now we can actually do stuff like reproduction and mutation stuff

//DNN duplication
StupidDNN::StupidDNN(StupidDNN& olddnn) {
	inputSize = olddnn.inputSize;
	outputSize = olddnn.outputSize;
	hiddenSize = olddnn.hiddenSize;
	hiddenLayerCount = olddnn.hiddenLayerCount;

	allocateMemory();
	wire();
	//copy weights and biases in order to have a wonderful copy of our dnn
	SubmitBiasArray(olddnn.biasArray);
	SubmitWeightArray(olddnn.weightArray);
	//Gosh I friggin love C++ (note: I wrote this before I tested the entire copy constructor)
	//(note: it worked. haha you all suck at coding)
	setActivate(olddnn.hiddens[0].Activate); //very ugly but it works
}

//Mutation via bias and weight modification
void StupidDNN::Mutate(float (*WeightModifier)(float weight), float (*BiasModifier)(float bias)) {
	for(unsigned int i = 0; i < getBiasCount(); i++) {
		biasArray[i] = BiasModifier(biasArray[i]);
	}
	for(unsigned int i = 0; i < getWeightCount(); i++) {
		weightArray[i] = WeightModifier(weightArray[i]);
	}
	//I know it's redundant when they memcpy the source to the dest but idk I'll let gcc and clang do whatever the frick they want
	SubmitBiasArray(biasArray);
	SubmitWeightArray(weightArray);
}
