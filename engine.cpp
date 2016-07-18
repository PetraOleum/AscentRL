#include "engine.h"
#include <stack>

Engine::Engine() {
	std::random_device rn;
	randomengine = std::mt19937(rn());
	roomdist = std::uniform_int_distribution<int>(
			MIN_ROOM_DIMENSION, 
			MAX_ROOM_DIMENSION
			);
	probdist = std::uniform_real_distribution<double>(0, 1);

	//Temp stuff with 1 region
	
	player = new Creature({0, 0}, activeRegion, CreatureType::Witch);
	activeRegion = new Region(10, 10, RoomType::Room);
	activeRegion->position = { 0, 0 };
	activeRegion->setForeground(player->getPosition(), Foreground::Witch);
	regions.push_back(activeRegion);
	refreshFOV();

}

Engine::~Engine() {
	for (Region* region : regions) {
		delete region;
	}
	delete player;
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
	Point originalPosition = player->getPosition();
	Background nb = relBaF(np, originalPosition).first;
	if (!bkgrProps.at(nb).passible)
		return false;
	if (activeRegion->getBackground(PAIR_SUM(np, originalPosition)) != nb) {
		swapRegions();
	}
	activeRegion->setForeground(player->getPosition(), underForeground);
	Point newposition = player->movePosition(direction);
	underForeground = activeRegion->getForeground(newposition);
	activeRegion->setForeground(newposition, Foreground::Witch);
	manageAltRegion();
	refreshFOV();

	return true;
}

BaF Engine::relBaF(Point point, const Point & relto, Region * region) {
	Point relpt = PAIR_SUM(relto, point);
	BaF regionBaF = {
		region->getBackground(relpt),
		region->getForeground(relpt)
	};
	Connection cn = region->connectionAt(relto);
	if (cn.to != NULL) {
		Point tpd = DISPLACEMENT(cn.direction);
		Point muldirr = PAIR_MULTIPLY(tpd, point);
		if (muldirr.first > 0 || muldirr.second > 0) {
			Point altrelpt = PAIR_SUM(relpt, PAIR_SUBTRACT(cn.toLocation, relto));
			BaF altBaF = {
				cn.to->getBackground(altrelpt),
				cn.to->getForeground(altrelpt)
			};
			if (altBaF.first != Background::EMPTYNESS) {
				return altBaF;
			}
		}
	}

	return regionBaF;
	
}

BaF Engine::relBaF(Point point, const Point & relto) {
	return relBaF(point, relto, activeRegion);
}

void Engine::refreshFOV() {
	if (visiblelocations != NULL)
		delete visiblelocations;
	visiblelocations = FOV(player->getPosition());

}

void Engine::manageAltRegion() {
	Background cpb = activeRegion->getBackground(player->getPosition());
	if (cpb == Background::Door || cpb == Background::MarkedDoor) {
		if (cpb == Background::Door) activeRegion->markDoor(player->getPosition());
		Connection tc = activeRegion->connectionAt(player->getPosition());
//		printf("manageAltRegion(), 0x%lx\n", (long int)tc.to);
		if (tc.to != NULL) {
			alternateRegion = tc.to;
			altDisplacement = PAIR_SUBTRACT(
					tc.toLocation,
					player->getPosition()
					);
			if (alternateRegion->getBackground(tc.toLocation) == Background::Door)
				alternateRegion->markDoor(tc.toLocation);
			altRegionLoaded = true;
		} else {
			RoomType nrt = (activeRegion->Type() == RoomType::Room)
				? RoomType::Corridor : RoomType::Room;
//			printf("%d -> %d\n", (int)activeRegion->Type(), (int)nrt);
			Region * nr = NULL;
			std::pair<Point, bool> freept;
			Point rpoint = PAIR_SUM(activeRegion->position, DISPLACEMENT(tc.direction));
			bool foundfree = false;
			if (probdist(randomengine) < EXISTING_ROOM_PROB)
				for (auto &&tr : regions) 
					if (tr->position == rpoint) {
						freept = tr->freeConnection(oppositeDirection(tc.direction));
						if (freept.second) {
							nr = tr;
							foundfree = true;
							break;
						}
					}
			if (!foundfree) {
				nr = new Region(roomdist(randomengine), roomdist(randomengine), nrt);
				nr->position = rpoint;
				freept = nr->freeConnection(oppositeDirection(tc.direction));
			}
			
			if (!freept.second) {
				altRegionLoaded = false;
				alternateRegion = NULL;
				if (!foundfree)
					delete nr;
				return;
			}
			if (!activeRegion->connectTo(nr, tc.direction, player->getPosition(), freept.first)) {
				fprintf(stderr, "This should not have happened: error connecting activeRegion to nr\n");
				altRegionLoaded = false;
				alternateRegion = NULL;
				if (!foundfree)
					delete nr;
				return;
			}
			if (!nr->connectTo(activeRegion, oppositeDirection(tc.direction), freept.first, player->getPosition())) {
				fprintf(stderr, "This should not have happened: error connecting nr to activeRegion (note: corrupted activeRegion)\n");
				altRegionLoaded = false;
				alternateRegion = NULL;
				if (!foundfree)
					delete nr;
				return;
			}
//			printf("Attached\n");
			if (!foundfree)
				regions.push_back(nr);
			alternateRegion = nr;
			altDisplacement = PAIR_SUBTRACT(
					freept.first,
					player->getPosition()
					);
			if (alternateRegion->getBackground(freept.first) == Background::Door)
				alternateRegion->markDoor(freept.first);
			altRegionLoaded = true;
//			printf("{%d, %d}, {%d, %d}, {%d, %d}\n", altDisplacement.first, altDisplacement.second, currentPosition.first, currentPosition.second, freept.first.first, freept.first.second);
		}
	} else {
		altRegionLoaded = false;
		alternateRegion = NULL;
	}
//	printf("AR = %ld\n", (long int)alternateRegion);
}

void Engine::swapRegions() {
	manageAltRegion();
	if (!altRegionLoaded) {
//		printf("AR not loaded. AR = %ld\n", (long int)alternateRegion);
		return;
	}
//	printf("Swapping\n");
	Point startPosition = player->getPosition();
	activeRegion->setForeground(startPosition, underForeground);
	Point npos = player->switchRegion(alternateRegion, altDisplacement);
	Point nad = PAIR_SUBTRACT(Point(0,0), altDisplacement);
	Region * tmp = activeRegion;
	activeRegion = alternateRegion;
	alternateRegion = tmp;
//	currentPosition = npos;
	altDisplacement = nad;
	underForeground = activeRegion->getForeground(npos);
	activeRegion->setForeground(npos, Foreground::Witch);
//	printf("{%d, %d}; {%d, %d}; {%d, %d}\n", activeRegion->position.first, activeRegion->position.second, alternateRegion->position.first, alternateRegion->position.second, player->getRegion()->position.first, player->getRegion()->position.second);
}

std::queue<Direction> * Engine::astar(Point start, Point finish, Point relativeTo) {
	using movement_cost_t = double;
	using move_t = std::pair<Point, Direction>;
	using costandmove = std::pair<movement_cost_t, move_t>;
	std::queue<Direction> * directions = new std::queue<Direction>;

	std::priority_queue<costandmove, std::vector<costandmove>, std::greater<costandmove> > fronteir;
	fronteir.push( 
			{
				0.0,
				{
					start,
					Direction::Up
				} 
			});

	std::map<Point, move_t> came_from;
	came_from[start] = {
		start,
		Direction::Up
	};
	std::map<Point, movement_cost_t> cost_so_far;
	cost_so_far[start] = 0.0;

	bool success = false;
	std::map<Point, Visibility> * myfov = FOV(relativeTo);

	if (!bkgrProps.at((*myfov)[start].background).passible) {
		delete myfov;
		return directions;
	}
	while (!fronteir.empty()) {
		auto current_state = fronteir.top();
		Point current = current_state.second.first;
		if (current == finish) {
			success = true;
		}
		movement_cost_t current_cost = current_state.first;
		fronteir.pop();
		for (const auto & disp : displacementMap) {
			move_t next = {
				PAIR_SUM(disp.second, current),
				disp.first
			};
			Background nb = (*myfov)[next.first].background;
//			BaF nbaf = relBaF(next.first, relativeTo);
			if (!bkgrProps.at(nb).passible)
				continue;
			movement_cost_t new_cost = ((disp.second.first != 0 && disp.second.second != 0) ? 1.41421356237 : 1) + current_cost;
			if (cost_so_far.find(next.first) == cost_so_far.end()) {
				fronteir.push(std::make_pair(new_cost, next));
				came_from[next.first] = {
					current,
					disp.first
				};
				cost_so_far[next.first] = new_cost;
			} else if (cost_so_far[next.first] > new_cost) {
				fronteir.push(std::make_pair(new_cost, next));
				came_from[next.first] = {
					current,
					disp.first
				};
				cost_so_far[next.first] = new_cost;
				
			}

		}

	}
	delete myfov;

	if (!success)
		return directions;

	std::stack<Direction> rpath;
	Point c2 = finish;
	while (c2 != start) {
		move_t cfm = came_from[c2];
		c2 = cfm.first;
		rpath.push(cfm.second);
	}

	while (!rpath.empty()) {
		directions->push(rpath.top());
		rpath.pop();
	}

	return directions;
}
