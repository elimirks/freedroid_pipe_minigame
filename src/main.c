#include "SDL.h"
#include "SDL_image.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "ponies.h"

// Distance between each node
#define DISTANCE 2

// Tile types
enum {
	BLANK,
	START,
	PIPE,
	END,
};

struct Point {
	unsigned short x;
	unsigned short y;
	int type;
};

int get_next_point(struct Point start, struct Point end, struct Point *current);

// What is firstMapPtr? It is a pointer to the first element in the array. TODO explain more 
void generate_map(int points, unsigned short* firstMapPtr);
void draw_pipe(int x, int y, int type);

SDL_Surface* screen = NULL;
SDL_Event event;

const unsigned short width = 30, height = 30;

int main(int argc, char* args[]) {

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

	srand(time(NULL));

	SDL_Init(SDL_INIT_EVERYTHING);
	screen = SDL_SetVideoMode(20*30, 20*30, 32, SDL_SWSURFACE);
	
	// For some reason if you use width and height as the initialization size, it messes stuff up.
	unsigned short map[30][30] = {BLANK};
	generate_map(1, &map[0][0]);

	short quit = 0;

	// Main loop
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.key.keysym.sym == SDLK_ESCAPE || event.key.keysym.sym == SDLK_RETURN) {
				// Make the main loop run one more time, and then stop.
				quit = 1;
			}
		}
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				draw_pipe(i, j, map[i][j]);
			}
		}
		// Update Screen
		SDL_Flip(screen);
	}
	SDL_Quit();

	return 0;
}

// Gets where the next point should be to complete a path
int get_next_point(struct Point start, struct Point end, struct Point *current) {

	/*
	TODO
	
	This needs to check the array of points to make sure that it stays away from anything important
	It would also be good to generate a random number of places to go in a specific direction so that the paths are not so predictable.
	 + Right now, the paths are either a straight line or diagonal. Why not spice it up a bit?

	*/

	short xDistance = (current->x > end.x) ? current->x - end.x : end.x - current->x;
	short yDistance = (current->y > end.y) ? current->y - end.y : end.y - current->y;
	
	// If the point is at the destination
	if (xDistance == 0 && yDistance == 0) {
		return 0;
	}

	if (xDistance > yDistance) {
		if (current->x > end.x) {
			current->x--;
		} else {
			current->x++;
		}
	} else {
		if (current->y > end.y) {
			current->y--;
		} else {
			current->y++;
		}
	}
	return 1;
}

// We use firstMapPtr to access the actual array (the map array in the main loop)
void generate_map(int points, unsigned short* firstMapPtr) {

	// FIXME
	unsigned short map[30][30] = {BLANK};

	unsigned short randX, randY;
	randX = rand() % (width - 1);
	randY = rand() % (height - 1);
	map[randX][randY] = START;
	
	// FIXME a better way to separate everything would be to use an array of structs with x and y coordinates
	int startX = randX, startY = randY;
	
	// Make sure that the point is at least 2 places away from the starting point
	while (!(randY < startY - DISTANCE || randX < startX - DISTANCE || randY > startY + DISTANCE || randX > startX + DISTANCE)) {
		randX = rand() % (width - 1);
		randY = rand() % (height - 1);
	}

	map[randX][randY] = END;

	struct Point start = {
		startX,
		startY,
		START,
	};
	struct Point end = {
		randX,
		randY,
		END,
	};
	struct Point current;
	current      = start;
	current.type = PIPE;
	struct Point randompoint;

	// Generate a random amount of points for the pipes to go to
	// FIXME this should do rand() % 3, I just put it at 1 for testing
	for (short i = (rand() % 1); i > 0; i--) {
		randompoint.x = rand() % (width - 1);
		randompoint.y = rand() % (height - 1);
		
		while (get_next_point(start, randompoint, &current)) {
			map[current.x][current.y] = PIPE;
		}
	}
	while (get_next_point(start, end, &current)) {
		// Make sure that the pipe doesn't overlap the end point!
		if (current.x == end.x && current.y == end.y) break;
		map[current.x][current.y] = PIPE;
	}
	// Write to the actual array
	for (unsigned short i = 0; i < 30; i++) {
		for (unsigned short j = 0; j < 30; j++) {
			// Keep adding onto the memory address to sift through the whole array
			*firstMapPtr++ = map[i][j];
		}
	}
}

void draw_pipe(int x, int y, int type) {
	Uint32 color;
	SDL_Rect sqr = {x*20, y*20, 20, 20};
	switch (type) {
		case START:
			color = SDL_MapRGB(screen->format, 0, 255, 0);
			SDL_FillRect(screen, &sqr, color);
		break;
		case END:
			color = SDL_MapRGB(screen->format, 255, 0, 0);
			SDL_FillRect(screen, &sqr, color);
		break;
		case PIPE:
			color = SDL_MapRGB(screen->format, 255, 255, 255);
			SDL_FillRect(screen, &sqr, color);
		break;
		default:
			color = SDL_MapRGB(screen->format, 0, 0, 0);
			SDL_FillRect(screen, &sqr, color);
		break;
	}
}

