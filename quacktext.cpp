#include "quacktext.h"

QuackText::QuackText(std::string ttf, int size, SDL_Window* window) {
	TTF_Init();
	renderer = SDL_CreateRenderer(window, -1, 0);
	TTF_Font* thing = TTF_OpenFont("Roboto-Regular.ttf", 25);
	this->font = thing;
}

QuackText::~QuackText() {
	TTF_CloseFont(font);
	TTF_Quit();
}

void QuackText::RenderText(SDL_Surface* surface, SDL_Rect* rect, std::string str, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	TTF_Font* thing3 = TTF_OpenFont("Roboto-Regular.ttf", 25);
	font = thing3;
	SDL_Color col = {r, g, b, a};
	SDL_Surface* thing = TTF_RenderText_Solid(font, str.c_str(), col);

	SDL_BlitSurface(thing, nullptr, surface, rect);

	SDL_FreeSurface(thing);
}
