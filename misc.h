#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#if defined WIN32
#include <windows.h>
#elif defined __linux__
#include <unistd.h>
#endif

#define TIME_CHUNK 500

void configure_console();
void wait_ms(uint32_t ms);
void wait_ms_on_loop(uint32_t ms, const volatile bool *_exiting);
void update_time(char* out, uint32_t outSize);
