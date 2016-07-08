#ifndef REGION_H
#define REGION_H

#include <utility>
#include <map>
#include <vector>
#include "sprites.h"

class Region;

using Point = std::pair<int, int>;

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

struct Connection {
	Point from;
	Region* to;
	Point toLocation;
	Direction direction;
};

enum class RoomType : uint8_t {
	Room,
	Corridor
};

class Region {
	private:

		std::map<Point, Background> points;

		std::map<Point, Foreground> foreground;

		std::vector<Connection> connections;

		int numConnections;

		int width;
		
		int height;

		bool addrandomemptyconnection(Direction direction);
		
	public:
		Region(int w, int h, RoomType type);

		~Region() {

		}

		inline Background getBackground(Point location) {
			auto it = points.find(location);
			if (it == points.end())
				return Background::EMPTYNESS;
			else 
				return it->second;
		}

		inline Foreground getForeground(Point location) {
			auto it = foreground.find(location);
			if (it == foreground.end())
				return Foreground::NONE;
			else
				return it->second;
		}

		inline void setForeground(Point location, Foreground fore) {
			foreground[location] = fore;
		}

		bool connectTo(Region* to, Direction direction, Point dpoint);

};

#endif
