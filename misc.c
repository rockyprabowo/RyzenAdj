#include "misc.h"

void wait_ms(int ms)
{
#if defined WIN32
	Sleep(ms);
#elif defined(__linux__)
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
	#ifdef WIN32
	localtime_s(&info,&rawtime);
	#elif defined(__linux__)
	localtime_r(&rawtime,&info);
	#endif

	strftime(out, outSize, "%X", &info);
}
