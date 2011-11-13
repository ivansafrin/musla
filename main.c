#include <stdio.h>
#include <math.h>
#include "musla.h"

double notes_freq_map[] = { 16.35, 17.32, 18.35, 19.45, 20.6, 21.83, 23.12, 24.5, 25.96, 27.5, 29.14, 30.87,
        32.7, 34.65, 36.71, 38.89, 41.2, 43.65, 46.25, 49, 51.91, 55, 58.27, 61.74,
        65.41, 69.3, 73.42, 77.78, 82.41, 87.31, 92.5, 98, 103.83, 110, 116.54, 123.47,
        130.81, 138.59, 146.83, 155.56, 164.81, 174.61, 185, 196, 207.65, 220, 233.08, 246.94,
        261.63, 277.18, 293.66, 311.13, 329.63, 349.23, 369.99, 392, 415.3, 440, 466.16, 493.88,
        523.25, 554.37, 587.33, 622.25, 659.26, 698.46, 739.99, 783.99, 830.61, 880, 932.33, 987.77,
        1046.5, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22, 1760, 1864.66, 1975.53,
        2093, 2217.46, 2349.32, 2489.02, 2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520, 3729.31, 3951.07,
        4186.01, 4434.92, 4698.64, 4978.03};

MUSLA_Song *MUSLA_ReadFile(const char *fileName) {
	MUSLA_Song *song = (MUSLA_Song*)malloc(sizeof(MUSLA_Song));
	song->duration = 16;
	song->bpm = 120;
	song->numTracks = 1;
	
	MUSLA_Track *track = (MUSLA_Track*)malloc(sizeof(MUSLA_Track));
	track->length = 16;
	//track->instrument = instrument;

	song->tracks = malloc(sizeof(void*));
	song->tracks[0] = track;
	return song;
}

void clampVal(double *val, double min, double max) {
	if(*val < min)
		*val = min;
	if(*val > max)
		*val = max;
}

double MUSLA_GetInstrumentValue(MUSLA_Instrument *instrument, double time, double freq) {
	double val;
	double adsrVal;
	ADSRState state = ATTACK;
	
	if(time >= instrument->A)
		state = DECAY;
	if(time >= instrument->D + instrument->A)
		state = SUSTAIN;
	if(time >= instrument->S + instrument->D + instrument->A)
		state = RELEASE;

	double sustainLevel = instrument->sustainLevel;
	switch(state) {
		case ATTACK:
			adsrVal = time/instrument->A;
		break;
		case DECAY:
			adsrVal = sustainLevel + ((1.0-sustainLevel) * (1.0-((time-instrument->A)/instrument->D)));
		break;
		case SUSTAIN:
			adsrVal = sustainLevel;
		break;
		case RELEASE:
			adsrVal = sustainLevel * (1.0-((time-instrument->A-instrument->D-instrument->S)/instrument->R)); 
		break;
	}

	clampVal(&adsrVal, 0.0, 1.0);
	val = adsrVal * sin((2.0 * M_PI * time * freq));
	return val;
}

double MUSLA_RenderFrame(MUSLA_Song *song, double time) {
	double val;
	int i;

	MUSLA_Instrument ins;
	ins.A = 0.1;
	ins.D = 0.1;	
	ins.S = 0.2;
	ins.R = 0.1;
	ins.sustainLevel = 0.1;

	MUSLA_Pattern pat;
	pat.length = 16;
	char notes[16] = {0,1,2,3,4,5,6,7,8,9,10,11,11,11,11,11};
	memcpy(pat.notes, notes, 16);
	
	MUSLA_Track track;
	track.pattern = &pat;
	track.length = 2;
	track.baseOctave = 3;
	track.resolution = 0.5;
	char pats[2] = {-1,2};
	memcpy(track.patmap, pats, 2);
	
	double bpm = song->bpm/track.resolution;
	
	int octaveOffset = floor(time/(pat.length*(60.0/bpm)));
	if(octaveOffset >= track.length)
		octaveOffset  = octaveOffset % track.length;
	int base_octave = track.baseOctave + track.patmap[octaveOffset];

	int notemap = floor(time / (60.0/bpm));
	if(notemap >= pat.length)
		notemap  = notemap % pat.length;

	int noteIndex = pat.notes[notemap] + (12*base_octave);
	if(noteIndex > 95)
		noteIndex = 95;
	val = MUSLA_GetInstrumentValue(&ins, fmod(time, 60.0/bpm), notes_freq_map[noteIndex]);


	for(i = 0; i < song->numTracks; i++) {
		MUSLA_Track *track = song->tracks[i];
	}
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
