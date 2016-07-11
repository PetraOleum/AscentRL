#ifndef ASCENTAPP_H
#define ASCENTAPP_H

#include <SDL2/SDL.h>
#include <cstdint>
#include <map>

#include "general.h"
#include "engine.h"

#define LOOP_TIME (1000/30)

#define INIT_SCREEN_WIDTH 500
#define INIT_SCREEN_HEIGHT 500

#define SQUARE_SIZE 32

class AscentApp {
	private:
		/// @brief Pointer to the engine
		Engine* engine = NULL;

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

		/// @brief Size of the sprites on the sheet in pixels
		const int SPRITE_SIZE = 32;

		/// @brief Number of sprites in a row on a sheet
		const int SPRITE_SHEET_WIDTH = 8;

		/// @brief Holds the background sprite sheet
		SDL_Texture* backgroundSpriteSheet = NULL;

		/// @brief The foreground sprite sheet
		SDL_Texture* foregroundSpriteSheet = NULL;

		/// @brief Initialise the background sprites
		///
		/// @return Success/fail
		bool Init_Background();

		/// @brief Initialise the foreground sprites
		///
		/// @return Success/fail
		bool Init_Foreground();

		/// @brief Map backgrounds to the SDL_Rect area of the sheet
		std::map<Background, SDL_Rect> backgroundSpriteRect;

		/// @brief Map foregrouds to the sprite sheet
		std::map<Foreground, SDL_Rect> foregroundSpriteRect;

		/// @brief Render a background of a square
		///
		/// @param background The value of the background
		/// @param xsquare Square x coord
		/// @param ysquare Square y coord
		void renderBackground(Background background, int xsquare, int ysquare);

		/// @brief Render a foreground of a square
		///
		/// @param foreground The value of the foreground
		/// @param xsquare Square x coord
		/// @param ysquare Square y coord
		void renderForeground(Foreground foreground, int xsquare, int ysquare);

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
