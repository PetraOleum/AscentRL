#include "creature.h"

Creature::Creature(Point position, Region * region, CreatureType type) {
	this->position = position;
	this->region = region;
	this->type = type;
}

Creature::~Creature() {

}
