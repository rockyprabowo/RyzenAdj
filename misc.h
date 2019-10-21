#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#if defined WIN32
#include <windows.h>
#define NULL_DEVICE "nul"
#elif defined __linux__
#include <unistd.h>
#define NULL_DEVICE "/dev/null"
#endif

#define VERB 4
#define INFO 3
#define WARN 2
#define ERR 1
#define NOOP 0
#define TIME_CHUNK 500

extern uint8_t g_verbosity;

void configure_console();
void wait_ms(uint32_t ms);
void wait_ms_on_loop(uint32_t ms, const volatile bool *_exiting);
void update_time(char* out, uint32_t outSize);
void __print(uint8_t verbosity, char *format, ...);
