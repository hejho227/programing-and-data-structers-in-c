#include "primlib.h"
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#define DELEY 10
#define BLOCKS 3
#define PEGS 6
#define BGCOLOR BLACK
#define PEGCOLOR WHITE
#define MAXPEGS 10
#define MAXBLOCKS 437
#define MINPEGS 3
#define MINBLOCKS 1
#define TEXTPLACEMENT 200
#define COLORHIGLITED 6
#define COLORS 5
#define PEGSCREEN 5 / 8
#define TRANSWIDTH 1 / 4
#define TRANSFERFIRST 1
#define TRANSFERSEC 2
#define TRANSFERTHIRD 3
#define TRANSFERSTATES 4


struct gameData
{
	int pegArr[PEGS][BLOCKS][2];
	int won, running, firstPegXCord, pegHeight, blockHeight, maxBlockWidth, transferColor, pegColorChange, pegWidth;
	float changeBlockWidth;
};
 

int initialConditionsCheck(){
	
	if (BLOCKS > MAXBLOCKS || BLOCKS < MINBLOCKS){
		printf("incorrect number of blocks\n");
		return 1;
		}
	if (PEGS > MAXPEGS || PEGS < MINPEGS){
		printf("incorrect number of pegs\n");
		return 1;
	}
	return 0;
}


void restartGame(struct gameData *data){
	data->won = 0;
	data->running = 1;
	data->pegColorChange = 0;
	int blockColor = 0;
	int blockSelector, pegSelector;


	for (pegSelector = 0; pegSelector < PEGS; pegSelector++){
		for(blockSelector = 0; blockSelector < BLOCKS; blockSelector++)
		if (pegSelector == 0){
			blockColor = (blockColor % COLORS) + 1;
			data->pegArr[pegSelector][blockSelector][0] = BLOCKS - blockSelector;
			data->pegArr[pegSelector][blockSelector][1] = blockColor;
		} else{
			data->pegArr[pegSelector][blockSelector][0] = 0;
		}
	}
}


int keyCheck(struct gameData *data){
	switch (gfx_getkey()){
		case SDLK_0:
			return 10;
		case SDLK_1:
			return 1;
		case SDLK_2:
			return 2;
		case SDLK_3:
			return 3;
		case SDLK_4:
			return 4;
		case SDLK_5:
			return 5;
		case SDLK_6:
			return 6;
		case SDLK_7:
			return 7;
		case SDLK_8:
			return 8;
		case SDLK_9:
			return 9;
		case SDLK_ESCAPE:
			data->running = 0;
			break;
		case SDLK_RETURN:
			if (data->won){
				restartGame(data);
			}
			break;
		default:
			return 0;
	}
	return -1;
}


void drawAll(struct gameData *data){
	int pegSelector, blockSelector, firstXCord, firstYCord, secondXCord, secondYCord, currentHeight, valueOfArr, blockColor;
	
	gfx_filledRect(0, 0, gfx_screenWidth() - 1, gfx_screenHeight() - 1,
	BGCOLOR);
	for (pegSelector = 1; pegSelector <= PEGS; pegSelector++) {
		firstXCord = (data->firstPegXCord * pegSelector) - data->pegWidth;
		firstYCord = gfx_screenHeight() - 1 - data->pegHeight;
		secondXCord = (data->firstPegXCord * pegSelector) + data->pegWidth;
		secondYCord = gfx_screenHeight() - 1;
		if (pegSelector == data->pegColorChange){
			gfx_filledRect(firstXCord, firstYCord, secondXCord, secondYCord, COLORHIGLITED);
		}else{
			gfx_filledRect(firstXCord, firstYCord, secondXCord, secondYCord, PEGCOLOR);
		}
	}
	
	for (pegSelector = 0; pegSelector < PEGS; pegSelector++) {
		currentHeight = gfx_screenHeight() - 1;
		for (blockSelector = 0; blockSelector < BLOCKS; blockSelector++) {
			valueOfArr = data->pegArr[pegSelector][blockSelector][0];
			if (valueOfArr > 0) {
				firstXCord = (data->firstPegXCord * (pegSelector + 1)) - (data->maxBlockWidth - data->changeBlockWidth * (BLOCKS - valueOfArr));
				secondXCord = (data->firstPegXCord * (pegSelector + 1)) + (data->maxBlockWidth - data->changeBlockWidth * (BLOCKS - valueOfArr));
				secondYCord = currentHeight;
				currentHeight -= data->blockHeight;
				firstYCord = currentHeight;
				currentHeight -= 1;
				blockColor = data->pegArr[pegSelector][blockSelector][1];
				gfx_filledRect(firstXCord, firstYCord, secondXCord, secondYCord, blockColor);
			}
		}
	}
}


void transferAnimation(struct gameData *data, int targetPeg, int targetBlock, int fromPeg, int fromBlock){
	int firstXCord, firstYCord, secondXCord, secondYCord;
	int transferHeight = gfx_screenHeight() - (data->blockHeight * (fromBlock + 1));
	int transferXCord = data->firstPegXCord * (fromPeg + 1);
	int transferValue = data->pegArr[fromPeg][fromBlock][0];
	int transferState = 1;
	int blockColor = data->pegArr[fromPeg][fromBlock][1];
	int targetTransferHeight = gfx_screenHeight() * TRANSWIDTH;
	int finalTransferHeight = gfx_screenHeight() - (data->blockHeight * (targetBlock));
	int targetXCord = data->firstPegXCord * (targetPeg + 1);


	data->pegArr[fromPeg][fromBlock][0] *= -1;
	
	while (transferState < TRANSFERSTATES)
	{
		drawAll(data);
		firstXCord = transferXCord - (data->maxBlockWidth - data->changeBlockWidth * (BLOCKS - transferValue));
		secondXCord = transferXCord + (data->maxBlockWidth - data->changeBlockWidth * (BLOCKS - transferValue));
		secondYCord = transferHeight;
		firstYCord = transferHeight - data->blockHeight;
		switch (transferState)
		{
		case TRANSFERFIRST:
			transferHeight--;
			if (transferHeight <= targetTransferHeight) transferState += 1;
			break;
		case TRANSFERSEC:
			if (transferXCord > targetXCord){
				transferXCord--;
			}else if (transferXCord < targetXCord){
				transferXCord++;
			}else{
				transferState += 1;
			}
			break;
		case TRANSFERTHIRD:
			transferHeight++;
			if (transferHeight >= finalTransferHeight){
				transferState += 1;
				data->pegArr[fromPeg][fromBlock][0] *= -1;
			}
			break;
		default:
			transferState = TRANSFERSTATES;
		}
		gfx_filledRect(firstXCord, firstYCord, secondXCord, secondYCord, blockColor);
		gfx_updateScreen();
	}
	

}


void changePlaces(struct gameData *data, int reciver, int snPlace, int sender, int fiPlace){
	transferAnimation(data, reciver, fiPlace, sender, snPlace);
	data->pegArr[reciver][fiPlace][0] = data->pegArr[sender][snPlace][0];
	data->pegArr[reciver][fiPlace][1] = data->pegArr[sender][snPlace][1];
	data->pegArr[sender][snPlace][0] = 0;
}


void changeLogic(struct gameData *data){
	int blockToSend, blockSelector;

	int sender = keyCheck(data) - 1;
	int reciver = -1;
	
	if(sender >= 0 && sender < PEGS && data->pegArr[sender][0][0] > 0){
		data->transferColor = data->pegArr[sender][0][1];
		data->pegArr[sender][0][1] = COLORHIGLITED;
		data->pegColorChange = sender + 1;
		drawAll(data);
		gfx_updateScreen();
		
		reciver = keyCheck(data) - 1;
		if(sender >= 0 && sender < PEGS && data->pegArr[sender][0][0] > 0){
			data->pegArr[sender][0][1] = data->transferColor;
			data->pegColorChange = -1;
		}
	}

	if (sender >= 0 && sender < PEGS && data->pegArr[sender][0][0] > 0 && reciver >= 0 && reciver < PEGS && sender != reciver){
		for (blockToSend = BLOCKS - 1; blockToSend > 0; blockToSend--){
			if (data->pegArr[sender][blockToSend][0] > 0){
				break;
			}
		}
		for (blockSelector = BLOCKS - 2; blockSelector >= 0; blockSelector--){
			if (data->pegArr[reciver][blockSelector][0] > 0){
				if (data->pegArr[reciver][blockSelector][0] > data->pegArr[sender][blockToSend][0]){
					changePlaces(data, reciver, blockToSend, sender, blockSelector + 1);
					break;
				}else{
					break;
				}
			}
		}
		if (data->pegArr[reciver][0][0] == 0){
			changePlaces(data, reciver, blockToSend, sender, 0);
		}
	}
}


void winCheck(struct gameData *data){
	int pegSelector;
	for(pegSelector = 1; pegSelector < PEGS; pegSelector++){
		if (data->pegArr[pegSelector][BLOCKS - 1][0] > 0){
			data->won = 1;
			gfx_textout(TEXTPLACEMENT, TEXTPLACEMENT, "Congratulation", RED);
		}
	}
}


int main(int argc, char* argv[])
{
	struct gameData data = {0};
	data.running = 1;

	int blockSelector;
	int blockColor = 0;

	if (initialConditionsCheck()) {
		exit(1);
	}
	if (gfx_init()) {
		exit(2);
	}
	for (blockSelector = 0; blockSelector < BLOCKS; blockSelector++) {
		blockColor = (blockColor % COLORS) + 1;
		data.pegArr[0][blockSelector][0] = BLOCKS - blockSelector;
		data.pegArr[0][blockSelector][1] = blockColor;
	}
	data.firstPegXCord = (gfx_screenWidth() - 1) / (PEGS + 1);
	data.pegHeight = gfx_screenHeight() * PEGSCREEN;
	data.blockHeight = data.pegHeight / (BLOCKS) - 1;
	data.maxBlockWidth = data.firstPegXCord / 2;
	data.changeBlockWidth = (float)data.maxBlockWidth / (BLOCKS + 1);
	data.pegWidth = (data.maxBlockWidth - data.changeBlockWidth * BLOCKS);
	while(data.running){
		drawAll(&data);
		
		winCheck(&data);
		gfx_updateScreen();
	
		changeLogic(&data);


		SDL_Delay(DELEY);
	}
	return 0;
}
