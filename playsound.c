/*
 * playsound.c
 *
 *  Created on: Mar 1, 2014
 *      Author: renan
 */

#include <alsa/asoundlib.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

snd_pcm_format_t format = SND_PCM_FORMAT_S16;
snd_pcm_sframes_t period_size;
unsigned int rate = 44100;
unsigned int period_time = 100000;
int channels = 1;
double freq = 440;

static void set_hardware_parameters(snd_pcm_t *handle,
		snd_pcm_hw_params_t *params) {
	int rc;
	int dir;

	// init
	snd_pcm_hw_params_any(handle, params);

	// set
	snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

	rc = snd_pcm_hw_params_set_format(handle, params, format);

	rc = snd_pcm_hw_params_set_channels(handle, params, channels);

	rc = snd_pcm_hw_params_set_rate_near(handle, params, &rate, &dir);

	rc = snd_pcm_hw_params_set_period_time_near(handle, params, &period_time,
			&dir);

	rc = snd_pcm_hw_params_get_period_size(params, &period_size, &dir);

	// push
	rc = snd_pcm_hw_params(handle, params);
	if (rc < 0) {
		fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
		return;
	}
}

static void generate_sine(const snd_pcm_channel_area_t *areas,
		snd_pcm_uframes_t offset, int count, double *_phase) {
	static double max_phase = 2. * M_PI;
	double phase = *_phase;
	double step = max_phase * freq / (double) rate;
	unsigned char *samples[channels];
	int steps[channels];
	unsigned int chn;
	int format_bits = snd_pcm_format_width(format);
	unsigned int maxval = (1 << (format_bits - 1)) - 1;
	int bps = format_bits / 8; /* bytes per sample */
	int phys_bps = snd_pcm_format_physical_width(format) / 8;
	int big_endian = snd_pcm_format_big_endian(format) == 1;
	int to_unsigned = snd_pcm_format_unsigned(format) == 1;

	/* verify and prepare the contents of areas */
	for (chn = 0; chn < channels; chn++) {
		if ((areas[chn].first % 8) != 0) {
			printf("areas[%i].first == %i, aborting...\n", chn,
					areas[chn].first);
			exit(EXIT_FAILURE);
		}
		samples[chn] = /*(signed short *)*/(((unsigned char *) areas[chn].addr)
				+ (areas[chn].first / 8));
		if ((areas[chn].step % 16) != 0) {
			printf("areas[%i].step == %i, aborting...\n", chn, areas[chn].step);
			exit(EXIT_FAILURE);
		}
		steps[chn] = areas[chn].step / 8;
		samples[chn] += offset * steps[chn];
	}
	/* fill the channel areas */
	while (count-- > 0) {
		union {
			float f;
			int i;
		} fval;
		int res, i;

		res = sin(phase) * maxval;

		if (to_unsigned)
			res ^= 1U << (format_bits - 1);
		for (chn = 0; chn < channels; chn++) {
			/* Generate data in native endian format */
			if (big_endian) {
				for (i = 0; i < bps; i++)
					*(samples[chn] + phys_bps - 1 - i) = (res >> i * 8) & 0xff;
			} else {
				for (i = 0; i < bps; i++)
					*(samples[chn] + i) = (res >> i * 8) & 0xff;
			}
			samples[chn] += steps[chn];
		}
		phase += step;
		if (phase >= max_phase)
			phase -= max_phase;
	}
	*_phase = phase;
}

static int xrun_recovery(snd_pcm_t *handle, int err) {
	printf("stream recovery\n");
	if (err == -EPIPE) { /* under-run */
		err = snd_pcm_prepare(handle);
		if (err < 0)
			printf("Can't recovery from underrun, prepare failed: %s\n",
					snd_strerror(err));
		return 0;
	} else if (err == -ESTRPIPE) {
		while ((err = snd_pcm_resume(handle)) == -EAGAIN)
			sleep(1); /* wait until the suspend flag is released */
		if (err < 0) {
			err = snd_pcm_prepare(handle);
			if (err < 0)
				printf("Can't recovery from suspend, prepare failed: %s\n",
						snd_strerror(err));
		}
		return 0;
	}
	return err;
}


void write_loop(snd_pcm_t *handle, signed short *samples,
		snd_pcm_channel_area_t *areas) {
	double phase = 0;
	signed short *ptr;
	int err, cptr;
	while (1) {
		generate_sine(areas, 0, period_size, &phase);
		ptr = samples;
		cptr = period_size;
		while (cptr > 0) {
			err = snd_pcm_writei(handle, ptr, cptr);
			if (err == -EAGAIN)
				continue;
			if (err < 0) {
				if (xrun_recovery(handle, err) < 0) {
					printf("Write error: %s\n", snd_strerror(err));
					exit(EXIT_FAILURE);
				}
				break; /* skip one period */
			}
			ptr += err * channels;
			cptr -= err;
		}
	}
}

void play_note(snd_pcm_t *handle, snd_pcm_hw_params_t *params) {
	int dir;
	int size;
	int loops;
	int i;
	int rc;
	/// ooops
}

void close_audio(snd_pcm_t *handle) {
	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	printf("audio close successfully\n");
}

int main(int argc, char *argv[]) {
	int rc;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	snd_pcm_channel_area_t *areas;
	unsigned int chn;
	signed short *samples;
	int dir;
	double phase = 0;
	double freq = 440;

	// initialize audio
	rc = snd_pcm_open(&handle, "plughw:0,0", SND_PCM_STREAM_PLAYBACK, 0);
	if (rc < 0) {
		fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
		return -1;
	}
	snd_pcm_hw_params_alloca(&params);
	set_hardware_parameters(handle, params);
	printf("audio opened successfully\n");

	// run

	samples = malloc(
			(period_size * channels * snd_pcm_format_physical_width(format))
					/ 8);
	if (samples == NULL) {
		printf("No enough memory\n");
		exit(EXIT_FAILURE);
	}
	areas = calloc(channels, sizeof(snd_pcm_channel_area_t));
	if (areas == NULL) {
		printf("No enough memory\n");
		exit(EXIT_FAILURE);
	}
	for (chn = 0; chn < channels; chn++) {
		areas[chn].addr = samples;
		areas[chn].first = chn * snd_pcm_format_physical_width(format);
		areas[chn].step = channels * snd_pcm_format_physical_width(format);
	}

	write_loop(handle, samples, areas);

	// play_note(handle, params);
	// close audio
	close_audio(handle);
	return 0;
}

