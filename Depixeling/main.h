//
//  main.h
//  ip2015
//
//  Created by Z Sweedyk on 1/17/15.
//  Copyright (c) 2015 Z Sweedyk. All rights reserved.
//

#ifndef ip2015_main_h
#define ip2015_main_h

#include "common.h"
#include "image.h"

extern int window_width;
extern int window_height;

extern Image* currentImage;
extern Image* originalImage;

extern bool quietMode;
extern bool textMode;


int  main (int argc, char** argv);
char* init (int argc, char** argv);
void usage ();
void display ();
void unreshape (int width, int height);
void reshape (int width, int height);


#endif
