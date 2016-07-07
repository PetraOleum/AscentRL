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
	regions.push_back(activeRegion);

}

Engine::~Engine() {
	for (Region* region : regions) {
		delete region;
	}
}

Background Engine::getBackground(Point point) {
	return activeRegion->getBackground(point);
}
