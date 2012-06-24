/*

TODO work on the random map generator

How will the generator ... well... generate? How will the game even work?

The user will have to connect wires to all of the points.
	+ user level = amount of time
	+ bot level = amount of points

The map should always be the same size... maybe
 + Yeah I think that would be good. Something like 50x50?

Algorithm
1. Generate the starting point at random. 
2. Generate all of the points at random at LEAST 2 spaces away from anything else
3. Gerenate paths of pipes to be able to connect to each point
	+ It will go in a random direction, but if you look at the big picture... it will end up going to the point.
	+ How it will do this is by generating, first, a random amount of points in between the end at the start. These could be anywhere, really.
	+ Next, it will 'play connect the dots' with these, until it connects to the end point
	+ There should also be some pipes that randomly decide to merge, and then cut off later.
		+ How this could work is.. if a pipe is on its way to it's next random point, but it runs into another pipe, it shares that pipe for a random amount of turns, and then breaks off to continue (and generate new random points)
	+ If there is any other pipes in the way, turn the pipe that is in the way into a T or cross pipe
		+ Other morphing of the paths could be done as well
4. Place random pipes all over the rest of the screen
5. Randomize the angles of everything

While generating, there will be arrays containing all of the end points, all of the pipes, etc. BUT the actual map will just be one array clumping them all together

*/

// Comment this out for release
#define __DEBUG__

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_rotozoom.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Distance between each node
#define DISTANCE 2
#define MAP_WIDTH 30
#define MAP_HEIGHT 30

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
	UP,
	DOWN,
	LEFT,
	RIGHT,
};

struct Tile {
	coor x;
	coor y;
	short type;
	short direction;
};

struct Tile* map;
void init_map();
void free_map();
struct Tile* get_tile(coor x, coor y);
void generate_map(int points);

int get_next_tile(struct Tile* start, struct Tile* end, struct Tile* current);
int get_tile_direction(struct Tile* comparer, struct Tile* comparee);
// What is firstMapPtr? It is a pointer to the first element in the array. TODO explain more 
void draw_tile(struct Tile* tile);

void load_images();
void free_images();

#ifdef __DEBUG__
void image_draw_test();
#endif

SDL_Surface* screen = NULL;
SDL_Surface* tileVertical = NULL;
SDL_Surface* tileHorizontal = NULL;
SDL_Surface* tileStart = NULL;
SDL_Surface* tileEnd = NULL;
SDL_Surface* tileCross = NULL;
SDL_Surface* tileIntersectUp = NULL;
SDL_Surface* tileIntersectDown = NULL;
SDL_Surface* tileIntersectLeft = NULL;
SDL_Surface* tileIntersectRight = NULL;
SDL_Surface* tileTurnDownRight = NULL;
SDL_Surface* tileTurnDownLeft = NULL;
SDL_Surface* tileTurnUpLeft = NULL;
SDL_Surface* tileTurnUpRight = NULL;
// This will not be needed for the final game, but is just here to occupy the blank tiles in the mean time
SDL_Surface* tileBlank = NULL;

SDL_Event event;

int main(int argc, char* args[]) {
	srand(time(NULL));
	init_map();
	generate_map(1);

	SDL_Init(SDL_INIT_EVERYTHING);
	screen = SDL_SetVideoMode(20*MAP_WIDTH, 20*MAP_HEIGHT, 32, SDL_SWSURFACE);

	load_images();

	short quit = 0;

	// Main loop
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_RETURN) {
				// Make the main loop run one more time, and then stop.
				quit = 1;
			}
		}
		for (int i = 0; i < MAP_WIDTH; i++) {
			for (int j = 0; j < MAP_HEIGHT; j++) {
				draw_tile(get_tile(i, j));
			}
		}
		// Update Screen
		SDL_Flip(screen);
	}
	free_images();
	free_map();
	SDL_Quit();

	return 0;
}

void load_images() {
	SDL_Surface* tileStraight = IMG_Load("graphics/pipe_minigame/straight.png");
	SDL_Surface* tileTurn = IMG_Load("graphics/pipe_minigame/turn.png");
	SDL_Surface* tileIntersect = IMG_Load("graphics/pipe_minigame/intersect.png");

	// We have to perform a 0 degree rotozoom on some of these so then we can free the initial images... it is basically cloning the image onto them

	tileVertical = rotozoomSurface(tileStraight, 0, 1, 0);
	tileHorizontal = rotozoomSurface(tileStraight, 90, 1, 0);
	
	tileIntersectRight = rotozoomSurface(tileIntersect, 0, 1, 0);
	tileIntersectDown = rotozoomSurface(tileIntersect, 90, 1, 0);
	tileIntersectLeft = rotozoomSurface(tileIntersect, 180, 1, 0);
	tileIntersectUp = rotozoomSurface(tileIntersect, 270, 1, 0);
	
	tileTurnDownRight = rotozoomSurface(tileTurn, 0, 1, 0);
	tileTurnUpRight = rotozoomSurface(tileTurn, 90, 1, 0);
	tileTurnUpLeft = rotozoomSurface(tileTurn, 180, 1, 0);
	tileTurnDownLeft = rotozoomSurface(tileTurn, 270, 1, 0);

	// These images do not need to be rotated into another variable, so we just load them directly
	tileCross = IMG_Load("graphics/pipe_minigame/cross.png");
	tileStart = IMG_Load("graphics/pipe_minigame/start.png");
	tileEnd = IMG_Load("graphics/pipe_minigame/end.png");

	SDL_FreeSurface(tileStraight);
	SDL_FreeSurface(tileTurn);
	SDL_FreeSurface(tileIntersect);
}

void free_images() {
	SDL_FreeSurface(tileVertical);
	SDL_FreeSurface(tileHorizontal);
	SDL_FreeSurface(tileStart);
	SDL_FreeSurface(tileEnd);
	SDL_FreeSurface(tileCross);
	SDL_FreeSurface(tileIntersectUp);
	SDL_FreeSurface(tileIntersectDown);
	SDL_FreeSurface(tileIntersectLeft);
	SDL_FreeSurface(tileIntersectRight);
	SDL_FreeSurface(tileTurnDownRight);
	SDL_FreeSurface(tileTurnDownLeft);
	SDL_FreeSurface(tileTurnUpLeft);
	SDL_FreeSurface(tileTurnUpRight);
}

#ifdef __DEBUG__

// This is just a little function to test if all of the sprites are loading properly. It will be removed soon.
void image_draw_test() {
	{
		SDL_Rect tmprect = {0, 0, 20, 20};
		SDL_BlitSurface(tileHorizontal, NULL, screen, &tmprect);
	}
	{
		SDL_Rect tmprect = {20, 0, 20, 20};
		SDL_BlitSurface(tileVertical, NULL, screen, &tmprect);
	}
	{
		SDL_Rect tmprect = {40, 0, 20, 20};
		SDL_BlitSurface(tileStart, NULL, screen, &tmprect);
	}
	{
		SDL_Rect tmprect = {60, 0, 20, 20};
		SDL_BlitSurface(tileEnd, NULL, screen, &tmprect);
	}
	{
		SDL_Rect tmprect = {80, 0, 20, 20};
		SDL_BlitSurface(tileCross, NULL, screen, &tmprect);
	}
	{
		SDL_Rect tmprect = {100, 0, 20, 20};
		SDL_BlitSurface(tileIntersectUp, NULL, screen, &tmprect);
	}
	{
		SDL_Rect tmprect = {0, 20, 20, 20};
		SDL_BlitSurface(tileIntersectRight, NULL, screen, &tmprect);
	}
	{
		SDL_Rect tmprect = {20, 20, 20, 20};
		SDL_BlitSurface(tileIntersectDown, NULL, screen, &tmprect);
	}
	{
		SDL_Rect tmprect = {40, 20, 20, 20};
		SDL_BlitSurface(tileIntersectLeft, NULL, screen, &tmprect);
	}
	{
		SDL_Rect tmprect = {60, 20, 20, 20};
		SDL_BlitSurface(tileTurnDownRight, NULL, screen, &tmprect);
	}
	{
		SDL_Rect tmprect = {80, 20, 20, 20};
		SDL_BlitSurface(tileTurnDownLeft, NULL, screen, &tmprect);
	}
	{
		SDL_Rect tmprect = {100, 20, 20, 20};
		SDL_BlitSurface(tileTurnUpLeft, NULL, screen, &tmprect);
	}
	{
		SDL_Rect tmprect = {0, 40, 20, 20};
		SDL_BlitSurface(tileTurnUpRight, NULL, screen, &tmprect);
	}
}

#endif

// Gets where the next point should be to complete a path
int get_next_tile(struct Tile* start, struct Tile* end, struct Tile *current) {

	/*
	TODO
	
	This needs to check the map to make sure that it stays away from anything important
	It would also be good to generate a random number of places to go in a specific direction so that the paths are not so predictable.
	 + Right now, the paths are either a straight line or diagonal. Why not spice it up a bit?

	*/

	short xDistance = (current->x > end->x) ? current->x - end->x : end->x - current->x;
	short yDistance = (current->y > end->y) ? current->y - end->y : end->y - current->y;
	
	// If the point is at the destination
	if (xDistance == 0 && yDistance == 0) {
		return 0;
	}

	if (xDistance > yDistance) {
		if (current->x > end->x) {
			current->x--;
		} else {
			current->x++;
		}
	} else {
		if (current->y > end->y) {
			current->y--;
		} else {
			current->y++;
		}
	}
	return 1;
}

// We use firstMapPtr to access the actual array (the map array in the main loop)
void generate_map(int points) {
	unsigned short randX, randY;
	randX = rand() % (MAP_WIDTH - 1);
	randY = rand() % (MAP_HEIGHT - 1);

	struct Tile* start = get_tile(randX, randY);

	start->type = START;
	start->x = randX;
	start->y = randY;
	start->direction = UP;
	
	// Make sure that the point is at least DISTANCE places away from the starting point
	while (!(randY < start->y - DISTANCE || randX < start->x - DISTANCE || randY > start->y + DISTANCE || randX > start->x + DISTANCE)) {
		randX = rand() % (MAP_WIDTH - 1);
		randY = rand() % (MAP_HEIGHT - 1);
	}

	struct Tile* end = get_tile(randX, randY);

	end->type = END;
	end->x = randX;
	end->y = randY;
	end->direction = UP;

	// This is not a real THANG
	struct Tile current;
	current      = *start;
	current.type = PIPE_STRAIGHT;
	struct Tile randompoint;

	// Generate a random amount of points for the pipes to go to
	for (short i = (rand() % 3); i > 0; i--) {
		randompoint.x = rand() % (MAP_WIDTH - 1);
		randompoint.y = rand() % (MAP_HEIGHT - 1);
		
		while (get_next_tile(start, &randompoint, &current)) {
			get_tile(current.x, current.y)->type = PIPE_STRAIGHT;
			get_tile(current.x, current.y)->x = current.x;
			get_tile(current.x, current.y)->y = current.y;
			get_tile(current.x, current.y)->direction = UP;
		}
	}
	// Just to check which direction the next point should be
	struct Tile lasttile = *start;
	struct Tile currenttile;
	while (get_next_tile(start, end, &current)) {
		// Make sure that the pipe doesn't overlap the end point!
		if (current.x == end->x && current.y == end->y) break;

		currenttile.x = current.x;
		currenttile.y = current.y;

		// FIXME this just needs to compare the current tile from the last tile to get the proper direction
		//  ++++ TODO make this do something other than PIPE_STRAIGHT
		get_tile(current.x, current.y)->type = PIPE_STRAIGHT;
		get_tile(current.x, current.y)->x = current.x;
		get_tile(current.x, current.y)->y = current.y;
		get_tile(current.x, current.y)->direction = UP;

		lasttile.x = current.x;
		lasttile.y = current.y;
		lasttile.type = get_tile(current.x, current.y)->type;
	}
}

// This function is meant to only get the direction of an adjecent tile
int get_tile_direction(struct Tile* comparer, struct Tile* comparee) {
	if (comparer->x > comparee->x) {
		return LEFT;
	} else if (comparer->x < comparee->x) {
		return RIGHT;
	} else if (comparer->y > comparee->y) {
		return UP;
	} else if (comparer->y < comparee->y) {
		return DOWN;
	} else {
		// FIXME there should probably be a 'stationary' thing as well
		return UP;
	}
}

void draw_tile(struct Tile* tile) {
	SDL_Rect tmprect = {tile->x*20, tile->y*20, 20, 20};
	switch (tile->type) {
		case START:
			SDL_BlitSurface(tileStart, NULL, screen, &tmprect);
		break;
		case END:
			SDL_BlitSurface(tileEnd, NULL, screen, &tmprect);
		break;
		case PIPE_STRAIGHT:
			SDL_BlitSurface(tileVertical, NULL, screen, &tmprect);
		break;
		default:
			SDL_BlitSurface(tileBlank, NULL, screen, &tmprect);
		break;
	}
}

void init_map() {
	map = malloc((MAP_WIDTH * MAP_HEIGHT) * sizeof(struct Tile));
}

void free_map() {
	free(map);
}

struct Tile* get_tile(coor x, coor y) {
	// Pointer magic :D
	return map + ((y * MAP_WIDTH) + x);
}

