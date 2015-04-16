#include "ip.h"
#include "main.h"
#include <algorithm>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <list>
#include <array>
#include <stdio.h>
/*
 * By
 * Yaxi (Lucy) Gao
 * Zhu, Huangjian(Sean)
 * why...
 */

void initializeGraph(int ht, int wd, int n, bool (*graph)[ht][wd][n]) {
}

/*
* depixelize the pixel art
*/


Image* ip_depixelize(Image* src, int scale)
{
    const int wd = src->getWidth();
    const int ht = src->getHeight();
    int bt = src->getBits();
    
    bool graph[ht][wd][8];
    //bool* g = graph[0][0][0];
    for (int j = 0; j < ht ; j++) {
        for (int i = 0; i< wd; i++) {
            for (int k =0; k<8; k++) {
                graph[j][i][k] = false;
            }
        }
    }
    for (int j = 0; j < ht; ++j){
        for (int i = 0; i < wd; ++i){
            cout << graph[j][i][6];
        }
        cout << endl;
    }
    
    cout << endl;
    //initializeGraph(ht, wd, 8, &graph);
    
    for (int j = 1; j < ht - 1; j++) {
        for (int i = 1; i< wd - 1; i++) {
            for (int k =0; k<8; k++) {
                graph[j][i][k] = true;
            }
        }
    }
    
    for (int j = 0; j < ht; ++j){
        for (int i = 0; i < wd; ++i){
            cout << graph[j][i][6];
        }
        cout << endl;
    }
    
    cout << endl;

    
    // initialize first row
    graph[0][0][4] = true;
    graph[0][wd-1][3] = true;
    graph[0][0][6] = true;
    graph[0][0][7] = true;
    graph[0][wd-1][5] = true;
    graph[0][wd-1][6] = true;
    for (int i = 1; i < wd - 1; i++){
        for (int k = 3; k < 8; ++k){
            graph[0][i][k] = true;
        }
    }
    
    // initialize bottom row
    graph[ht-1][0][4] = true;
    graph[ht-1][wd-1][3] = true;
    graph[ht-1][0][1] = true;
    graph[ht-1][0][2] = true;
    graph[ht-1][wd-1][0] = true;
    graph[ht-1][wd-1][1] = true;
    for (int i = 1; i < wd - 1; i++){
        for (int k = 0; k < 5; ++k){
            graph[ht-1][i][k] = true;
        }
    }
    
    for (int j = 0; j < ht; ++j){
        for (int i = 0; i < wd; ++i){
            cout << graph[j][i][6];
        }
        cout << endl;
    }
    cout << endl;
    
    // initialize first column
    for (int j = 1; j < ht-1; j++){
        graph[j][0][1] = true;
        graph[j][0][2] = true;
        graph[j][0][4] = true;
        graph[j][0][6] = true;
        graph[j][0][7] = true;
    }
    
    // initialize last column
    for (int j = 1; j < ht-1; j++){
        graph[j][wd-1][0] = true;
        graph[j][wd-1][1] = true;
        graph[j][wd-1][3] = true;
        graph[j][wd-1][5] = true;
        graph[j][wd-1][6] = true;
    }
    
    
    
    for (int j = 0; j < ht; ++j){
        for (int i = 0; i < wd; ++i){
            cout << graph[j][i][6];
        }
        cout << endl;
    }

    Image* newImage = new Image(wd*scale,ht*scale,bt);
    for (int j = 0; j<ht*scale; j++) {
        for(int i=0; i< wd*scale; i++) {
            if(graph[j/scale][i/scale][4] && (j%scale==4) && (i%scale > 4)) {
                // do nothing
            } else if(graph[j/scale][i/scale][3] && (j%scale==4) && (i%scale <= 4)) {
                // do nothing
            } else if (graph[j/scale][i/scale][6] && (i%scale==4) && (j%scale > 4)) {
                // do nothing to make it black
            } else if (graph[j/scale][i/scale][1] && (i%scale==4) && (j%scale <= 4)) {
                // do nothing to make it black
            } else if (graph[j/scale][i/scale][7] && (i%scale==j%scale) && (j%scale >4)) {
                // do nothing to make it black
            } else if (graph[j/scale][i/scale][0] && (i%scale==j%scale) && (j%scale <=4)) {
                // do nothing to make it black
            } else if (graph[j/scale][i/scale][2] && (i%scale==(scale-j%scale-2)) && (i%scale > 4)) {
                // do nothing to make it black
            } else if (graph[j/scale][i/scale][5] && (i%scale==(scale-j%scale-2)) && (i%scale <=4)) {
                // do nothing to make it black
            } else {
                Pixel p = src->getPixel(i/scale, j/scale);
                newImage->setPixel(i, j, p);
            }
        }
    }
    

    
    return newImage;
}




