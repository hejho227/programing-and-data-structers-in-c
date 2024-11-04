#include <limits.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>




long strtol(const char* nPtr, char** endPtr, int base)
{
	errno = 0;
	const char *currChar = nPtr, *startChar = nPtr;
	long int result = 0;
	int sign = 1, overflow = 0, changed = 0, iGiveUp = 0;
	int translated;

	if ((base < 2 || base > 36) && base != 0){
		errno = EINVAL;
		return 0;
	}

	while (isspace(*currChar)) currChar++;

	if (*currChar == '+'){
		sign = 1;
		currChar++;
	}else if (*currChar == '-'){
		sign = -1;
		currChar++;
	}

	if (base == 0){
		if (*currChar == '0'){
			if (*(currChar + 1) == 'x' || *(currChar + 1) == 'X'){
				base = 16;
				currChar += 2;
				iGiveUp = 1;
			}else {
				base = 8;
				currChar++;
			}
		}else base = 10;
	}else if (base == 16 && *currChar == '0' && (*(currChar + 1) == 'x' || *(currChar + 1) == 'X')) {
		currChar += 2;
		iGiveUp = 1;
	}



	while (*currChar != '\0'){
		if (*currChar >= '0' && *currChar <= '9'){
			translated = *currChar - '0';
			if (changed == 0) changed = 1;
		}else if (isalpha(*currChar)){
			translated = toupper(*currChar) - 'A' + 10;
			if (changed == 0) changed = 1;
		}else{
			if (changed == 0) currChar = startChar;
			break;
		}

		if (translated >= base){
			currChar -= iGiveUp;
			break;
		}

		
		if (sign == -1){
			if (result < (LONG_MIN + translated)/base){
				overflow = -1;
			}else result = (result * base) - translated;
		}else if (sign == 1){
			if (result > (LONG_MAX - translated)/base){
				overflow = 1;
			}else result = (result * base) + translated;
		}

		currChar++;

	}
	if (changed == 0) currChar -= iGiveUp; 
	if (endPtr != NULL) *endPtr = (char*)currChar;

	if (overflow == 1){
		errno = ERANGE;
		return LONG_MAX;
	}else if (overflow == -1){
		errno = ERANGE;
		return LONG_MIN;
	}
	return result;
}
