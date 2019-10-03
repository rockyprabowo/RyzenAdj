#include "misc.h"

void configure_console() {
#if defined WIN32
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
#endif
}

void wait_ms(unsigned int ms)
{
#if defined WIN32
	Sleep(ms);
#elif defined __linux__
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, NULL);
#endif
}

void current_time(char* out, unsigned int outSize)
{
	time_t rawtime = time(NULL);
	struct tm info;
#if defined WIN32
	localtime_s(&info,&rawtime);
#elif defined __linux__
	localtime_r(&rawtime,&info);
#endif

	strftime(out, outSize, "%X", &info);
}
