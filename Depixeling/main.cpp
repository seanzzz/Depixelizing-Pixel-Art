#include "main.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "control.h"


/*
 * IMPORTANT - DO NOT CHANGE THIS FILE - IMPORTANT
 */

/* hello */

int  window_width  = 300;
int  window_height = 300;

Image* currentImage  = NULL;
Image* originalImage = NULL;

bool quietMode = false;
bool textMode  = false;


int main (int argc, char** argv)
{
    // initialize parameters
    char* toLoad = init(argc, argv);
    
    if (textMode)
    {
        if (toLoad)
            image_load(toLoad);
        textMenuLoop();
    }
    else
    {
        // set up the window
        glutInit(&argc, &argv[0]);
        glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
        glutInitWindowPosition(100,100);
        glutInitWindowSize(window_width, window_height);
        glutCreateWindow("Depixelizing Pixel Art - Lucy&Sean");
        
        // register call back functions
        glutDisplayFunc(display);
        glutReshapeFunc(unreshape);
        
        glClearColor(0.0,0.0,0.0,0.0);
        glDisable(GL_DEPTH_TEST);
        
        // setup main menu
        make_menu();
        
        // register keyboard callback function
        glutKeyboardFunc(keyboard_func);
        
        if (toLoad)
            image_load(toLoad);
        
        // wait for something to happen
        glutMainLoop();
    }
    return 0;
}


char* init (int argc, char** argv)
{
    // init random number generator
    //srand48(time(0));
    
    char* toLoad = NULL;
    
    // parse the command line options
    bool noMoreArgs  = false;
    bool noMoreFlags = false;
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if (noMoreArgs)
                usage();
            
            if (!noMoreFlags && argv[i][0] == '-')
            {
                switch (argv[i][1])
                {
                    case 't':
                        textMode = true;
                        break;
                        
                    case 'q':
                        quietMode = true;
                        break;
                        
                    case '-':
                        if (argv[i][2] == '\0')
                            noMoreFlags = true;
                        else
                            usage();
                        break;
                        
                    default:
                        usage();
                }
            }
            else
            {
                noMoreArgs = true;
                //        image_load(argv[i]);
                toLoad = argv[i];
            }
        }
    }
    
    return toLoad;
}


void usage ()
{
    cerr << "usage: ./ip [ -t ] [ -q ] [ -- ] [ file ]" << endl;
    exit(-1);
}


void display ()
{
    if (textMode)
        return;
    
    // check if there have been any openGL problems
    GLenum errCode = glGetError();
    if (errCode != GL_NO_ERROR)
    {
        const GLubyte* errString = gluErrorString(errCode);
        cerr << "OpenGL error: " << errString << endl;
    }
    
    // clear the frame buffer
    glClear(GL_COLOR_BUFFER_BIT);
    
    // draw the image
    if (currentImage)
        currentImage->glDrawPixelsWrapper();
    
    // swap buffers
    glutSwapBuffers();
}


void unreshape (int width, int height)
{
    // don't allow user to manuall resize the window
    reshape(window_width, window_height);
}


void reshape (int width, int height)
{
    // set window height and width
    window_width  = max(width,  64);
    window_height = max(height, 64); 
    
    if (textMode)
        return;
    
    // change the actual window's size
    glutReshapeWindow(window_width, window_height);
    
    // the lower left corner of the viewport is 0,0
    // the upper right corner is width, height
    glViewport(0, 0, (GLint) window_width, (GLint) window_height);  
    
    // setup orthographic projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, window_width, 0.0, window_height);
    
    // default mode should be modelview
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
