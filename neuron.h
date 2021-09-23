#ifndef NEURON_H
#define NEURON_H


class Neuron {
	public:
		Neuron(Neuron* inputs, unsigned int size_inputs);
		float (*Activate)(float in);
		void Compute();
		static float sigmoid(float in);
		float getOutput();
		float* getWeights();
		float getWeight(unsigned int n);
		void setWeight(unsigned int n, float w);
		void setWeights(float* weights);
		float getBias();
		void setBias(float b);
		float getInput(unsigned int n);
	protected:
		float output;
	private:
		Neuron* inputs;
		float* weights;
		unsigned int inputCount; //Swear on Jah if one y'all connect more than 4,294,whatever,000 neurons imma make this closed source
		float bias;
};

#endif // NEURON_H
			  //qt creator adding auto comments like "the volume up button increases volume
