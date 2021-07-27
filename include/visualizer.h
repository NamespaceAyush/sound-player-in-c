#ifndef VISUALIZER_H
#define VISUALIZER_H
#include <fftw3.h>
#include <SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <getopt.h>
#include <pthread.h>
#include <semaphore.h>

#define WIDTH 840
#define HEIGHT 600
#define SAMPLE_RATE 44100
#define BARS 300
#define THICKNESS 2
#define DISTANCE 5
#define FIT_FACTOR 30
struct HSV
{
double h,s,v;
};

struct RGB
{
double r,g,b;
};

void createRGB(struct RGB*,double,double,double);
void conversion(struct HSV,struct RGB*);


struct AudioData
{
Uint8* position;
Uint32 length;
SDL_AudioFormat format;
fftw_plan plan;
fftw_complex *in;
fftw_complex *out;
SDL_Renderer *renderer;
struct RGB* color; 
SDL_Point* timeDomain;
};

struct wrapper
{
Uint8* stream;
struct AudioData* audio;
};

static sem_t play;
static int NSAMPLES;
static int MODE;

void forwardChunk(void*,Uint8*,int);
double Get16bitAudioSample(Uint8*,SDL_AudioFormat);
void* visualizerOutput(void*);
void changeMode();








#endif
