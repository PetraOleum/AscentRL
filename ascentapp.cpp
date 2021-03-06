#include "ascentapp.h"
#include <cstdio>
#include <sstream>

/// @brief Extract the alfanumeric from a SDL_KeyboardEvent
///
/// @param keyEvent The key event
///
/// @return a-zA-Z0-9, or '\0' if not one of those
char getalphanumeric(SDL_KeyboardEvent * keyEvent);

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
	currentlyDisplaying = windowType::Map;
	userInputRequested = InputType::Standard;
	return true;
}

void AscentApp::OnLoop() {

	if (!plan.empty() && currentlyDisplaying == windowType::Map) {
		if (!engine->Act(plan.front())) {
			while (!plan.empty())
				plan.pop();
		} else
			plan.pop();
	}

	if (toggledfullscreen) 
		toggledfullscreen--;

	if (engine->playerHP() <= 0) {
		currentlyDisplaying = windowType::LossScreen;
		userInputRequested = InputType::EndGame;
	}
}

void AscentApp::OnRender() {
	updateWinParameters();
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(renderer);

	switch (currentlyDisplaying) {
		case windowType::Map:
			MapRender();
			break;
		case windowType::Inventory:
			InventoryRender();
			break;
		case windowType::LossScreen:
			LossScreenRender();
			break;
		default:
			fprintf(stderr, "Nonimplemented window type %d", (int)currentlyDisplaying);
			break;
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
			switch (userInputRequested) {
				case InputType::Standard:
					onKeyDown_Standard(&event->key);
					break;
				case InputType::InventoryItemToDrop:
				case InputType::InventoryItemToView:
					onKeyDown_Inventory(&event->key);
					break;
				case InputType::EndGame:
					onKeyDown_EndGame(&event->key);
					break;
				default:
					fprintf(stderr, "Non-implemented InputType\n");
					break;
			}
		case SDL_MOUSEBUTTONDOWN:
			if (mouseInSquares && event->button.button == SDL_BUTTON_LEFT && userInputRequested == InputType::Standard) {
				Point dest = {
					mouseSquareX - numSquaresX / 2,
					mouseSquareY - numSquaresY / 2
				};
				std::queue<Direction> * moves = engine->playerAstar(Point(0,0), dest);
				while (!moves->empty()) {
//					printf("%d\n", (int)moves->front());
					plan.push({ActionType::Move, moves->front(), '\0'});
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

void AscentApp::onKeyDown_Standard(SDL_KeyboardEvent * keyEvent) {
	switch (keyEvent->keysym.sym) {
		case SDLK_F11:
			togglefullscreen();
			break;
		case SDLK_ESCAPE:
			setfullscreen(false);
			break;
		case SDLK_LEFT:
		case SDLK_h:
		case SDLK_KP_4:
			plan.push({ActionType::Move, Direction::Left, '\0'});
			break;
		case SDLK_RIGHT:
		case SDLK_l:
		case SDLK_KP_6:
			plan.push({ActionType::Move, Direction::Right, '\0'});
			break;
		case SDLK_UP:
		case SDLK_k:
		case SDLK_KP_8:
			plan.push({ActionType::Move, Direction::Up, '\0'});
			break;
		case SDLK_DOWN:
		case SDLK_j:
		case SDLK_KP_2:
			plan.push({ActionType::Move, Direction::Down, '\0'});
			break;
		case SDLK_y:
		case SDLK_KP_7:
			plan.push({ActionType::Move, Direction::UpLeft, '\0'});
			break;
		case SDLK_u:
		case SDLK_KP_9:
			plan.push({ActionType::Move, Direction::UpRight, '\0'});
			break;
		case SDLK_b:
		case SDLK_KP_1:
			plan.push({ActionType::Move, Direction::DownLeft, '\0'});
			break;
		case SDLK_n:
		case SDLK_KP_3:
			plan.push({ActionType::Move, Direction::DownRight, '\0'});
			break;
		case SDLK_KP_5:
		case SDLK_PERIOD:
			plan.push({ActionType::NONE, Direction::NONE, '\0'});
			break;
		case SDLK_g:
			plan.push({ActionType::Pickup, Direction::NONE, '\0'});
			break;
		case SDLK_d:
//			plan.push({ActionType::Drop, Direction::NONE, 'a'}); //Temporary implementation
			currentlyDisplaying = windowType::Inventory;
			userInputRequested = InputType::InventoryItemToDrop;
			break;
		case SDLK_i: // Viewing inventory only partially inplemented
			currentlyDisplaying = windowType::Inventory;
			userInputRequested = InputType::InventoryItemToView;
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
	std::stringstream msgstringstream;

	switch (userInputRequested) {
		case InputType::Standard: 
			{
				Foreground tb = engine->underWitch();
				if (tb != Foreground::NONE) {
					msgstringstream << engine->underItemString() << "\n";
				} 
				msgstringstream << "HP: " << engine->playerHP();
				if (engine->playerMaxHP())
					msgstringstream << " (MAX)";
				if (!engine->playerAlive())
					msgstringstream << " (Dead)";
			}
			break;
		case InputType::InventoryItemToDrop:
			msgstringstream << "Select an item to drop (a-zA-Z); Esc to cancel\n";
			break;
		case InputType::InventoryItemToView:
			msgstringstream << "Select an item to view (Not implemented); Esc to cancel\n";
			break;
		case InputType::EndGame:
			msgstringstream << "Esc to quit";
			break;
		default:
			return;
	}


	std::string msgstring = msgstringstream.str();

	if (msgstring.length() == 0)
		return;
	SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, msgstring.c_str(), {0xFF, 0xFF, 0xFF, 0xFF}, windowWidth - MESSAGE_BORDER * 4);
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
//		message_w + MESSAGE_BORDER * 2,
		windowWidth,
		message_h + MESSAGE_BORDER * 2

	};
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xC0);
	SDL_RenderFillRect(renderer, &messageBackgroundRect);
	SDL_RenderCopy(renderer, statusMessage, NULL, &messageRect);
}

void AscentApp::renderHPLine(double HPPercentage, int xsquare, int ysquare) {
	if (HPPercentage > 1)
		HPPercentage = 1;
	else if (HPPercentage < 0)
		HPPercentage = 0;

	int xos = xOfSquare(xsquare);
	int yos = yOfSquare(ysquare);

	int divider = SQUARE_SIZE * HPPercentage;

	SDL_Rect remainingHealthRect = {
		xos, yos,
		divider,
		3
	};
	SDL_Rect redHealthRect = {
		xos + divider,
		yos,
		SQUARE_SIZE - divider,
		3
	};

	SDL_SetRenderDrawColor(renderer, 0x00, 0xBF, 0x00, 0xFF);
	SDL_RenderFillRect(renderer, &remainingHealthRect);
//	SDL_RenderDrawLine(renderer,
//			xos,
//			yos,
//			xos + divider - 1,
//			yos);
	SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
	SDL_RenderFillRect(renderer, &redHealthRect);
//	SDL_RenderDrawLine(renderer,
//			xos + divider,
//			yos,
//			xos + SQUARE_SIZE - 1,
//			yos);
}

void AscentApp::MapRender() {
	for (int x = 0; x < numSquaresX; x++)
		for (int y = 0; y < numSquaresY; y++) {
			Point corP = Point(x - numSquaresX / 2, y - numSquaresY / 2);
			renderBackground(engine->getBackground(corP), x, y);
			renderForeground(engine->getForeground(corP), x, y);
			if (engine->seeCreatureHere(corP))
				renderHPLine(engine->creatureHPPercentHere(corP), x, y);
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
}

void AscentApp::InventoryRender() {
	std::stringstream invstream;
	const Inventory& inventory = engine->getPlayerInventory();
	if (inventory.total() > 0) {
		invstream << "Inventory:\n";
		for (int i = 0; i < 26 * 2; i++) {
			char c = INV_indextochar(i);
			inventory_entry_t ientry = inventory[c];
			if (ientry.second > 0) {
				invstream << c << " - " << itemProperties.at(ientry.first).name << " (" << ientry.second << ")\n";
			}
		}
	} else {
		invstream << "Inventory Empty\nPress Esc";
	}
	std::string invstring = invstream.str();
	SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, invstring.c_str(), {0xFF, 0xFF, 0xFF, 0xFF}, windowWidth - MESSAGE_BORDER * 4);
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
		MESSAGE_BORDER,
		message_w,
		message_h
	};
	SDL_RenderCopy(renderer, statusMessage, NULL, &messageRect);
}

void AscentApp::onKeyDown_Inventory(SDL_KeyboardEvent * keyEvent) {
	// If is an alphanumeric char, use
	char anchar = getalphanumeric(keyEvent);
	if (anchar != '\0') 
		if (userInputRequested == InputType::InventoryItemToDrop) {
			dropItem(anchar);
			return;
		} // When implementing InventoryItemToView, need something here
	
	// Other key input types
	switch (keyEvent->keysym.sym) {
		case SDLK_F11:
			togglefullscreen();
			break;
		case SDLK_ESCAPE:
			currentlyDisplaying = windowType::Map;
			userInputRequested = InputType::Standard;
			break;
		default:
			break;
	}
}

char getalphanumeric(SDL_KeyboardEvent * keyEvent) {

	bool shifted = (keyEvent->keysym.mod & KMOD_SHIFT) ^ (keyEvent->keysym.mod & KMOD_CAPS);

	switch (keyEvent->keysym.sym) {
		case SDLK_0:
		case SDLK_KP_0:
			return '0';
		case SDLK_1:
		case SDLK_KP_1:
			return '1';
		case SDLK_2:
		case SDLK_KP_2:
			return '2';
		case SDLK_3:
		case SDLK_KP_3:
			return '3';
		case SDLK_4:
		case SDLK_KP_4:
			return '4';
		case SDLK_5:
		case SDLK_KP_5:
			return '5';
		case SDLK_6:
		case SDLK_KP_6:
			return '6';
		case SDLK_7:
		case SDLK_KP_7:
			return '7';
		case SDLK_8:
		case SDLK_KP_8:
			return '8';
		case SDLK_9:
		case SDLK_KP_9:
			return '9';
		case SDLK_a:
			if (shifted)
				return 'A';
			else
				return 'a';
		case SDLK_b:
			if (shifted)
				return 'B';
			else
				return 'b';
			break;
		case SDLK_c:
			if (shifted)
				return 'C';
			else
				return 'c';
			break;
		case SDLK_d:
			if (shifted)
				return 'D';
			else
				return 'd';
			break;
		case SDLK_e:
			if (shifted)
				return 'E';
			else
				return 'e';
			break;
		case SDLK_f:
			if (shifted)
				return 'F';
			else
				return 'f';
			break;
		case SDLK_g:
			if (shifted)
				return 'G';
			else
				return 'g';
			break;
		case SDLK_h:
			if (shifted)
				return 'H';
			else
				return 'h';
			break;
		case SDLK_i:
			if (shifted)
				return 'I';
			else
				return 'i';
			break;
		case SDLK_j:
			if (shifted)
				return 'J';
			else
				return 'j';
			break;
		case SDLK_k:
			if (shifted)
				return 'K';
			else
				return 'k';
			break;
		case SDLK_l:
			if (shifted)
				return 'L';
			else
				return 'l';
			break;
		case SDLK_m:
			if (shifted)
				return 'M';
			else
				return 'm';
			break;
		case SDLK_n:
			if (shifted)
				return 'N';
			else
				return 'n';
			break;
		case SDLK_o:
			if (shifted)
				return 'O';
			else
				return 'o';
			break;
		case SDLK_p:
			if (shifted)
				return 'P';
			else
				return 'p';
			break;
		case SDLK_q:
			if (shifted)
				return 'Q';
			else
				return 'q';
			break;
		case SDLK_r:
			if (shifted)
				return 'R';
			else
				return 'r';
			break;
		case SDLK_s:
			if (shifted)
				return 'S';
			else
				return 's';
			break;
		case SDLK_t:
			if (shifted)
				return 'T';
			else
				return 't';
			break;
		case SDLK_u:
			if (shifted)
				return 'U';
			else
				return 'u';
			break;
		case SDLK_v:
			if (shifted)
				return 'V';
			else
				return 'v';
			break;
		case SDLK_w:
			if (shifted)
				return 'W';
			else
				return 'w';
			break;
		case SDLK_x:
			if (shifted)
				return 'X';
			else
				return 'x';
			break;
		case SDLK_y:
			if (shifted)
				return 'Y';
			else
				return 'y';
			break;
		case SDLK_z:
			if (shifted)
				return 'Z';
			else
				return 'z';
			break;
		default:
			return '\0';
			break;
	}
}

void AscentApp::togglefullscreen() {
	if (toggledfullscreen == 0) {
		setfullscreen(!fullscreen);
		toggledfullscreen = 2; //Needs to be greater than 1 to avoid toggling too fast
	}
	
}

void AscentApp::setfullscreen(bool val) {
	SDL_SetWindowFullscreen(
			window,
			val ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
	fullscreen = val;
}

void AscentApp::onKeyDown_EndGame(SDL_KeyboardEvent * keyEvent) {
	switch (keyEvent->keysym.sym) {
		case SDLK_ESCAPE:
			running = false;
			break;
		case SDLK_F11:
			togglefullscreen();
			break;
		default:
			break;
	}
}

void AscentApp::LossScreenRender() {
	SDL_Surface* textSurface = TTF_RenderText_Blended(font, "YOU DIED", {0xFF, 0xFF, 0xFF, 0xFF});
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
		(windowWidth - message_w) / 2,
		(windowHeight - message_h) / 3,
		message_w,
		message_h
	};
	SDL_RenderCopy(renderer, statusMessage, NULL, &messageRect);

}
