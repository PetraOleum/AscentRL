#ifndef SPRITES_H
#define SPRITES_H

#include <cstdint>
#include <utility>
#include <map>
#include <string>

using Point = std::pair<int, int>;
class Creature;

/// @brief Absolute value
///
/// @param A The value to take the abs of 
///
/// @return The abs of the value
#define ABS(A) ((A < 0) ? (-(A)) : (A))


/// @brief Background sprites, in the order they appear on the sheet
enum class Background : uint8_t {
	EMPTYNESS = 0,
	TiledFloor,
	DirtWall,
	GemWall,
	DirtFloor,
	StoneWall,
	Door,
	MarkedDoor,
	/// @brief The total number of background sprites
	TOTAL
};

/// @brief Foreground sprites
enum class Foreground : uint8_t {
	NONE = 0,
	Witch,
	Gold,
	Staff,
	Chest,
	Rat,
	TOTAL
};

/// @brief Type of creature
enum class CreatureType : uint8_t {
	NONE,
	Witch,
	Rat
};

enum class ItemType : uint8_t {
	NONE,
	Gold,
	Staff,
	Chest
};

/// @brief Store item properties
struct ItemProperties {
	const char* name;
};

/// @brief Map item types to their properties
const std::map<ItemType, ItemProperties> itemProperties = {
	{ItemType::NONE, {"NONE"}},
	{ItemType::Gold, {"Gold"}},
	{ItemType::Staff, {"Staff"}},
	{ItemType::Chest, {"Chest"}}
};

/// @brief Properties of a background
struct BackgroundProperties {
	bool passible;
	bool transparent;
	bool placeable;
};

/// @brief Holds the properties of a background
const std::map<Background, BackgroundProperties> bkgrProps = {
	{Background::EMPTYNESS, {false, false, false}},
	{Background::TiledFloor, {true, true, true}},
	{Background::DirtWall, {false, false, false}},
	{Background::StoneWall, {false, false, false}},
	{Background::Door, {true, false, false}},
	{Background::MarkedDoor, {true, false, false}},
	{Background::TOTAL, {false, false, false}}
};

struct ForegroundProperties {
	const char* name;
};

const std::map<Foreground, ForegroundProperties> foreProps = {
	{Foreground::NONE, {"None"}},
	{Foreground::Witch, {"Witch"}},
	{Foreground::Gold, {"Gold"}},
	{Foreground::Staff, {"Staff"}},
	{Foreground::Chest, {"Chest"}},
	{Foreground::Rat, {"Rat"}},
	{Foreground::TOTAL, {"\"Total\" element"}}
};

/// @brief Enum to hold directions
enum class Direction : uint8_t {
	Up,
	Right,
	Left,
	Down,
	UpRight,
	DownRight,
	DownLeft,
	UpLeft,
	NONE
};

/// @brief Enum to hold direction types
enum class ActionType : uint8_t {
	Move,
	Attack,
	Pickup,
	Drop,
	NONE
};

/// @brief Get the "oppisite" direction to the one provided
///
/// @param direction A direction
///
/// @return The opposite direction, or Direction::Up if not a valid direction
inline Direction oppositeDirection(Direction direction) {
	switch (direction) {
		case Direction::Up:
			return Direction::Down;
		case Direction::Right:
			return Direction::Left;
		case Direction::Left:
			return Direction::Right;
		case Direction::Down:
			return Direction::Up;
		case Direction::UpRight:
			return Direction::DownLeft;
		case Direction::DownRight:
			return Direction::UpLeft;
		case Direction::DownLeft:
			return Direction::UpRight;
		case Direction::UpLeft:
			return Direction::DownRight;
		default:
			return Direction::NONE;
	}
}

/// @brief Map a direction to its displacement value
const std::map<Direction, Point> displacementMap = {
	{Direction::Up, {0, -1}},
	{Direction::Right, {1, 0}},
	{Direction::Left, {-1, 0}},
	{Direction::Down, {0, 1}},
	{Direction::UpLeft, {-1, -1}},
	{Direction::UpRight, {1, -1}},
	{Direction::DownRight, {1, 1}},
	{Direction::DownLeft, {-1, 1}},
	{Direction::NONE, {0,0}}
};

/// @brief Macro to get displacement
#define DISPLACEMENT(A) (displacementMap.at(A))

/// @brief Macro for adding pairs (of any type)
#define PAIR_SUM(A, B) (std::make_pair(A.first + B.first, A.second + B.second))

#define PAIR_SUBTRACT(A, B) (std::make_pair(A.first - B.first, A.second - B.second))

#define PAIR_MULTIPLY(A, B) (std::make_pair(A.first * B.first, A.second * B.second))

/// @brief Swap first and second parts of A
#define PAIR_SWIVEL(A) (std::make_pair(A.second, A.first))


/// @brief Properties of a creature type
struct creatureProperties {
	/// @brief Corresponding foreground of creature
	Foreground foreground;
	/// @brief Speed of creature
	double speed;
	/// @brief Base HP of creature
	int HP;
	/// @brief addition to die roll of attack
	int baseAttack;
	/// @brief Dice to roll for attack e.g. d4
	int attackDice;
	/// @brief bonus to-hit
	int hitBonus;
	/// @brief AC of creature
	int AC;
	/// @brief Probability of regenerating a hitpoint each turn
	double regen;
};

/// @brief CreatureType to creatureProperties map
const std::map<CreatureType, creatureProperties> creaturePropertiesMap = {
	{CreatureType::NONE, {
				     Foreground::NONE,
				     0.0,
				     0,
				     0,
				     0,
				     0,
				     0,
				     0
			     }},
	{CreatureType::Witch, {
				      Foreground::Witch,
				      1.0,
				      20,
				      2,
				      6,
				      4,
				      14,
				      0.2
			      }},
	{CreatureType::Rat, {
				    Foreground::Rat,
				    1.0,
				    5,
				    0,
				    4,
				    0,
				    10,
				    0.1
			    }}
};

/// @brief Get the foreground corresponding to the creature
///
/// @param creature The creature
///
/// @return The corresponding Foreground (Foreground::NONE if not defined)
inline Foreground getCreatureForeground(CreatureType creature) {
	auto it = creaturePropertiesMap.find(creature);
	if (it == creaturePropertiesMap.end())
		return Foreground::NONE;
	return it->second.foreground;
}

/// @brief Get the default properties of the creature
///
/// @param creature The creature
///
/// @return The properties
inline creatureProperties getCreatureProperties(CreatureType creature) {
	auto it = creaturePropertiesMap.find(creature);
	if (it == creaturePropertiesMap.end())
		creaturePropertiesMap.at(CreatureType::NONE);
	return it->second;
}

Foreground getCreaturePointerForeground(Creature * creature);

/// @brief Map foregrounds back to creature type
const std::map<Foreground, CreatureType> foregroundCreatures = {
	{Foreground::Witch, CreatureType::Witch},
	{Foreground::Rat, CreatureType::Rat}
};

/// @brief Get the creature corresponding to the foreground
///
/// @param foreground The foreground
///
/// @return The corresponding CreatureType (CreatureType::NONE if not defined)
inline CreatureType getForegroundCreature(Foreground foreground) {
	auto it = foregroundCreatures.find(foreground);
	if (it == foregroundCreatures.end())
		return CreatureType::NONE;
	return it->second;
}

const std::map<ItemType, Foreground> itemForegrounds = {
	{ItemType::Gold, Foreground::Gold},
	{ItemType::Staff, Foreground::Staff},
	{ItemType::Chest, Foreground::Chest},
	{ItemType::NONE, Foreground::NONE}
};

inline Foreground getItemForeground(ItemType type) {
	auto it = itemForegrounds.find(type);
	if (it == itemForegrounds.end())
		return Foreground::NONE;
	return it->second;
}
/// @brief Enum that describes whether the creatures on the same team
enum class Team : uint8_t {
	/// @brief Default
	NONE,
	/// @brief The player, and any allied monsters
	Player,
	/// @brief The monsters
	Monsters
};

/// @brief Hold the Visibility information + foreground/background of a square and other stuff to pass to displaying function
struct Visibility {
	/// @brief Whether the square is visible
	bool visible;
	/// @brief Value of the background
	Background background;
	/// @brief Value of the foreground
	Foreground foreground;
	/// @brief The type of creature here
	CreatureType creature;
	/// @brief The team of the creature
	Team team;
	/// @brief The type of item here
	ItemType item;
	/// @brief The hp proportion (0 to 1) of the creature here
	double creatureHP;
};

/// @brief All the information about an action
struct Action {
	/// @brief The type of action
	ActionType type;
	/// @brief The direction
	Direction direction;
	/// @brief The item in the inventory
	char inventoryItem;
};


#endif
