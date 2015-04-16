#ifndef IP_H
#define IP_H


#include "common.h"
#include "image.h"


/*
* IMPORTANT - DO NOT CHANGE THE INTERFACES DEFINED HERE - IMPORTANT
*
* It's very important that the interface to your program does not
* change from what is provided, so that automated testing scripts
* will work.  If you add additional requests for input, these scripts
* will no longer work and it will negatively affect your grade.  Each
* method has sufficient information to carry out the required task.
*
* The only method you are may get more user input in is ip_warp().
* This option will not be tested automatically, since everyone's
* will be different, so you may ask for whatever input is necessary.
* To support multiple warps, have ip_warp() print a menu of options.
*
* Of course, you may add whatever other functions you may need.
*/


Image*	ip_depixelize (Image* src, int scale);
void initializeGraph(int ht, int wd, int n, bool graph[ht][wd][n]);



#endif // IP_H
