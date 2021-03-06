
#include <sndfile.h>

typedef struct {
	double A;
	double D;
	double S;
	double R;
	double sustainLevel;
	char name[16];
	char type;
} MUSLA_Instrument;

typedef enum {
   ATTACK,
   DECAY,
   SUSTAIN,
   RELEASE
} ADSRState;
 

typedef struct {
	char name[16];
	int length;
	char notes[2048][8];
} MUSLA_Pattern;

typedef struct {
	MUSLA_Instrument *instrument;
	int length;
	MUSLA_Pattern *pattern;
	char patmap[64];
	int baseOctave;
	double resolution;
	double volume;
} MUSLA_Track;

typedef struct {
	double duration;
	double bpm;

	int numTracks;
	MUSLA_Track **tracks;

	int numInstruments;
	MUSLA_Instrument **instruments;

	int numPatterns;
	MUSLA_Pattern **patterns;

} MUSLA_Song;


MUSLA_Song *MUSLA_ReadFile(const char *fileName);
int MUSLA_WriteSong(MUSLA_Song *song, const char *fileName, double sampleRate);
void MUSLA_DestroySong(MUSLA_Song *song);
double *MUSLA_RenderSong(MUSLA_Song *song, double sampleRate);
double MUSLA_RenderFrame(MUSLA_Song *song, double time);
double MUSLA_GetInstrumentValue(MUSLA_Instrument *instrument, double time, double freq, double sustainExtra);

void MUSLA_Error(const char *msg, int lineNumber);
