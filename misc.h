#if defined(WIN32)
#include <windows.h>
#include <time.h> 
#elif defined(_POSIX_C_SOURCE)
#if _POSIX_C_SOURCE >= 199309L
#include <time.h> 
#else 
#include <unistd.h>
#endif
#endif

void wait_ms(int ms);
void current_time(char* out, unsigned int outSize);