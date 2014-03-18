/*
 * playsound.c
 *
 *  Created on: Mar 1, 2014
 *      Author: renan
 */

#include <stdio.h>

// #define ALSA_PCM_NEW_HW_PARAMS_API

#include <alsa/asoundlib.h>

static snd_pcm_t *handle;
static snd_pcm_hw_params_t *params;
static snd_pcm_uframes_t frames;

int dir;
int rc;

void open_audio() {

	unsigned int rate;




	printf(">>> 1 - handle: %u\n", handle);

	/* Open PCM device for playback. */
	rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);

	printf(">>> 2 - handle: %u\n", handle);

	if (rc < 0) {
		fprintf(stderr, "unable to open pcm device: %s\n", snd_strerror(rc));
		return;
	}

	/* Allocate a hardware parameters object. */
	snd_pcm_hw_params_alloca(&params);

	/* Fill it in with default values. */
	snd_pcm_hw_params_any(handle, params);

	printf(">>> 3 - handle: %u\n", handle);

	/* Set the desired hardware parameters. */

	/* Interleaved mode */
	snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

	printf(">>> 4 - handle: %u\n", handle);

	/* Signed 16-bit little-endian format */
	rc = snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);
	printf(">>> 5 - result: %i\n", rc);
	printf(">>> 5 - handle: %u\n", handle);

	/* Two channels (stereo) */
	rc = snd_pcm_hw_params_set_channels(handle, params, 1);

	printf(">>> 6 - result: %i\n", rc);
	printf(">>> 6 - handle: %u\n", handle);

	/* 44100 bits/second sampling rate (CD quality) */
	rate = 44100;
	rc = snd_pcm_hw_params_set_rate_near(handle, params, &rate, &dir);

	printf(">>> 7 - result: %i\n", rc);
	printf(">>> 7 - handle: %u\n", handle);

	/* Set period size to 32 frames. */
	frames = 32;
	rc = snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);
	printf(">>> 8 - result: %i\n", rc);
	printf(">>> 8 - handle: %u\n", handle);

	/* Write the parameters to the driver */
	rc = snd_pcm_hw_params(handle, params);
	printf(">>> 9 - result: %i\n", rc);
	printf(">>> 9 - handle: %u\n", handle);
	printf(">>> 9 - params: %u\n", params);

	rc = snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	printf(">>> XXXXX - frames = %d\n", frames);

	if (rc < 0) {
		fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
		return;
	}

	frames = 1;
	rc = snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	printf(">>> YYYY - a - frames = %d\n", frames);

	frames = 2;
	rc = snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	printf(">>> YYYY - b - frames = %d\n", frames);

	frames = 3;
	rc = snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	printf(">>> YYYY - c - frames = %d\n", frames);

}

void play_note() {
	int ret;
	char *msg;

	/*	snd_pcm_uframes_t frames;
	 unsigned int val;
	 int dir;
	 int size;
	 char *buffer;
	 long loops;
	 int rc;
	 int i;

	 val = 44100/2;
	 frames = 32;

	 printf("playing note....\n");
	 Use a buffer large enough to hold one period
	 snd_pcm_hw_params_get_period_size(*params, &frames, &dir);
	 size = frames * 4;  2 bytes/sample, 2 channels
	 buffer = (char *) malloc(size);


	 We want to loop for 5 seconds
	 snd_pcm_hw_params_get_period_time(*params, &val, &dir);

	 5 seconds in microseconds divided by
	 * period time
	 loops = 15000000 / val;

	 while (loops > 0) {
	 loops--;
	 for(i=0; i < size; i++) {
	 buffer[i] = 200;
	 }
	 rc = read(0, buffer, size);
	 if (rc == 0) {
	 fprintf(stderr, "end of file on input\n");
	 break;
	 } else if (rc != size) {
	 fprintf(stderr, "short read: read %d bytes\n", rc);
	 }
	 rc = snd_pcm_writei(*handle, buffer, frames);
	 if (rc == -EPIPE) {
	 EPIPE means underrun
	 fprintf(stderr, "underrun occurred\n");
	 snd_pcm_prepare(*handle);
	 } else if (rc < 0) {
	 fprintf(stderr, "error from writei: %s\n", snd_strerror(rc));
	 } else if (rc != (int) frames) {
	 fprintf(stderr, "short write, write %d frames\n", rc);
	 }
	 }

	 */
}

void close_audio() {
	snd_pcm_drain(handle);
	snd_pcm_close(handle);
	printf("audio close successfully\n");
}

void main(int argc, char *argv[]) {

	open_audio();

	printf("audio opened successfully\n");

	frames = 5;
	rc = snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	printf(">>> YYYY - d - frames = %d\n", frames);

	// play_note();
	//close_audio(&handle);
}

