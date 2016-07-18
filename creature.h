#ifndef CREATURE_H
#define CREATURE_H

#include "general.h"
#include "region.h"
#include <map>

/// @brief Type of creature
enum class CreatureType : uint8_t {
	NONE,
	Witch,
	Rat
};

/// @brief Map creature types to Foregrounds
const std::map<CreatureType, Foreground> creatureForegrounds = {
	{CreatureType::NONE, Foreground::NONE},
	{CreatureType::Witch, Foreground::Witch},
};

/// @brief Get the foreground corresponding to the creature
///
/// @param creature The creature
///
/// @return The corresponding Foreground (Foreground::NONE if not defined)
inline Foreground getCreatureForeground(CreatureType creature) {
	auto it = creatureForegrounds.find(creature);
	if (it == creatureForegrounds.end())
		return Foreground::NONE;
	return it->second;
}

/// @brief Map foregrounds back to creature type
const std::map<Foreground, CreatureType> foregroundCreatures = {
	{Foreground::Witch, CreatureType::Witch}
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

/// @brief Class that holds a creature; use as defined by new
class Creature {
	private:
		/// @brief The position of the creature within the region
		Point position;
		/// @brief The region that the creature is in
		Region * region;
		/// @brief The type of creature that the creature is
		CreatureType type;

	public:
		/// @brief Constructor, specifying starting position, region, type
		///
		/// @param position The starting position
		/// @param region Pointer to the starting region
		/// @param type The type of creature
		Creature(Point position, Region * region, CreatureType type);

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
			return (position = PAIR_SUM(position, delta));
		}

		/// @brief Get the sprite (i.e. foreground) corresponding to the creature type
		///
		/// @return The foreground
		inline Foreground Sprite() const {
			return getCreatureForeground(type);
		}

};

#endif
