#ifndef REGION_H
#define REGION_H

#include <utility>
#include <map>
#include <vector>
#include "sprites.h"

class Region;

using Point = std::pair<int, int>;

/// @brief Enum to hold directions
enum class Direction : uint8_t {
	Up,
	Right,
	Left,
	Down,
	UpRight,
	DownRight,
	DownLeft,
	UpLeft
};

/// @brief Get the "oppisite" direction to the one provided
///
/// @param direction A direction
///
/// @return The opposite direction, or Direction::Up if not a valid direction
inline Direction oppositeDirection(Direction direction) {
	switch (direction) {
		case Direction::Up:
			return Direction::Down;
		case Direction::Right:
			return Direction::Left;
		case Direction::Left:
			return Direction::Right;
		case Direction::Down:
			return Direction::Up;
		case Direction::UpRight:
			return Direction::DownLeft;
		case Direction::DownRight:
			return Direction::UpLeft;
		case Direction::DownLeft:
			return Direction::UpRight;
		case Direction::UpLeft:
			return Direction::DownRight;
		default:
			return Direction::Up;
	}
}

/// @brief Hold a connnection between regions
struct Connection {
	Point from;
	Region* to;
	Point toLocation;
	Direction direction;
};

/// @brief Types of room
enum class RoomType : uint8_t {
	/// @brief A rectangular area with four doors
	Room,
	/// @brief A corridor, potentially with several doors
	Corridor
};

/// @brief Class for the region (i.e. room)
class Region {
	private:

		/// @brief Foregrounds
		std::map<Point, Background> points;

		/// @brief Backgrounds
		std::map<Point, Foreground> foreground;

		/// @brief Connections
		std::vector<Connection> connections;

		/// @brief The number of potential connections (i.e. doors)
		int numConnections;

		/// @brief Width of room. In rectagular room, does not include walls. Corridors may be smaller than this
		int width;
		
		/// @brief Height of room
		int height;

		/// @brief Attempt to add a (random, empty) connection
		///
		/// @param direction A direction, e.g. Direction::Up means on the top etc
		///
		/// @return Success/fail
		bool addrandomemptyconnection(Direction direction);
		
	public:
		/// @brief Constructor
		///
		/// @param w Width
		/// @param h Height
		/// @param type Time of room
		Region(int w, int h, RoomType type);

		/// @brief Destructor
		~Region() {

		}

		/// @brief Expose background
		///
		/// @param location A location
		///
		/// @return Background::EMPTYNESS if nothing in map, otherwise contents of map
		inline Background getBackground(Point location) {
			auto it = points.find(location);
			if (it == points.end())
				return Background::EMPTYNESS;
			else 
				return it->second;
		}

		/// @brief Expose foreground
		///
		/// @param location A location
		///
		/// @return Foreground::NONE if nothing in map, otherwise contents of map
		inline Foreground getForeground(Point location) {
			auto it = foreground.find(location);
			if (it == foreground.end())
				return Foreground::NONE;
			else
				return it->second;
		}

		/// @brief Set the foreground
		///
		/// @param location A location to set at
		/// @param fore Value to set to
		inline void setForeground(Point location, Foreground fore) {
			foreground[location] = fore;
		}

		/// @brief Connect this region to another (need to run on other, if mutual)
		///
		/// @param to Region to connect to
		/// @param direction Direction to connect in
		/// @param dpoint Destination point connecting to
		///
		/// @return Success/fail
		bool connectTo(Region* to, Direction direction, Point dpoint);

};

#endif
