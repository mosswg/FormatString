#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>		/* sprintf */
#include <stdlib.h>		/* malloc, calloc, realloc */
#include <string.h>		/* strlen, strcpy, etc */
#include <stdbool.h>	/* bools */
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */
#include <math.h>		/* floor */


#ifndef INT_MAX 
	#define INT_MAX 2147483647
#endif


#if  (defined(WIN32) || defined(_WIN64))
	#define WIN
	#define SIZE_OF_VA_ARG sizeof(__int64)
#endif

#ifndef _MSC_VER
	#define strcat_s(a, b, c) strcat(a, c)
	#define strcpy_s(a, b, c) strcpy(a, c)
#endif


#ifndef WIN
		#define SIZE_OF_VA_ARG 8
		#define _VA_POINTER_AT_IDX_L(args, idx) *(long*)(args->reg_save_area + (((idx) + 1) * args->gp_offset))
		#define _VA_POINTER_AT_IDX_C(args, idx) (char*)(args->reg_save_area + (((idx) + 1) * args->gp_offset))
		#define _VA_POINTER_AT_IDX_S(args, idx) *(char**)(args->reg_save_area + (((idx) + 1) * args->gp_offset))
		#define _VA_POINTER_AT_IDX_D(args, idx) *(double*)(args->reg_save_area + (args->fp_offset + (2 * (idx) * args->gp_offset)))
#else
	#define _VA_POINTER_AT_IDX_L(args, idx) *(long*)(args + ((idx) * SIZE_OF_VA_ARG))
	#define _VA_POINTER_AT_IDX_C(args, idx) (char*)(args + ((idx) * SIZE_OF_VA_ARG))
	#define _VA_POINTER_AT_IDX_S(args, idx) *(char**)(args + ((idx) * SIZE_OF_VA_ARG))
	#define _VA_POINTER_AT_IDX_D(args, idx) *(double*)(args + ((idx) * SIZE_OF_VA_ARG))
#endif

#ifndef _FLOATING_POINT_MAX_ZERO_BYTES_
	#define _FLOATING_POINT_MAX_ZERO_BYTES_ 2
#endif


// STRING
typedef struct string { char* string; int length; void (*append)(struct string*, char*); } string;
static inline void AppendString(string* dest, char* str) { int len = strlen(dest->string) + strlen(str);	dest->string = realloc(dest->string, len);	strcat_s(dest->string, len, str); }
static inline char* CreateString(char* str) { if (str[0] == '\0') { return calloc(1, 1); } size_t len = strlen(str);	char* out = malloc(len + 1);	strcpy_s(out, len, str);	return out; }
#define NewString(value) {.string = CreateString(value), .length = strlen(value) + 1, .append = AppendString}
#define EmptyString() {.string = calloc(1,1), .length = 1, .append = appendString}
#define SetString(dest, value) free(dest.string); dest.length = strlen(value) + 1; dest.string = calloc(dest.length, sizeof(char)); if (dest.length != 0) strcpy_s(dest.string, dest.length, value) 
#define SetEmptyString(dest) free(dest.string); dest.length = 1; dest.string = calloc(1, 1)
#define AppendString(dest, str) dest.append(&dest, str)
#define AppendChar(dest, char) dest.string = realloc(dest.string, ++dest.length+1); dest.string[dest.length-2] = char; dest.string[dest.length-1] = '\0'
#define isEmptyString(str) str.string[0] == '\0'
#define CopyString(str, out) out = out ? realloc(out, str.length) : calloc(str.length, 1); strcpy_s(out, str.length, str.string)
#define BeforeIndex(str, idx, out) out = out ? realloc(out, str.length) : calloc(str.length, 1); strcpy_s(out, str.length, str.string); out[idx] = '\0'
#define AfterIndex(str, idx, out) out = out ? realloc(out, str.length) : calloc(str.length, 1); strcpy_s(out, str.length, str.string); out += (idx+1)
static inline bool ContainsChar(string str, char character) { for (int i = 0; i < str.length; i++) { if (str.string[i] == character) { return true; } } return false; }
static inline int CharPosition(string str, char character) { for (int i = 0; i < str.length; i++) { if (str.string[i] == character) { return i; } } return -1; }

static inline char* CharArrayAfterChar(char* array, char afterChar) {
	int charIdx = -1;
	for (int i = 0; i < strlen(array); i++) {
		if (array[i] == afterChar) { 
			charIdx = i;
			break;
		} 
	}
	return (charIdx + 1) ? array + charIdx : "";
}
static inline int CharArrayLengthBeforeChar(char* array, char afterChar) {
	for (int i = 0; i < strlen(array); i++) {
		if (array[i] == afterChar) {
			return i;
		}
	}
	return -1;
}

// ARRAY
static inline bool isInArray(int* array, int arraySize, int value) {
	for (int i = 0; i < arraySize; i++)
		if (array[i] == value)
			return true;
	return false;
}
#define ArrayMax(array, len, dest) dest = 0; for (int i = 0; i < len; i++) dest = dest > array[i] ? dest : array[i]
void ArrayMaxPos(int*, int, int, int*);
#define ArrayMin(array, len, dest) dest = INT_MAX; for (int i = 0; i < len; i++) dest = dest < array[i] ? dest : array[i]
void ArrayMinPos(int*, int, int, int*);
#define ArrayMinAndMax(array, len, min, max) min = INT_MAX; max = 0; for (int i = 0; i < len; i++) {max = max > array[i] ? max : array[i]; min = min < array[i] ? min : array[i]; }
#define ArrayPrint(array, len) fputs("{ ", stdout); for (int i = 0; i < len; i++) { printf("%lf ", array[i]); } putchar('}')

#define ThrowError(function, error) printf("ERROR IN FUNCTION \"%s\": %s", function, error); exit(-1)




void ArrayMaxPos(int* array, int len, int ignore, int* dest) {
	int max = 0;
	*dest = 0;
	if (ignore == 0) {
		for (int i = 0; i < len; i++) {
			*dest = array[i] > max
				? i
				: *dest;
			max = array[i] > max ? array[i] : max;
		}
	}
	else {
		int* ignoreIdx = calloc(ignore, sizeof(int));
		for (int i = 0; i < ignore; i++) {
			ArrayMaxPos(array, len, ignore - (i + 1), ignoreIdx + i);
		}

		for (int i = 0; i < len; i++) {
			*dest = (array[i] > max && (!isInArray(array, len, i)) ? i : *dest);
			max = (array[i] > max) && (!isInArray(ignoreIdx, ignore, i)) ? array[i] : max;
		}
		free(ignoreIdx);
	}
}



void ArrayMinPos(int* array, int len, int ignore, int* dest) {
	int min = INT_MAX;
	*dest = 0;
	if (ignore == 0) {
		for (int i = 0; i < len; i++) {
			*dest = array[i] < min ? i : *dest;
			min = array[i] < min ? array[i] : min;
		}
	}
	else {
		int* ignoreIdx = calloc(ignore, sizeof(int));
		for (int i = 0; i < ignore; i++) {
			ArrayMinPos(array, len, ignore - (i + 1), ignoreIdx + i);
		}

		for (int i = 0; i < len; i++) {
			*dest = (array[i] < min && (!isInArray(ignoreIdx, ignore, i)) ? i : *dest);
			min = (array[i] < min) && (!isInArray(ignoreIdx, ignore, i)) ? array[i] : min;
		}
		free(ignoreIdx);
	}
}



static inline bool isCharOrInt(int x) {
	// true == int
	// false == char

	return (x > 255) /* value is greater than max char value */ || (*((char*)&x + 1) == 0); /* if an int is less than 255 then the 2nd byte of the int must be 0. Using this we can reference x then cast it to a char ptr and increment it by one and then dereference it. If the value is not zero then x must be a character */
}

const char topFourBits = (char)240;
const char bottomFourBits = (char)15;


/*
									Double or Long Detection
	------------------------------------------------------------------------------------------

										Fundamenals 
	va_lists are char arrays with size 8 that store either the data or a pointer to an array or struct.
	64-bit Doubles are stored with 1 bit for sign and 10 bits for an exponent. The rest of the bits 53 bits are Mantissa. 
	
										  Method
	Nearly all doubles contain at least one exponent bit thus if we can detect the first 1 and a half bytes (12 bits) and any 
	of them are set then we can assume it is a double. This falls apart with very high longs but in my tests it is 100% accurate up to at least 10,000,000 for longs 
	and 10,000 for doubles (incremeted by 0.0001)
*/

/*
	Gets the argument at index idx of args va_list then detects whether its a double or long (See method above).
*/
#ifndef WIN // LINUX
double va_get_num(va_list args, int idx, int numOfLongs, int numOfFloats, bool detection, bool* isFloating)
{
	double dArg = _VA_POINTER_AT_IDX_D(args, idx - numOfLongs);
	long lArg = _VA_POINTER_AT_IDX_L(args, idx - numOfFloats);
#else // WIN
 double va_get_num(va_list args, int idx, bool detection, bool* isFloating)
	{
	double dArg = _VA_POINTER_AT_IDX_D(args, idx);
	long lArg = _VA_POINTER_AT_IDX_L(args, idx);
#endif
	if (detection) {
		*isFloating = true;
		int zeroBytes = _FLOATING_POINT_MAX_ZERO_BYTES_;
		bool Exponent = false;
		bool Mantissa = false;
		if (((floor(dArg) != dArg) && (ceil(dArg)) != dArg) && !((dArg == 0 && (dArg) == 0.0f))) {
			if (!(floor(dArg) == 0 && dArg < 0.0000001)) {
				for (int i = SIZE_OF_VA_ARG - 1; i >= 0; i--) {
					if (*(((char*)&lArg) + i) != '\0') {
						{
							if (i == SIZE_OF_VA_ARG - 1 || (i == SIZE_OF_VA_ARG - 2 && (char)(*(((char*)&lArg) + i) & topFourBits) != '\0')) {
								Exponent = true;
								zeroBytes = !zeroBytes ? zeroBytes : --zeroBytes;
							}
							if (i != SIZE_OF_VA_ARG - 1 || ((i == SIZE_OF_VA_ARG - 2) && (char)(*(((char*)&lArg) + i) & bottomFourBits) != '\0')) {
								Mantissa = true;
								zeroBytes = !zeroBytes ? zeroBytes : --zeroBytes;
							}
						}
					}
				}
			}	
		}
		else {
			*isFloating = true;
		}
		*isFloating &= (Exponent & (zeroBytes == 0));
	}
	
	if (!*isFloating) 
		return (double)(lArg);
	else 
		return (dArg);
	
	return -1;
}


/*
									Char or String Detection
	------------------------------------------------------------------------------------------

	Fundamenals: va_lists are char arrays with size 8 that store either the data or a pointer to an array or struct.

	Because chars are only 1 byte and pointers are not if the second byte in the char array is 0 then we know it must be a character otherwise it must be a string.

*/

/*
	Gets the argument at index idx of args va_list then detects whether its a char or string (See method above). 

	numFloatArgs is needed for linux compatiblity
*/
char* va_get_char(va_list args, int idx, int numFloatArgs) {
	bool isChar = *(_VA_POINTER_AT_IDX_C(args, (idx - numFloatArgs)) + 1) == 0;

	int len = strlen(isChar ? _VA_POINTER_AT_IDX_C(args, idx - numFloatArgs) : _VA_POINTER_AT_IDX_S(args, idx - numFloatArgs));
	char* arg = calloc(len + 1, sizeof(char));
	

	strcpy_s(arg, len+1, isChar ? _VA_POINTER_AT_IDX_C(args, idx - numFloatArgs) : _VA_POINTER_AT_IDX_S(args, idx - numFloatArgs));
	return arg;
}




/*

	_BufferPtr: A pointer to a char array, either allocated or not, where the formated string will be placed
	_Format: The format that will be placed into _BufferPtr
	_Args: The arguments which will be placed into the formated string

	Syntax: Curley Brackets will be completely removed* and replaced by the argument matching their number or letter in
	the _Format string.

	Arguments will be read by lowest value first. e.g. if 55 is the lowest number in curley brackets it will be interperated as the first number passed

	Numbers: The syntax for numbers is {n} where n a number pertaining to the desired argument.
	Numbers will be automatically detected as either long format or IEEE-754 (double) format.
	If automatic detection is incorrect and l or f can be added to specify format.

	Chars: The syntax for numbers is {*} where * is a letter pertaining to the desired argument.


	* Currently the only way to retain curley brackets is to pass them as a char or char*

*/
void vformats(char** _BufferPtr, const char* _Format, va_list _Args) {
	if (!_Format || _Format[0] == '\0') {
		*_BufferPtr = calloc(1, 1);
		return;
	}


	// Args
	char* _Buffer = *_BufferPtr;


	// Counters
	int stringPos = 0;
	int newStringPos = 0;


	// State identifiers
	bool readingFormatSpec = false;
	bool isNum = false;
	char currentChar = '\0';


	// Replacement Varables
	int currentNumPos = 0;
	int currentCharPos = 0;
	bool readingDecimal = 0;
	int* numsByAppearence = calloc(currentNumPos, sizeof(int));
	int* charsByAppearence = calloc(currentCharPos, sizeof(int));
	bool* isNumArr = calloc(currentNumPos + currentCharPos, sizeof(bool));
	bool* isDouble = calloc(currentNumPos, sizeof(bool));
	bool* isExplicit = calloc(currentNumPos, sizeof(bool));
	string numberStr = NewString("");
	char replacementChar = '\0';

	int startLen = strlen(_Format);
	for (int i = 0; i < startLen; i++) {
		currentChar = _Format[i];
		if (readingFormatSpec || currentChar == '{') {
			readingFormatSpec = true;
			if (currentChar == '}') {
				readingFormatSpec = false;
				if (isNum && !isInArray(numsByAppearence, currentNumPos, atoi(numberStr.string))) {
					isNumArr = realloc(isNumArr, currentNumPos + currentCharPos + 1);
					isNumArr[currentNumPos + currentCharPos] = isNum;
					numsByAppearence = realloc(numsByAppearence, ++currentNumPos * sizeof(int));
					isDouble = realloc(isDouble, currentNumPos * sizeof(bool));
					isExplicit = realloc(isExplicit, currentNumPos * sizeof(bool));
					isExplicit[currentNumPos - 1] = replacementChar != '\0';
					if (replacementChar != '\0') {
						isDouble[currentNumPos-1] = (replacementChar == 'f');
					}
					numsByAppearence[currentNumPos - 1] = atoi(numberStr.string);
					SetString(numberStr, "");
				}
				else if (!isNum && !isInArray(charsByAppearence, currentCharPos, (int)replacementChar)) {
					isNumArr = realloc(isNumArr, currentNumPos + currentCharPos + 1);
					isNumArr[currentNumPos + currentCharPos] = isNum;
					charsByAppearence = realloc(charsByAppearence, ++currentCharPos * sizeof(int));
					charsByAppearence[currentCharPos - 1] = replacementChar;
				}
				replacementChar = '\0';
				isNum = false;
				readingDecimal = false;
			}
			else if (currentChar == '.') {
				readingDecimal = true;
			}
			else if (currentChar >= '0' && currentChar <= '9' && !readingDecimal) {
				AppendChar(numberStr, currentChar);
				isNum = true;
			}
			else if ((currentChar >= 'A' && currentChar <= 'Z') || (currentChar >= 'a' && currentChar <= 'z')) {
				replacementChar = currentChar;
			}
		}
	}

	// Replacement Values
	int numberOfArgs = currentNumPos + currentCharPos;
	int ignoreAmount = 0;
	int numberOfNumArgs = currentNumPos;
	#ifndef WIN
	int numGeneral = 0;
	int numFloat = 0;
	#endif
	int currentArg = 0;
	double* replacementNums = calloc(numberOfNumArgs, sizeof(double));
	if (numberOfNumArgs != 0) {
		for (int i = 0; i < numberOfArgs; i++) {
			#ifdef WIN
				if (!isNumArr[i]) continue;
			#else
				if (!isNumArr[i]) { numGeneral++; continue; }
			#endif
				ArrayMinPos(numsByAppearence, numberOfNumArgs, ignoreAmount++, &currentArg);
			
			#ifdef WIN
				replacementNums[currentArg] = va_get_num(_Args, i, !isExplicit[currentArg], (isDouble + currentArg));
			#else // LINUX
				replacementNums[currentArg] = va_get_num(_Args, i, numGeneral, numFloat, !isExplicit[currentArg], (isDouble + currentArg));
				numGeneral = isDouble[currentArg] ? numGeneral : numGeneral + 1;
				numFloat = isDouble[currentArg] ? numFloat + 1 : numFloat;
			#endif

		}
	}

	
	int numberOfCharArgs = currentCharPos;
	char** replacementChars = calloc(numberOfCharArgs, 1 * sizeof(char*));
	if (numberOfCharArgs != 0) {
		currentArg = 0;
		ignoreAmount = 0;
		for (int i = 0; i < numberOfArgs; i++) {
			if (isNumArr[i]) continue;
			ArrayMinPos(charsByAppearence, numberOfCharArgs, ignoreAmount++, &currentArg);
			#ifdef WIN
				replacementChars[currentArg] = va_get_char(_Args, i, 0); 
			#else // LINUX
				replacementChars[currentArg] = va_get_char(_Args, i, numFloat);
			#endif
		}
	}



	isNum = false;
	SetEmptyString(numberStr);
	int posAsInt = 0;
	char* temp = calloc(100, sizeof(char));
	while (_Format[stringPos] != '\0') {
		currentChar = _Format[stringPos];
		if (!readingFormatSpec && currentChar != '{') { // Pass values
			_Buffer = realloc(_Buffer, newStringPos + 1);
			_Buffer[newStringPos++] = _Format[stringPos];
			_Buffer[newStringPos] = '\0';
		}
		else { // Read replacement / replace
			if (currentChar == '}') { // Replacement
				if (isNum) {
					int currentNumber;
					int decimalAmount = -1;
					if (!ContainsChar(numberStr, '.'))
						currentNumber = atoi(numberStr.string);
					else {
						int periodIdx = CharPosition(numberStr, '.');
						char* tempStr = NULL;

						BeforeIndex(numberStr, periodIdx, tempStr);
						currentNumber = atoi(tempStr);

						AfterIndex(numberStr, periodIdx, tempStr);
						decimalAmount = atoi(tempStr);

						free(tempStr - (periodIdx + 1));
					}
					for (int i = 0; i < numberOfNumArgs; i++) {
						if (numsByAppearence[i] == currentNumber) {
							posAsInt = i;
							break;
						}
					}

					if (isDouble[posAsInt]) {
						sprintf(temp, "%lf", replacementNums[posAsInt]);
						if (decimalAmount != -1) {
							temp[(decimalAmount + CharArrayLengthBeforeChar(temp, '.')) + 1] = '\0';
							decimalAmount = -1;
						}
					}
					else {
						sprintf(temp, "%ld", (long)replacementNums[posAsInt]);
					}
					int replacementLen = strlen(temp);

					newStringPos += replacementLen;
					_Buffer = realloc(_Buffer, newStringPos + 2);
					_Buffer[newStringPos - replacementLen] = '\0';


					strcat(_Buffer, temp);

					SetEmptyString(numberStr);

					isNum = false;
				}
				else {
					for (int i = 0; i < numberOfCharArgs; i++) {
						if (charsByAppearence[i] == replacementChar) {
							posAsInt = i;
							break;
						}
					}
					int replacementLen = strlen(replacementChars[posAsInt]);

					newStringPos += replacementLen;
					_Buffer = realloc(_Buffer, newStringPos + 2);
					_Buffer[newStringPos - replacementLen] = '\0';

					strcat(_Buffer, (replacementChars[posAsInt]));

				}

				readingFormatSpec = false;
			}
			else { // Reading replacement value
				readingFormatSpec = true;


				if ((currentChar >= '0' && currentChar <= '9') || currentChar == '.') {
					AppendChar(numberStr, currentChar);
					isNum = true;
				}
				else if ((currentChar >= 'A' && currentChar <= 'Z') || (currentChar >= 'a' && currentChar <= 'z')) {
					replacementChar = currentChar;
				}


			}


		}
		stringPos++;
	}


	_Buffer[newStringPos + 1] = '\0';
	*_BufferPtr = _Buffer;


	for (int i = 0; i < numberOfCharArgs; i++) {
		free(replacementChars[i]);
	}
	free(replacementChars);
	free(numsByAppearence);
	free(charsByAppearence);
	free(isNumArr);
	free(replacementNums);
	free(isDouble);
	free(isExplicit);
	free(temp);
	free(numberStr.string);
}


/*
	_BufferPtr: A pointer to a char array, either allocated or not, where the formated string will be placed
	_Format: The format that will be placed into _BufferPtr

	Syntax: Curley Brackets will be completely removed* and replaced by the argument matching their number or letter in
	the _Format string.

	Arguments will be read by lowest value first. e.g. if 55 is the lowest number in curley brackets it will be interperated as the first number passed

	Numbers: The syntax for numbers is {n} where n a number pertaining to the desired argument.
	Numbers will be automatically detected as either long format or IEEE-754 (double) format.
	If automatic detection is incorrect and l or f can be added to specify format.

	Chars: The syntax for numbers is {*} where * is a letter pertaining to the desired argument.


	* Currently the only way to retain curley brackets is to pass them as a char or char*

*/
void formats(char** _Buffer, char* _Format, ...) {
	va_list args;

	va_start(args, _Format);

	vformats(_Buffer, _Format, args);

	va_end(args);
}


/*
	Prints a formated string using vformats

	_Format: The format that will be printed after replacement

	Syntax: Curley Brackets will be completely removed* and replaced by the argument matching their number or letter in
	the _Format string.

	Arguments will be read by lowest value first. e.g. if 55 is the lowest number in curley brackets it will be interperated as the first number passed

	Numbers: The syntax for numbers is {n} where n a number pertaining to the desired argument.
	Numbers will be automatically detected as either long format or IEEE-754 (double) format.
	If automatic detection is incorrect and l or f can be added to specify format.

	Chars: The syntax for numbers is {*} where * is a letter pertaining to the desired argument.


	* Currently the only way to retain curley brackets is to pass them as a char or string
*/
void vfprint_f(FILE* _Stream, char* _Format, va_list args) {
	char* Buffer = malloc(0);

	vformats(&Buffer, _Format, args);

	fputs(Buffer, _Stream);

	free(Buffer);
}


/*
	Prints a formated string using vfprint_f
	
	_Format: The format that will be printed after replacement

	Syntax: Curley Brackets will be completely removed* and replaced by the argument matching their number or letter in
	the _Format string.

	Arguments will be read by lowest value first. e.g. if 55 is the lowest number in curley brackets it will be interperated as the first number passed

	Numbers: The syntax for numbers is {n} where n a number pertaining to the desired argument.
	Numbers will be automatically detected as either long format or IEEE-754 (double) format.
	If automatic detection is incorrect and l or f can be added to specify format.

	Chars: The syntax for numbers is {*} where * is a letter pertaining to the desired argument.


	* Currently the only way to retain curley brackets is to pass them as a char or char*

*/
void print_f(char* _Format, ...) {
	va_list args;

	va_start(args, _Format);

	vfprint_f(stdout, _Format, args);

	va_end(args);
}



bool _TestFormatString(int iterations, double doubleInc) {
	char* buffer1 = calloc(16, 1);
	char* buffer2 = malloc(0);


	for (long i = 0; i < iterations; i++) {
		sprintf(buffer1, "%ld", i);
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\bi: %ld", i);
		formats(&buffer2, "{0}", i);
		if (strcmp(buffer1, buffer2) != 0) {
			printf("INEQUAL: %ld != %s != %s\n", i, buffer1, buffer2);
			return false;
		}

	}


	// Exits if i is zero because formatString interperates 0 as long
	for (double i = doubleInc; i < iterations * doubleInc; i += doubleInc) {
		printf("\b\b\b\b\b\b\b\b\b\b\b\b\bi: %lf", i);
		sprintf(buffer1, "%lf", i);
		formats(&buffer2, "{0f}", i);
		if (strcmp(buffer1, buffer2) != 0) {
			printf("INEQUAL: %lf != %s != %s\n", i, buffer1, buffer2);
			return false;
		}
	}

	print_f("\nAll Tests Passed For {0} Iterations With a Double Increment Of {1}", iterations, doubleInc);

	free(buffer1);
	free(buffer2);

	return true;
}
