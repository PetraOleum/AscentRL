#include "region.h"
#include <random>
#include <sstream>
#include <iostream>
#include <iomanip>

/// @brief A static random number generator, for the region
std::mt19937 gen;
/// @brief Whether or not the generator has been initialised
bool initgen = false;
/// @brief A int distribution, to be initialised as needed
std::uniform_int_distribution<int> idist;
/// @brief A reals distribution, to be initialised once
std::uniform_real_distribution<double> probdist;

Region::Region(int w, int h, RoomType type) {
	if (!initgen) { // Initialise probdist - but only once
		std::random_device rd;
		gen = std::mt19937(rd());
		probdist = std::uniform_real_distribution<double>(0, 1);
		initgen = true;
	}
	width = w;
	height = h;
	this->type = type;
	switch (type) {
		case RoomType::Room:
			for (int x = 0; x < w; x++) {
				for (int y = 0; y < h; y++) {
					Point tp = Point(x, y);
					points[tp] = Background::TiledFloor;
					if (probdist(gen) < GOLD_PROB)
						placeItem(tp, ItemType::Gold);
					if (probdist(gen) < STAFF_PROB)
						placeItem(tp, ItemType::Staff);
					if (probdist(gen) < CHEST_PROB)
						placeItem(tp, ItemType::Chest);
				}
				points[Point(x, -1)] = Background::StoneWall;
				points[Point(x, h)] = Background::StoneWall;
			}
			for (int y = -1; y <=h; y++) {
				points[Point(-1, y)] = Background::StoneWall;
				points[Point(w, y)] = Background::StoneWall;
			}
//			points[Point(5, 3)] = Background::DirtWall;
			{
				idist = std::uniform_int_distribution<int>(4, 4 + (w + h) / 2);
				int maxconnections = idist(gen);
				numConnections = 0;
				idist = std::uniform_int_distribution<int>(0, 4);
				for (uint8_t i = 0; i < 4; i++) {
					if (addrandomemptyconnection((Direction)(i)))
						numConnections++;
				}
				for (uint8_t i = 4; i < maxconnections; i++) {
					if (addrandomemptyconnection((Direction)(idist(gen))))
						numConnections++;
				}
//				printf("%d->%d\n", maxconnections, numConnections);
			}
			break;
		case RoomType::Corridor:
			for (int x = -1; x <= w; x++)
				for (int y = -1; y <= h; y++)
					points[Point(x, y)] = Background::StoneWall;
			{
				std::uniform_int_distribution<int> ydist(0, h - 1);
				int y = ydist(gen);
				for (int x = 0; x < w; x++)
					points[Point(x, y)] = Background::TiledFloor;
				addrandomemptyconnection(Direction::Left, Point(-1, y));
				addrandomemptyconnection(Direction::Right, Point(w, y));
			} 
			{
				std::uniform_int_distribution<int> xdist(0, w - 1);
				int x = xdist(gen);
				for (int y = 0; y < h; y++)
					points[Point(x, y)] = Background::TiledFloor;
				addrandomemptyconnection(Direction::Up, Point(x, -1));
				addrandomemptyconnection(Direction::Down, Point(x, h));
			}
			break;
		case RoomType::Spiral:
			for (int x = 0; x < w; x++) {
				for (int y = 0; y < h; y++) {
					Point tp = Point(x, y);
					points[tp] = Background::TiledFloor;
//					if (probdist(gen) < GOLD_PROB)
//						placeItem(tp, ItemType::Gold);
//					if (probdist(gen) < STAFF_PROB)
//						placeItem(tp, ItemType::Staff);
//					if (probdist(gen) < CHEST_PROB)
//						placeItem(tp, ItemType::Chest);
				}
				points[Point(x, -1)] = Background::StoneWall;
				points[Point(x, h)] = Background::StoneWall;
			}
			for (int y = -1; y <=h; y++) {
				points[Point(-1, y)] = Background::StoneWall;
				points[Point(w, y)] = Background::StoneWall;
			}
			if (h >= 3 && w >= 3){ // Draw spiral
				int layers = (MIN(w, h) - 2) / 4;
				int layercount = 0;
				Point currentlyDrawing = {1,0};
				Direction drawdir = Direction::Down;
				auto contLine = [layers, &layercount, w, h, &drawdir, this](Point pt) -> bool {
//					printf("%d\n", (layercount + 1) * 2);
					switch (drawdir) {
						case Direction::Down:
							return pt.second < h - (layercount + 1) * 2;
						case Direction::Up:
							return pt.second >= (layercount + 1) * 2;
						case Direction::Left:
							return pt.first >= (layercount + 2) * 2; // Because want to actually spiral
						case Direction::Right:
//							printf("%d\n", w - (layercount + 1) * 2);
							return pt.first < w - (layercount + 1) * 2;
						default:
							fprintf(stderr, "Direction weirdness here\n");
							return false;
					}
				};
				auto turnDir = [&layercount, &drawdir]() {
					switch (drawdir) {
						case Direction::Down:
							drawdir = Direction::Right;
							break;
						case Direction::Up:
							drawdir = Direction::Left;
							break;
						case Direction::Left:
							drawdir = Direction::Down;
							layercount++;
							break;
						case Direction::Right:
							drawdir = Direction::Up;
							break;
						default:
							fprintf(stderr, "Direction weirdness here\n");
							break;
					}
				};
				while (layercount < layers && currentlyDrawing.second >= 0 && currentlyDrawing.first >= 0) {
//					printf("Printing at %d:%d\n", currentlyDrawing.first, currentlyDrawing.second);
					points[currentlyDrawing] = Background::StoneWall;
					if (!contLine(currentlyDrawing)) {
						turnDir();
//						printf("Turning to direction %d; layercount = %d\n", (int)drawdir, layercount);
					}
					currentlyDrawing = PAIR_SUM(currentlyDrawing, DISPLACEMENT(drawdir));
				}
				points[Point(1, 3)] = Background::Door;
			}
			{
				addrandomemptyconnection(Direction::Up, {0, -1});
				idist = std::uniform_int_distribution<int>(4, 4 + (w + h) / 2);
				int maxconnections = idist(gen);
				numConnections = 0;
				idist = std::uniform_int_distribution<int>(0, 4);
				for (uint8_t i = 0; i < 4 && i < maxconnections; i++) {
					if (addrandomemptyconnection((Direction)(i)))
						numConnections++;
				}
				for (uint8_t i = 4; i < maxconnections; i++) {
					if (addrandomemptyconnection((Direction)(idist(gen))))
						numConnections++;
				}
			}
			break;
		default:
			fprintf(stderr, "Unimplemented RoomType\n");
			break;
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

	return addrandomemptyconnection(direction, p);
}

bool Region::connectTo(Region* to, Direction direction, Point opoint, Point dpoint) {
//	for (unsigned int i = 0; i < connections.size(); i++) {
//		if (connections[i].direction == direction && connections[i].to == NULL) {
//			connections[i].to = to;
//			connections[i].toLocation = dpoint;
//			return true;
//		}
//	}
//	for (auto & it : connections) 
//		if (it.second.direction == direction && it.second.to == NULL) {
//			it.second.to = to;
//			it.second.toLocation = dpoint;
//			return true;
//		}
	auto it = connections.find(opoint);
	if (it == connections.end())
		return false;
	if (it->second.direction != direction)
		return false;
	if (it->second.to != NULL)
		return false;
	if (it->second.from != opoint)
		return false;
	it->second.to = to;
	it->second.toLocation = dpoint;
	return true;
}

std::pair<Point, bool> Region::freeConnection(Direction dir) {
	for (auto & it : connections)
		if (it.second.direction == dir && it.second.to == NULL)
			return {
				it.second.from,
				true
			};
	return {
		{0,0},
		false
	};
}

bool Region::markDoor(Point point) {
	if (getBackground(point) != Background::Door)
		return false;
	points[point] = Background::MarkedDoor;
	return true;
}

bool Region::addrandomemptyconnection(Direction direction, Point location) {

	if (points[location] == Background::Door || points[location] == Background::MarkedDoor)
		return false;

	for (uint8_t i = (uint8_t)Direction::Up; i <= (uint8_t)Direction::Down; i++) {
		Point pd = PAIR_SUM(location, DISPLACEMENT((Direction)i));
		if (points[pd] == Background::Door || points[pd] == Background::MarkedDoor)
			return false;
	}

	points[location] = Background::Door;

	Connection nConnection = {
		location,
		NULL,
		Point(0, 0),
		direction
	};
//	connections.push_back(nConnection);
	connections[location] = nConnection;
	return true;
}

std::string Region::itemHereString(Point location) const {
	auto it = items.find(location);
	if (it == items.end())
		return "Here: Nothing.";
	if (it->second.empty())
		return "Here: Nothing.";
	std::string str = "Here: ";

	bool nfirst = false;
	for (auto item : it->second) {
		if (nfirst)
			str += ", ";
		nfirst = true;
		str += foreProps.at(getItemForeground(item)).name;
	}

	str += ".";


	return str;
}

std::string Region::ToString(bool showItems) const {
	std::stringstream ts;
	ts << "this: \t";
	ts << std::hex;
	ts << std::showbase << std::internal << std::setfill('0') << std::setw(16);
	ts << (long int)this;
	ts << std::dec;
	ts << "\n";
	ts << "Posiiton: " << this->position.first << ", " << this->position.second << "\n";
	ts << "Creatures:\n";
	for (auto it : this->creatures) {
		if (it.second != NULL) {
			ts << "\tAt " << it.first.first << ", " << it.first.second << ":  ";
			ts << std::hex;
			ts << (long int)it.second;
			ts << std::dec;
			ts << " (";
			ts << foreProps.at(this->getForeground(it.first)).name;
			ts << ")\n";
		}
	}
	if (showItems) {
		ts << "Items:\n";
		for (auto it : this->items) {
			if (!it.second.empty()) {
				ts << "\t(" << it.first.first << ", " << it.first.second << ")  ";
				ts << this->itemHereString(it.first) << "\n";
			}
		}
	}

	return ts.str();
}
