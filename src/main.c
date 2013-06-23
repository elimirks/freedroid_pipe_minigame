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


New algo!
Use a CSP.

Generate the ENTIRE map completely randomly, with random tiles everywhere. After placing them all, run the CSP over and start changing tiles.

Algo:
1. Generate the starting point at random.
2. Generate several end points.
3. Place random pipes all over the map.
4. Try to draw paths to each end point.
 - If it is impossible to connect to an end point, change some tiles to allow it.
5. Randomize the orientation of all of the pipes.

*/

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_rotozoom.h"
#include <stdio.h>
#include <time.h>

#include "Tiles.h"

struct Map map;

void draw_tile(struct Tile *tile);

void load_images();
void free_images();

void image_draw_test();

SDL_Surface *screen;
SDL_Surface *tileVertical, *tileHorizontal, *tileStart, *tileEnd, *tileCross;
SDL_Surface *tileIntersectUp, *tileIntersectDown, *tileIntersectLeft, *tileIntersectRight;
SDL_Surface *tileTurnDownRight, *tileTurnDownLeft, *tileTurnUpLeft, *tileTurnUpRight;
// This will not be needed for the final game, but is just here to occupy the blank tiles in the mean time
SDL_Surface* tileBlank = NULL;

SDL_Event event;

int main(int argc, char* args[]) {
	srand(time(NULL));
	init_map(&map, 20, 20);
	generate_map(&map, 1);
	
	SDL_Init(SDL_INIT_EVERYTHING);
	screen = SDL_SetVideoMode(20 * map.width, 20 * map.height, 32, SDL_SWSURFACE);
	
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
		for (int i = 0; i < map.width * map.height; i++) {
			//draw_tile(get_tile(&map, i % map.width, i / map.width));
		}
		image_draw_test();
		// Update Screen
		SDL_Flip(screen);
	}
	free_images();
	free_map(&map);
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

void draw_tile(struct Tile* tile) {
	SDL_Rect tmprect = {tile->x*20, tile->y*20, 20, 20};
	int dir = tile->direction;
	switch (tile->type) {
		case START:
			SDL_BlitSurface(tileStart, NULL, screen, &tmprect);
		break;
		case END:
			SDL_BlitSurface(tileEnd, NULL, screen, &tmprect);
		break;
		case PIPE_STRAIGHT:
			if (dir & (RIGHT | LEFT)) {
				SDL_BlitSurface(tileHorizontal, NULL, screen, &tmprect);
			} else if (dir & (UP | DOWN)) {
				SDL_BlitSurface(tileVertical, NULL, screen, &tmprect);
			}
		break;
		case PIPE_TURN:
			if (dir & RIGHT) {
				SDL_BlitSurface(tileTurnDownRight, NULL, screen, &tmprect);
			} else if (dir & UP) {
				SDL_BlitSurface(tileTurnUpRight, NULL, screen, &tmprect);
			} else if (dir & LEFT) {
				SDL_BlitSurface(tileTurnUpLeft, NULL, screen, &tmprect);
			} else if (dir & DOWN) {
				SDL_BlitSurface(tileTurnDownLeft, NULL, screen, &tmprect);
			}
		break;
		case PIPE_CROSS:
			SDL_BlitSurface(tileCross, NULL, screen, &tmprect);
		break;
		case PIPE_INTERSECT:
			if (dir & RIGHT) {
				SDL_BlitSurface(tileIntersectRight, NULL, screen, &tmprect);
			} else if (dir & UP) {
				SDL_BlitSurface(tileIntersectUp, NULL, screen, &tmprect);
			} else if (dir & LEFT) {
				SDL_BlitSurface(tileIntersectLeft, NULL, screen, &tmprect);
			} else if (dir & DOWN) {
				SDL_BlitSurface(tileIntersectDown, NULL, screen, &tmprect);
			}
		break;
		default:
			SDL_BlitSurface(tileBlank, NULL, screen, &tmprect);
		break;
	}
}

