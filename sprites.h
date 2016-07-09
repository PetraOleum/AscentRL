#ifndef SPRITES_H
#define SPRITES_H

#include <cstdint>
#include <map>

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

/// @brief Properties of a background
struct BackgroundProperties {
	bool passible;
	bool transparent;
};

/// @brief Holds the properties of a background
const std::map<Background, BackgroundProperties> bkgrProps = {
	{Background::EMPTYNESS, {false, false}},
	{Background::TiledFloor, {true, true}},
	{Background::DirtWall, {false, false}},
	{Background::StoneWall, {false, false}},
	{Background::Door, {true, false}},
	{Background::TOTAL, {false, false}}
};

#endif
