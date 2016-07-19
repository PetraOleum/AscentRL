#ifndef ENGINE_H
#define ENGINE_H

#include <utility>
#include <random>
#include <vector>
#include <queue>
#include "general.h"
#include "region.h"
#include "creature.h"

/// @brief Minium size of a room
#define MIN_ROOM_DIMENSION 2
/// @brief Maximum size of a room
#define MAX_ROOM_DIMENSION 10

/// @brief Probability of attempting to select existing room
#define EXISTING_ROOM_PROB 0.80

/// @brief The radius of the field of view
#define FOV_RADIUS 15

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

		/// @brief distribution from 0 to 1
		std::uniform_real_distribution<double> probdist;

		/// @brief The player creature
		Creature* player = NULL;

		/// @brief Vector to hold all regions, to allow deletion
		std::vector<Region*> regions;

		/// @brief Manage the alternate region
		void manageAltRegion();

		/// @brief Swap the alternate and active regions
		void swapRegions();

		/// @brief Temp variable for the foreground of the square you're in
		Foreground underForeground = Foreground::NONE;

		/// @brief Map of visible squares, accessed indirectly by the app
		std::map<Point, Visibility>* visiblelocations = NULL;

		/// @brief Converter between point and actual location
		///
		/// @param point Point
		/// @param relto What point is relative to
		/// @param region The region point is on
		///
		/// @return Pair of Foreground/Background
		BaF relBaF(Point point, const Point & relto, Region * region);

		/// @brief Refresh field of view map
		void refreshFOV();

		/// @brief Get the FOV from and relative to a point
		///
		/// @param point The point (e.g. currentPosition)
		/// @param region The region FOV is working on
		///
		/// @return A new std::map of visibility, relative to point
		std::map<Point, Visibility>* FOV(Point point, Region * region);

	public:
		/// @brief Constructor
		Engine();
		

		/// @brief Destructor
		~Engine();

		/// @brief get background of point relative to currentPosition
		///
		/// @param point Point relative to currentPosition
		///
		/// @return The background value, if visible
		Background getBackground(Point point);

		/// @brief Get foreground of point relative to currentPosition
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
			return player->getPosition();
		}

		/// @brief Run astar to find the fastest route between two points
		///
		/// @param start The starting point
		/// @param finish The ending point
		/// @param relativeTo A point to which this is relative to, e.g. currentPosition
		/// @param region The region that the astar is working on
		///
		/// @return A queue of Directions that will lead you to finish from start via the fastest route as generated by astar
		std::queue<Direction> * astar(Point start, Point finish, Point relativeTo, Region * region);

		/// @brief Astar from the POV of the player
		///
		/// @param start Starting location (generally 0,0)
		/// @param finish Finish location
		///
		/// @return Queue of directions
		inline std::queue<Direction> * playerAstar(Point start, Point finish) {
			return astar(start, finish, player->getPosition(), player->getRegion());
		}

		/// @brief Expose underForeground
		///
		/// @return Reference to underForeground
		inline const Foreground & underWitch() {
			return underForeground;
		}

};

#endif
