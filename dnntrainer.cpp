#include <cmath>
#include "dnntrainer.h"

DNNTrainer::DNNTrainer(int inputSize, int outputSize, int hiddenSize, int hiddenLayers, int threads) {
	this->threads = threads;
}

void DNNTrainer::setTrain(idx* file_images, idx* file_labels) {
	images = file_images;
	labels = file_labels;
}

void DNNTrainer::setTest(idx* file_images, idx* file_labels) {
	testImages = file_images;
	testLabels = file_labels;
}

float DNNTrainer::TrainEvaluate(StupidDNN *dnn, int batchSize) {
	unsigned char* imgBuffer = (unsigned char*)malloc(28*28);
	int trueLabel = -1;
	float trashness = 0;
	for(int img = 0; img < batchSize; img++) {
		images->getItem(img, imgBuffer);
		for(int i = 0; i < 28*28; i++){
			float test = (float)((unsigned int)imgBuffer[i] / 255.f);
			float* thing = dnn->inputs + i;
			*thing = test;
		}
		labels->getItem(img, (unsigned char*)&trueLabel);
		dnn->Compute();
		for(int i = 0; i < 10; i++) {
			trashness += 0.1f * powf((trueLabel == i ? 1 : 0) - dnn->outputs[i], 2);
		}
	}
	trashness /= (float)batchSize;
	free(imgBuffer);
	return 1.f - trashness;
}

int DNNTrainer::getResult(StupidDNN *dnn, unsigned char *buffer) {
	for(int i = 0; i < 28*28; i++) {
		dnn->inputs[i] = (float/*just being safe dont mind this cast*/)((unsigned int)buffer[i] / 255.f);
	}
	dnn->Compute();
	int result = 0;
	//Quick Max: just assume 0 is the result and set it if any is stronger
	for(int i = 0; i < 10; i++) {
		if(dnn->outputs[i] > dnn->outputs[result]) {
			result = i;
		}
	}
	return result;
}

float DNNTrainer::TrainEvaluate(StupidDNN *dnn) {
	return TrainEvaluate(dnn, 60000);
}

float DNNTrainer::TestEvaluate(StupidDNN *dnn, int batchSize) {
	unsigned char* imgBuffer = (unsigned char*)malloc(28*28);
	int correct = 0;
	int answer = 0;
	for(int img = 0; img < batchSize; img++) {
		testImages->getItem(img, imgBuffer);
		testLabels->getItem(img, (unsigned char*)&answer);
		correct += getResult(dnn, imgBuffer) == answer ? 1 : 0;
	}
	free(imgBuffer);
	return (float)correct / (float)batchSize; //gcc/clang/icc please optimize this ;-;
}
