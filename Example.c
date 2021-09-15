#include "FormatString.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#if defined(WIN32) || defined(_WIN64)
    #if _MSC_VER
        #define __COMPILER_STR__ "Visual Studio"
        #define __COMPILER_VERSION__ _MSC_VER
    #elif defined(__MINGW64__)
        #define __COMPILER_STR__ "MinGW 64-bit"
    #elif defined(__MINGW32__)
        #define __COMPILER_STR__ "MinGW 32-bit"
    #else
        #define __COMPILER_STR__ "Unknown"
    #endif
    #define __OS__ "Windows"
#else
    #if defined(__clang__)
        #define __COMPILER_STR__ "clang"
        #define __COMPILER_VERSION__ __GNUC__
    #elif defined(__GNUC__)
        #define __COMPILER_STR__ "gcc"
        #define __COMPILER_VERSION__ __GNUC__
    #else
        #define __COMPILER_STR__ "Unknown"
    #endif
#define __EXPLICT_FLOATS__
#define __OS__ "Linux"
#endif
int main() {
    srand(time(0));

    #ifdef __EXPLICT_FLOATS__
        print_f("Hello {a}: {b} Version {0}\nFloat restricted length: {1.1f}\nFloat unrestricted length: {1f}", "Linux", __COMPILER_STR__, __COMPILER_VERSION__, ((float)rand()/(float)(RAND_MAX)) * 100);
    #else
        print_f("Hello {a}: {b} Version {0}\nFloat restricted length: {1.1}\nFloat unrestricted length: {1}", "Linux", __COMPILER_STR__, __COMPILER_VERSION__, ((float)rand()/(float)(RAND_MAX)) * 100);
    #endif


	printf("\n");
}
