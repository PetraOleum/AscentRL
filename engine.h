#ifndef ENGINE_H
#define ENGINE_H

#include <utility>
#include <random>
#include <vector>
#include "general.h"
#include "region.h"

/// @brief Minium size of a room
#define MIN_ROOM_DIMENSION 1
/// @brief Maximum size of a room
#define MAX_ROOM_DIMENSION 10

/// @brief The radius of the field of view
#define FOV_RADIUS 9

/// @brief Background/foreground pair
using BaF = std::pair<Background, Foreground>;

/// @brief Hold the Visibility information + foreground/background of a square
struct Visibility {
	/// @brief Whether the square is visible
	bool visible;
	/// @brief Value of the background
	Background background;
	/// @brief Value of the foreground
	Foreground foreground;
};

/// @brief Class for the game engine
class Engine {
	private:
		/// @brief Random number engine
		std::mt19937 randomengine;

		/// @brief Distribution for room dimensions
		std::uniform_int_distribution<int> roomdist;

		/// @brief Vector to hold all regions, to allow deletion
		std::vector<Region*> regions;

		/// @brief The currently active region
		Region* activeRegion = NULL;

		/// @brief The current position of the player in the active region
		Point currentPosition = Point(0, 0);

		/// @brief Temp variable for the foreground of the square you're in
		Foreground underForeground = Foreground::NONE;

		/// @brief Map of visible squares, accessed indirectly by the app
		std::map<Point, Visibility>* visiblelocations = NULL;

		/// @brief Converter between point (relative to current position) and actual location
		///
		/// @param point Point (relative to currentPosition)
		/// @param relto Point relative to what
		///
		/// @return Pair of foreground/background
		BaF relBaF(Point point, const Point & relto);

		/// @brief Refresh field of view map
		void refreshFOV();

		/// @brief Get the FOV from and relative to a point
		///
		/// @param point The point (e.g. currentPosition)
		///
		/// @return A new std::map of visibility, relative to point
		std::map<Point, Visibility>* FOV(Point point);

	public:
		/// @brief Constructor
		Engine();
		

		/// @brief Destructor
		~Engine();

		/// @brief get background of point
		///
		/// @param point Point relative to currentPosition
		///
		/// @return The background value, if visible
		Background getBackground(Point point);

		/// @brief Get foreground of point
		///
		/// @param point Point relative to currentPosition
		///
		/// @return The foreground value, if visible
		Foreground getForeground(Point point);

		/// @brief Move in a direction
		///
		/// @param direction The (8-way) direction to move in
		///
		/// @return Success/fail
		bool Move(Direction direction);

		/// @brief Expose currentPosition
		///
		/// @return currentPosition by value
		inline Point getCurrentPosition() {
			return currentPosition;
		}

};

#endif
