#ifndef REGION_H
#define REGION_H

#include <utility>
#include <map>
#include <vector>
#include "general.h"

class Region;



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
		std::map<Point, Connection> connections;

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
		bool connectTo(Region* to, Direction direction, Point opoint, Point dpoint);

		/// @brief Get the connection at a point (i.e. translate door coords to the connection)
		///
		/// @param point The coords
		///
		/// @return Copy of the connection
		inline Connection connectionAt(Point point) {
			auto it = connections.find(point);
			if (it == connections.end()) 
				return {
					{0,0}, 
					NULL, 
					{0,0}, 
					Direction::Up
				};
			else
				return it->second;
		}

		/// @brief Get the location of a free connection
		///
		/// @param dir The direction it's going
		///
		/// @return Pair of the point, and true/false is free
		std::pair<Point, bool> freeConnection(Direction dir);

};


#endif
