#ifndef ENGINE_H
#define ENGINE_H

#include <utility>
#include <random>
#include <vector>
#include "sprites.h"
#include "region.h"

#define MIN_ROOM_DIMENSION 1
#define MAX_ROOM_DIMENSION 10

using Point = std::pair<int, int>;

class Engine {
	private:
		std::mt19937 randomengine;

		std::uniform_int_distribution<int> roomdist;

		std::vector<Region*> regions;

		Region* activeRegion = NULL;

		Point currentPosition = Point(0, 0);

		Foreground underForeground = Foreground::NONE;

	public:
		Engine();
		

		~Engine();

		Background getBackground(Point point);

		Foreground getForeground(Point point);

		bool Move(Direction direction);

		inline Point getCurrentPosition() {
			return currentPosition;
		}

};

#endif
