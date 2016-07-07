#include "region.h"

Region::Region(int w, int h, int numconnections) {
	width = w;
	height = h;
	numConnections = numconnections;
	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			points[Point(x, y)] = Background::TiledFloor;
		}
		points[Point(x, -1)] = Background::StoneWall;
		points[Point(x, h)] = Background::StoneWall;
	}
	for (int y = -1; y <=h; y++) {
		points[Point(-1, y)] = Background::StoneWall;
		points[Point(w, y)] = Background::StoneWall;
	}
	
}
