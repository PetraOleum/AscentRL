#include "engine.h"

Engine::Engine() {
	std::random_device rn;
	randomengine = std::mt19937(rn());
	roomdist = std::uniform_int_distribution<int>(
			MIN_ROOM_DIMENSION, 
			MAX_ROOM_DIMENSION
			);

	//Temp stuff with 1 region
	
	activeRegion = new Region(10, 5, RoomType::Room);
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
	Point np;
	switch (direction) {
		case Direction::Right:
			np = Point(1, 0);
			break;
		case Direction::Left:
			np = Point(-1, 0);
			break;
		case Direction::Up:
			np = Point(0, -1);
			break;
		case Direction::Down:
			np = Point(0, 1);
			break;
		case Direction::UpLeft:
			np = Point(- 1, - 1);
			break;
		case Direction::UpRight:
			np = Point(1, -1);
			break;
		case Direction::DownLeft:
			np = Point(- 1, 1);
			break;
		case Direction::DownRight:
			np = Point(1, 1);
			break;
		default:
			return false;
	}

	Background nb = relBaF(np).first;
	if (!bkgrProps.at(nb).passible)
		return false;
	
	activeRegion->setForeground(currentPosition, underForeground);
	underForeground = activeRegion->getForeground(np);
	currentPosition = Point(currentPosition.first + np.first, currentPosition.second + np.second);
	activeRegion->setForeground(currentPosition, Foreground::Witch);
	refreshFOV();

	return true;
}

BaF Engine::relBaF(Point point) {
	//Very temporary impl
	Point relpt = Point(point.first + currentPosition.first, point.second + currentPosition.second);
	return BaF(
			activeRegion->getBackground(relpt),
			activeRegion->getForeground(relpt)
		  );
}

void Engine::refreshFOV() {
	if (visiblelocations != NULL)
		delete visiblelocations;
	visiblelocations = new std::map<Point, Visibility>;

	BaF curpt = relBaF(Point(0, 0));
	Visibility curvs = {
		true,
		curpt.first,
		curpt.second
	};
	(*visiblelocations)[Point(0,0)] = curvs;
	
	using AnglePair = std::pair<double, double>;

	std::vector<AnglePair> * currentBlocked = new std::vector<AnglePair>;
	std::vector<AnglePair> * nextLineBlocked = new std::vector<AnglePair>;

	// SSW octant actually x negative, y positive
	for (int y = 1; y < FOV_RADIUS; y++) {
		// Copy next line into current line, and refresh next line
		for (auto ap : *nextLineBlocked)
			currentBlocked->push_back(ap);
		delete nextLineBlocked;
		nextLineBlocked = new std::vector<AnglePair>;

		double arange = 1.0 / (double)(y + 1);
		for (int x = 0; x <= y; x++) {
			// Because x is negative, this needs to be the negative
			int cellno = -x;
			Point tp(cellno, y);
			double startingAngle = cellno * arange;
			double endingAngle = startingAngle + arange;
			double centreAngle = startingAngle + arange / 2;
			BaF thisCell = relBaF(tp);
			bool tct = bkgrProps.at(thisCell.first).transparent;
			bool centreAngleBlocked = false;
			bool endingAngleBlocked = false;
			bool startingAngleBlocked = false;
			bool tcs = true;
			for (auto ap : *currentBlocked) {
				if (startingAngle >= ap.first && startingAngle < ap.second)
					startingAngleBlocked = true;
				if (endingAngle > ap.first && endingAngle <= ap.second)
					endingAngleBlocked = true;
				if (centreAngle > ap.first && centreAngle < ap.second)
					centreAngleBlocked = true;
				if (tct) {
					if (centreAngleBlocked || (startingAngleBlocked && endingAngleBlocked)) {
						tcs = false;
						break;
					}
				} else {
					if (centreAngleBlocked && startingAngleBlocked && endingAngleBlocked) {
						tcs = false;
						break;
					}
				}
			}

			if (tcs) {
				Visibility thisvs = {
					true,
					thisCell.first,
					thisCell.second
				};
				(*visiblelocations)[tp] = thisvs;
				if (!tct)
					nextLineBlocked->push_back(
							AnglePair(
								startingAngle, 
								endingAngle));
				
			}


		}
	}

	delete nextLineBlocked;
	delete currentBlocked;

	// SSE octant - x positive, y positive
	nextLineBlocked = new std::vector<AnglePair>;
	currentBlocked = new std::vector<AnglePair>;

	for (int y = 1; y < FOV_RADIUS; y++) {

		for (auto ap : *nextLineBlocked)
			currentBlocked->push_back(ap);
		delete nextLineBlocked;
		nextLineBlocked = new std::vector<AnglePair>;


		double arange = 1.0 / (double)(y + 1);
		for (int x = 0; x <= y; x++) {
			// Don't need -x this time!
			Point tp(x, y);
			double startingAngle = x * arange;
			// Mostly the same from here on
			double endingAngle = startingAngle + arange;
			double centreAngle = startingAngle + arange / 2;
			BaF thisCell = relBaF(tp);
			bool tct = bkgrProps.at(thisCell.first).transparent;
			bool centreAngleBlocked = false;
			bool endingAngleBlocked = false;
			bool startingAngleBlocked = false;
			bool tcs = true;
			for (auto ap : *currentBlocked) {
				if (startingAngle >= ap.first && startingAngle < ap.second)
					startingAngleBlocked = true;
				if (endingAngle > ap.first && endingAngle <= ap.second)
					endingAngleBlocked = true;
				if (centreAngle > ap.first && centreAngle < ap.second)
					centreAngleBlocked = true;
				if (tct) {
					if (centreAngleBlocked || (startingAngleBlocked && endingAngleBlocked)) {
						tcs = false;
						break;
					}
				} else {
					if (centreAngleBlocked && startingAngleBlocked && endingAngleBlocked) {
						tcs = false;
						break;
					}
				}
			}

			if (tcs) {
				Visibility thisvs = {
					true,
					thisCell.first,
					thisCell.second
				};
				(*visiblelocations)[tp] = thisvs;
				if (!tct)
					nextLineBlocked->push_back(
							AnglePair(
								startingAngle, 
								endingAngle));
				
			}
		}
	}
	delete nextLineBlocked;
	delete currentBlocked;


//	Try to impose circularity maybe
//	for (auto & vs : *visiblelocations) {
//		if (vs.first.first * vs.first.first + vs.first.second * vs.first.second > FOV_RADIUS * FOV_RADIUS)
//			vs.second.visible = false;
//	}

}
