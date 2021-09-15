#include "FormatString.h"
#include <stdio.h>

#ifdef _MSC_VER
#define __COMPILER_STR__ "Visual Studio"
#define __OS__ "Windows"
#elif defined(__clang__)
#define __COMPILER_STR__ "clang"
#define __OS__ "Linux"
#elif defined(__MINGW64__)
#define __COMPILER_STR__ "MinGW 64-bit"
#define __OS__ "Windows"
#elif defined(__MINGW32__)
#define __COMPILER_STR__ "MinGW 32-bit"
#define __OS__ "Windows"
#elif defined(__GNUC__)
#define __COMPILER_STR__ "gcc"
#define __OS__ "Windows"
#else
#define __COMPILER_STR__ "Unknown"
#define __OS__ "Unknown"
#endif
int main() {
	
	print_f("Hello {a}: {b}", "Linux", __COMPILER_STR__);


	printf("\n");
}
