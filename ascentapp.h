#ifndef ASCENTAPP_H
#define ASCENTAPP_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <cstdint>
#include <map>

#include "general.h"
#include "engine.h"

/// @brief 1000 / FPS
#define LOOP_TIME (1000/30)

#define INIT_SCREEN_WIDTH 500
#define INIT_SCREEN_HEIGHT 500

//#define FONT_PATH "font/liberation-serif/LiberationSerif-Regular.ttf"
#define FONT_PATH "font/liberation-mono/LiberationMono-Regular.ttf"

#define FONT_POINT 16

#define MESSAGE_BORDER 8

#define SQUARE_SIZE 32

/// @brief The app class
class AscentApp {
	private:
		/// @brief The type of window to be rendered
		enum class windowType : uint8_t {
			/// @brief Show the game map
			Map,
			/// @brief Show the inventory
			Inventory,
			/// @brief Endgame (loss) screen
			LossScreen
		} currentlyDisplaying;

		/// @brief The input type taken
		enum class InputType : uint8_t {
			/// @brief Standard navigation on the map
			Standard,
			/// @brief Select an inventory item to drop
			InventoryItemToDrop,
			/// @brief Select an inventory item to view (not implemented)
			InventoryItemToView,
			/// @brief Endgame
			EndGame
		} userInputRequested;

		/// @brief Pointer to the engine
		Engine* engine = NULL;

		/// @brief Is the app running
		bool running = false;

		/// @brief Is the app fullscreen
		bool fullscreen = false;

		/// @brief Whether or not fullscreen has been toggled this loop
		uint8_t toggledfullscreen = 0;

		/// @brief The font (liberation serif)
		TTF_Font *font = NULL;

		/// @brief The status message
		SDL_Texture* statusMessage = NULL;

		/// @brief Holds the window
		SDL_Window* window = NULL;

		/// @brief Holds the renderer
		SDL_Renderer* renderer = NULL;

		/// @brief Handle a key down
		///
		/// @param keyEvent The event data
		void onKeyDown_Standard(SDL_KeyboardEvent * keyEvent);

		/// @brief Handle a key down in the inventory
		///
		/// @param keyEvent The event data
		void onKeyDown_Inventory(SDL_KeyboardEvent * keyEvent);

		/// @brief Handke a key down in the endgame screen
		///
		/// @param keyEvent The event data
		void onKeyDown_EndGame(SDL_KeyboardEvent * keyEvent);

		/// @brief Draw the status box on the bottom lines
		void drawStatusBox();

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

		/// @brief Render the HP line at a square
		///
		/// @param HPPercentage The HP fraction (not actually percentage)
		/// @param xsquare Square x coord
		/// @param ysquare Square y coord
		void renderHPLine(double HPPercentage, int xsquare, int ysquare);

		/* Controls stuff */

		/// @brief Planned moves
		std::queue<Action> plan;

		/// @brief Drop an item
		///
		/// @param item The inventory char code of the item
		inline void dropItem(char item) {
			plan.push({ActionType::Drop, Direction::NONE, item});
			currentlyDisplaying = windowType::Map;
			userInputRequested = InputType::Standard;
		}

		/// @brief Toggle whether or not currently fullscreen
		void togglefullscreen();

		/// @brief Set whether or not currently fullscreen
		///
		/// @param val The new value
		void setfullscreen(bool val);

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


		/// @brief Rendering code for rendering the map
		void MapRender();

		/// @brief Rendering code for the inventory
		void InventoryRender();

		/// @brief Rendering code for the loss screen
		void LossScreenRender();
};

#endif
