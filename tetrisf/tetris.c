#include "primlib.h"
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <time.h>

#define COLUMNS 20
#define AREAHEIGHT 540
#define LINES (COLUMNS / 2)
#define AREAWIDTH (AREAHEIGHT / 2)
#define BLOCKS 7
#define BGCOLOR BLACK
#define LINECOR WHITE
#define STATBLOCKID 1
#define MOVEBLOCKID 2
#define PIVOTBLOCKID 3
#define REPLACEBLOCKID 10
#define STATIONARYBLO RED
#define MOVINGBLO YELLOW
#define PIVOT GREEN
#define MOVETIME 100
#define BLOCKMAXSIZE 7
#define DELEY 10
#define TEXTPLACEMENT 200
#define SPAWNOFFSET 2
#define LEFT 1
#define RIGHT 2
#define DOWN 3
#define ROTATE 4
#define NOOFBLOCKS 7
#define PIECESCOL 2
#define PIECESROW 4



const int PIECES[NOOFBLOCKS][PIECESCOL][PIECESROW] = {
	{
    {3, 2, 0, 0},
    {2, 2, 0, 0}
	},
	{
    {2, 3, 2, 2},
    {0, 0, 0, 0}
	},
	{
    {2, 3, 2, 0},
    {0, 0, 2, 0}
	},
	{
    {2, 3, 2, 0},
    {2, 0, 0, 0}
	},
	{
    {0, 2, 2, 0},
    {2, 3, 0, 0}
	},
	{
    {2, 2, 0, 0},
    {0, 3, 2, 0}
	},
	{
    {0, 2, 0, 0},
    {2, 3, 2, 0}
	}
};


typedef struct 
{
	int gameArea[COLUMNS][LINES];
	int blockOrder[BLOCKS];
	int rotatedBlock[BLOCKMAXSIZE][BLOCKMAXSIZE];
	int lost, running, firstLineX, firstLineY, lineSize, blockHeight, stopMove, moveAllowed, timer, pivotColumn, pivotLine, queue;
}gameData;


void removeLine(gameData *data){
	int columnSelctor, lineSelector;
	int empty = 0, downLines = 0;


	for(columnSelctor = 0; columnSelctor < COLUMNS; columnSelctor++){
		empty = 0;
		for(lineSelector = 0; lineSelector < LINES; lineSelector++){
			if (data->gameArea[columnSelctor][lineSelector] == 0 && downLines == 0){
				empty = 1;
				break;
			}else if (data->gameArea[columnSelctor][lineSelector] == 0){
				empty = 1;
			}
			if (downLines){
				data->gameArea[columnSelctor - downLines][lineSelector] = data->gameArea[columnSelctor][lineSelector];
				data->gameArea[columnSelctor][lineSelector] = 0;
			}
		}
		if (empty == 0){
			for(lineSelector = 0; lineSelector < LINES; lineSelector++){
				data->gameArea[columnSelctor - downLines][lineSelector] = 0;
			}
			downLines += 1;
		}
	}
}


void restartGame(gameData *data){
	data->lost = 0;
	data->running = 1;
	data->blockOrder[NOOFBLOCKS - 1] = -1;
	int columnSelctor, lineSelector;


	for (columnSelctor = 0; columnSelctor < COLUMNS; columnSelctor++){
		for(lineSelector = 0; lineSelector < LINES; lineSelector++)
		data->gameArea[columnSelctor][lineSelector] = 0;
	}
}


void clearCopy(gameData *data){
	int columnSelctor, lineSelector;

	for (columnSelctor = 0; columnSelctor < BLOCKMAXSIZE; columnSelctor++){
		for (lineSelector = 0; lineSelector < BLOCKMAXSIZE; lineSelector++){
			data->rotatedBlock[columnSelctor][lineSelector] = 0;
		}
	}
}


int rotate(gameData *data){ 
	int columnSelctor, lineSelector, selectedCol, selectedLin, colPosition, linPosition, origCol, origLin;

	for (columnSelctor = 0; columnSelctor < BLOCKMAXSIZE; columnSelctor++){
		for (lineSelector = 0; lineSelector < BLOCKMAXSIZE; lineSelector++){
			selectedCol = columnSelctor - BLOCKMAXSIZE / 2;
			selectedLin = lineSelector - BLOCKMAXSIZE / 2;
			colPosition = (selectedCol * cos(M_PI/2) - selectedLin * sin(M_PI/2)) + BLOCKMAXSIZE / 2;
			linPosition = (selectedCol * sin(M_PI/2) + selectedLin * cos(M_PI/2)) + BLOCKMAXSIZE / 2;
			origCol = data->pivotColumn + (columnSelctor - BLOCKMAXSIZE / 2);
			origLin = data->pivotLine + (lineSelector - BLOCKMAXSIZE / 2);
			if (origCol < 0 || origLin < 0 || origCol >= COLUMNS || origLin >= LINES){
				if(data->rotatedBlock[columnSelctor][lineSelector] == 0){
					data->rotatedBlock[columnSelctor][lineSelector] = -1;
				}else{
					clearCopy(data);
					return 0;
				}
			}else if (data->gameArea[origCol][origLin] == STATBLOCKID){
				if (data->rotatedBlock[columnSelctor][lineSelector] != 0){
					clearCopy(data);
					return 0;
				}
				data->rotatedBlock[columnSelctor][lineSelector] = STATBLOCKID;
			}else if (data->gameArea[origCol][origLin] == PIVOTBLOCKID){
				data->rotatedBlock[columnSelctor][lineSelector] = PIVOTBLOCKID;
			}else if (data->gameArea[origCol][origLin] == 0);
			else if (data->rotatedBlock[colPosition][linPosition] == 0){
				data->rotatedBlock[colPosition][linPosition] = MOVEBLOCKID;
			}else {
				clearCopy(data);
				return 0;
			}
		}
	}

	for (columnSelctor = 0; columnSelctor < BLOCKMAXSIZE; columnSelctor++){
		for (lineSelector = 0; lineSelector < BLOCKMAXSIZE; lineSelector++){
			origCol = data->pivotColumn + (columnSelctor - BLOCKMAXSIZE / 2);
			origLin = data->pivotLine + (lineSelector - BLOCKMAXSIZE / 2);
			if (origCol >= 0 && origLin >= 0 && origCol < COLUMNS && origLin < LINES && data->rotatedBlock[columnSelctor][lineSelector] >= 0){
				data->gameArea[origCol][origLin] = data->rotatedBlock[columnSelctor][lineSelector];
				data->rotatedBlock[columnSelctor][lineSelector] = 0;
			}
		}
	}
	return 1;
}


int move(gameData *data, int direction){
	int columnSelctor, lineSelector, origCol, origLin;
	int moved = 0;


	for (columnSelctor = 0; columnSelctor < BLOCKMAXSIZE; columnSelctor++){
		for (lineSelector = 0; lineSelector < BLOCKMAXSIZE; lineSelector++){
			origCol = data->pivotColumn + (columnSelctor - BLOCKMAXSIZE / 2);
			origLin = data->pivotLine + (lineSelector - BLOCKMAXSIZE / 2);
			if (origCol < 0 || origLin < 0 || origCol >= COLUMNS || origLin >= LINES){
				continue;
			}else if (data->gameArea[origCol][origLin] > STATBLOCKID && data->gameArea[origCol][origLin] < REPLACEBLOCKID){
				if (origCol >= 0 && origLin == 0 && direction < 0){
					clearCopy(data);
					return 0;
				}
				if (origCol >=0 && origLin == LINES - 1 && direction > 0){
					clearCopy(data);
					return 0;
				}
			}
			switch (data->gameArea[origCol][origLin])
			{
			case 0:
				break;
			case STATBLOCKID:
				if (data->rotatedBlock[columnSelctor][lineSelector] > STATBLOCKID && data->rotatedBlock[columnSelctor][lineSelector] < REPLACEBLOCKID){
					clearCopy(data);
					return 0;
				}
				data->rotatedBlock[columnSelctor][lineSelector] = STATBLOCKID;
				break;
			default:
				if (lineSelector + direction >= 0 && lineSelector + direction < BLOCKMAXSIZE){
					if (data->rotatedBlock[columnSelctor][lineSelector + direction] == STATBLOCKID){
						clearCopy(data);
						return 0;
					}
				data->rotatedBlock[columnSelctor][lineSelector + direction] = data->gameArea[origCol][origLin];
				if (data->rotatedBlock[columnSelctor][lineSelector] == 0)data->rotatedBlock[columnSelctor][lineSelector] = REPLACEBLOCKID;
				}
				moved += 1;
				break;
			}
		}
	}
			


	for (columnSelctor = 0; columnSelctor < BLOCKMAXSIZE; columnSelctor++){
		for (lineSelector = 0; lineSelector < BLOCKMAXSIZE; lineSelector++){
			origCol = data->pivotColumn + (columnSelctor - BLOCKMAXSIZE / 2);
			origLin = data->pivotLine + (lineSelector - BLOCKMAXSIZE / 2);
			if (origCol >= 0 && origLin >= 0 && origCol < COLUMNS && origLin < LINES && data->rotatedBlock[columnSelctor][lineSelector] > STATBLOCKID){
				if (data->rotatedBlock[columnSelctor][lineSelector] == REPLACEBLOCKID) data->gameArea[origCol][origLin] = 0;
				else data->gameArea[origCol][origLin] = data->rotatedBlock[columnSelctor][lineSelector];
			}
		}
	}
	if(moved)data->pivotLine += direction;
	clearCopy(data);
	return 1;
}


int keyCheck(gameData *data){
	switch (gfx_pollkey()){
		case SDLK_LEFT:
			move(data, -1);
			return LEFT;
		case SDLK_RIGHT:
			move(data, 1);
			return RIGHT;
		case SDLK_DOWN:
			data->moveAllowed = 1;
			data->timer = 1;
			return DOWN;
		case SDLK_SPACE:
			rotate(data);
			return ROTATE;
		case SDLK_ESCAPE:
			data->running = 0;
			break;
		case SDLK_RETURN:
			restartGame(data);
			break;
		default:
			return 0;
	}
	return -1;
}


void loseCheck(gameData *data){
	while(data->lost == 1){
	gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1,
		BGCOLOR);
	gfx_textout(TEXTPLACEMENT, TEXTPLACEMENT, "You lost", STATBLOCKID);
	gfx_updateScreen();
	keyCheck(data);
	}
}


void moveLegCheck(gameData *data, int columnSelector, int lineSelector){
	if(data->stopMove == 1 || columnSelector <= 0 || data->gameArea[columnSelector - 1][lineSelector] == 1){
		data->stopMove = 1;
		data->moveAllowed = 0;
		data->gameArea[columnSelector][lineSelector] = STATBLOCKID;
	}else if (data->moveAllowed){
		if (data->gameArea[columnSelector][lineSelector] == PIVOTBLOCKID){
			data->pivotColumn = columnSelector - 1;
			data->pivotLine = lineSelector;
		}
		data->gameArea[columnSelector - 1][lineSelector] = data->gameArea[columnSelector][lineSelector];
		data->gameArea[columnSelector][lineSelector] = 0;
	}
}


int spawnBlock(gameData *data){
	int columnSelctor, lineSelector, current, blockSelected;
	int orgLine = LINES / 2 - SPAWNOFFSET;
	int orgColumn = COLUMNS - SPAWNOFFSET;

	for (current = 0; current < NOOFBLOCKS; current++){
		if (data->blockOrder[current] >=  0){
			blockSelected = data->blockOrder[current];
			data->blockOrder[current] = -1;
			break;
		}
	}

	for (columnSelctor = 0; columnSelctor < PIECESCOL; columnSelctor++){
		for (lineSelector = 0; lineSelector < PIECESROW; lineSelector++){
			if (PIECES[blockSelected][columnSelctor][lineSelector] == 0){
				continue;
			}
			if (data->gameArea[orgColumn + columnSelctor][orgLine + lineSelector] != 1){
				data->gameArea[orgColumn + columnSelctor][orgLine + lineSelector] = PIECES[blockSelected][columnSelctor][lineSelector];
				if (PIECES[blockSelected][columnSelctor][lineSelector] == PIVOTBLOCKID){
					data->pivotColumn = orgColumn + columnSelctor;
					data->pivotLine = orgLine + lineSelector;
				}
			}else{
				data->lost = 1;
				loseCheck(data);
				return 0;
			}
		}
	}
	data->stopMove = 0;
	data->moveAllowed = 1;
	data->timer = 1;
	return 1;
}


void drawAll(gameData *data){
	int columnSelector, lineSelector, firstXCord, firstYCord, secondXCord, secondYCord;
	int allStationary = 1;

	gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1,
	BGCOLOR);

	for (columnSelector = 0; columnSelector < COLUMNS; columnSelector++){
		firstYCord = data->firstLineY - (data->lineSize * columnSelector);
		secondYCord = data->firstLineY - (data->lineSize * (columnSelector + 1));
		for (lineSelector = 0; lineSelector < LINES; lineSelector++) {
			firstXCord = data->firstLineX + (data->lineSize * lineSelector);
			secondXCord = data->firstLineX + (data->lineSize * (lineSelector + 1));
			switch (data->gameArea[columnSelector][lineSelector]){
				case STATBLOCKID:
					gfx_filledRect(firstXCord, firstYCord - 1, secondXCord, secondYCord + 1, STATIONARYBLO);
					break;
				case MOVEBLOCKID:
					moveLegCheck(data, columnSelector, lineSelector);
					gfx_filledRect(firstXCord, firstYCord - 1, secondXCord, secondYCord + 1, MOVINGBLO);
					allStationary = 0;
					break;
					case PIVOTBLOCKID:
						moveLegCheck(data, columnSelector, lineSelector);
					gfx_filledRect(firstXCord, firstYCord - 1, secondXCord, secondYCord + 1, PIVOT);
					allStationary = 0;
					break;
				default:
					break;
			}
			gfx_rect(firstXCord, firstYCord, secondXCord, secondYCord, LINECOR);
		}
	}
	if (allStationary) {
		removeLine(data);
		spawnBlock(data);	
	}
}


void generateQueue(gameData *data){
	int temp, destination;
	for(int selector = 0; selector < NOOFBLOCKS; selector++){
		data->blockOrder[selector] = selector;
	}
	for(int selector = 0; selector < NOOFBLOCKS; selector++){
		destination = (rand() % (NOOFBLOCKS - selector)) + selector;
		temp = data->blockOrder[selector];
		data->blockOrder[selector] = data->blockOrder[destination];
		data->blockOrder[destination] = temp;
	}
}


int main(int argc, char* argv[])
{
	gameData data = {0};
	data.running = 1;

	srand(time(NULL));

	if (gfx_init()) {
		exit(2);
	}
	
	data.firstLineX = (gfx_screenWidth() - AREAWIDTH) / 2;
	data.firstLineY = (gfx_screenHeight() + AREAHEIGHT) / 2;
	data.lineSize = AREAHEIGHT / COLUMNS;

	generateQueue(&data);

	while(data.running){
		drawAll(&data);
		
		gfx_updateScreen();

		if (data.stopMove == 0 && data.timer == 0){
			data.moveAllowed = 1;
		}else{
			data.moveAllowed = 0;
		}

		if (data.blockOrder[NOOFBLOCKS - 1] == -1)generateQueue(&data);
		keyCheck(&data);
		data.timer = (data.timer + 1) % MOVETIME;

		SDL_Delay(DELEY);
	}
	return 0;
}
