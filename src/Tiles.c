#include "Tiles.h"

void init_map(struct Map *map, coor width, coor height) {
	map->width = width;
	map->height = height;
	map->tiles = malloc((map->width * map->height) * sizeof(struct Tile));
	
	struct Tile *currentTile;
	// Set all of the default values.
	for (int i = 0; i < map->height * map->height; i++) {
		currentTile = get_tile(map, i % map->width, i / map->width);
		currentTile->x = i % map->width;
		currentTile->y = i / map->width;
		currentTile->type = BLANK;
		currentTile->direction = UP;
	}
}

void free_map(struct Map *map) {
	free(map->tiles);
}

struct Tile *get_tile(struct Map *map, coor x, coor y) {
	// Pointer magic :D
	return map->tiles + ((y * map->width) + x);
}

