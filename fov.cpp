#include "engine.h"

std::map<Point, Visibility>* Engine::FOV(Point point) {
	auto visMap = new std::map<Point, Visibility>;

	BaF curpt = relBaF({0,0}, point);
	Visibility curvs = {
		true,
		curpt.first,
		curpt.second
	};
	(*visMap)[Point(0,0)] = curvs;

	// An implementation of Restrictive Precise Angle Shadowcasting, 
	// as described at http://www.roguebasin.com/index.php?title=Restrictive_Precise_Angle_Shadowcasting&oldid=40520

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
			double startingAngle = x * arange;
			double endingAngle = startingAngle + arange;
			double centreAngle = startingAngle + arange / 2;
			BaF thisCell = relBaF(tp, point);
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
			BaF thisCell = relBaF(tp, point);
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

	//NNE octant - x positive, y negative
	nextLineBlocked = new std::vector<AnglePair>;
	currentBlocked = new std::vector<AnglePair>;

	for (int y = 1; y < FOV_RADIUS; y++) {

		for (auto ap : *nextLineBlocked)
			currentBlocked->push_back(ap);
		delete nextLineBlocked;
		nextLineBlocked = new std::vector<AnglePair>;


		double arange = 1.0 / (double)(y + 1);
		for (int x = 0; x <= y; x++) {
			Point tp(x, -y);
			double startingAngle = x * arange;
			// Mostly the same from here on
			double endingAngle = startingAngle + arange;
			double centreAngle = startingAngle + arange / 2;
			BaF thisCell = relBaF(tp, point);
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

	// NNW octant - x negative, y negative
	nextLineBlocked = new std::vector<AnglePair>;
	currentBlocked = new std::vector<AnglePair>;

	for (int y = 1; y < FOV_RADIUS; y++) {

		for (auto ap : *nextLineBlocked)
			currentBlocked->push_back(ap);
		delete nextLineBlocked;
		nextLineBlocked = new std::vector<AnglePair>;


		double arange = 1.0 / (double)(y + 1);
		for (int x = 0; x <= y; x++) {
			Point tp(-x, -y);
			double startingAngle = x * arange;
			// Mostly the same from here on
			double endingAngle = startingAngle + arange;
			double centreAngle = startingAngle + arange / 2;
			BaF thisCell = relBaF(tp, point);
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
	
	//ESE octant - x positive, y positive
	nextLineBlocked = new std::vector<AnglePair>;
	currentBlocked = new std::vector<AnglePair>;

	for (int x = 1; x < FOV_RADIUS; x++) {

		for (auto ap : *nextLineBlocked)
			currentBlocked->push_back(ap);
		delete nextLineBlocked;
		nextLineBlocked = new std::vector<AnglePair>;


		double arange = 1.0 / (double)(x + 1);
		for (int y = 0; y <= x; y++) {
			Point tp(x, y);
			double startingAngle = y * arange;
			// Mostly the same from here on
			double endingAngle = startingAngle + arange;
			double centreAngle = startingAngle + arange / 2;
			BaF thisCell = relBaF(tp, point);
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

	// ENE octant - x positive, y negative
	nextLineBlocked = new std::vector<AnglePair>;
	currentBlocked = new std::vector<AnglePair>;

	for (int x = 1; x < FOV_RADIUS; x++) {

		for (auto ap : *nextLineBlocked)
			currentBlocked->push_back(ap);
		delete nextLineBlocked;
		nextLineBlocked = new std::vector<AnglePair>;


		double arange = 1.0 / (double)(x + 1);
		for (int y = 0; y <= x; y++) {
			Point tp(x, -y);
			double startingAngle = y * arange;
			// Mostly the same from here on
			double endingAngle = startingAngle + arange;
			double centreAngle = startingAngle + arange / 2;
			BaF thisCell = relBaF(tp, point);
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

	//WSW octant - x negative, y positive
	nextLineBlocked = new std::vector<AnglePair>;
	currentBlocked = new std::vector<AnglePair>;

	for (int x = 1; x < FOV_RADIUS; x++) {

		for (auto ap : *nextLineBlocked)
			currentBlocked->push_back(ap);
		delete nextLineBlocked;
		nextLineBlocked = new std::vector<AnglePair>;


		double arange = 1.0 / (double)(x + 1);
		for (int y = 0; y <= x; y++) {
			Point tp(-x, y);
			double startingAngle = y * arange;
			// Mostly the same from here on
			double endingAngle = startingAngle + arange;
			double centreAngle = startingAngle + arange / 2;
			BaF thisCell = relBaF(tp, point);
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

	// WNW octant - x negative, y negative
	nextLineBlocked = new std::vector<AnglePair>;
	currentBlocked = new std::vector<AnglePair>;

	for (int x = 1; x < FOV_RADIUS; x++) {

		for (auto ap : *nextLineBlocked)
			currentBlocked->push_back(ap);
		delete nextLineBlocked;
		nextLineBlocked = new std::vector<AnglePair>;


		double arange = 1.0 / (double)(x + 1);
		for (int y = 0; y <= x; y++) {
			Point tp(-x, -y);
			double startingAngle = y * arange;
			// Mostly the same from here on
			double endingAngle = startingAngle + arange;
			double centreAngle = startingAngle + arange / 2;
			BaF thisCell = relBaF(tp, point);
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


	return visMap;
}
