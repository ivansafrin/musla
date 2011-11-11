#include <stdio.h>
#include <math.h>
#include "musla.h"

MUSLA_Song *MUSLA_ReadFile(const char *fileName) {
	MUSLA_Song *song = (MUSLA_Song*)malloc(sizeof(MUSLA_Song));
	song->duration = 5;
	song->bpm = 120;
	song->numTracks = 1;
	
	MUSLA_Track *track = (MUSLA_Track*)malloc(sizeof(MUSLA_Track));
	track->length = 16;
	//track->instrument = instrument;

	song->tracks = malloc(sizeof(void*));
	song->tracks[0] = track;
	return song;
}

double MUSLA_GetInstrumentValue(MUSLA_Instrument *instrument, double time) {
	double val;
	double freq = 440;

	double adsrVal;
	ADSRState state = ATTACK;
	
	if(time > instrument->A)
		state = DECAY;
	if(time > instrument->D)
		state = SUSTAIN;
	if(time > instrument->S)
		state = RELEASE;

	switch(state) {
		case ATTACK:
			adsrVal = time/instrument->A;
		break;
		case DECAY:
			adsrVal = 0.5+ ((time-instrument->A)/(instrument->D - instrument->A) * 0.5);
		break;
		case SUSTAIN:
			adsrVal = 0.5;
		break;
		case RELEASE:
			adsrVal = ((time-instrument->S)/(instrument->R - instrument->S) * 0.5);
		break;
	}

	val = sin(2.0 * adsrVal * M_PI * time * freq);
	//val *= adsrVal;

	return val;
}

double MUSLA_RenderFrame(MUSLA_Song *song, double time) {
	double val;
	int i;

	MUSLA_Instrument ins;
	ins.A = 2.5;
	ins.D = 0.2;	
	ins.S = 0.7;
	ins.R = 0.3;
	val = MUSLA_GetInstrumentValue(&ins, time);

	for(i = 0; i < song->numTracks; i++) {
		MUSLA_Track *track = song->tracks[i];
	}
//	double freq = 440;
//	val = sin(2.0 * M_PI * time * freq);
	return val;
}

double *MUSLA_RenderSong(MUSLA_Song *song, double sampleRate) {
	long numFrames = song->duration * sampleRate;
	double *buffer = (double *) malloc(numFrames * sizeof(double));

	long f;
	for (f=0 ; f<numFrames ; f++) {
		double time = f * song->duration / numFrames;
		buffer[f] = MUSLA_RenderFrame(song, time);	
	}

	return buffer;
}

int MUSLA_WriteSong(MUSLA_Song *song, const char *fileName, double sampleRate) {
	long numFrames = song->duration * sampleRate;

	double *buffer = MUSLA_RenderSong(song, sampleRate);
	if (buffer == NULL) {
		fprintf(stderr, "Could not allocate buffer for output\n");
	}

	SF_INFO info;
	info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	info.channels = 1;
	info.samplerate = sampleRate;

	SNDFILE *sndFile = sf_open(fileName, SFM_WRITE, &info);
	if (sndFile == NULL) {
		fprintf(stderr, "Error opening sound file '%s': %s\n", fileName, sf_strerror(sndFile));
		free(buffer);
		return -1;
	}
	long writtenFrames = sf_writef_double(sndFile, buffer, numFrames);
	if (writtenFrames != numFrames) {
		fprintf(stderr, "Did not write enough frames for source\n");
		sf_close(sndFile);
		free(buffer);
		return -1;
	}

	sf_write_sync(sndFile);
	sf_close(sndFile);
	free(buffer);
}

void MUSLA_DestroySong(MUSLA_Song *song) {
	free(song);
}

int main(int argc, char *argv[]) {
	printf("MUSical LAnguage compiler.\n");
	MUSLA_Song *song = MUSLA_ReadFile("test.mus");
	MUSLA_WriteSong(song, "test.wav", 44100);
	MUSLA_DestroySong(song);
	return 0;
}
