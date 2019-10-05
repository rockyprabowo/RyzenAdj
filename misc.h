#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#if defined WIN32
#include <windows.h>
#elif defined __linux__
#include <unistd.h>
#endif

#define OUTPUT_VERBOSE 4
#define OUTPUT_INFO 3
#define OUTPUT_WARNING 2
#define OUTPUT_ERROR 1
#define OUTPUT_QUIET 0
#define TIME_CHUNK 500

extern uint8_t g_verbosity;

void configure_console();
void wait_ms(uint32_t ms);
void wait_ms_on_loop(uint32_t ms, const volatile bool *_exiting);
void update_time(char* out, uint32_t outSize);
void __print(uint8_t verbosity, char *format, ...);
