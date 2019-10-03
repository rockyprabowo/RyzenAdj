#if defined(WIN32)
#include <windows.h>
#include <time.h>
#elif defined(__linux__)
#include <time.h>
#include <unistd.h>
#endif

void configure_console();
void wait_ms(int ms);
void current_time(char* out, unsigned int outSize);
