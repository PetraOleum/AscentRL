#include "creature.h"
#include <stack>
#include <cassert>
#include <sstream>
#include <iostream>
#include <iomanip>

Foreground getCreaturePointerForeground(Creature* creature) {
	return getCreatureForeground(creature->getType());
}

Creature::Creature(Point position, Region * region, CreatureType type, Team team) {
	this->position = position;
	this->region = region;
	this->type = type;
	this->properties = getCreatureProperties(type);
	this->team = team;
	dweapon = std::uniform_int_distribution<int>(1, this->properties.attackDice);
	std::random_device rd;
	gen = std::mt19937(rd());
}

Creature::~Creature() {

}

Direction Creature::propose_action() {
	assert(cvismap != NULL);

	target = findTarget();
	delete plan;
	plan = astar({0,0}, target);
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
			Visibility npvis = (*cvismap)[next.first];
//			if (npvis.team == this->team)
//				continue;
			Background nb = npvis.background;
			
//			BaF nbaf = relBaF(next.first, relativeTo);
			if (!bkgrProps.at(nb).passible)
				continue;
			movement_cost_t new_cost = ((disp.second.first != 0 && disp.second.second != 0) ? 1.41421356237 : 1) + ((npvis.team == this->team) ? 2 : 0) + current_cost;
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
	Point tt = target;
	for (auto it : *cvismap)
		if (it.second.visible && it.second.creature != CreatureType::NONE && it.second.team != this->team) {
			tt = it.first;
		}
	return tt;
//	return {0,0};
}


std::string Creature::ToString() const {
	std::stringstream ts;
	ts << "this: \t";
	ts << std::hex;
	ts << std::showbase << std::internal << std::setfill('0') << std::setw(16);
	ts << (long int)this;
	ts << std::dec;
	ts << "\n";
	ts << "Type: \t";
	try {
		ts << foreProps.at(this->Sprite()).name << "\n";
	} catch (std::exception e) {
		ts << (uint8_t)this->type << " (Could not get name: " << e.what() << ")\n";
	}
	ts << "Position: \t" << this->position.first << ", " << this->position.second << "\n";
	ts << "Target: \t" << this->target.first << ", " << this->target.second << "\n";
	if (this->plan != NULL)
		if (!this->plan->empty())
			ts << "Next: \t" << DISPLACEMENT(this->plan->front()).first << ", " << DISPLACEMENT(this->plan->front()).second << "\n";
	ts << "HP: \t" << this->properties.HP << "\n";
	ts << "AC: \t" << this->properties.AC << "\n";
	ts << "Weapon: \t d" << this->properties.attackDice << " + " << this->properties.baseAttack << "\n";
	ts << "Region: \t";
	ts << std::hex;
	ts << std::showbase << std::internal << std::setfill('0') << std::setw(16);
	ts << (long int)this->region;
	ts << std::dec;
	ts << " (" << this->region->position.first << ", " << this->region->position.second << ")\n";

	return ts.str();
}

void Creature::kill() {
	this->properties.HP = 0;
	this->region = NULL;
	this->position = {0,0};
	this->target = {0,0};
	this->killed = true;
}
