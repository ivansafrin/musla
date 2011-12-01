#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

void MUSLA_Error(const char *msg, int lineNumber) {
	printf("Error on line %d (%s)\n", lineNumber, msg);
}

int MUSLA_SetSongOptions(MUSLA_Song *song, char *line, int lineNumber) {
	char *pch = strtok (line, " ");
	int tokenIndex = 0;
	while (pch) {
		switch(tokenIndex) {
			case 0:
			break;
			case 1:
			{
				double songDuration = atof(pch);	
				printf("Song duration: %f\n", songDuration);	
				song->duration = songDuration;
			}
			break;
			case 2:
			{
				double songBpm = atof(pch);	
				printf("Song BPM: %f\n", songBpm);	
				song->bpm = songBpm;
			}
			break;
			default:
				MUSLA_Error("Too many options for S(ong).", lineNumber);
				return 0;
			break;
		}
		pch = strtok (NULL, " ");
		tokenIndex++;
	}
	if(tokenIndex != 3) {
		MUSLA_Error("Not enough options for S(ong).", lineNumber);
		return 0;
	}
	return 1;
}


int MUSLA_AddInstrument(MUSLA_Song *song, char *line, int lineNumber) {
	MUSLA_Instrument *ins = (MUSLA_Instrument*)malloc(sizeof(MUSLA_Instrument));;
	char *pch = strtok (line, " ");
	int tokenIndex = 0;
	while (pch) {
		switch(tokenIndex) {
			case 0:
			break;
			case 1:
				strcpy(ins->name, pch);
				printf("Adding instrument: %s\n", ins->name);
			break;
			case 2:
				if(strlen(pch) != 1) {
					MUSLA_Error("Invalid instrument type.", lineNumber);
					return 0;
				}
				ins->type = pch[0];
			break;
			case 3:
			{
				double A = atof(pch);	
				ins->A = A;
			}
			break;
			case 4:
			{
				double D = atof(pch);	
				ins->D = D;
			}
			break;
			case 5:
			{
				double S = atof(pch);	
				ins->S = S;
			}
			break;
			case 6:
			{
				double R = atof(pch);	
				ins->R = R;
			}
			break;
			case 7:
			{
				double sustainLevel = atof(pch);	
				ins->sustainLevel = sustainLevel;
			}
			break;
			default:
				MUSLA_Error("Too many options for I(nstrument).", lineNumber);
				return 0;
			break;
		}
		pch = strtok (NULL, " ");
		tokenIndex++;
	}
	if(tokenIndex != 8) {
		MUSLA_Error("Not enough options for I(nstrument).", lineNumber);
		return 0;
	}

	song->numInstruments++;
	song->instruments = realloc(song->instruments, sizeof(void*) * song->numInstruments);
	song->instruments[song->numInstruments-1] = ins;
	
	return 1;
}

int MUSLA_AddPattern(MUSLA_Song *song, char *line, int lineNumber) {
	MUSLA_Pattern *pat = (MUSLA_Pattern*)malloc(sizeof(MUSLA_Pattern));;
	char *pch = strtok (line, " ");
	int tokenIndex = 0;
	int chordPos = 0;
	int isChord = 0;

	char notes[2048][8];
	memset(notes, 255, 2048*8);
	int i;
	int noteLen = 0;

	while (pch) {
		switch(tokenIndex) {
			case 0:
			break;
			case 1:
				strcpy(pat->name, pch);
				printf("Adding pattern: %s\n", pat->name);
			break;
			default:
			{
				for(i=0; i < strlen(pch); i++) {
					switch(pch[i]) {
						case 'A':
							notes[noteLen][chordPos] = 9;
							if(isChord) {
								chordPos++;
							} else {
								noteLen++;
							}
						break;
						case 'B':
							notes[noteLen][chordPos] = 11;
							if(isChord) {
								chordPos++;
							} else {
								noteLen++;
							}
						break;
						case 'C':
							notes[noteLen][chordPos] = 0;
							if(isChord) {
								chordPos++;
							} else {
								noteLen++;
							}
						break;
						case 'D':
							notes[noteLen][chordPos] = 2;
							if(isChord) {
								chordPos++;
							} else {
								noteLen++;
							}
						break;
						case 'E':
							notes[noteLen][chordPos] = 4;
							if(isChord) {
								chordPos++;
							} else {
								noteLen++;
							}
						break;
						case 'F':
							notes[noteLen][chordPos] = 5;
							if(isChord) {
								chordPos++;
							} else {
								noteLen++;
							}
						break;
						case 'G':
							notes[noteLen][chordPos] = 7;
							if(isChord) {
								chordPos++;
							} else {
								noteLen++;
							}
						break;
						case '#':	
							if(noteLen > 0 || (isChord && chordPos > 0)) {
								if(isChord) {
									notes[noteLen][chordPos-1]--;
								} else {
									notes[noteLen-1][chordPos]--;
								}
							} else {
								MUSLA_Error("# is not a valid note.", lineNumber);
								return 0;
							}
						break;
						case '1':
						case '2':
						case '3':
						case '4':
						case '5':
						case '6':
						case '7':
						case '8':
						{
							if(noteLen > 0 || (isChord && chordPos > 0)) {
								char val[2];
								val[0] = pch[i];
								val[1] = '\0';
								if(isChord) {
									notes[noteLen][chordPos-1] += (12*atoi(val));
								} else {
									notes[noteLen-1][chordPos] += (12*atoi(val));
								}
							} else {
								char _err[128];
								sprintf(_err, "%c is not a valid note.", pch[i]);
								MUSLA_Error(_err, lineNumber);
								return 0;
							}
						}
						break;
						case '.':
							if(isChord) {
								MUSLA_Error("Only notes are allowed inside chord.", lineNumber);
								return 0;
							}
							// pause
							notes[noteLen][chordPos] = 255;
							noteLen++;
						break;
						case '-':
							if(isChord) {
								MUSLA_Error("Only notes are allowed inside chord.", lineNumber);
								return 0;
							}
							// sustain
							notes[noteLen][chordPos] = 254;
							noteLen++;
						break;
						case '[':
							chordPos = 0;
							isChord = 1;
						break;
						case ']':
							chordPos = 0;
							isChord = 0;
							noteLen++;
						break;
						case '\n':
						break;
						default:
							{
							char _err[128];
							sprintf(_err, "%c is not a valid note.", pch[i]);
							MUSLA_Error(_err, lineNumber);
							return 0;
							}
						break;
					}
				}
			}
			break;
		}
		pch = strtok (NULL, " ");
		tokenIndex++;
	}
	if(tokenIndex < 3) {
		MUSLA_Error("Not enough options for P(attern).", lineNumber);
		return 0;
	}

	pat->length = noteLen;
	memcpy(pat->notes, notes, 2048*8);

	song->numPatterns++;
	song->patterns = realloc(song->patterns, sizeof(void*) * song->numPatterns);
	song->patterns[song->numPatterns-1] = pat;
	
	return 1;
}

MUSLA_Instrument *MUSLA_GetInstrumentByName(MUSLA_Song *song, char *name) {
	int i;
	for(i = 0; i < song->numInstruments; i++) {
		MUSLA_Instrument *ins = song->instruments[i];
		if(strcmp(name, ins->name) == 0) {
			return ins;
		}
	}
	return 0;
}

MUSLA_Pattern *MUSLA_GetPatternByName(MUSLA_Song *song, char *name) {
	int i;
	for(i = 0; i < song->numPatterns; i++) {
		MUSLA_Pattern *pat = song->patterns[i];
		if(strcmp(name, pat->name) == 0) {
			return pat;
		}
	}
	return 0;
}

int MUSLA_AddTrack(MUSLA_Song *song, char *line, int lineNumber) {
	MUSLA_Track *track = (MUSLA_Track*)malloc(sizeof(MUSLA_Track));;
	char *pch = strtok (line, " ");
	int tokenIndex = 0;
	
	
	char pats[2048];
	int patLen = 0;

	while (pch) {
		switch(tokenIndex) {
			case 0:
			break;
			case 1:
			{
				double volume = atof(pch);	
				track->volume = volume;
			}
			break;
			case 2:
			{
				double baseOctave = atoi(pch);	
				track->baseOctave = baseOctave;
			}
			break;
			case 3:
			{
				double resolution = atof(pch);	
				track->resolution = resolution;
			}
			break;
			case 4:
			{
				MUSLA_Instrument *ins = MUSLA_GetInstrumentByName(song, pch);
				if(ins) {
					track->instrument = ins;
				} else {
					char _err[128];
					sprintf(_err, "Could not find instrument %s.", pch);
					MUSLA_Error(_err, lineNumber);
					return 0;
				}
			}
			break;
			case 5:
			{
				MUSLA_Pattern *pat = MUSLA_GetPatternByName(song, pch);
				if(pat) {
					track->pattern = pat;
				} else {
					char _err[128];
					sprintf(_err, "Could not find pattern %s.", pch);
					MUSLA_Error(_err, lineNumber);
					return 0;
				}
			}
			break;
			default:
				pats[patLen] = atoi(pch);
				patLen++;
			break;
		}
		pch = strtok (NULL, " ");
		tokenIndex++;
	}

	if(tokenIndex < 6) {
		MUSLA_Error("Not enough options for T(track).", lineNumber);
		return 0;
	}

	track->length = patLen;
	memcpy(track->patmap, pats, patLen);
	printf("Adding track with instrument: %s and pattern: %s\n", track->instrument->name, track->pattern->name);

	song->numTracks++;
	song->tracks = realloc(song->tracks, sizeof(void*) * song->numTracks);
	song->tracks[song->numTracks-1] = track;
	
	return 1;
}

int MUSLA_ProcessLine(MUSLA_Song *song, char *line, int lineNumber) {
	char _err[128];
	switch(line[0]) {
		case 'S':
			return MUSLA_SetSongOptions(song, line, lineNumber);
		break;
		case 'I':
			return MUSLA_AddInstrument(song, line, lineNumber);
		break;
		case 'P':
			return MUSLA_AddPattern(song, line, lineNumber);
		break;
		case 'T':
			return MUSLA_AddTrack(song, line, lineNumber);
		break;
		case '#':
		case ' ':
		case '\n':
			// comments
		break;
		default:
			sprintf(_err, "%c is not a valid definition type.", line[0]);
			MUSLA_Error(_err, lineNumber);
			return 0;
		break;
	}
	return 1;
}

MUSLA_Song *MUSLA_ReadFile(const char *fileName) {
	MUSLA_Song *song = (MUSLA_Song*)malloc(sizeof(MUSLA_Song));
	song->duration = 5;
	song->bpm = 120;
	song->numTracks = 0;
	song->numInstruments = 0;
	song->numPatterns = 0;
	song->instruments = malloc(sizeof(void*));
	song->tracks = malloc(sizeof(void*));
	song->patterns = malloc(sizeof(void*));

	FILE *file = fopen(fileName, "r");
	if (!file) {
		printf("Invalid input file specified\n");
		return 0;
	}
	
	int lineNumber = 0;
	char line[4096];
	while(fgets(line, sizeof(line), file)) {
		lineNumber++;
		if(!MUSLA_ProcessLine(song, line, lineNumber)) {
			return 0;
		}
	}
	return song;
}

void clampVal(double *val, double min, double max) {
	if(*val < min)
		*val = min;
	if(*val > max)
		*val = max;
}

double MUSLA_GetInstrumentValue(MUSLA_Instrument *instrument, double time, double freq, double sustainExtra) {
	double val;
	double adsrVal;
	ADSRState state = ATTACK;
	
	double sustain = instrument->S + sustainExtra;

	if(time >= instrument->A)
		state = DECAY;
	if(time >= instrument->D + instrument->A)
		state = SUSTAIN;
	if(time >= sustain + instrument->D + instrument->A)
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
			adsrVal = sustainLevel * (1.0-((time-instrument->A-instrument->D-sustain)/instrument->R)); 
		break;
	}

	double baseVal = 0;
	switch(instrument->type) {
		case 'S':
			baseVal = sin((2.0 * M_PI * time * freq));
		break;
		case 'Q':
			baseVal = (fmod(time * freq, 1.0) < 0.5) ? 1.0 : 0.0;
			baseVal = 1.0 - (baseVal * 2.0);
		break;
		case 'T':
			baseVal = 1.0-(fabs(1.0 - fmod(time * freq, 2.0))*2.0);
		break;
		case 'N':
			baseVal = -1.0 + (((double)rand()/(double)RAND_MAX)*2.0);
		break;
	}
		

	clampVal(&adsrVal, 0.0, 1.0);
	val = adsrVal * baseVal;
	return val;
}

double MUSLA_RenderTrackAtOffset(MUSLA_Track *track, int octaveOffset, double time, double timeOffset, double bpm) {
	double val;
	if(octaveOffset >= track->length)
		octaveOffset  = octaveOffset % track->length;
	int base_octave = track->baseOctave + track->patmap[octaveOffset];

	MUSLA_Pattern *pat = track->pattern;
	int notemap = floor((time-timeOffset) / (60.0/bpm));
	if(notemap >= pat->length)
		notemap  = notemap % pat->length;
	
	int chordCount = 0;
	int i;
	val = 0;
	for(i=0; i < 8; i++) {

	if(pat->notes[notemap][i] == (char)255)  {
		val += 0;
	} else if(pat->notes[notemap][i] == (char)254) {
	//	int noteIndex = pat->notes[notemap-1][i] + (12*base_octave);
	//	if(noteIndex > 95)
	//		noteIndex = 95;
	//	val = MUSLA_GetInstrumentValue(track->instrument, fmod(time, 60.0/bpm) + timeOffset, notes_freq_map[noteIndex], 0);
		val += 0;
	} else {
		int noteIndex = pat->notes[notemap][i] + (12*base_octave);
		if(noteIndex > 95)
			noteIndex = 95;

		double sustainExtra = 0;
		int nextNotemap = notemap + 1;
		if(nextNotemap >= pat->length)
			nextNotemap  = nextNotemap % pat->length;
			
		if(pat->notes[nextNotemap][i] == (char)254) {
			sustainExtra = (60.0/bpm);
		}
		val += MUSLA_GetInstrumentValue(track->instrument, fmod(time, 60.0/bpm) + timeOffset, notes_freq_map[noteIndex], sustainExtra/4);
		chordCount++;
	}

	}

	if(!chordCount)
		chordCount = 1;
	return val/chordCount;
}

double MUSLA_RenderTrack(MUSLA_Track *track, double time, double songBpm) {
	double val;
	double bpm = songBpm/track->resolution;

	double instrumentDuration = track->instrument->A + track->instrument->D + track->instrument->S + track->instrument->R;
	int backSamples = ceil(instrumentDuration / (60.0/bpm));
	int j;
	val = 0;
	for(j=0; j < backSamples; j++) {
			double timeOffset = (60.0/bpm) * ((double)j);	
			int octaveOffset = floor((time-timeOffset)/(track->pattern->length*(60.0/bpm)));
			if(octaveOffset >= 0) {
				val += MUSLA_RenderTrackAtOffset(track, octaveOffset, time, timeOffset, bpm);
			}
	}
	
	// TODO: Fix this hacky normalization
	val /= ((double)backSamples);
	//val *= 2.0;
	return val;
}

double MUSLA_RenderFrame(MUSLA_Song *song, double time) {
	double val = 0;
	int i;

	for(i = 0; i < song->numTracks; i++) {
		MUSLA_Track *track = song->tracks[i];
		val += (MUSLA_RenderTrack(track, time, song->bpm) * track->volume);
	}
	val /= ((double)song->numTracks);
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
	printf("MUS(ical) LA(nguage) command line synthesizer.\n\n");
	if(argc != 3) {
		printf("Invalid options. See below for proper command format\n");
		printf("\n\n./musla input.mus output.wav\n\n\n");
		return 1;
	}
	MUSLA_Song *song = MUSLA_ReadFile(argv[1]);

	if(!song) {
		printf("Aborting...\n");
		return 1;
	}

	MUSLA_WriteSong(song, argv[2], 44100);
	MUSLA_DestroySong(song);
	
	printf("Song written to file!\n");
	return 0;
}
