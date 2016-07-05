#ifndef ASCENTAPP_H
#define ASCENTAPP_H

#include <SDL2/SDL.h>
#include <cstdint>
#include <map>

#include "sprites.h"

#define LOOP_TIME (1000/30)

#define INIT_SCREEN_WIDTH 500
#define INIT_SCREEN_HEIGHT 500

#define SQUARE_SIZE 32

class AscentApp {
	private:
		/// @brief Is the app running
		bool running = false;

		/// @brief Is the app fullscreen
		bool fullscreen = false;

		/// @brief Holds the window
		SDL_Window* window = NULL;

		/// @brief Holds the surface
		SDL_Surface* screensurface = NULL;

		/// @brief Holds the renderer
		SDL_Renderer* renderer = NULL;

		/// @brief Handle a key down
		///
		/// @param keyEvent The event data
		void onKeyDown(SDL_KeyboardEvent * keyEvent);

		/* Translating between pixels on the screen and squares */
		
		/// @brief Mouse position (X)
		int mouseX = 0;

		/// @brief Mouse position (Y)
		int mouseY = 0;

		/// @brief The height of the window
		int windowWidth = INIT_SCREEN_WIDTH;

		/// @brief The width of the window
		int windowHeight = INIT_SCREEN_HEIGHT;

		/// @brief The number of squares being displayed (x-axis)
		int numSquaresX = INIT_SCREEN_WIDTH / SQUARE_SIZE;

		/// @brief The number of squares being displayed (y-axis)
		int numSquaresY = INIT_SCREEN_HEIGHT / SQUARE_SIZE;

		/// @brief X-coord of the "origin" point
		int originX = (INIT_SCREEN_WIDTH % SQUARE_SIZE) / 2;

		/// @brief Y-coord of the "origin" point
		int originY = (INIT_SCREEN_HEIGHT % SQUARE_SIZE) / 2;

		/// @brief The X-coord of the square the mouse is in
		int mouseSquareX = 0;

		/// @brief The Y-coord of the square the mouse is in
		int mouseSquareY = 0;

		/// @brief Whether or not the mouse is currently in a valid square
		bool mouseInSquares = false;

		/// @brief Call to update window wize and mouse position
		inline void updateWinParameters() {
			SDL_GetWindowSize(window, &windowWidth, &windowHeight);
			SDL_GetMouseState(&mouseX, &mouseY);
			numSquaresX = windowWidth / SQUARE_SIZE;
			numSquaresY = windowHeight / SQUARE_SIZE;
			originX = (windowWidth % SQUARE_SIZE) / 2;
			originY = (windowHeight % SQUARE_SIZE) / 2;
			mouseSquareX = (mouseX - originX) / SQUARE_SIZE;
			mouseSquareY = (mouseY - originY) / SQUARE_SIZE;
			mouseInSquares = (mouseX >= originX) 
				&& (mouseY >= originY) 
				&& (mouseSquareX < numSquaresX) 
				&& (mouseSquareY < numSquaresY);
		}

		/// @brief Convert from square to pixel coord (X axis)
		///
		/// @param squareX The square's x-coord
		///
		/// @return The pixel (X) coord of the leftmost pixel
		inline int xOfSquare(int squareX) {
			return SQUARE_SIZE * squareX + originX;
		}

		/// @brief Convert from square to pixel coord (Y axis)
		///
		/// @param squareY The square's x-coord
		///
		/// @return The pixel (Y) coord of the topmost pixel
		inline int yOfSquare(int squareY) {
			return SQUARE_SIZE * squareY + originY;
		}

		/* Sprite stuff */

		const int SPRITE_SIZE = 32;

		const int SPRITE_SHEET_WIDTH = 8;

		SDL_Texture* backgroundSpriteSheet = NULL;

		bool Init_Background();

		std::map<Background, SDL_Rect> backgroundSpriteRect;

		void renderBackground(Background background, int xsquare, int ysquare);

	public:
		/// @brief Constructor
		AscentApp() {

		}

		/// @brief Destructor
		~AscentApp() {

		}

		/// @brief Run the app
		///
		/// @return Execution status
		int OnExecute();

		/// @brief Initialise the app
		///
		/// @return Success/failure
		bool OnInit();

		/// @brief Handle events
		///
		/// @param event The event to handle
		void OnEvent(SDL_Event* event);

		/// @brief Run for every loop
		void OnLoop();

		/// @brief Rendering code
		void OnRender();

		/// @brief Cleanup code at finish
		void OnCleanup();
};

#endif
