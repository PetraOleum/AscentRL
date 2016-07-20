#include "creature.h"
#include <stack>
#include <cassert>

Foreground getCreaturePointerForeground(Creature* creature) {
	return getCreatureForeground(creature->getType());
}

Creature::Creature(Point position, Region * region, CreatureType type) {
	this->position = position;
	this->region = region;
	this->type = type;
}

Creature::~Creature() {

}

Direction Creature::propose_action() {
	assert(cvismap != NULL);

	if (plan->empty()) {
		Point goal = findTarget();
		delete plan;
		plan = astar({0,0}, goal);
	}
	if (!plan->empty()) {
		Direction nd = plan->front();
		plan->pop();
		return nd;
	}
	return Direction::NONE;
	
}

std::queue<Direction> * Creature::astar(Point start, Point finish) {
	std::queue<Direction> * directions = new std::queue<Direction>;
	if (!bkgrProps.at((*cvismap)[start].background).passible) {
		return directions;
	}
	using movement_cost_t = double;
	using move_t = std::pair<Point, Direction>;
	using costandmove = std::pair<movement_cost_t, move_t>;

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
			Background nb = (*cvismap)[next.first].background;
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

Point Creature::findTarget() {
	if (cvismap == NULL)
		return {0,0};
	for (auto it : *cvismap)
		if (it.second.visible && it.second.foreground == Foreground::Witch) 
			return it.first;
	return {0,0};
}
