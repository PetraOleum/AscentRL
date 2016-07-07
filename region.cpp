#include "region.h"
#include <random>

std::mt19937 gen;
bool initgen = false;
std::uniform_int_distribution<int> idist;

Region::Region(int w, int h, int numconnections) {
	if (!initgen) {
		std::random_device rd;
		gen = std::mt19937(rd());
	}
	width = w;
	height = h;
	numConnections = numconnections;
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			points[Point(x, y)] = Background::TiledFloor;
		}
		points[Point(x, -1)] = Background::StoneWall;
		points[Point(x, h)] = Background::StoneWall;
	}
	for (int y = -1; y <=h; y++) {
		points[Point(-1, y)] = Background::StoneWall;
		points[Point(w, y)] = Background::StoneWall;
	}
	numConnections = 4;
	for (uint8_t i = 0; i < 4; i++) {
		if (i < numconnections)
			addrandomemptyconnection((Direction)i);
		else {
			numConnections = numconnections;
		}
	}
	
}

bool Region::addrandomemptyconnection(Direction direction) {
	Point p;
	switch (direction) {
		case Direction::Up:
			idist = std::uniform_int_distribution<int>(0, width - 1);
			p = Point(idist(gen), -1);
			break;
		case Direction::Right:
			idist = std::uniform_int_distribution<int>(0, height - 1);
			p = Point(width, idist(gen));
			break;
		case Direction::Down:
			idist = std::uniform_int_distribution<int>(0, width - 1);
			p = Point(idist(gen), height);
			break;
		case Direction::Left:
			idist = std::uniform_int_distribution<int>(0, height - 1);
			p = Point(-1, idist(gen));
			break;
		default:
			return false;
	};

	if (points[p] == Background::Door)
		return false;

	points[p] = Background::Door;

	Connection nConnection = {
		p,
		NULL,
		Point(0, 0),
		direction
	};
	connections.push_back(nConnection);

	return true;
}

bool Region::connectTo(Region* to, Direction direction, Point dpoint) {
	for (unsigned int i = 0; i < connections.size(); i++) {
		if (connections[i].direction == direction && connections[i].to == NULL) {
			connections[i].to = to;
			connections[i].toLocation = dpoint;
			return true;
		}
	}
	return false;
}
