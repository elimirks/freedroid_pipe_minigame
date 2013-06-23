#ifndef __TILES_H__
#define __TILES_H__

#include <stdint.h>
#include <stdlib.h>

// coordinate. (x and y position) data type
#define coor uint8_t

// Different tile types.
enum TILE_TYPES {
	BLANK = 0,
	START,
	PIPE_STRAIGHT,
	PIPE_TURN,
	PIPE_INTERSECT,
	PIPE_CROSS,
	END,
};

// Possible directions for a tile to be facing.
enum DIRECTIONS {
	UP    = 1 << 0,
	DOWN  = 1 << 1,
	LEFT  = 1 << 2,
	RIGHT = 1 << 3
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

int manhattan_tile_distance(struct Tile *tileA, struct Tile *tileB);

void generate_map(struct Map *map, int points);

#endif

