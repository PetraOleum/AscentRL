#ifndef CREATURE_H
#define CREATURE_H

#include "general.h"
#include "region.h"
#include <map>
#include <queue>
#include <random>
#include "inventory.h"

/// @brief Class that holds a creature; use as defined by new
class Creature {
	private:
		/// @brief The position of the creature within the region
		Point position;
		/// @brief The region that the creature is in
		Region * region;
		/// @brief The type of creature that the creature is
		CreatureType type;

		/// @brief The visibility map
		std::map<Point, Visibility> * cvismap = NULL;
		
		/// @brief The plan of moves
		std::queue<Direction> * plan = new std::queue<Direction>;

		/// @brief Find the target to move to
		///
		/// @return Location of the witch
		Point findTarget();

		/// @brief The target
		Point target = Point(0,0);

		/// @brief The properties
		creatureProperties properties;

		/// @brief Random number generator
		std::mt19937 gen;

		/// @brief The d20
		std::uniform_int_distribution<int> d20 = std::uniform_int_distribution<int>(1, 20);

		/// @brief Weapon dice
		std::uniform_int_distribution<int> dweapon;

		/// @brief Has creature been killed?
		bool killed = false;

		/// @brief On monster team by default, not NONE
		Team team = Team::Monsters;

		/// @brief The inventory of the creature
		Inventory inventory;

	public:
		/// @brief Constructor, specifying starting position, region, type
		///
		/// @param position The starting position
		/// @param region Pointer to the starting region
		/// @param type The type of creature
		/// @param team The team the creature is on
		Creature(Point position, Region * region, CreatureType type, Team team);
		Creature(Point position, Region * region, CreatureType type) : Creature(position, region, type, Team::Monsters) {}
		Creature(Point position, Region * region) : Creature(position, region, CreatureType::Rat, Team::Monsters) {}

		/// @brief Creature destructor
		~Creature();

		/// @brief Get const reference to the position
		///
		/// @return The position
		inline const Point& getPosition() const {
			return position;
		}

		/// @brief Get pointer to the region
		///
		/// @return The region
		inline Region * getRegion() const {
			return region;
		}

		/// @brief Get const reference to the type
		///
		/// @return The type
		inline const CreatureType& getType() const {
			return type;
		}

		/// @brief Move the position
		///
		/// @param displacement Displacement to move the position by
		///
		/// @return The new position
		inline const Point& movePosition(const Point& displacement) {
			target = PAIR_SUBTRACT(target, displacement);
			return (position = PAIR_SUM(position, displacement));
		}

		/// @brief Move the position
		///
		/// @param direction The direction to move 1 unit in
		///
		/// @return The new position
		inline const Point& movePosition(const Direction& direction) {
			return movePosition(DISPLACEMENT(direction));
		}

		/// @brief Switch the region and adjust position by delta
		///
		/// @param newRegion The new region
		/// @param delta The delta to *add* to the initial position when switching
		///
		/// @return The new position
		inline const Point& switchRegion(Region * newRegion, const Point& delta) {
			region = newRegion;
			target = PAIR_SUM(target, delta);
			return (position = PAIR_SUM(position, delta));
		}

		/// @brief Get the sprite (i.e. foreground) corresponding to the creature type
		///
		/// @return The foreground
		inline Foreground Sprite() const {
			return getCreatureForeground(type);
		}

		/// @brief Propose an action
		///
		/// @return The direction
		Direction propose_action();

		/// @brief Astar for the creature
		///
		/// @param start The Start location
		/// @param finish The end location
		///
		/// @return The plan
		std::queue<Direction> * astar(Point start, Point finish);

		/// @brief Update the field of view
		///
		/// @param fovmap The map (do be deleted when replaced)
		inline void updateFOV(std::map<Point, Visibility> * fovmap) {
			if (cvismap != NULL)
				delete cvismap;
			cvismap = fovmap;
		}

		/// @brief Get the state of the creature as a string
		///
		/// @return State string
		std::string ToString() const;

		/// @brief Expose reference to properties
		///
		/// @return The properties
		inline const creatureProperties& Properties() const {
			return properties;
		}

		/// @brief Roll attack dice
		///
		/// @return A d20 pluss the hit bonus
		inline int rollToAttack() {
			return d20(gen) + this->properties.hitBonus;
		}

		/// @brief Roll the weapon dice and add baseAttack
		///
		/// @return The value of the roll
		inline int rollWeapon() {
			return dweapon(gen) + this->properties.baseAttack;
		}

		/// @brief Take a hit of specified damage
		///
		/// @param hpDamage The amount of damage to take
		///
		/// @return Whether the creature is still alive
		inline bool takeHit(int hpDamage) {
			this->properties.HP -= hpDamage;
			return (this->properties.HP > 0);
		}

		/// @brief Determing if the creature is alive
		///
		/// @return True if the HP is greater than 0 and has not been "killed"
		inline bool isAlive() const {
			if (killed)
				return false;
			return (this->properties.HP > 0);
		}

		/// @brief Kill this creature
		void kill();

		/// @brief Determine if the creature is at maximum health (or greater)
		///
		/// @return If is at max
		inline bool maxHealth() const {
			return (this->properties.HP >= getCreatureProperties(this->type).HP);
		}

		/// @brief Get the fraction of health the creature is on
		///
		/// @return Current HP / creatureProperties HP, assuming not 0
		inline double healthFraction() const {
			double creatureHP = getCreatureProperties(this->type).HP;
			if (creatureHP == 0)
				return 0;
			return (this->properties.HP) / creatureHP;
		}

		/// @brief Heal creature by specified amount
		///
		/// @param healing The amout to heal byj
		///
		/// @return If now at max health
		inline bool heal(int healing) {
			this->properties.HP += healing;
			if (this->maxHealth()) {
				this->properties.HP = getCreatureProperties(this->type).HP;
				return true;
			}
			return false;
		}

		/// @brief Reference to the HP
		///
		/// @return HP (const reference)
		inline const int& HP() const {
			return this->properties.HP;
		}

		/// @brief Expose AC
		///
		/// @return Reference to AC
		inline const int& AC() const {
			return this->properties.AC;
		}

		/// @brief Expose the team
		///
		/// @return Const reference to the team
		inline const Team& creatureTeam() {
			return team;
		}

		/// @brief Expose inventory
		///
		/// @return Const reference to the inventory object
		inline const Inventory& getInventory() {
			return inventory;
		}

		/// @brief Take (i.e. remove) an item from the inventory of the creature
		///
		/// @param item The item
		/// @param count The number of said items
		///
		/// @return Whether it is successful
		inline bool take(ItemType item, unsigned int count = 1) {
			return this->inventory.remove(item, count);
		}

		/// @brief Give (i.e. add) an item to the inventory of the creature
		///
		/// @param item The item
		/// @param count The number of said items
		///
		/// @return Whether the addition is successful
		inline bool give(ItemType item, unsigned int count = 1) {
			return this->inventory.add(item, count);
		}

};


#endif
