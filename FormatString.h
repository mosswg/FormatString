#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>		/* sprintf */
#include <stdlib.h>		/* malloc, calloc, realloc */
#include <string.h>		/* strlen, strcpy, etc */
#include <stdbool.h>	/* bools */
#include <stdarg.h>     /* va_list, va_start, va_arg, va_end */


#ifndef INT_MAX 
	#define INT_MAX 2147483647
#endif


#if (defined(WIN32) || defined(_WIN64))
	#define WIN
#endif

#ifndef WIN
	#define strcat_s(a, b, c) strcat(a, c)
	#define strcpy_s(a, b, c) strcpy(a, c)
#endif


#ifndef WIN
    #define IS_NOT_WINDOWS 1
    #define SIZE_OF_VA_ARG 8
    #define _VA_POINTER_AT_IDX_L(args, index) *(long*)(((((index < ((((unsigned long)args->gp_offset) - ((unsigned long)args->fp_offset))/((unsigned long)args->gp_offset))) * ((unsigned long)args->reg_save_area)) + (!((index < ((((unsigned long)args->gp_offset) - ((unsigned long)args->fp_offset))/((unsigned long)args->gp_offset)))) * ((unsigned long)args->overflow_arg_area))) + (((index) + 1) * args->gp_offset)))
    #define _VA_POINTER_AT_IDX_C(args, index) (char*)(((((index < ((((unsigned long)args->gp_offset) - ((unsigned long)args->fp_offset))/((unsigned long)args->gp_offset))) * ((unsigned long)args->reg_save_area)) + (!((index < ((((unsigned long)args->gp_offset) - ((unsigned long)args->fp_offset))/((unsigned long)args->gp_offset)))) * ((unsigned long)args->overflow_arg_area))) + (((index) + 1) * args->gp_offset)))
    #define _VA_POINTER_AT_IDX_S(args, index) *(char**)(((((index < ((((unsigned long)args->gp_offset) - ((unsigned long)args->fp_offset))/((unsigned long)args->gp_offset))) * ((unsigned long)args->reg_save_area)) + (!((index < ((((unsigned long)args->gp_offset) - ((unsigned long)args->fp_offset))/((unsigned long)args->gp_offset)))) * ((unsigned long)args->overflow_arg_area))) + (((index) + 1) * args->gp_offset)))
    #define _VA_POINTER_AT_IDX_D(args, index) *(double*)(args->reg_save_area + (args->fp_offset + (2 * (index) * args->gp_offset)))
#else
    #define IS_NOT_WINDOWS 0
	#define _VA_POINTER_AT_IDX_L(args, index) *(long*)(args + ((index) * sizeof(__int64)))
	#define _VA_POINTER_AT_IDX_C(args, index) (char*)(args + ((index) * sizeof(__int64)))
	#define _VA_POINTER_AT_IDX_S(args, index) *(char**)(args + ((index) * sizeof(__int64)))
	#define _VA_POINTER_AT_IDX_D(args, index) *(double*)(args + ((index) * sizeof(__int64)))
#endif

#ifndef _FLOATING_POINT_MAX_ZERO_BYTES_
	#define _FLOATING_POINT_MAX_ZERO_BYTES_ 2
#endif


// STRING
typedef struct string { char* string; int length; void (*append)(struct string*, char*); } string;
static inline void append_string(string* dest, char* str) { int len = strlen(dest->string) + strlen(str); dest->string = realloc(dest->string, len); strcat_s(dest->string, len, str); }
static inline char* create_string(const char* str) { if (str[0] == '\0') { return calloc(1, 1); } size_t len = strlen(str);	char* out = malloc(len + 1);	strcpy_s(out, len, str);	return out; }
static inline string new_string(const char* value) {
    string out = {.string = create_string(value),.length = strlen(value) + 1,.append = append_string};
    return out;
}
static inline string empty_string() {
    string out = {.string = calloc(1,1), .length = 1, .append = append_string};
    return out;
}
static inline void set_string(string *dest, const char* value) {
    free(dest->string);
    dest->length = strlen(value) + 1;
    dest->string = malloc(dest->length * sizeof(char));
    if ((dest->length-1) != 0)
        strcpy_s(dest->string, dest.length, value);
}
static inline void set_empty_string(string* dest) {
    free(dest->string);
    dest->length = 1;
    dest->string = calloc(1, sizeof(char));
}
static inline void append_char(string* dest, char c) {
    dest->length += 1;
    dest->string = realloc(dest->string, dest->length+1);
    dest->string[dest->length-2] = c;
    dest->string[dest->length-1] = '\0';
}
#define is_empty_string(str) str.string[0] == '\0'
static inline void copy_string(string str, char* dest) {
    dest = dest ? realloc(dest, str.length) : calloc(str.length, 1);
    strcpy_s(dest, str.length, str.string);
}
static inline void before_index(string str, int index, char** out) {
    if (*out) {
        free(*out);
    }

    *out = calloc(str.length, 1);
    strcpy_s(*out, str.length, str.string);
    (*out)[index] = '\0';
}
static inline void after_index(string str, int index, char** out) {
    if (*out) {
        free(*out);
    }
    (*out) = calloc(str.length, 1);
    strcpy_s((*out), str.length, str.string);
    (*out) += (index + 1);
}
static inline bool contains_char(string str, char character) { for (int i = 0; i < str.length; i++) { if (str.string[i] == character) { return true; } } return false; }
static inline int char_position(string str, char character) { for (int i = 0; i < str.length; i++) { if (str.string[i] == character) { return i; } } return -1; }

static inline char* char_array_after_char(char* array, char afterChar) {
	int charIdx = -1;
	for (int i = 0; i < strlen(array); i++) {
		if (array[i] == afterChar) { 
			charIdx = i;
			break;
		} 
	}
	return (charIdx + 1) ? array + charIdx : "";
}
static inline int char_array_length_before_char(char* array, char afterChar) {
	for (int i = 0; i < strlen(array); i++) {
		if (array[i] == afterChar) {
			return i;
		}
	}
	return -1;
}

// ARRAY
static inline bool is_in_array(int* array, int arraySize, int value) {
	for (int i = 0; i < arraySize; i++)
		if (array[i] == value)
			return true;
	return false;
}
void array_max_pos(int*, int, int, int*);
void array_min_pos(int*, int, int, int*);
#define array_min_and_max(array, len, min, max) min = INT_MAX; max = 0; for (int i = 0; i < (len); i++) {max = max > array[i] ? max : array[i]; min = min < array[i] ? min : array[i]; }

#define throw_error(function, error) printf("ERROR IN FUNCTION \"%s\": %s", function, error); exit(-1)


static inline bool is_char_or_int(int x) {
	// true == int
	// false == char

	return (x > 255) /* value is greater than max char value */ || (*((char*)&x + 1) == 0); /* if an int is less than 255 then the 2nd byte of the int must be 0. Using this we can reference x then cast it to a char ptr and increment it by one and then dereference it. If the value is not zero then x must be a character */
}


void va_get_num(va_list, int, int, int, bool, bool*, double*);


void va_get_char(va_list, int, int, char**);




void vformats(char**, const char*, va_list);


void formats(char**, char*, ...);


void vfprint_f(FILE*, char*, va_list);


void print_f(char*, ...);


bool _TestFormatString(int, double);
