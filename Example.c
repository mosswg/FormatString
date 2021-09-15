#include "FormatString.h"
#include <string.h>
#include <stdio.h>

#ifdef _MSC_VER
#define __COMPILER_STR__ "Visual Studio"
#elif defined(__clang__)
#define __COMPILER_STR__ "clang"
#elif defined(__MINGW64__)
#define __COMPILER_STR__ "MinGW 64-bit"
#elif defined(__MINGW32__)
#define __COMPILER_STR__ "MinGW 32-bit"
#elif defined(__GNUC__)
#define __COMPILER_STR__ "gcc"
#else
#define __COMPILER_STR__ "Unknown"
#endif
int main() {
	
	print_f("Hello {a}: {b}", "Linux", __COMPILER_STR__);


	printf("\n");
}
