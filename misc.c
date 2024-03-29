#include "misc.h"

void configure_console() {
#if defined WIN32
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD dwMode = 0;
	GetConsoleMode(hOut, &dwMode);
	dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
	SetConsoleMode(hOut, dwMode);
#endif
	// Redirect stderr to null device aka black hole.
	if (g_verbosity == NOOP)
		freopen(NULL_DEVICE, "w", stderr);
}

void do_sleep(uint32_t ms) {
#if defined WIN32
	Sleep(ms);
#elif defined __linux__
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms % 1000) * 1000000;
	nanosleep(&ts, NULL);
#endif
}

void wait_ms(uint32_t ms) {
	do_sleep(ms);
}

void wait_ms_on_loop(uint32_t ms, const volatile bool *_exiting)
{
	bool long_wait = ms > TIME_CHUNK;
	uint32_t wait_chunk = long_wait ? ms / TIME_CHUNK : 0;
	uint32_t wait_leftover = long_wait ? ms % TIME_CHUNK : ms;
	uint32_t wait_time = long_wait ? TIME_CHUNK : ms;
	char current_time[10];

	for (uint32_t c = 0; c < wait_chunk; c++) {
		do_sleep(wait_time);
		if (*_exiting) {
			puts("");
			__print(INFO, "Exit signal caught.\n", current_time);
			return;
		}
	}
	do_sleep(wait_leftover);
}

void update_time(char *out, uint32_t outSize) {
	time_t rawtime = time(NULL);
	struct tm info;
#if defined WIN32
	localtime_s(&info, &rawtime);
#elif defined __linux__
	localtime_r(&rawtime, &info);
#endif
	strftime(out, outSize, "%X", &info);
}

void __print(uint8_t verbosity, char *format, ...)
{
	char current_time[10];
	va_list args;

	va_start(args, format);
	update_time(current_time, sizeof(current_time));

	if (g_verbosity < verbosity)
		return;
	if (g_verbosity >= VERB)
		fprintf(stdout, "[%s] ", current_time);
	fflush(stdout);

	switch (verbosity)
	{
	case VERB:
	case WARN:
	case INFO:
	default:
		vfprintf(stdout, format, args);
		break;
	case ERR:
		vfprintf(stderr, format, args);
		break;
	case NOOP:
		break;
	}
	fflush(stdout);
	va_end(args);
}
