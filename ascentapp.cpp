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
	engine = new Engine;
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
	if (TTF_Init() == -1) {
		fprintf(stderr, "Could not initliase SDL_ttf: %s\n", TTF_GetError());
		return false;
	}
	font = TTF_OpenFont(FONT_PATH, FONT_POINT);
	if (font == NULL) {
		fprintf(stderr, "Could not load font: %s\n", TTF_GetError());
		return false;
	}
	if ((renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED)) == NULL) {
		fprintf(stderr, "Could not create renderer. SDL error: %s\n", SDL_GetError());
		return false;
	}
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	if (Init_Background() == false)
		return false;
	if (Init_Foreground() == false)
		return false;
	running = true;
	return true;
}

void AscentApp::OnLoop() {

	if (!plan.empty()) {
		if (!engine->Move(plan.front())) {
			while (!plan.empty())
				plan.pop();
		} else
			plan.pop();
	}
}

void AscentApp::OnRender() {
	updateWinParameters();
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(renderer);
	for (int x = 0; x < numSquaresX; x++)
		for (int y = 0; y < numSquaresY; y++) {
			Point corP = Point(x - numSquaresX / 2, y - numSquaresY / 2);
			renderBackground(engine->getBackground(corP), x, y);
			renderForeground(engine->getForeground(corP), x, y);
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
	drawStatusBox();
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
		case SDL_MOUSEBUTTONDOWN:
			if (mouseInSquares && event->button.button == SDL_BUTTON_LEFT) {
				Point dest = {
					mouseSquareX - numSquaresX / 2,
					mouseSquareY - numSquaresY / 2
				};
				std::queue<Direction> * moves = engine->playerAstar(Point(0,0), dest);
				while (!moves->empty()) {
//					printf("%d\n", (int)moves->front());
					plan.push(moves->front());
					moves->pop();
				}
				delete moves;
//				printf("\n");
			}
		default:
			break;
	}
}

void AscentApp::OnCleanup() {
	delete engine;
	SDL_DestroyTexture(backgroundSpriteSheet);
	TTF_CloseFont(font);
	font = NULL;

	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	window = NULL;
	renderer = NULL;
	TTF_Quit();
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
		case SDLK_LEFT:
		case SDLK_h:
		case SDLK_KP_4:
			plan.push(Direction::Left);
			break;
		case SDLK_RIGHT:
		case SDLK_l:
		case SDLK_KP_6:
			plan.push(Direction::Right);
			break;
		case SDLK_UP:
		case SDLK_k:
		case SDLK_KP_8:
			plan.push(Direction::Up);
			break;
		case SDLK_DOWN:
		case SDLK_j:
		case SDLK_KP_2:
			plan.push(Direction::Down);
			break;
		case SDLK_y:
		case SDLK_KP_7:
			plan.push(Direction::UpLeft);
			break;
		case SDLK_u:
		case SDLK_KP_9:
			plan.push(Direction::UpRight);
			break;
		case SDLK_b:
		case SDLK_KP_1:
			plan.push(Direction::DownLeft);
			break;
		case SDLK_n:
		case SDLK_KP_3:
			plan.push(Direction::DownRight);
			break;
		case SDLK_KP_5:
		case SDLK_PERIOD:
			engine->doMonsterTurns();
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
	for (uint8_t i = (uint8_t)Background::EMPTYNESS; i < (uint8_t)Background::TOTAL; i++) {
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
	if (background == Background::EMPTYNESS)
		return;
	SDL_Rect orect = backgroundSpriteRect[background];
	SDL_Rect drect = {
		xOfSquare(xsquare), 
		yOfSquare(ysquare), 
		SQUARE_SIZE, 
		SQUARE_SIZE
	};
	SDL_RenderCopy(renderer, backgroundSpriteSheet, &orect, &drect);
}

bool AscentApp::Init_Foreground() {
	SDL_Surface* fgload = NULL;
	if ((fgload = SDL_LoadBMP("ForegroundSprites.bmp")) == NULL) {
		fprintf(stderr, "Could not load foreground sprites. SDL error: %s\n", SDL_GetError());
		return false;
	}
	SDL_SetColorKey(fgload, SDL_TRUE, SDL_MapRGB(fgload->format, 0xFF, 0, 0xFF));
	if ((foregroundSpriteSheet = SDL_CreateTextureFromSurface(renderer, fgload)) == NULL) {
		fprintf(stderr, "Could not convert foreground surface to texture. SDL error: %s\n", SDL_GetError());
		return false;
	}
	SDL_FreeSurface(fgload);
	int x = 0;
	int y = 0;
	for (uint8_t i = (uint8_t)Foreground::NONE; i < (uint8_t)Foreground::TOTAL; i++) {
		foregroundSpriteRect[(Foreground)i] = {
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

void AscentApp::renderForeground(Foreground foreground, int xsquare, int ysquare) {
	if (foreground == Foreground::NONE)
		return;
	SDL_Rect orect = foregroundSpriteRect[foreground];
	SDL_Rect drect = {
		xOfSquare(xsquare),
		yOfSquare(ysquare),
		SQUARE_SIZE,
		SQUARE_SIZE
	};
	SDL_RenderCopy(renderer, foregroundSpriteSheet, &orect, &drect);
}

void AscentApp::drawStatusBox() {
	Foreground tb = engine->underWitch();
	if (tb != Foreground::NONE) {
//		printf("Here: %s\n", foreProps.at(tb).name);
//		const char * msg = foreProps.at(tb).name;
		std::string msgstring = engine->underItemString();
//		msgstring += msg;
		SDL_Surface* textSurface = TTF_RenderText_Blended(font, msgstring.c_str(), {0xFF, 0xFF, 0xFF, 0xFF});
		if (textSurface == NULL) {
			fprintf(stderr, "Couldn\'t make the text surface. %s\n", TTF_GetError());
			return;
		}
		int message_w = textSurface->w;
		int message_h = textSurface->h;
		if (statusMessage != NULL)
			SDL_DestroyTexture(statusMessage);
		statusMessage = SDL_CreateTextureFromSurface(renderer, textSurface);
		SDL_FreeSurface(textSurface);
		if (statusMessage == NULL) {
			fprintf(stderr, "Couldn\'t create the text texture. %s\n", SDL_GetError());
			return;
		}
		SDL_Rect messageRect = {
			MESSAGE_BORDER,
			windowHeight - message_h - MESSAGE_BORDER,
			message_w,
			message_h
		};
		SDL_Rect messageBackgroundRect = {
			0,
			windowHeight - message_h - MESSAGE_BORDER * 2,
			message_w + MESSAGE_BORDER * 2,
			message_h + MESSAGE_BORDER * 2
		};
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xC0);
		SDL_RenderFillRect(renderer, &messageBackgroundRect);
		SDL_RenderCopy(renderer, statusMessage, NULL, &messageRect);
	}
}
