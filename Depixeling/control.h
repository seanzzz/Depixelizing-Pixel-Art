#ifndef CONTROL_H
#define CONTROL_H

#include "common.h"


/*
 * IMPORTANT - DO NOT CHANGE THIS FILE - IMPORTANT
 */


// init calls
int make_menu();

// glut callbacks
void menu_func (int value);
void keyboard_func (unsigned char key, int x, int y);
void mouse_click_func (int button, int state, int x, int y);
void mouse_move_func (int x, int y);
void idle_func ();

// menu calls
void menu_help ();
void image_load (const char* filename);
void image_save (const char* filename);
void image_print_info ();
void image_revert ();

// text mode calls
void textMenuLoop ();

// helper functions
int getFilterSize();
int getInt(const char* message);
double getDouble(const char* message);
double getPositiveDouble(const char* message);


#endif // CONTROL_H
