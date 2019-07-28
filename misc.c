#include "misc.h"

void wait_ms(int ms)
{
#if defined WIN32
	Sleep(ms);
#elif defined(_POSIX_C_SOURCE)
#if _POSIX_C_SOURCE >= 199309L
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, NULL);
#else
	usleep(ms * 1000);
#endif
#endif
}

void current_time(char* out, unsigned int outSize)
{
	time_t rawtime = time(NULL);
	struct tm info;
	localtime_s(&info,&rawtime);

	strftime(out, outSize, "%X", &info);
}
