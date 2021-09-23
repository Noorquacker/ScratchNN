#ifndef QUACKTEXT_H
#define QUACKTEXT_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <string>
#include <cmath>


class QuackText {
	public:
		QuackText(std::string ttf, int size, SDL_Window* window);
		~QuackText();
		void RenderText(SDL_Surface* surface, SDL_Rect* rect, std::string str, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
	private:
		TTF_Font* font;
		SDL_Renderer* renderer;
};

#endif // QUACKTEXT_H
