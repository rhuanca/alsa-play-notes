/*
 * playsound.c
 *
 *  Created on: Mar 1, 2014
 *      Author: renan
 */

#include <asm-generic/errno-base.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>
#include <alsa/error.h>
#include <alsa/pcm.h>
#include <alsa/version.h>

void print_asound_info() {
	int val;

	printf("ALSA library version: %s\n", SND_LIB_VERSION_STR);

	printf("\nPCM stream types:\n");
	for (val = 0; val <= SND_PCM_STREAM_LAST; val++)
		printf("  %s\n", snd_pcm_stream_name((snd_pcm_stream_t) val));

	printf("\nPCM access types:\n");
	for (val = 0; val <= SND_PCM_ACCESS_LAST; val++)
		printf("  %s\n", snd_pcm_access_name((snd_pcm_access_t) val));

	printf("\nPCM formats:\n");
	for (val = 0; val <= SND_PCM_FORMAT_LAST; val++)
		if (snd_pcm_format_name((snd_pcm_format_t) val) != NULL)
			printf("  %s (%s)\n", snd_pcm_format_name((snd_pcm_format_t) val),
					snd_pcm_format_description((snd_pcm_format_t) val));

	printf("\nPCM subformats:\n");
	for (val = 0; val <= SND_PCM_SUBFORMAT_LAST; val++)
		printf("  %s (%s)\n", snd_pcm_subformat_name((snd_pcm_subformat_t) val),
				snd_pcm_subformat_description((snd_pcm_subformat_t) val));

	printf("\nPCM states:\n");
	for (val = 0; val <= SND_PCM_STATE_LAST; val++)
		printf("  %s\n", snd_pcm_state_name((snd_pcm_state_t) val));

}

void test_open() {
	int rc;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	unsigned int val, val2;
	int dir;
	snd_pcm_uframes_t frames;

	/* Open PCM device for playback. */
	rc = snd_pcm_open(&handle, "plughw:", SND_PCM_STREAM_PLAYBACK, 0);
	if (rc < 0) {
		fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
		exit(1);
	}

	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_alloca(&params);

	/* Fill it in with default values. */
	snd_pcm_hw_params_any(handle, params);

	/* Set the desired hardware parameters. */

	/* Interleaved mode */
	snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

	/* Signed 16-bit little-endian format */
	snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

	/* Two channels (stereo) */
	snd_pcm_hw_params_set_channels(handle, params, 2);

	/* 44100 bits/second sampling rate (CD quality) */
	val = 44100;
	snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);

	/* Write the parameters to the driver */
	rc = snd_pcm_hw_params(handle, params);
	if (rc < 0) {
		fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
		exit(1);
	}

	/* Display information about the PCM interface */

	printf("PCM handle name = '%s'\n", snd_pcm_name(handle));

	printf("PCM state = %s\n", snd_pcm_state_name(snd_pcm_state(handle)));

	snd_pcm_hw_params_get_access(params, (snd_pcm_access_t *) &val);
	printf("access type = %s\n", snd_pcm_access_name((snd_pcm_access_t) val));

	snd_pcm_hw_params_get_format(params, &val);
	printf("format = '%s' (%s)\n", snd_pcm_format_name((snd_pcm_format_t) val),
			snd_pcm_format_description((snd_pcm_format_t) val));

	snd_pcm_hw_params_get_subformat(params, (snd_pcm_subformat_t *) &val);
	printf("subformat = '%s' (%s)\n",
			snd_pcm_subformat_name((snd_pcm_subformat_t) val),
			snd_pcm_subformat_description((snd_pcm_subformat_t) val));

	snd_pcm_hw_params_get_channels(params, &val);
	printf("channels = %d\n", val);

	snd_pcm_hw_params_get_rate(params, &val, &dir);
	printf("rate = %d bps\n", val);

	snd_pcm_hw_params_get_period_time(params, &val, &dir);
	printf("period time = %d us\n", val);

	snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	printf("period size = %d frames\n", (int) frames);

	snd_pcm_hw_params_get_buffer_time(params, &val, &dir);
	printf("buffer time = %d us\n", val);

	snd_pcm_hw_params_get_buffer_size(params, (snd_pcm_uframes_t *) &val);
	printf("buffer size = %d frames\n", val);

	snd_pcm_hw_params_get_periods(params, &val, &dir);
	printf("periods per buffer = %d frames\n", val);

	snd_pcm_hw_params_get_rate_numden(params, &val, &val2);
	printf("exact rate = %d/%d bps\n", val, val2);

	val = snd_pcm_hw_params_get_sbits(params);
	printf("significant bits = %d\n", val);

	snd_pcm_hw_params_get_tick_time(params, &val, &dir);
	printf("tick time = %d us\n", val);

	val = snd_pcm_hw_params_is_batch(params);
	printf("is batch = %d\n", val);

	val = snd_pcm_hw_params_is_block_transfer(params);
	printf("is block transfer = %d\n", val);

	val = snd_pcm_hw_params_is_double(params);
	printf("is double = %d\n", val);

	val = snd_pcm_hw_params_is_half_duplex(params);
	printf("is half duplex = %d\n", val);

	val = snd_pcm_hw_params_is_joint_duplex(params);
	printf("is joint duplex = %d\n", val);

	val = snd_pcm_hw_params_can_overrange(params);
	printf("can overrange = %d\n", val);

	val = snd_pcm_hw_params_can_mmap_sample_resolution(params);
	printf("can mmap = %d\n", val);

	val = snd_pcm_hw_params_can_pause(params);
	printf("can pause = %d\n", val);

	val = snd_pcm_hw_params_can_resume(params);
	printf("can resume = %d\n", val);

	val = snd_pcm_hw_params_can_sync_start(params);
	printf("can sync start = %d\n", val);

	snd_pcm_close(handle);
}

void test_sound() {
	long loops;
	int rc;
	int size;
	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;
	unsigned int val;
	int dir;
	snd_pcm_uframes_t frames;
	char *buffer;

	/* Open PCM device for playback. */
	rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
	if (rc < 0) {
		fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
		exit(1);
	}

	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_alloca(&params);

	/* Fill it in with default values. */
	snd_pcm_hw_params_any(handle, params);

	/* Set the desired hardware parameters. */

	/* Interleaved mode */
	snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

	/* Signed 16-bit little-endian format */
	snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

	/* Two channels (stereo) */
	snd_pcm_hw_params_set_channels(handle, params, 2);

	/* 44100 bits/second sampling rate (CD quality) */
	val = 44100;
	snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);

	/* Set period size to 32 frames. */
	frames = 32;
	snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

	/* Write the parameters to the driver */
	rc = snd_pcm_hw_params(handle, params);
	if (rc < 0) {
		fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
		exit(1);
	}

	/* Use a buffer large enough to hold one period */
	snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	size = frames * 4; /* 2 bytes/sample, 2 channels */
	buffer = (char *) malloc(size);

	/* We want to loop for 5 seconds */
	snd_pcm_hw_params_get_period_time(params, &val, &dir);
	/* 5 seconds in microseconds divided by
	 * period time */
	printf(">>> val: %u\n", val);
	loops = 5000000 / val;

	printf(">>> loops: %u\n", loops);

	while (loops > 0) {
		loops--;
		rc = read(0, buffer, size);
		if (rc == 0) {
			fprintf(stderr, "end of file on input\n");
			break;
		} else if (rc != size) {
			fprintf(stderr, "short read: read %d bytes\n", rc);
		}
		rc = snd_pcm_writei(handle, buffer, frames);
		if (rc == -EPIPE) {
			/* EPIPE means underrun */
			fprintf(stderr, "underrun occurred\n");
			snd_pcm_prepare(handle);
		} else if (rc < 0) {
			fprintf(stderr, "error from writei: %s\n", snd_strerror(rc));
		} else if (rc != (int) frames) {
			fprintf(stderr, "short write, write %d frames\n", rc);
		}
	}

	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	free(buffer);
}

int open_audio(snd_pcm_t **handle, snd_pcm_hw_params_t **params) {
	int rc;
	unsigned int val, val2;
	int dir;
	snd_pcm_uframes_t frames;

	/* Open PCM device for playback. */
	rc = snd_pcm_open(handle, "default", SND_PCM_STREAM_PLAYBACK, 0);


	if (rc < 0) {
		fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
		return -1;
	}

	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_alloca(params);

	/* Fill it in with default values. */
	snd_pcm_hw_params_any(*handle, *params);

	/* Set the desired hardware parameters. */

	/* Interleaved mode */
	snd_pcm_hw_params_set_access(*handle, *params, SND_PCM_ACCESS_RW_INTERLEAVED);

	/* Signed 16-bit little-endian format */
	snd_pcm_hw_params_set_format(*handle, *params, SND_PCM_FORMAT_S16_LE);

	/* Two channels (stereo) */
	snd_pcm_hw_params_set_channels(*handle, *params, 2);

	/* 44100 bits/second sampling rate (CD quality) */
	val = 44100;
	snd_pcm_hw_params_set_rate_near(*handle, *params, &val, &dir);

	/* Set period size to 32 frames. */
	frames = 32;
	snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

	/* Write the parameters to the driver */
	rc = snd_pcm_hw_params(*handle, *params);

	if (rc < 0) {
		fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
		return -1;
	}

	return 1;
}

void play_note(snd_pcm_t **handle, snd_pcm_hw_params_t **params) {
	snd_pcm_uframes_t frames;
	unsigned int val;
	int dir;
	int size;
	char *buffer;
	long loops;
	int rc;

	val = 44100/2;
	frames = 32;

	printf("playing note....\n");
	/* Use a buffer large enough to hold one period */
	snd_pcm_hw_params_get_period_size(*params, &frames, &dir);
	size = frames * 4; /* 2 bytes/sample, 2 channels */
	buffer = (char *) malloc(size);

	printf(">>> --1 frames: %u \n", frames);

	/* We want to loop for 5 seconds */
	snd_pcm_hw_params_get_period_time(*params, &val, &dir);

	printf(">>> --2 val : %u\n", val);
	/* 5 seconds in microseconds divided by
	 * period time */
	loops = 15000000 / val;

	printf(">>> --3---loops: %u\n", loops);
	while (loops > 0) {
		loops--;
		rc = read(0, buffer, size);
		if (rc == 0) {
			fprintf(stderr, "end of file on input\n");
			break;
		} else if (rc != size) {
			fprintf(stderr, "short read: read %d bytes\n", rc);
		}
		rc = snd_pcm_writei(*handle, buffer, frames);
		if (rc == -EPIPE) {
			/* EPIPE means underrun */
			fprintf(stderr, "underrun occurred\n");
			snd_pcm_prepare(*handle);
		} else if (rc < 0) {
			fprintf(stderr, "error from writei: %s\n", snd_strerror(rc));
		} else if (rc != (int) frames) {
			fprintf(stderr, "short write, write %d frames\n", rc);
		}
	}

	printf(">>> --7\n");
}

void close_audio(snd_pcm_t **handle) {
	snd_pcm_drain(*handle);
	snd_pcm_close(*handle);
	printf("audio close successfully\n");
}

int main(int argc, char *argv[]) {

	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;

	if(open_audio(&handle, &params) < 0) {
		printf("Unable to open audio\n");
		return -1;
	}

	printf("audio opened successfully\n");

	play_note(&handle, &params);

	close_audio(&handle);


	// test_sound();

	return 0;
}

