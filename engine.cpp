#include "engine.h"

Engine::Engine() {
	std::random_device rn;
	randomengine = std::mt19937(rn());
	roomdist = std::uniform_int_distribution<int>(
			MIN_ROOM_DIMENSION, 
			MAX_ROOM_DIMENSION
			);

	//Temp stuff with 1 region
	
	activeRegion = new Region(10, 5, 4);
	activeRegion->setForeground(currentPosition, Foreground::Witch);
	regions.push_back(activeRegion);

}

Engine::~Engine() {
	for (Region* region : regions) {
		delete region;
	}
}

Background Engine::getBackground(Point point) {
//	Point adjp = Point(point.first + currentPosition.first, point.second + currentPosition.second);
	Point adjp = point;
	return activeRegion->getBackground(adjp);
}

Foreground Engine::getForeground(Point point) {
//	Point adjp = Point(point.first + currentPosition.first, point.second + currentPosition.second);
	Point adjp = point;
	return activeRegion->getForeground(adjp);
}

bool Engine::Move(Direction direction) {
	Point np;
	switch (direction) {
		case Direction::Right:
			np = Point(1 + currentPosition.first, currentPosition.second);
			break;
		case Direction::Left:
			np = Point(currentPosition.first - 1, currentPosition.second);
			break;
		case Direction::Up:
			np = Point(currentPosition.first, currentPosition.second - 1);
			break;
		case Direction::Down:
			np = Point(currentPosition.first, currentPosition.second + 1);
			break;
		case Direction::UpLeft:
			np = Point(currentPosition.first - 1, currentPosition.second - 1);
			break;
		case Direction::UpRight:
			np = Point(currentPosition.first + 1, currentPosition.second - 1);
			break;
		case Direction::DownLeft:
			np = Point(currentPosition.first - 1, currentPosition.second + 1);
			break;
		case Direction::DownRight:
			np = Point(currentPosition.first + 1, currentPosition.second + 1);
			break;
		default:
			return false;
	}

	Background nb = getBackground(np);
	if (!passiblity.at(nb))
		return false;
	
	activeRegion->setForeground(currentPosition, underForeground);
	underForeground = activeRegion->getForeground(np);
	currentPosition = np;
	activeRegion->setForeground(currentPosition, Foreground::Witch);

	return true;
}
