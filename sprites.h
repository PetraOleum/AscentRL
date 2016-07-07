#ifndef SPRITES_H
#define SPRITES_H

#include <cstdint>
#include <map>


/// @brief Background sprites, in the order they appear on the sheet
enum class Background : uint8_t {
	EMPTYNESS = 0,
	TiledFloor,
	DirtWall,
	GemWall,
	DirtFloor,
	StoneWall,
	Door,
	/// @brief The total number of background sprites
	TOTAL
};

/// @brief Foreground sprites
enum class Foreground : uint8_t {
	NONE = 0,
	Witch,
	Gold,
	TOTAL
};

/// @brief get the passiblity
const std::map<Background, bool> passiblity = {
	{Background::EMPTYNESS, false},
	{Background::TiledFloor, true},
	{Background::DirtWall, false},
	{Background::StoneWall, false},
	{Background::Door, true},
	{Background::TOTAL, false}
};

#endif
