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
	
	Region * StartRegion = new Region(10, 10, RoomType::Spiral);
	player = new Creature({0, 0}, StartRegion, CreatureType::Witch, Team::Player);
	player->give(ItemType::Gold, 1);
	player->give(ItemType::NONE, 17);
	player->give(ItemType::Staff);
	player->give(ItemType::Gold, 20);
//	Creature * rat = new Creature({5, 5}, StartRegion, CreatureType::Rat);
//	StartRegion->putCreature(rat->getPosition(), rat);
//	creatures.push_back(rat);
	StartRegion->position = { 0, 0 };
	StartRegion->putCreature( player->getPosition(), player);
	regions.push_back(StartRegion);
	refreshFOV();

}

Engine::~Engine() {
	for (Region* region : regions) {
		delete region;
	}
	for (Creature* creature : creatures) {
		delete creature;
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

bool Engine::seeCreatureHere(Point point) {
	if (visiblelocations == NULL) {
		fprintf(stderr, "Accessing FOV map before creation\n");
		return false;
	}
	auto it = visiblelocations->find(point);
	if (it == visiblelocations->end())
		return false;
	else if (it->second.visible)
		return it->second.creature != CreatureType::NONE;
	else
		return false;
}

double Engine::creatureHPPercentHere(Point point) {
	if (visiblelocations == NULL) {
		fprintf(stderr, "Accessing FOV map before creation\n");
		return 0.0;
	}
	auto it = visiblelocations->find(point);
	if (it == visiblelocations->end())
		return 0.0;
	else if (it->second.visible)
		return it->second.creatureHP;
	else 
		return 0.0;

}

//bool Engine::Move(Direction direction) {
//	if (!monsterMove(player, direction))
//		return false;
//	doMonsterTurns();
//
//	return true;
//}

bool Engine::Act(Action action) {
	switch (action.type) {
		case ActionType::Move:
			if (!monsterMove(player, action.direction))
				return false;
			break;
		case ActionType::Pickup:
			if (action.direction != Direction::NONE)
				return false;
			if (!monsterPickUp(player))
				return false;
			break;
		case ActionType::Attack:
			if (!monsterAttack(player, action.direction))
				return false;
			break;
		case ActionType::Drop:
			if (action.direction != Direction::NONE)
				return false;
			if (!monsterDrop(player, action.inventoryItem))
				return false;
			break;
		case ActionType::NONE:
			break;
		default:
			fprintf(stderr, "Attempted to make a non-implemented action of val %d:%d\n", (int)action.type, (int)action.direction);
			return false;
	};

	doMonsterTurns();
	return true;
}

BaF Engine::relBaF(Point point, const Point & relto, Region * region) {
	Point relpt = PAIR_SUM(relto, point);
	BaF regionBaF = {
		region->getBackground(relpt),
		region->getForeground(relpt),
		getForegroundCreature(region->getForeground(relpt)),
		(region->hasCreature(relpt)) ? region->getCreature(relpt)->creatureTeam() : Team::NONE,
		region->topItem(relpt),
		(region->hasCreature(relpt)) ? region->getCreature(relpt)->healthFraction() : 0
	};
	Connection cn = region->connectionAt(relto);
	if (cn.to != NULL) {
		Point tpd = DISPLACEMENT(cn.direction);
		Point muldirr = PAIR_MULTIPLY(tpd, point);
		if (muldirr.first > 0 || muldirr.second > 0) {
			Point altrelpt = PAIR_SUM(relpt, PAIR_SUBTRACT(cn.toLocation, relto));
			BaF altBaF = {
				cn.to->getBackground(altrelpt),
				cn.to->getForeground(altrelpt),
				getForegroundCreature(cn.to->getForeground(altrelpt)),
				(cn.to->hasCreature(altrelpt)) ? cn.to->getCreature(altrelpt)->creatureTeam() : Team::NONE,
				cn.to->topItem(altrelpt),
				(cn.to->hasCreature(altrelpt)) ? cn.to->getCreature(altrelpt)->healthFraction() : 0
			};
			if (altBaF.background != Background::EMPTYNESS) {
				return altBaF;
			}
		}
	}

	return regionBaF;
	
}

//BaF Engine::relBaF(Point point, const Point & relto) {
//	return relBaF(point, relto, player->getRegion());
//}

void Engine::refreshFOV() {
	if (visiblelocations != NULL)
		delete visiblelocations;
	visiblelocations = FOV(player->getPosition(), player->getRegion());
}

void Engine::manageAltRegion(Region * curregion, const Point& position) {
//	Region * playerRegion = player->getRegion();
	Background cpb = curregion->getBackground(position);
	if (cpb == Background::Door || cpb == Background::MarkedDoor) {
		if (cpb == Background::Door) curregion->markDoor(position);
		Connection tc = curregion->connectionAt(position);
//		printf("manageAltRegion(), 0x%lx\n", (long int)tc.to);
		if (tc.to != NULL) {
			Region * alternateRegion = tc.to;
//			altDisplacement = PAIR_SUBTRACT(
//					tc.toLocation,
//					player->getPosition()
//					);
			if (alternateRegion->getBackground(tc.toLocation) == Background::Door)
				alternateRegion->markDoor(tc.toLocation);
//			altRegionLoaded = true;
		} else {
			RoomType nrt = (curregion->Type() == RoomType::Corridor)
				? RoomType::Room : RoomType::Corridor;
//			printf("%d -> %d\n", (int)activeRegion->Type(), (int)nrt);
			Region * nr = NULL;
			std::pair<Point, bool> freept;
			Point rpoint = PAIR_SUM(curregion->position, DISPLACEMENT(tc.direction));
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
//				altRegionLoaded = false;
//				alternateRegion = NULL;
				if (!foundfree)
					delete nr;
				return;
			}
			if (!curregion->connectTo(nr, tc.direction, position, freept.first)) {
				fprintf(stderr, "This should not have happened: error connecting activeRegion to nr. Is this a \"real\" door?\n");
//				altRegionLoaded = false;
//				alternateRegion = NULL;
				if (!foundfree)
					delete nr;
				return;
			}
			if (!nr->connectTo(curregion, oppositeDirection(tc.direction), freept.first, position)) {
				fprintf(stderr, "This should not have happened: error connecting nr to activeRegion (note: corrupted activeRegion)\n");
//				altRegionLoaded = false;
//				alternateRegion = NULL;
				if (!foundfree)
					delete nr;
				return;
			}
//			printf("Attached\n");
			if (!foundfree) {
				PopulateNewRegion(nr);
				regions.push_back(nr);
			}
			Region * alternateRegion = nr;
//			altDisplacement = PAIR_SUBTRACT(
//					freept.first,
//					player->getPosition()
//					);
			if (alternateRegion->getBackground(freept.first) == Background::Door)
				alternateRegion->markDoor(freept.first);
//			altRegionLoaded = true;
//			printf("{%d, %d}, {%d, %d}, {%d, %d}\n", altDisplacement.first, altDisplacement.second, currentPosition.first, currentPosition.second, freept.first.first, freept.first.second);
		}
	} else {
//		altRegionLoaded = false;
//		alternateRegion = NULL;
	}
//	printf("AR = %ld\n", (long int)alternateRegion);
}

void Engine::swapRegions(Creature * creature) {
	manageAltRegion(creature->getRegion(), creature->getPosition());
	Connection ccon = creature->getRegion()->connectionAt(creature->getPosition());
	if (ccon.to == NULL) {
		fprintf(stderr, "AR not loaded.");
		return;
	}
//	Point startPosition = creature->getPosition();
//	Region * playerStartRegion = creature->getRegion();
//	playerStartRegion->putCreature(startPosition, NULL);
//	playerStartRegion->setForeground(startPosition, underForeground);
	Point npos = creature->switchRegion(ccon.to, PAIR_SUBTRACT(ccon.toLocation, creature->getPosition()));
	Region * newCreatureRegion = creature->getRegion();
//	underForeground = newCreatureRegion->getForeground(npos);
//	newCreatureRegion->setForeground(npos, Foreground::Witch);
	newCreatureRegion->putCreature(npos, player);
}

std::queue<Direction> * Engine::astar(Point start, Point finish, Point relativeTo, Region * region) {
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
	std::map<Point, Visibility> * myfov = FOV(relativeTo, region);

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

void Engine::doMonsterTurns() {
	for (unsigned int i = 0; i < creatures.size(); i++) {
		Creature * monster = creatures[i];
		if (monster->isAlive()) {
			monster->updateFOV(FOV(monster->getPosition(), monster->getRegion()));
			monsterMove(monster, monster->propose_action());
			if (!monster->maxHealth())
				if (probdist(randomengine) < monster->Properties().regen)
					monster->heal(1);
		}
	}
	refreshFOV();
	if (probdist(randomengine) < player->Properties().regen)
		player->heal(1);
//	printf("HP: %d\n", player->Properties().HP);
//	ReportState();
}

bool Engine::monsterMove(Creature * creature, Direction direction) {
	Region * cRegion = creature->getRegion();
	manageAltRegion(cRegion, creature->getPosition());
	Point np = DISPLACEMENT(direction);
	Point originalPosition = creature->getPosition();
	auto nbaf = relBaF(np, originalPosition, cRegion);
	Background nb = nbaf.background;
	if (!bkgrProps.at(nb).passible)
		return false;
	if (nbaf.creatureHere != CreatureType::NONE) {
		Creature * defender = cRegion->getCreature(PAIR_SUM(np, originalPosition));
		if (defender == NULL) {
			//Must be across a region boundary
			Connection cn = cRegion->connectionAt(originalPosition);
			if (cn.to == NULL) //Something funny
				return false;
			defender = cn.to->getCreature(PAIR_SUM(cn.toLocation, np));
			if (defender == NULL)
				return false;
		}
		handleAttack(creature, defender);
		return true;
	}
	if (cRegion->getBackground(PAIR_SUM(np, originalPosition)) != nb)
		swapRegions(creature);
	cRegion = creature->getRegion();
	cRegion->putCreature(creature->getPosition(), NULL);
	Connection potentialConnection = cRegion->connectionAt(creature->getPosition());
	if (potentialConnection.to != NULL)
		potentialConnection.to->putCreature(potentialConnection.toLocation, NULL);
	Point newposition = creature->movePosition(direction);
	cRegion->putCreature(newposition, creature);
	manageAltRegion(cRegion, creature->getPosition());
	potentialConnection = cRegion->connectionAt(newposition);
	if (potentialConnection.to != NULL)
		potentialConnection.to->putCreature(potentialConnection.toLocation, creature);
	return true;

}

bool Engine::monsterAttack(Creature * creature, Direction direction) {
	Region * cRegion = creature->getRegion();
	manageAltRegion(cRegion, creature->getPosition());
	Point np = DISPLACEMENT(direction);
	Point originalPosition = creature->getPosition();
	auto nbaf = relBaF(np, originalPosition, cRegion);
	if (nbaf.creatureHere != CreatureType::NONE) {
		Creature * defender = cRegion->getCreature(PAIR_SUM(np, originalPosition));
		if (defender == NULL) {
			//Must be across a region boundary
			Connection cn = cRegion->connectionAt(originalPosition);
			if (cn.to == NULL) //Something funny
				return false;
			defender = cn.to->getCreature(PAIR_SUM(cn.toLocation, np));
			if (defender == NULL)
				return false;
		}
		handleAttack(creature, defender);
		return true;
	}
	return false;
}

void Engine::ReportState() {
//	for (Region * region : regions)
//		printf("Region:\n%s\n", region->ToString(false).c_str());
//	printf("Player:\n%s\n", player->ToString().c_str());
	for (Creature * cr : creatures)
		printf("Creature:\n%s\n", cr->ToString().c_str());
}

bool Engine::handleAttack(Creature * attacker, Creature * defender) {
	if (attacker == NULL || defender == NULL)
		return false;
	if (attacker->creatureTeam() == defender->creatureTeam())
		return false;
	int attackval = attacker->rollToAttack();
	if (attackval >= defender->AC())
		if (!defender->takeHit(attacker->rollWeapon()))
			removeFromGame(defender);
	return true;
}

void Engine::removeFromGame(Creature * deadded) {
	// This not the place to handle the death of the player, and kill()ing them here will cause a segfault
	if (deadded == player) {
		return;
	}
	if (deadded == NULL)
		return;
	Region * dcregion = deadded->getRegion();
	Connection cn = dcregion->connectionAt(deadded->getPosition());
	if (cn.to != NULL) {
		cn.to->putCreature(cn.toLocation, NULL);
	}
	dcregion->putCreature(deadded->getPosition(), NULL);
	deadded->kill();
}

void Engine::PopulateNewRegion(Region * region) {
	for (int x = 0; x < region->Width(); x++)
		for (int y = 0; y < region->Height(); y++) {
			Background rb = region->getBackground(Point(x, y));
			auto bp = bkgrProps.at(rb);
			if (bp.passible && rb != Background::Door && rb != Background::MarkedDoor) {
				if (!region->hasCreature(Point(x, y)))
					if (probdist(randomengine) < 0.1)
					{
						Creature * cr = new Creature(Point(x, y), region, CreatureType::Rat);
						cr->give(ItemType::Gold, 1);
						region->putCreature(Point(x, y), cr);
						creatures.push_back(cr);
					}
			
			}
		}
}

bool Engine::monsterPickUp(Creature * creature) {
	Region * cregion = creature->getRegion();
	Point cpos = creature->getPosition();
	ItemType topitem = cregion->takeItem(cpos);
	if (topitem == ItemType::NONE)
		return false; //Nothing there
	if (!player->give(topitem)) {
		cregion->placeItem(cpos, topitem); //Put it back
		return false; //Arguably should be true - depending on whether it takes an action or not
	}

	

	return true;
}

bool Engine::monsterDrop(Creature * creature, char invindex) {
	Region * cregion = creature->getRegion();
	Point cpos = creature->getPosition();
	if (!bkgrProps.at(cregion->getBackground(cpos)).placeable)
		return false; //Can't drop here
	ItemType iType = creature->getInventory()[invindex].first;
	if (!creature->take(iType))
		return false; //Failed to get this item from the inventory
	cregion->placeItem(cpos, iType);

	return true;
}
