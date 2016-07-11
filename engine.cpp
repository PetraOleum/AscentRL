#include "engine.h"

Engine::Engine() {
	std::random_device rn;
	randomengine = std::mt19937(rn());
	roomdist = std::uniform_int_distribution<int>(
			MIN_ROOM_DIMENSION, 
			MAX_ROOM_DIMENSION
			);

	//Temp stuff with 1 region
	
	activeRegion = new Region(20, 10, RoomType::Room);
	activeRegion->setForeground(currentPosition, Foreground::Witch);
	regions.push_back(activeRegion);
	refreshFOV();

}

Engine::~Engine() {
	for (Region* region : regions) {
		delete region;
	}
}

Background Engine::getBackground(Point point) {
//	Point adjp = Point(point.first + currentPosition.first, point.second + currentPosition.second);
//	Point adjp = point;
	if (visiblelocations == NULL) {
		fprintf(stderr, "Accessing FOV map before creation\n");
		return Background::EMPTYNESS;
	}
	auto it = visiblelocations->find(point);
	if (it == visiblelocations->end())
		return Background::EMPTYNESS;
	else if (it->second.visible)
		return it->second.background;
	else
		return Background::EMPTYNESS;
}

Foreground Engine::getForeground(Point point) {
//	Point adjp = Point(point.first + currentPosition.first, point.second + currentPosition.second);
//	Point adjp = point;
	if (visiblelocations == NULL) {
		fprintf(stderr, "Accessing FOV map before creation\n");
		return Foreground::NONE;
	}
	auto it = visiblelocations->find(point);
	if (it == visiblelocations->end())
		return Foreground::NONE;
	else if (it->second.visible)
		return it->second.foreground;
	else
		return Foreground::NONE;
}

bool Engine::Move(Direction direction) {
	Point np = DISPLACEMENT(direction);

	Background nb = relBaF(np, currentPosition).first;
	if (!bkgrProps.at(nb).passible)
		return false;
	
	activeRegion->setForeground(currentPosition, underForeground);
	underForeground = activeRegion->getForeground(np);
//	currentPosition = Point(currentPosition.first + np.first, currentPosition.second + np.second);
	currentPosition = PAIR_SUM(currentPosition, np);
	activeRegion->setForeground(currentPosition, Foreground::Witch);
	refreshFOV();

	return true;
}

BaF Engine::relBaF(Point point, const Point & relto) {
	//Very temporary impl
	Point relpt = PAIR_SUM(relto, point);
	return BaF(
			activeRegion->getBackground(relpt),
			activeRegion->getForeground(relpt)
		  );
}

void Engine::refreshFOV() {
	if (visiblelocations != NULL)
		delete visiblelocations;
	visiblelocations = FOV(currentPosition);

}
