#ifndef SPRITES_H
#define SPRITES_H

#include <cstdint>
#include <map>

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

struct BackgroundProperties {
	bool passible;
	bool transparent;
};

/// @brief get the passiblity
const std::map<Background, BackgroundProperties> bkgrProps = {
	{Background::EMPTYNESS, {false, false}},
	{Background::TiledFloor, {true, true}},
	{Background::DirtWall, {false, false}},
	{Background::StoneWall, {false, false}},
	{Background::Door, {true, false}},
	{Background::TOTAL, {false, false}}
};

#endif
