// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2018-2019 Jiaxun Yang <jiaxun.yang@flygoat.com> */
/* Ryzen NB SMU Service Request Tool */

#include <string.h>
#include <signal.h>
#include "lib/ryzenadj.h"
#include "misc.h"
#include "argparse.h"

#define STRINGIFY2(X) #X
#define STRINGIFY(X) STRINGIFY2(X)

#define _do_adjust(ARG) \
do{ \
	while(ARG != 0){    \
		if(!set_##ARG(ry, ARG)){   \
			if (reapply_every > 0 && initial_info_printed) break; \
			printf(STRINGIFY(ARG) " set to %d (hex: %x)\n", ARG, ARG);    \
			break;  \
		} else {    \
			printf("Failed to set" STRINGIFY(ARG) " \n");   \
			err = -1; \
			break;  \
		}   \
	} \
}while(0);

ryzen_access* global_ryzen_access_ptr;

static const char *const usage[] = {
	"ryzenadj [options] [[--] args]",
	"ryzenadj [options]",
	NULL,
};

void signal_handler(int signal) {
	switch(signal) {
		case SIGABRT:
		case SIGINT:
			puts("\nExit signal caught.");
			break;
		default:
			puts("\nFIXME: Implement a proper signal handler.");
			break;
	}
	cleanup_ryzenadj(*global_ryzen_access_ptr);
	exit(0);
}

void register_ryzen_access(ryzen_access* _ry) {
	global_ryzen_access_ptr = _ry;
}

int main(int argc, const char **argv)
{
	ryzen_access ry;
	int err = 0;

	uint32_t info = 0, stapm_limit = 0, fast_limit = 0, slow_limit = 0, slow_time = 0, stapm_time = 0, tctl_temp = 0;
	uint32_t vrm_current = 0, vrmsoc_current = 0, vrmmax_current = 0, vrmsocmax_current = 0, psi0_current = 0, psi0soc_current = 0;
	uint32_t max_socclk_freq = 0, min_socclk_freq = 0, max_fclk_freq = 0, min_fclk_freq = 0, max_vcn = 0, min_vcn = 0, max_lclk = 0, min_lclk = 0;
	uint32_t max_gfxclk_freq = 0, min_gfxclk_freq = 0;
	uint32_t reapply_every = 0, error_count = 0;
	bool initial_info_printed = false;
	char last_time[16] = "";

	struct argparse_option options[] = {
		OPT_HELP(),
		OPT_GROUP("Options"),
		OPT_BOOLEAN('i', "info", &info, "Show information (W.I.P.)"),
		OPT_GROUP("Settings"),
		OPT_U32('a', "stapm-limit", &stapm_limit, "Sustained power limit (mW)"),
		OPT_U32('b', "fast-limit", &fast_limit, "Fast PPT power limit (mW)"),
		OPT_U32('c', "slow-limit", &slow_limit, "Slow PPT power limit (mW)"),
		OPT_U32('d', "slow-time", &slow_time, "Slow PPT constant time (S)"),
		OPT_U32('e', "stapm-time", &stapm_time, "STAPM constant time (S)"),
		OPT_U32('f', "tctl-temp", &tctl_temp, "Tctl temperature (℃)"),
		OPT_U32('g', "vrm-current", &vrm_current, "VRM Current Limit (mA)"),
		OPT_U32('j', "vrmsoc-current", &vrmsoc_current, "VRM SoC Current Limit (mA)"),
		OPT_U32('k', "vrmmax-current", &vrmmax_current, "VRM Maximum Current Limit (mA)"),
		OPT_U32('l', "vrmsocmax-current", &vrmsocmax_current, "VRM SoC Maximum Current Limit (mA)"),
		OPT_U32('m', "psi0-current", &psi0_current, "PSI0 Current Limit (mA)"),
		OPT_U32('n', "psi0soc-current", &psi0soc_current, "PSI0 SoC Current Limit (mA)"),
		OPT_U32('o', "max-socclk-frequency", &max_socclk_freq, "Maximum SoC Clock Frequency (MHz)"),
		OPT_U32('p', "min-socclk-frequency", &min_socclk_freq, "Minimum SoC Clock Frequency (MHz)"),
		OPT_U32('q', "max-fclk-frequency", &max_fclk_freq, "Maximum Transmission (CPU-GPU) Frequency (MHz)"),
		OPT_U32('r', "min-fclk-frequency", &min_fclk_freq, "Minimum Transmission (CPU-GPU) Frequency (MHz)"),
		OPT_U32('s', "max-vcn", &max_vcn, "Maximum Video Core Next (VCE - Video Coding Engine) (Value)"),
		OPT_U32('t', "min-vcn", &min_vcn, "Minimum Video Core Next (VCE - Video Coding Engine) (Value)"),
		OPT_U32('u', "max-lclk", &max_lclk, "Maximum Data Launch Clock (Value)"),
		OPT_U32('v', "min-lclk", &min_lclk, "Minimum Data Launch Clock (Value)"),
		OPT_U32('w', "max-gfxclk", &max_gfxclk_freq, "Maximum GFX Clock (Value)"),
		OPT_U32('x', "min-gfxclk", &min_gfxclk_freq, "Minimum GFX Clock (Value)"),
		OPT_U32('z', "reapply-every", &reapply_every, "Reapply configuration with delay (in milisecond)"),
		OPT_GROUP("P-State Functions"),
		OPT_END(),
	};


	struct argparse argparse;
	argparse_init(&argparse, options, usage, 0);
	argparse_describe(&argparse, "\n Ryzen Power Management adjust tool.", "\nWARNING: Use at your own risk!\nBy Jiaxun Yang <jiaxun.yang@flygoat.com>, Under LGPL.\nVersion: v0." STRINGIFY(RYZENADJ_VER));
	// argc = argparse_parse(&argparse, argc, argv);
	argparse_parse(&argparse, argc, argv);

	if (argc == 1) {
		printf("No parameter was set.\n");
		argparse_usage(&argparse);
		exit(-1);
	}

	signal(SIGABRT, signal_handler);
	signal(SIGINT, signal_handler);

	ry = init_ryzenadj();
	register_ryzen_access(&ry);
	if(!ry){
		puts("Unable to init ryzenadj, check permission");
		return -1;
	}

	if (reapply_every > 0) {
		if (reapply_every < 100)
			puts("WARNING: Delay below 100 ms is not recommended!");
		printf("Reapply configuration after %d ms delay\n", reapply_every);
		puts("Press Ctrl+C to exit.");
		puts("");
	}
	do {
		_do_adjust(stapm_limit);
		_do_adjust(fast_limit);
		_do_adjust(slow_limit);
		_do_adjust(slow_time);
		_do_adjust(stapm_time);
		_do_adjust(tctl_temp);
		_do_adjust(vrm_current);
		_do_adjust(vrmsoc_current);
		_do_adjust(vrmmax_current);
		_do_adjust(vrmsocmax_current);
		_do_adjust(psi0_current);
		_do_adjust(psi0soc_current);
		_do_adjust(max_socclk_freq);
		_do_adjust(min_socclk_freq);
		_do_adjust(max_fclk_freq);
		_do_adjust(min_fclk_freq);
		_do_adjust(max_vcn);
		_do_adjust(min_vcn);
		_do_adjust(max_lclk);
		_do_adjust(min_lclk);
		_do_adjust(max_gfxclk_freq);
		_do_adjust(min_gfxclk_freq);
		initial_info_printed = true;
		if(reapply_every == 0) break;
		current_time(last_time, 15);
		printf("\033[2K\r[Delay: %d ms] Settings applied at %s (error count: %d)",
			reapply_every,
			last_time,
			err != 0 ? ++error_count : error_count
			);
		fflush(stdout);
		wait_ms(reapply_every);
	} while (true);

	puts("Cleaning up.");
	cleanup_ryzenadj(ry);
	puts("Bye.");

	return err;
}
