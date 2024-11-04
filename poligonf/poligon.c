#include "primlib.h"
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define COLOR RED
#define BGCOLOR BLACK
#define PI 3.1415
#define ROTSPEED 1
#define DEGINACIRCLE 360
#define MAXRADIUS 250
#define MINRADIUS 50
#define RADIUSSPEED 1
#define VERTICES 7
#define DELEY 10

int running = 1;
int degres = 0;
int protFromOverflow;
int radGrowing = 1;
int startX;
int startY;
int firstX;
int firstY;
int secondX;
int secondY;
int radius = 140;
float radiansOfFir;
float radiansOfSec;
float angle;


int reduceToRange(int ang)
{
	if(ang < 0) ang = -1 * ang;
	while(ang >= DEGINACIRCLE){
		ang -= DEGINACIRCLE;
	}
	return ang;
}


int main(int argc, char* argv[])
{
	protFromOverflow = reduceToRange(ROTSPEED);
	if (gfx_init()) {
		exit(3);
	}
	angle = DEGINACIRCLE / (float)VERTICES;
	startX = (gfx_screenWidth() - 1) / 2;
	startY = (gfx_screenHeight() - 1) / 2;
	while(running){
		if(radGrowing){
			if(radius + RADIUSSPEED < MAXRADIUS){
				radius += RADIUSSPEED;
			}else{
				radGrowing = 0;
			}
		}else{
			if(radius - RADIUSSPEED > MINRADIUS){
				radius -= RADIUSSPEED;
			}else{
				radGrowing = 1;
			}
		}
		degres = reduceToRange(degres + protFromOverflow);
		gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1,
		BGCOLOR);
		for(int noOfPoint = 0; noOfPoint < VERTICES; noOfPoint++){
			radiansOfFir = reduceToRange(angle * noOfPoint + degres) * 2 * PI / DEGINACIRCLE;
			radiansOfSec = reduceToRange(angle * (noOfPoint + 1) + degres) * 2 * PI / DEGINACIRCLE;
			firstX = startX + radius * sin(radiansOfFir);
			firstY = startY + radius * cos(radiansOfFir);
			secondX = startX + radius * sin(radiansOfSec);
			secondY = startY + radius * cos(radiansOfSec);
			gfx_line(firstX, firstY, secondX, secondY, COLOR);
		}
		gfx_updateScreen();
		if(gfx_isKeyDown(SDLK_ESCAPE)){
			 running = 0;
		}
		SDL_Delay(DELEY);
	}
	return 0;
}
