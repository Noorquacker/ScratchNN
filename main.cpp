#include <iostream>
#include <fstream>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <cmath>
#include <SDL2/SDL_thread.h>
#include "quacktext.h"
#include "idx.h"
#include "neuron.h"
#include "inputneuron.h"
#include "stupiddnn.h"
#include "dnntrainer.h"

using namespace std;

struct doublepointer { //Literally just 2 pointers in a cast because threads be like that
		DNNTrainer* trainer;
		StupidDNN* dnn;
		int batchSize;
		float* output;
};

int runTrainer(void* threadSetting) {
	doublepointer settings = *((doublepointer*)threadSetting);
	*(settings.output) = settings.trainer->TrainEvaluate(settings.dnn, settings.batchSize);
	return 0;
}

float randomDeviate(float in) {
	return in + 0.25f*((float)rand()*2.f / (float)RAND_MAX - 1.f);
}

float null(float in) {
	return in;
}

int initSDL(SDL_Window** window, SDL_Surface** surface) {
	if(SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "SDL couldn't initialize\n");
		return 1;
	}
	*window = SDL_CreateWindow("ScratchNN", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 1000, SDL_WINDOW_SHOWN);
	if(*window == NULL) {
		fprintf(stderr, "cant make window\n");
		return 1;
	}
	*surface = SDL_GetWindowSurface(*window);
	if(*surface == NULL) {
		fprintf(stderr, "cant get surface\n");
		return 1;
	}
	return 0;
}

void set_pixel(SDL_Surface *surface, int x, int y, Uint32 pixel) {
  Uint32 *target_pixel = (Uint32*)((Uint8*)surface->pixels + y*surface->pitch + x * sizeof *target_pixel);
  *target_pixel = pixel;
}

int main() {
	idx imgdata((char*)"train-images-idx3-ubyte"), imglabels((char*)"train-labels-idx1-ubyte");
	idx imgtest((char*)"t10k-images-idx3-ubyte"), labeltest((char*)"t10k-labels-idx1-ubyte");
	if(!(imgdata.good() && imgtest.good() && imglabels.good() && labeltest.good())) {
		fprintf(stderr, "PANIC PANIC PANIC SOMETHING HAPPENED WITH THE MNIST FILES\n");
		return 69;
	}
	if(!(imgdata.magic == 2051 && imglabels.magic == 2049)) {
		fprintf(stderr, "Invalid magic numbers for IDX files: expected 2051 and 2049, got %i and %i\n", imgdata.magic, imglabels.magic);
		return 1;
	}

	//aight NOW we gonna do sdl stuff
	SDL_Window* window = NULL;
	SDL_Surface* surface = NULL;
	initSDL(&window, &surface);
	SDL_Event e;
	bool drawing = true;
	QuackText* quackText = new QuackText("times.ttf", 12, window);


	//StupidDNN* dnn = (StupidDNN*)malloc(sizeof(StupidDNN));
	//StupidDNN* dnn2 = (StupidDNN*)malloc(sizeof(StupidDNN));
	printf("Proceeding to test all...\n");
	DNNTrainer* trainer = new DNNTrainer(0, 0, 0, 0, 0);
	trainer->setTrain(&imgdata, &imglabels);
	trainer->setTest(&imgtest, &labeltest);

	//new(dnn) StupidDNN(784, 10, 10, 1);
	//new(dnn2) StupidDNN(784, 10, 10, 1);

	/*
	StupidDNN* dnn = (StupidDNN*)malloc(sizeof(StupidDNN));
	new(dnn) StupidDNN(784, 10, 10, 1);
	StupidDNN* dnn2 = (StupidDNN*)malloc(sizeof(StupidDNN));
	new(dnn2) StupidDNN(784, 10, 10, 1);
	*/

	int cpus = sysconf(_SC_NPROCESSORS_ONLN) / 2;
	cpus = 12;
	StupidDNN** dnns = (StupidDNN**)malloc(sizeof(StupidDNN)*cpus);
	for(int i = 0; i < cpus; i++) {
		dnns[i] = new StupidDNN(784, 10, 10, 1);
	}

	float* weights = new float[dnns[0]->getWeightCount()];
	for(unsigned int i = 0; i < dnns[0]->getWeightCount(); i++) {
		weights[i] = 1.f;
	}
	float* biases = new float[dnns[0]->getBiasCount()];
	for(unsigned int i = 0; i < dnns[0]->getBiasCount(); i++) {
		biases[i] = 0.f;
	}

	for(int i = 0; i < cpus; i++) {
		dnns[i]->SubmitBiasArray(biases);
		dnns[i]->SubmitWeightArray(weights);
	}

	delete[] biases;
	delete[] weights;
	printf("Begin: %f\n", trainer->TrainEvaluate(dnns[0], 200));
	fflush(stdout);

	SDL_Thread* threads[cpus];
	doublepointer threadSettings[cpus];
	float marks[cpus]; //You know an Indian wrote the code when grades are called marks
	int winner = 0;
	printf("Okay, we're STARTING!\n");
	fflush(stdout);
	for(int gen = 0; gen < 10; gen++) {
		// Compiler, do wtf you want with this
		for(int cpu = 0; cpu < cpus; cpu++) {
			threadSettings[cpu].dnn = dnns[cpu];
			threadSettings[cpu].trainer = trainer;
			threadSettings[cpu].batchSize = 200;
			threadSettings[cpu].output = &(marks[0]) + cpu;
			threads[cpu] = SDL_CreateThread(runTrainer, "Massive neural network goes nom on your cpu", (void*)&threadSettings[cpu]);
		}
		for(int cpu = 0; cpu < cpus; cpu++) {
			SDL_WaitThread(threads[cpu], NULL);
		}
		for(int cpu = 0; cpu < cpus; cpu++) {
			winner = marks[cpu] > marks[winner] ? marks[cpu] : winner;
		}
		for(int cpu = 0; cpu < cpus; cpu++) {
			if(winner != cpu) {
				dnns[cpu]->~StupidDNN(); //Haha get rekt by my artificial selection you virgin victim of my Darwinist code
				new(dnns[cpu]) StupidDNN(*dnns[cpu]);
				dnns[cpu]->Mutate(randomDeviate, randomDeviate);
			}
			else {
				printf("Winner of gen %i had accuracy %f\n", gen, marks[winner]);
				fflush(stdout);
			}
		}
		//SDL_UpdateWindowSurface(window);
	}
	printf("Final: %f\n", trainer->TrainEvaluate(dnns[winner], 200));
	fflush(stdout);
	printf("Thing: %f\n", trainer->TestEvaluate(dnns[winner], 500));
	//free(dnn);
	//free(dnn2);
	delete trainer;

	//the draw loop for this thread
	while(drawing) {
		Uint32 a = SDL_GetTicks();

		//idk why we would draw all of this stuff but whatever here it is anyways
		//kinda cool i guess
		for(int j = 0; j < 10; j++) {
			for(int i = 0; i < 10; i++) {
				unsigned char bruh[28*28];
				imgdata.getItem(i+j*10, bruh);
				for(int x = 0; x < 28; x++) {
					for(int y = 0; y < 28; y++) {
						Uint32 pixel = (bruh[y*28+x] << 16) + (bruh[y*28+x] << 8) + bruh[y*28+x];
						set_pixel(surface, x+1+i*28*2, y+1+j*29, pixel);
					}
				}
			}
		}

		/*
		 * You don't matter
		 * (the code, not you, reader)
		 * (you matter a lot :)

		fit1 = trainer->TrainEvaluate(dnn,2000);
		fit2 = trainer->TrainEvaluate(dnn2,2000);
		if(fit1 > fit2) {
			dnn2->~StupidDNN();
			new(dnn2) StupidDNN(*dnn);
			dnn2->Mutate(randomDeviate, null);
		}
		else{
			dnn->~StupidDNN();
			new(dnn) StupidDNN(*dnn2);
			dnn->Mutate(randomDeviate, null);
		}
		*/

		SDL_Rect rectthing = {0, 0, 100, 100};
		quackText->RenderText(surface, &rectthing, "bruh", 255, 255, 255, 255);


		while(SDL_PollEvent(&e) != 0) {
			if(e.type == SDL_QUIT) {
				drawing = false;
			}
			else if(e.type == SDL_KEYDOWN) {
				cout << SDL_GetKeyName(e.key.keysym.sym) << endl;
			}
		}
		fflush(stdout);
		SDL_UpdateWindowSurface(window);
		if(SDL_GetTicks() - a < 17) {
			usleep((SDL_GetTicks() - a) * 1000);
		}
	}
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}
