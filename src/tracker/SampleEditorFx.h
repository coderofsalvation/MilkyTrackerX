#pragma once

#ifndef SAMPLEDITORFX_H
#define SAMPLEDITORFX_H

#include "BasicTypes.h"
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

typedef float real;
typedef struct { real Re; real Im; } complex;

#ifndef PI
#define PI 3.14159265358979323846264338327950288
#endif

struct Filter {
	float lp;     // lowpass output
	float hp;     // highpass output
	float bp;     // bandpass output
	float notch;  // notch output
	float cutoff; // hz
	float q;      // 0-1
};

extern void print_vector(const char* title, complex* x, int n);
extern complex complex_mult(complex a, complex b);
extern void fft(complex* v, int n, complex* tmp);
extern void ifft(complex* v, int n, complex* tmp);
extern int convolve(float* x, float* h, int lenX, int lenH, float** output);

extern void filter(struct Filter* f, float input);
extern void filter_init(struct Filter* f);
extern void xfade(float* in, float* out, pp_int32 len, int fadein /*percent*/, int fadeout /*percent*/);

class SampleEditorFx
{
};
#endif