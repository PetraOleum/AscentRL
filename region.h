#ifndef REGION_H
#define REGION_H

#include <utility>
#include <map>
#include <vector>
#include <deque>
#include <string>
#include "general.h"

#define GOLD_PROB 0.075
#define STAFF_PROB 0.003
#define CHEST_PROB 0.05


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

		/// @brief Creatures
		std::map<Point, Creature *> creatures;

		std::map<Point, std::deque<ItemType>> items;

		/// @brief Backgrounds
		std::map<Point, Background> points;

		/// @brief Foregrounds
//		std::map<Point, Foreground> foreground;

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

		/// @brief Add a not-actually random connection at a specific point
		///
		/// @param direction The direction
		/// @param location The location
		///
		/// @return Success/fail
		bool addrandomemptyconnection(Direction direction, Point location);

		/// @brief The type of room this is
		RoomType type;

	public:
		/// @brief Record of the "position" of the region
		Point position;
		
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
		inline Foreground getForeground(Point location) const {
//			auto it = foreground.find(location);
//			if (it == foreground.end())
//				return Foreground::NONE;
//			else
//				return it->second;
			auto it = creatures.find(location);
			if (it != creatures.end() && it->second != NULL)
				return getCreaturePointerForeground(it->second);
			else {
				auto it = items.find(location);
				if (it == items.end())
					return Foreground::NONE;
				else if (it->second.empty())
					return Foreground::NONE;
				else
					return getItemForeground(it->second[0]);
			}
		}

		/// @brief Place an item on the bottom of the queue at a particiular location
		///
		/// @param location The location
		/// @param item The item
		inline void placeItem(Point location, ItemType item) {
			items[location].push_back(item);
		}

		/// @brief Take an item from the top of the queue at a particular location
		///
		/// @param location The location
		///
		/// @return The item
		inline ItemType takeItem(Point location) {
			if (items[location].empty())
				return ItemType::NONE;
			else {
				ItemType item = items[location][0];
				items[location].pop_front();
				return item;
			}
		}

		/// @brief View the item at the top of the queue at a particular location
		///
		/// @param location The location
		///
		/// @return The item
		inline ItemType topItem(Point location) {
			if (items[location].empty())
				return ItemType::NONE;
			else {
				ItemType item = items[location][0];
				return item;
			}
		}
		
		/// @brief Get the creature at a location
		///
		/// @param location The location
		///
		/// @return Pointer to the creature
		inline Creature * getCreature(Point location) {
			auto it = creatures.find(location);
			if (it == creatures.end())
				return it->second;
			else
				return NULL;
		}

		/// @brief Whether or not there is a creature at that location
		///
		/// @param location The location
		///
		/// @return True if there is one there, i.e. is not null
		inline bool hasCreature(Point location) {
			return (getCreature(location) != NULL);
		}

		/// @brief Place (overwrite) the creature at a location
		///
		/// @param location The location
		/// @param creature The creature
		inline void putCreature(Point location, Creature * creature) {
			creatures[location] = creature;
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

		/// @brief Mark a door
		///
		/// @param point Point to mark at
		///
		/// @return Success/fail
		bool markDoor(Point point);

		/// @brief Expose the room type
		///
		/// @return Reference to the roomtype
		inline const RoomType & Type() {
			return type;
		}

		/// @brief Get a "here" string for all the items at a location
		///
		/// @param location The location
		///
		/// @return The string produced
		std::string itemHereString(Point location) const;

		/// @brief Render the state of the region as a string
		///
		/// @return The string
		std::string ToString(bool showItems = true) const;

};


#endif
