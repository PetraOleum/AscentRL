#include "ascentapp.h"
#include <cstdio>

int AscentApp::OnExecute() {
	if (!OnInit())
		return -1;

	SDL_Event event;

	while (running) {
		Uint32 lstart = SDL_GetTicks();
		while (SDL_PollEvent(&event))
			OnEvent(&event);
		OnLoop();
		OnRender();
		Uint32 lend = SDL_GetTicks();
		if ((lend - lstart) < LOOP_TIME)
			SDL_Delay(LOOP_TIME - (lend - lstart));
	}

	OnCleanup();
	return 0;
}

bool AscentApp::OnInit() {

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "Could not initialise SDL. SDL error: %s\n", SDL_GetError());
		return false;
	}
	window = SDL_CreateWindow("AscentRL", 
			SDL_WINDOWPOS_UNDEFINED, 
			SDL_WINDOWPOS_UNDEFINED, 
			INIT_SCREEN_WIDTH, 
			INIT_SCREEN_HEIGHT, 
			SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (window == NULL) {
		fprintf(stderr, "Window could not be created. SDL error: %s\n", SDL_GetError());
		return false;
	}
	screensurface = SDL_GetWindowSurface(window);
	if ((renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)) == NULL) {
		fprintf(stderr, "Could not create renderer. SDL error: %s\n", SDL_GetError());
		return false;
	}
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	if (Init_Background() == false)
		return false;
	running = true;
	return true;
}

void AscentApp::OnLoop() {

}

void AscentApp::OnRender() {
	updateWinParameters();
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(renderer);
	for (int x = 0; x < numSquaresX; x++)
		for (int y = 0; y < numSquaresY; y++) {
			if (x % 3 == 0)
				renderBackground(Background::DirtWall, x, y);
			else
				renderBackground(Background::DirtFloor, x, y);
		}
	if (mouseInSquares) {
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0xFF, 0xFF);
		int qss = SQUARE_SIZE / 4;
		int tqss = (SQUARE_SIZE * 3) / 4;
		int xos = xOfSquare(mouseSquareX);
		int yos = yOfSquare(mouseSquareY);
		SDL_RenderDrawLine(renderer, xos,
				yos + qss,
				xos,
				yos + tqss);
		SDL_RenderDrawLine(renderer, xos + SQUARE_SIZE - 1,
				yos + qss,
				xos + SQUARE_SIZE - 1,
				yos + tqss);
		SDL_RenderDrawLine(renderer, xos + qss,
				yos,
				xos + tqss,
				yos);
		SDL_RenderDrawLine(renderer, xos + qss,
				yos + SQUARE_SIZE - 1,
				xos + tqss,
				yos + SQUARE_SIZE - 1);
	}

	SDL_RenderPresent(renderer);
}

void AscentApp::OnEvent(SDL_Event* event) {
	switch (event->type) {
		case SDL_QUIT:
			running = false;
			break;
		case SDL_KEYDOWN:
			onKeyDown(&event->key);
			break;
		default:
			break;
	}
}

void AscentApp::OnCleanup() {
	SDL_DestroyTexture(backgroundSpriteSheet);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	window = NULL;
	renderer = NULL;
	SDL_Quit();
}

void AscentApp::onKeyDown(SDL_KeyboardEvent * keyEvent) {
	switch (keyEvent->keysym.sym) {
		case SDLK_F11:
			if (fullscreen) {
				SDL_SetWindowFullscreen(
						window, 
						0);
				fullscreen = false;
			} else {
				SDL_SetWindowFullscreen(
						window, 
						SDL_WINDOW_FULLSCREEN_DESKTOP);
				fullscreen = true;
			}
			break;
		case SDLK_ESCAPE:
			if (fullscreen) {
				SDL_SetWindowFullscreen(
						window,
						0);
				fullscreen = false;
			}
			break;
		default:
			break;
	}
}

bool AscentApp::Init_Background() {
	SDL_Surface* bssload = NULL;
	if ((bssload = SDL_LoadBMP("BackgroundSprites.bmp")) == NULL) {
		fprintf(stderr, "Could not load background sprites. SDL error: %s\n", SDL_GetError());
		return false;
	}
	if ((backgroundSpriteSheet = SDL_CreateTextureFromSurface(renderer, bssload)) == NULL) {
		fprintf(stderr, "Could not convert background surface to texture. SDL error: %s\n", SDL_GetError());
		return false;
	}
	SDL_FreeSurface(bssload);
	int x = 0;
	int y = 0;
	for (uint8_t i = (uint8_t)Background::TiledFloor; i < (uint8_t)Background::TOTAL; i++) {
		backgroundSpriteRect[(Background)i] = {
			x * SPRITE_SIZE, 
			y * SPRITE_SIZE, 
			SPRITE_SIZE, 
			SPRITE_SIZE
		};
		x++;
		if (x == SPRITE_SHEET_WIDTH) {
			x = 0;
			y++;
		}
	}
	return true;
}

void AscentApp::renderBackground(Background background, int xsquare, int ysquare) {
	SDL_Rect orect = backgroundSpriteRect[background];
	SDL_Rect drect = {
		xOfSquare(xsquare), 
		yOfSquare(ysquare), 
		SQUARE_SIZE, 
		SQUARE_SIZE
	};
	SDL_RenderCopy(renderer, backgroundSpriteSheet, &orect, &drect);
}
