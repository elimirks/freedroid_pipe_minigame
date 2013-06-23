#include <stdio.h>

// Minimum distance between each major node
#define DISTANCE 2

#include "Tiles.h"

// Some structs used in the pathfinding algorithm.
// Used for a linked list, icydk.
struct TileGroup {
	struct Tile *tile;
	struct TileGroup *nextTile;
};

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

int manhattan_tile_distance(struct Tile* tileA, struct Tile* tileB) {
	int xDistance = tileA->x > tileB->x ? tileA->x - tileB->x : tileB->x - tileA->x;
	int yDistance = tileA->y > tileB->y ? tileA->y - tileB->y : tileB->y - tileA->y;
	return xDistance + yDistance;
}

void generate_random_end_point(struct Map *map, struct Tile* start) {
	coor randX = rand() % (map->width - 1);
	coor randY = rand() % (map->height - 1);
	struct Tile* endTile;
	
	do {
		while (!(randY < start->y - DISTANCE || randX < start->x - DISTANCE || randY > start->y + DISTANCE || randX > start->x + DISTANCE)) {
			randX = rand() % (map->width - 1);
			randY = rand() % (map->height - 1);
		}
		endTile = get_tile(map, randX, randY);
	// Make sure the selected tile does not have another tile type set already.
	} while (endTile->type != BLANK);
	
	endTile->type = END;
	endTile->direction = UP;
}

/*
Possible Orientations For Successor: struct Tile *succ from struct Tile *parent.
return UP | RIGHT, etc.
done
*/

int get_possible_successor_orientations(struct Tile *succ, int coming_from_direction) {
	switch (succ->type) {
		case PIPE_STRAIGHT:
			if (coming_from_direction == LEFT || coming_from_direction == RIGHT) {
				// This tile is the same from both right and left, same with up and down.
				return RIGHT;
			} else {
				return UP;
			}
		break;
		case PIPE_TURN:
			if (coming_from_direction == UP) {
				return RIGHT | DOWN;
			} else if (coming_from_direction == DOWN) {
				return LEFT | UP;
			} else if (coming_from_direction == RIGHT) {
				return LEFT | DOWN;
			} else if (coming_from_direction == LEFT) {
				return RIGHT | UP;
			}
		case PIPE_INTERSECT:
			if (coming_from_direction == UP) {
				return RIGHT | LEFT | DOWN;
			} else if (coming_from_direction == DOWN) {
				return RIGHT | LEFT | UP;
			} else if (coming_from_direction == RIGHT) {
				return DOWN | LEFT | UP;
			} else if (coming_from_direction == LEFT) {
				return DOWN | RIGHT | UP;
			}
		break;
		// By default. This will match the cross intersection, start, end points, etc.
		// Basically, tiles which could be accessed from all of the sides.
		default:
			return UP;
	}
}

// TODO create a method that will get the successors of a tile depending on the map.
// It will take into account different tile orientations. (with exceptions of straight tiles)

// NOTE!! You are reponsible for this tile group!
// This method will override the tile group without caring about deallocating.
// 'group' is a pointer to a the successor tile group object.
void get_successors(struct Map *map, struct TileGroup *group, struct Tile *tile) {
	// Check only certain tiles depending on the type and orientation.
	switch (tile->type) {
		// With both of these cases, we could go in any direction.
		case START:
		case PIPE_CROSS:
			// Ensure the map bounds.
			if (tile->x - 1 >= 0) { // TODO get the tile that is here, and make comparisons on how it could connect.
				// Left.
			}
			if (tile->x + 1 < map->width) {
				// Right.
			}
			if (tile->y - 1 >= 0) {
				// Up.
			}
			if (tile->y + 1 < map->height) {
				// Down.
			}
		break;
		case PIPE_STRAIGHT:
		break;
		case PIPE_TURN:
		break;
		case PIPE_INTERSECT:
		break;
	}
}

// Checks a TileGroup to see if a Tile is contained in that group.
// Returns 0 if false, 1 if true, -1 if error. (tile is null, for example)
int tile_is_in_group(struct TileGroup *path, struct Tile *tile) {
	if (tile == NULL) return -1;
	
	struct Tile *currentTile;
	
	// Loop until null.
	while (currentTile = path->tile) {
		// For now we don't care about the tile type.
		if (currentTile->x == tile->x && currentTile->y == tile->y && currentTile->direction == tile->direction) {
			return 1;
		}
	}
	
	return 0;
}

// Frees all of the TileGroup objects in a list. 
// Use the should_free_tiles parameter to choose weather to free the contained tiles as well.
void free_tile_group(struct TileGroup *group, int should_free_tiles) {
	struct TileGroup *nextGroup;
	
	do {
		nextGroup = group->nextTile;
		if (should_free_tiles) {
			free(group->tile);
		}
		free(group);
	// Loop until the next group is null.
	} while (group = nextGroup);
}

void generate_map(struct Map *map, int points) {
	unsigned short randX, randY;
	randX = rand() % (map->width - 1);
	randY = rand() % (map->height - 1);
	
	struct Tile* start = get_tile(map, randX, randY);
	
	start->type = START;
	start->direction = UP;
	
	// Generate all of the end points.
	while (points-- > 0) {
		generate_random_end_point(map, start);
	}
	
	// Generate random pipes all over the uncovered spaces.
	
	coor currentX, currentY;
	struct Tile* currentTilePointer;
	
	for (int i = 0; i < map->width * map->height; i++) {
		currentX = i % map->width, currentY = i / map->width;
		currentTilePointer = get_tile(map, currentX, currentY);
		
		if (currentTilePointer->type == BLANK) {
			int tileChoice = rand() % 9;
			// Generate straight and turned pipes more often.
			if (tileChoice >= 0 && tileChoice <= 2) {
				currentTilePointer->type = PIPE_STRAIGHT;
			} else if (tileChoice >= 3 && tileChoice <= 5) {
				currentTilePointer->type = PIPE_TURN;
			} else if (tileChoice == 6 || tileChoice == 7) {
				currentTilePointer->type = PIPE_INTERSECT;
			} else if (tileChoice == 8) {
				currentTilePointer->type = PIPE_CROSS;
			}
			currentTilePointer->direction = 1 << rand() % 4;
		}
	}
	
	// TODO builds teh fs function!!! And then DFS!!! Good old times... :)
	
	// TODO Try to find a path to each end point.
	// We should be keeping track of each end point.
	
	// Use A* search to find the end point.
	// Woo AI algorithms!
}

/*

Pathfinding algorithm:

Use a UCS priority queue method using the amount of tiles in the path as the cost.

We will have to count the directions of the tiles as different paths as well.

*/

