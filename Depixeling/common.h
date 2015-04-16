#ifndef COMMON_H
#define COMMON_H


//#include "win32.h"
#include <math.h>
#include <iostream>
#include <assert.h>
#include <vector>

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#endif


// constants

#define EPSILON         1e-5
#define MAX_LINE        1024
#define MAX_PATH        1024
#define MAX_NAME	128

#ifndef M_PI            // from math.h
  #define M_PI          3.14159265358979323846
  #define M_PI_2        1.57079632679489661923
  #define M_PI_4        0.78539816339744830962
  #define M_1_PI        0.31830988618379067154
  #define M_2_PI        0.63661977236758134308
  #define M_E           2.7182818284590452354
#endif // M_PI

/*
* definitions for the required sampling techniques
*/
#define I_NEAREST	0
#define I_BILINEAR	1
#define I_GAUSSIAN	2

// macros

#define sqr(x)          ((x)*(x))
#define cube(x)		((x) * (x) * (x))
#define max(a,b)        ((a)>(b)?(a):(b))
#define min(a,b)        ((a)<(b)?(a):(b))

#define deg2rad(x)      ((x)*M_PI/180.0)
#define rad2deg(x)      ((x)*180.0/M_PI)

#define clamp(x,a,b)    (  ((a)<(b))				\
			 ? ((x)<(a))?(a):(((x)>(b))?(b):(x))	\
			 : ((x)<(b))?(b):(((x)>(a))?(a):(x))	\
			)

#define sign(x)         ((x)<0?-1:1)


// stl defs for convenience
#define VECTOR(t)		std::vector < t >
#define MAP(s,t)		std::map < s, t >


#endif // COMMON_H
