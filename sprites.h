#ifndef SPRITES_H
#define SPRITES_H

#include <cstdint>



/// @brief Background sprites, in the order they appear on the sheet
enum class Background : uint8_t {
	TiledFloor = 0,
	DirtWall,
	GemWall,
	DirtFloor,
	StoneWall,
	/// @brief The total number of background sprites
	TOTAL
};



#endif
