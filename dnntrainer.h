#ifndef DNNTRAINER_H
#define DNNTRAINER_H
#include "stupiddnn.h"
#include "idx.h"

class DNNTrainer {
	public:
		DNNTrainer(int inputSize, int outputSize, int hiddenSize, int hiddenLayers, int threads);
		void setTrain(idx* file_images, idx* file_labels);
		void setTest(idx* file_images, idx* file_labels);
		float TrainEvaluate(StupidDNN* dnn);
		float TrainEvaluate(StupidDNN* dnn, int batchSize);
		float TestEvaluate(StupidDNN* dnn);
		float TestEvaluate(StupidDNN* dnn, int batchSize);
		int getResult(StupidDNN* dnn, unsigned char* buffer);
	private:
		int threads;
		int dnnsLength;
		StupidDNN* dnns;
		idx* images;
		idx* labels;
		idx* testImages;
		idx* testLabels;
		float* fitnesses;
};

#endif // DNNTRAINER_H
