#include "engine.h"

std::map<Point, Visibility>* Engine::FOV(Point point, Region * region) {
	auto visMap = new std::map<Point, Visibility>;

	BaF curpt = relBaF({0,0}, point, region);
	Visibility curvs = {
		true,
		curpt.background,
		curpt.foreground,
		curpt.creatureHere,
		curpt.creatureTeam,
		curpt.itemHere,
		curpt.HPPerHere
	};
	(*visMap)[Point(0,0)] = curvs;

	// An implementation of Restrictive Precise Angle Shadowcasting, 
	// as described at http://www.roguebasin.com/index.php?title=Restrictive_Precise_Angle_Shadowcasting&oldid=40520

	using AnglePair = std::pair<double, double>;

	auto fovlambda = [visMap, point, region, this](int xTransform, int yTransform) {
		std::vector<AnglePair> * currentBlocked = new std::vector<AnglePair>;
		std::vector<AnglePair> * nextLineBlocked = new std::vector<AnglePair>;

		for (int y = 1; y < FOV_RADIUS; y++) {
			// Copy next line into current line, and refresh next line
			for (auto ap : *nextLineBlocked)
				currentBlocked->push_back(ap);
			delete nextLineBlocked;
			nextLineBlocked = new std::vector<AnglePair>;

			double arange = 1.0 / (double)(y + 1);
			for (int x = 0; x <= y; x++) {
				Point tp(x * xTransform, y * yTransform);
				double startingAngle = x * arange;
				double endingAngle = startingAngle + arange;
				double centreAngle = startingAngle + arange / 2;
				BaF thisCell = relBaF(tp, point, region);
				bool tct = bkgrProps.at(thisCell.background).transparent;
				bool centreAngleBlocked = false;
				bool endingAngleBlocked = false;
				bool startingAngleBlocked = false;
				bool tcs = true;
				for (auto ap : *currentBlocked) {
					if (startingAngle >= ap.first && startingAngle <= ap.second)
						startingAngleBlocked = true;
					if (endingAngle >= ap.first && endingAngle <= ap.second)
						endingAngleBlocked = true;
					if (centreAngle >= ap.first && centreAngle <= ap.second)
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
						thisCell.background,
						thisCell.foreground,
						thisCell.creatureHere,
						thisCell.creatureTeam,
						thisCell.itemHere,
						thisCell.HPPerHere
					};
					(*visMap)[tp] = thisvs;
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
	};
	
	auto fovlambda2 = [visMap, point, region, this](int xTransform, int yTransform) {
		std::vector<AnglePair> * currentBlocked = new std::vector<AnglePair>;
		std::vector<AnglePair> * nextLineBlocked = new std::vector<AnglePair>;

		for (int x = 1; x < FOV_RADIUS; x++) {
			// Copy next line into current line, and refresh next line
			for (auto ap : *nextLineBlocked)
				currentBlocked->push_back(ap);
			delete nextLineBlocked;
			nextLineBlocked = new std::vector<AnglePair>;

			double arange = 1.0 / (double)(x + 1);
			for (int y = 0; y <= x; y++) {
				Point tp(x * xTransform, y * yTransform);
				double startingAngle = y * arange;
				double endingAngle = startingAngle + arange;
				double centreAngle = startingAngle + arange / 2;
				BaF thisCell = relBaF(tp, point, region);
				bool tct = bkgrProps.at(thisCell.background).transparent;
				bool centreAngleBlocked = false;
				bool endingAngleBlocked = false;
				bool startingAngleBlocked = false;
				bool tcs = true;
				for (auto ap : *currentBlocked) {
					if (startingAngle >= ap.first && startingAngle <= ap.second)
						startingAngleBlocked = true;
					if (endingAngle >= ap.first && endingAngle <= ap.second)
						endingAngleBlocked = true;
					if (centreAngle >= ap.first && centreAngle <= ap.second)
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
						thisCell.background,
						thisCell.foreground,
						thisCell.creatureHere,
						thisCell.creatureTeam,
						thisCell.itemHere,
						thisCell.HPPerHere
					};
					(*visMap)[tp] = thisvs;
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
	};




	// SSW octant - x negative, y positive
	fovlambda(-1, 1);

	// SSE octant - x positive, y positive
	fovlambda(1, 1);

	// NNE octant - x positive, y negative
	fovlambda(1, -1);

	// NNW octant - x negative, y negative
	fovlambda(-1, -1);
	
	// ESE octant - x positive, y positive
	fovlambda2(1, 1);

	// ENE octant - x positive, y negative
	fovlambda2(1, -1);

	// WSW octant - x negative, y positive
	fovlambda2(-1, 1);

	// WNW octant - x negative, y negative
	fovlambda2(-1, -1);


	return visMap;
}
