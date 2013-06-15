#ifndef __TILES_H__
#define __TILES_H__

#include <stdint.h>
#include <stdlib.h>

// coordinate. (x and y position) data type
#define coor uint8_t

enum TILE_TYPES {
	BLANK,
	START,
	PIPE_STRAIGHT,
	PIPE_TURN,
	PIPE_INTERSECT,
	PIPE_CROSS,
	END,
};

enum DIRECTIONS {
	UP = 0,
	DOWN = 1,
	LEFT = 2,
	RIGHT = 3,
};

struct Tile {
	coor x;
	coor y;
	short type;
	short direction;
};

struct Map {
	struct Tile* tiles;
	coor height, width;
};

void init_map(struct Map *map, coor width, coor height);
void free_map(struct Map *map);

struct Tile *get_tile(struct Map *map, coor x, coor y);

#endif

