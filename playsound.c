/*
 * playsound.c
 *
 *  Created on: Mar 1, 2014
 *      Author: renan
 */

#include <alsa/asoundlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#include <string.h>

snd_pcm_uframes_t frames;

static void set_hardware_parameters(snd_pcm_t *handle,
		snd_pcm_hw_params_t *params) {
	int rc;
	int dir;
	unsigned int rate;

	/* Fill it in with default values. */
	snd_pcm_hw_params_any(handle, params);

	/* Set the desired hardware parameters. */

	/* Interleaved mode */
	snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

	/* Signed 16-bit little-endian format */
	rc = snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

	/* Two channels (stereo) */
	rc = snd_pcm_hw_params_set_channels(handle, params, 1);

	/* 44100 bits/second sampling rate (CD quality) */
	rate = 44100;
	rc = snd_pcm_hw_params_set_rate_near(handle, params, &rate, &dir);

	/* Set period size to 32 frames. */
	frames = 32;
	rc = snd_pcm_hw_params_set_period_size_near(handle, params, &frames, &dir);

	/* Write the parameters to the driver */
	rc = snd_pcm_hw_params(handle, params);

	if (rc < 0) {
		fprintf(stderr, "unable to set hw parameters: %s\n", snd_strerror(rc));
		return;
	}
}

void play_note(snd_pcm_t *handle, snd_pcm_hw_params_t *params) {
	int dir;
	snd_pcm_uframes_t frames;
	snd_pcm_uframes_t buffer_size;
	unsigned int period_time;
	unsigned int rate;
	unsigned int periods;
	char *buffer;
	int size;
	int loops;
	int i;
	int rc;

	snd_pcm_hw_params_get_period_size(params, &frames, &dir);
	snd_pcm_hw_params_get_period_time(params, &period_time, &dir);
	snd_pcm_hw_params_get_periods(params, &periods, &dir);
	snd_pcm_hw_params_get_rate(params, &rate, &dir);
	snd_pcm_hw_params_get_buffer_size(params, &buffer_size);

	size = frames * 2;
	buffer = (char *) malloc(size);

	printf("frames = %i\n", (int)frames);
	printf("period time = %i\n", period_time);
	printf("periods = %i\n", periods);
	printf("rate = %i\n", rate);
	printf("buffer size = %i\n", (int)buffer_size);

	printf("calculated buffer size = %i\n", size);

	loops = 5000000 / period_time;
	printf("loops = %i\n", loops);
	while (loops > 0) {
		loops--;
		for (i = 0; i < size; i++) {
			buffer[i] = (char) i*i*i;
		}
		rc = snd_pcm_writei(handle, buffer, frames);
		if(rc<0) {
			printf(">>> rc = %i\n", rc);
		}


	    if (rc == -EPIPE) {
	      /* EPIPE means underrun */
	      fprintf(stderr, "underrun occurred\n");
	      snd_pcm_prepare(handle);
	    } else if (rc < 0) {
	      fprintf(stderr,
	              "error from writei: %s\n",
	              snd_strerror(rc));
	    }  else if (rc != (int)frames) {
	      fprintf(stderr,
	              "short write, write %d frames\n", rc);
	    }
	}
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
	play_note(handle, params);

	// close audio
	close_audio(handle);
	return 0;
}

