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

void _sleep(uint32_t ms) {
#if defined WIN32
	Sleep(ms);
#elif defined __linux__
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, NULL);
#endif
}

inline void wait_ms(uint32_t ms) {
	_sleep(ms);
}

inline void wait_ms_on_loop(uint32_t ms, volatile bool *_exiting)
{
	bool long_wait = ms > TIME_CHUNK;
	uint32_t wait_chunk = long_wait ? ms / TIME_CHUNK : 0;
	uint32_t wait_leftover = long_wait ? ms % TIME_CHUNK : ms;
	uint32_t wait_time = long_wait ? TIME_CHUNK : ms;

	for(int c = 0;c < wait_chunk;c++) {
		// printf("%d\n%d\n%d\n", long_wait, wait_chunk, wait_leftover);
		// printf("%d %d", c, wait_chunk);
		_sleep(wait_time);
		if(*_exiting) return;
	}
	// printf("%d\n", wait_leftover);
	_sleep(wait_leftover);

}

void update_time(char* out, uint32_t outSize)
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
