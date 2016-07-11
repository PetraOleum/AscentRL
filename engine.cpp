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
	manageAltRegion(); //Make sure valid
	Point np = DISPLACEMENT(direction);

	Background nb = relBaF(np, currentPosition).first;
	if (!bkgrProps.at(nb).passible)
		return false;
	if (activeRegion->getBackground(PAIR_SUM(np, currentPosition)) != nb) {
		swapRegions();
	}
	activeRegion->setForeground(currentPosition, underForeground);
	underForeground = activeRegion->getForeground(np);
//	currentPosition = Point(currentPosition.first + np.first, currentPosition.second + np.second);
	currentPosition = PAIR_SUM(currentPosition, np);
	activeRegion->setForeground(currentPosition, Foreground::Witch);
	manageAltRegion();
	refreshFOV();

	return true;
}

BaF Engine::relBaF(Point point, const Point & relto) {
	//Less temporary IMPL?
	Point relpt = PAIR_SUM(relto, point);
	BaF activeBaF = {
		activeRegion->getBackground(relpt),
		activeRegion->getForeground(relpt)
	};
	if (altRegionLoaded && activeBaF.first == Background::EMPTYNESS) {
		Point altrelpt = PAIR_SUM(relto, altDisplacement);
		
		BaF altBaF = {
			alternateRegion->getBackground(altrelpt),
			alternateRegion->getForeground(altrelpt)
		};
		return altBaF;
	} else
		return activeBaF;
}

void Engine::refreshFOV() {
	if (visiblelocations != NULL)
		delete visiblelocations;
	visiblelocations = FOV(currentPosition);

}

void Engine::manageAltRegion() {
	if (activeRegion->getBackground(currentPosition) == Background::Door) {
		Connection tc = activeRegion->connectionAt(currentPosition);
		printf("manageAltRegion(), 0x%lx\n", (long int)tc.to);
		if (tc.to != NULL) {
			alternateRegion = tc.to;
			altDisplacement = PAIR_SUBTRACT(
					tc.toLocation,
					currentPosition
					);
			altRegionLoaded = true;
		} else {
			Region * nr = new Region(4, 4, RoomType::Room);
			auto freept = nr->freeConnection(oppositeDirection(tc.direction));
			
			if (!freept.second) {
				altRegionLoaded = false;
				alternateRegion = NULL;
				delete nr;
				return;
			}
			if (!activeRegion->connectTo(nr, tc.direction, currentPosition, freept.first)) {
				fprintf(stderr, "This should not have happened: error connecting activeRegion to nr\n");
				altRegionLoaded = false;
				alternateRegion = NULL;
				delete nr;
				return;
			}
			if (!nr->connectTo(activeRegion, oppositeDirection(tc.direction), freept.first, currentPosition)) {
				fprintf(stderr, "This should not have happened: error connecting nr to activeRegion (note: corrupted activeRegion)\n");
				altRegionLoaded = false;
				alternateRegion = NULL;
				delete nr;
				return;
			}
			printf("Attached\n");
			regions.push_back(nr);
			alternateRegion = nr;
			altDisplacement = PAIR_SUBTRACT(
					freept.first,
					currentPosition
					);
			altRegionLoaded = true;
			printf("{%d, %d}, {%d, %d}, {%d, %d}\n", altDisplacement.first, altDisplacement.second, currentPosition.first, currentPosition.second, freept.first.first, freept.first.second);

			
			
		}
	} else {
		altRegionLoaded = false;
		alternateRegion = NULL;
	}
}

void Engine::swapRegions() {
	manageAltRegion();
	if (!altRegionLoaded)
		return;
	printf("Swapping");
	activeRegion->setForeground(currentPosition, underForeground);
	Point npos = PAIR_SUM(currentPosition, altDisplacement);
	Point nad = PAIR_SUBTRACT(Point(0,0), altDisplacement);
	Region * tmp = activeRegion;
	activeRegion = alternateRegion;
	alternateRegion = tmp;
	currentPosition = npos;
	altDisplacement = nad;
	underForeground = activeRegion->getForeground(currentPosition);
	activeRegion->setForeground(currentPosition, Foreground::Witch);
}
