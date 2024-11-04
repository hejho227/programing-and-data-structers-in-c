#include "rand_malloc.h"
#include <stdio.h>
#include <stdlib.h>

#define AMOUNT 20

char* getLine();
void printAll();



int main()
{
	int running = 1, ptrAmount = 0, ptrAlloc = AMOUNT, detErr = 0;
	char** ptrArr = calloc(AMOUNT, sizeof(char*));
	if (ptrArr == NULL){
		printf("\ninitial allocating fixing\n");
		return 1;
	}
	int* ptrArrSize = calloc(AMOUNT, sizeof(int));
	if (ptrArrSize == NULL){
		printf("\ninitial allocating fixing\n");
		free(ptrArr);
		return 1;
	}
	int selector, lineLength;

	char** newPtrArr = NULL;
	int* newPtrSize = NULL;
	while (running)
	{
		ptrArr[ptrAmount] = getLine(&running, &lineLength);
		if (ptrArr[ptrAmount] == NULL) goto EXIT;
		ptrArrSize[ptrAmount] = lineLength;
		ptrAmount += 1;
		if (ptrAmount == ptrAlloc){
			newPtrArr = realloc(ptrArr, (AMOUNT + ptrAlloc) * sizeof(char*));
			if (newPtrArr == NULL)goto EXIT;
			ptrArr = newPtrArr;

			newPtrSize = realloc(ptrArrSize, (AMOUNT + ptrAlloc) * sizeof(int));
			if (newPtrSize == NULL)goto EXIT;
			ptrAlloc += AMOUNT;
			ptrArrSize = newPtrSize;
		}
		
	}
	printAll(ptrAmount, ptrArrSize, ptrArr);
	printf("done\n");
	goto CLEAR;
EXIT:
	printf("Memory allocation failed\nexiting...\n");
	detErr = 1;
CLEAR:
	for (selector = 0; selector < ptrAmount; selector++){
		free(ptrArr[selector]);
	}
	free(ptrArrSize);
	free(ptrArr);
	if (detErr) return 1;
	else return 0;
}


char* getLine(int *running, int *lineLength)
{
	int currentAmount = 0, currentAlloc = AMOUNT;
	char character = 0;
	char* array = calloc(AMOUNT, sizeof(char));
	if (array == NULL) return NULL;
	char* newArray = NULL;

	while (character != EOF || character != '\n')
	{
		character = getchar();
		if (character == EOF) {
			*running = 0;
			array[currentAmount] = '\0';
			*lineLength = currentAmount;
			return array;
		}else if (character == ' '){
			array[currentAmount] = '\0';
		}else if (character == '\n'){
			array[currentAmount] = '\0';
			*lineLength = currentAmount;
			return array;
		}else array[currentAmount] = character;
		currentAmount += 1;
		if (currentAmount >= currentAlloc){
			newArray = realloc(array, (AMOUNT + currentAlloc) * sizeof(char));
			currentAlloc += AMOUNT;
			if (newArray == NULL) goto EXIT1;
			array = newArray;
		}
	}
	*lineLength = currentAmount;
	return array;

EXIT1:
	free(array);
	return NULL;
}


void printAll(int ptrAmount, int *ptrArrSize,char** ptrArr){
	printf("\n\ntransforming...\n\n");
 	for (int selector = ptrAmount - 1; selector >= 0; selector--){
 		for (int sizeSelector = ptrArrSize[selector]; sizeSelector >= 0; sizeSelector--){
			if ((ptrArr[selector][sizeSelector] != '\0' && sizeSelector != 0) || (sizeSelector == ptrArrSize[selector])) continue;
			if(sizeSelector == 0 && ptrArr[selector][sizeSelector] != '\0') {
 				printf("%s\n", &ptrArr[selector][sizeSelector]);
			}else if (sizeSelector == 0)
 				printf("%s\n", &ptrArr[selector][sizeSelector + 1]);
			else{
				printf("%s ", &ptrArr[selector][sizeSelector + 1]);
			}

 		}
	}
 }