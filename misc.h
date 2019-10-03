#include <time.h>
#if defined WIN32
#include <windows.h>
#elif defined __linux__
#include <unistd.h>
#endif

void configure_console();
void wait_ms(unsigned int ms);
void current_time(char* out, unsigned int outSize);
