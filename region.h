#ifndef REGION_H
#define REGION_H

#include <utility>
#include <map>
#include <vector>
#include "sprites.h"

class Region;

using Point = std::pair<int, int>;

enum class CardinalDirection : uint8_t {
	Up,
	Right,
	Left,
	Down
};

struct Connection {
	Point from;
	Region* to;
	Point toLocation;
	CardinalDirection direction;
};

class Region {
	private:
		std::map<Point, Background> points;

		std::vector<Connection> connections;

		int numConnections;

		int width;
		
		int height;
		
	public:
		Region(int w, int h, int numconnections);

		~Region() {

		}

		inline Background getBackground(Point location) {
			auto it = points.find(location);
			if (it == points.end())
				return Background::EMPTYNESS;
			else 
				return it->second;
		}

};

#endif
