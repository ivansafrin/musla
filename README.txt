musla
=====

Command line synthesizer and musical markup language

Requires libsndfile, you can get it here: http://www.mega-nerd.com/libsndfile/

Simple tutorial below. See .mus files for more examples. 

# This is a comment (line must start with #)
# S is for Song: DURATION(IN SECONDS) BPM
S 16 120
# I is for Instrument: NAME TYPE(S=SINE T=TRIANGLE Q=SQUARE N=NOISE)
# ADSR VALUES IN SECONDS SUSTAIN VOLUME
I LEAD T 0.1 0.05 0.1 0.1 0.5
# P is for Pattern NAME followed by pattern notes
# You can add # after a note and use . for pauses
# You can add octave offsets after a note (e.g. E1G2) 
P PAT1 EDCDEEE.DDD.EGG.
# T is for Track: VOLUME BASE_OCTAVE BPM_MOD INSTRUMENT PATTERN
# followed by pattern octave offsets 
T 1.0 3 0.75 LEAD PAT1 1 2
