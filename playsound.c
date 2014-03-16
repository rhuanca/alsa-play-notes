/*
 * playsound.c
 *
 *  Created on: Mar 1, 2014
 *      Author: renan
 */

#include <alsa/asoundlib.h>
#include <alsa/error.h>
#include <alsa/pcm.h>
#include <alsa/version.h>
#include <stdio.h>
#include <string.h>

int open_audio(snd_pcm_t **handle, snd_pcm_hw_params_t **params) {
	int rc;
	unsigned int rate;
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
	snd_pcm_hw_params_set_access(*handle, *params,
			SND_PCM_ACCESS_RW_INTERLEAVED);

	/* Signed 16-bit little-endian format */
	snd_pcm_hw_params_set_format(*handle, *params, SND_PCM_FORMAT_S16_LE);

	/* Two channels (stereo) */
	snd_pcm_hw_params_set_channels(*handle, *params, 1);

	/* 44100 bits/second sampling rate (CD quality) */
	rate = 44100;
	snd_pcm_hw_params_set_rate_near(*handle, *params, &rate, &dir);

	/* Write the parameters to the driver */
	rc = snd_pcm_hw_params(*handle, *params);

	if (rc < 0) {
		fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
		return -1;
	}

	return 1;
}

void play_note(snd_pcm_t **handle, snd_pcm_hw_params_t **params) {
	int dir;
	snd_pcm_uframes_t frames;
	printf(">>> *params = %u\n", *params);
	printf(">>> params = %u\n", params);
	snd_pcm_hw_params_get_period_size(*params, &frames, &dir);
	printf("period size = %u frames\n", (int) frames);

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

void close_audio(snd_pcm_t **handle) {
	snd_pcm_drain(*handle);
	snd_pcm_close(*handle);
	printf("audio close successfully\n");
}

int main(int argc, char *argv[]) {

	snd_pcm_t *handle;
	snd_pcm_hw_params_t *params;

	if (open_audio(&handle, &params) < 0) {
		printf("Unable to open audio\n");
		return -1;
	}

	printf("audio opened successfully\n");
	play_note(&handle, &params);

	close_audio(&handle);

	// test_sound();

	return 0;
}

