#include "main.h"
#include "ip.h"
#include <algorithm>
#include <math.h>
#include <list>
#include <tuple>
#include <array>
#include <cmath>
#include <stdio.h>
#include <time.h>
#include "control.h"
#include <iterator>
#include <vector>


/*
 * IMPORTANT - DO NOT CHANGE THIS FILE - IMPORTANT
 */

int  window_width  = 300;
int  window_height = 300;
int scaleFactor = 12;

Image* currentImage  = NULL;
Image* originalImage = NULL;

bool quietMode = false;
bool textMode  = false;
bool displayMode = false;

float basisMatrix[3][3] = {
    {1, 1, 0},
    { -2, 2, 0},
    {1, -2, 1}};


/********************Graph relate********************/
const int wd = 22;
const int ht = 32;
bool graph[ht][wd][8];
int graphWeight[ht][wd][8];
int adjacentRow;
int adjacentCol;
int offset[8][2];
float reshapedPixels[ht][wd][8][2];
list<vector<float> > edges;
vector<vector<vector<float> > > vertices;
list<list<vector<float> > > bsplineVertices;
vector<bool> flag;


/*
 * Decide if two adjacent pixels are considered "close"
 * in terms of their YUV clolrs.
 */
bool similarColor(Pixel p1, Pixel p2) {
    
    double r1,g1,b1,r2,g2,b2;
    r1 = p1.getColor(0);
    g1 = p1.getColor(1);
    b1 = p1.getColor(2);
    r2 = p2.getColor(0);
    g2 = p2.getColor(1);
    b2 = p2.getColor(2);
    double y1 = 0.299*r1+0.587*g1+0.114*b1;
    double u1 = -0.147*r1-0.289*g1+0.436*b1;
    double v1 = 0.615*r1-0.515*g1-0.100*b1;
    double y2 = 0.299*r2+0.587*g2+0.114*b2;
    double u2 = -0.147*r2-0.289*g2+0.436*b2;
    double v2 = 0.615*r2-0.515*g2-0.100*b2;
    
    return (abs(y1-y2) < 48./255 && abs(u1-u2) < 7./255 && abs(v1-v2) < 6./255);
    
}


/*
 * return the kth adjacent pixel of pixel (j,i)
 */
Pixel getAdjacentPixel(Image* src, int j, int i, int k) {
    switch (k) {
        case 0:
            i--;
            j--;
            break;
        case 1:
            j--;
            break;
        case 2:
            j--;
            i++;
            break;
        case 3:
            i--;
            break;
        case 4:
            i++;
            break;
        case 5:
            j++;
            i--;
            break;
        case 6:
            j++;
            break;
        case 7:
            j++;
            i++;
            break;
        default:
            break;
    }
    adjacentRow = j;
    adjacentCol = i;
    return (src->getPixel(i, j));
}


/*
 *  Initialize the connected graph
 */
void initializeGraph(Image* src, int scale) {
    
    for (int j = 0; j < ht ; j++) {
        for (int i = 0; i< wd; i++) {
            for (int k =0; k<8; k++) {
                graph[j][i][k] = false;
                graphWeight[j][i][k] = 0;
            }
        }
    }
    
    for (int j = 1; j < ht - 1; j++) {
        for (int i = 1; i< wd - 1; i++) {
            for (int k =0; k<8; k++) {
                graph[j][i][k] = similarColor(src->getPixel(i, j), getAdjacentPixel(src,j,i,k));
            }
        }
    }
    
    // initialize first row
    graph[0][0][4] = similarColor(src->getPixel(0, 0), src->getPixel(1, 0));
    graph[0][wd-1][3] = similarColor(src->getPixel(wd-1, 0), src->getPixel(wd-2, 0));
    graph[0][0][6] = similarColor(src->getPixel(0, 0), src->getPixel(0, 1));
    graph[0][0][7] = similarColor(src->getPixel(0, 0), src->getPixel(1, 1));
    graph[0][wd-1][5] = similarColor(src->getPixel(wd-1, 0), src->getPixel(wd-2, 1));
    graph[0][wd-1][6] = similarColor(src->getPixel(wd-1, 0), src->getPixel(wd-1, 1));
    for (int i = 1; i < wd - 1; i++){
        for (int k = 3; k < 8; ++k){
            graph[0][i][k] = similarColor(src->getPixel(i, 0), getAdjacentPixel(src,0,i,k));
        }
    }
    
    // initialize bottom row
    graph[ht-1][0][4] = similarColor(src->getPixel(0, ht-1), src->getPixel(1, ht-1));
    graph[ht-1][wd-1][3] = similarColor(src->getPixel(wd-1, ht-1), src->getPixel(wd-2, ht-1));
    graph[ht-1][0][1] = similarColor(src->getPixel(0, ht-1), src->getPixel(0, ht-2));
    graph[ht-1][0][2] = similarColor(src->getPixel(0, ht-1), src->getPixel(1, ht-2));
    graph[ht-1][wd-1][0] = similarColor(src->getPixel(wd-1, ht-1), src->getPixel(wd-2, ht-2));
    graph[ht-1][wd-1][1] = similarColor(src->getPixel(wd-1, ht-1), src->getPixel(wd-1, ht-2));
    for (int i = 1; i < wd - 1; i++){
        for (int k = 0; k < 5; ++k){
            graph[ht-1][i][k] = similarColor(src->getPixel(i, ht-1), getAdjacentPixel(src,ht-1,i,k));
        }
    }
    
    // initialize first column
    for (int j = 1; j < ht-1; j++){
        graph[j][0][1] = similarColor(src->getPixel(0, j), src->getPixel(0,j-1));
        graph[j][0][2] = similarColor(src->getPixel(0, j), src->getPixel(1,j-1));
        graph[j][0][4] = similarColor(src->getPixel(0, j), src->getPixel(1,j));
        graph[j][0][6] = similarColor(src->getPixel(0, j), src->getPixel(0,j+1));
        graph[j][0][7] = similarColor(src->getPixel(0, j), src->getPixel(1,j+1));
    }
    
    // initialize last column
    for (int j = 1; j < ht-1; j++){
        graph[j][wd-1][0] = similarColor(src->getPixel(wd-1, j), src->getPixel(wd-2,j-1));
        graph[j][wd-1][1] = similarColor(src->getPixel(wd-1, j), src->getPixel(wd-1,j-1));
        graph[j][wd-1][3] = similarColor(src->getPixel(wd-1, j), src->getPixel(wd-2,j));
        graph[j][wd-1][5] = similarColor(src->getPixel(wd-1, j), src->getPixel(wd-2,j+1));
        graph[j][wd-1][6] = similarColor(src->getPixel(wd-1, j), src->getPixel(wd-1,j+1));
    }
}


/*
 * Remove crosses of 2x2 block with the exact same color.
 */
void removeCross(Image* src) {
    for (int j = 0; j < ht-1; j++) {
        for (int i = 0; i < wd-1; i++) {
            Pixel p0 = src->getPixel(i, j);
            Pixel p1 = getAdjacentPixel(src, j, i, 4);
            Pixel p2 = getAdjacentPixel(src, j, i, 6);
            Pixel p3 = getAdjacentPixel(src, j, i, 7);
            if (similarColor(p0, p1) && similarColor(p0, p2) && similarColor(p0, p3) && similarColor(p1, p2)) {
                graph[j][i][7] = false;
                graph[j+1][i+1][0] = false;
                graph[j][i+1][5] = false;
                graph[j+1][i][2] = false;
            }
        }
    }
}

/*
 * Return the count of valence of the given pixel
 */
int countValence(int j, int i) {
    int count = 0;
    for (int k = 0; k < 8; k++) {
        if (graph[j][i][k])
            ++count;
    }
    return count;
}

/*
 * Return the direction of the next pixel
 */
int nextPixel(int j, int i, int dirFrom) {
    for (int k = 0; k < 8; k++) {
        if (graph[j][i][k] && (dirFrom != 7-k)) {
            return k;
        }
    }
    return -1;
}

/*
 * Curves heuristics
 */
void curves(Image* src, int j, int i, int k) {
    graphWeight[j][i][k] += 1;
    adjacentRow = j;
    adjacentCol = i;
    int direction = k;
    while ((countValence(adjacentRow, adjacentCol) == 2) && (adjacentCol != i && adjacentRow != j)) {
        direction = nextPixel(adjacentRow, adjacentCol, direction);
        graphWeight[j][i][k] += 1;
        getAdjacentPixel(src, adjacentRow, adjacentCol, direction);
    }
    
    if (k == 7) {
        adjacentRow = j + 1;
        adjacentCol = i + 1;
    } else {
        adjacentRow = j + 1;
        adjacentCol = i - 1;
    }
    direction = 7-k;
    while ((countValence(adjacentRow, adjacentCol) == 2) && (adjacentCol != i && adjacentRow != j)) {
        direction = nextPixel(adjacentRow, adjacentCol, direction);
        graphWeight[j][i][k] += 1;
        getAdjacentPixel(src, adjacentRow, adjacentCol, direction);
    }
}

/*
 * Sparse Pixels heuristics
 */
void sparsePixels() {
    
}


/*
 * Islands heuristics
 */
void islands(int j, int i) {
    if (countValence(j, i) == 1) {
        graphWeight[j][i][7] += 5;
    }
    if (countValence(j+1, i+1) ==1) {
        graphWeight[j][i][7] += 5;
    }
    if (countValence(j, i+1) == 1) {
        graphWeight[j][i+1][5] += 5;
    }
    if (countValence(j+1, i) == 1) {
        graphWeight[j][i+1][5] += 5;
    }
}


/*
 * Aggregate the three heuristics
 */
void heuristics(Image* src) {
    for (int j = 0; j < ht-1; j++) {
        for (int i = 0; i < wd-1; i++) {
            if (graph[j][i][7] && graph[j+1][i][2]) {
                if (!graph[j][i][4] && !graph[j][i][6] && !graph[j][i+1][6] && !graph[j+1][i+1][3]) {
                    islands(j,i);
                    curves(src, j, i, 7);
                    curves(src, j, i+1, 5);
                    sparsePixels();
                    // keep the edge with the higher weight
                    if (graphWeight[j][i][7] > graphWeight[j][i+1][5]) {
                        graph[j][i+1][5] = false;
                        graph[j+1][i][2] = false;
                    } else {
                        graph[j][i][7] = false;
                        graph[j+1][i+1][0] = false;
                    }
                }
            }
        }
    }
}


/*
 * Generate the Voronoi Graph
 */
void reshapePic () {
    for (int col = 0; col < wd; col++) {
        for (int row = 0; row < ht; row++) {
            for(int k = 0; k < 8; k++) {
                reshapedPixels[row][col][k][0] = -1;
                reshapedPixels[row][col][k][1] = -1;
            }
        }
    }
    
    
    for (int col = 0; col < wd; col++) {
        for (int row = 0; row < ht; row++) {
            // top left
            if (graph[row][col][0]) {
                // may need a shift of 0.5
                reshapedPixels[row][col][0][0] = col - 0.75;
                reshapedPixels[row][col][0][1] = row - 0.25;
                reshapedPixels[row][col][1][0] = col - 0.25;
                reshapedPixels[row][col][1][1] = row - 0.75;
            } else if ((col > 0) && (graph[row][col-1][2])) {
                // may need a shift of 0.5
                reshapedPixels[row][col][0][0] = -1.;
                reshapedPixels[row][col][0][1] = -1.;
                reshapedPixels[row][col][1][0] = col - 0.25;
                reshapedPixels[row][col][1][1] = row - 0.25;
            } else if ((col > 0) && (!graph[row][col][0]) && (!graph[row][col-1][2])) {
                reshapedPixels[row][col][0][0] = -1.;
                reshapedPixels[row][col][0][1] = -1.;
                reshapedPixels[row][col][1][0] = col - 0.5;
                reshapedPixels[row][col][1][1] = row - 0.5;
            }
            
            // top right
            if (graph[row][col][2]) {
                // may need a shift of 0.5
                reshapedPixels[row][col][2][0] = col + 0.25;
                reshapedPixels[row][col][2][1] = row - 0.75;
                reshapedPixels[row][col][3][0] = col + 0.75;
                reshapedPixels[row][col][3][1] = row - 0.25;
            } else if ((col < wd-1) && (graph[row][col+1][0])) {
                // may need a shift of 0.5
                reshapedPixels[row][col][2][0] = -1.;
                reshapedPixels[row][col][2][1] = -1;
                reshapedPixels[row][col][3][0] = col + 0.25;
                reshapedPixels[row][col][3][1] = row - 0.25;
            } else if ((col < wd-1) && (!graph[row][col][2]) && (!graph[row][col+1][0])) {
                reshapedPixels[row][col][2][0] = -1.;
                reshapedPixels[row][col][2][1] = -1.;
                reshapedPixels[row][col][3][0] = col + 0.5;
                reshapedPixels[row][col][3][1] = row - 0.5;
            }
            
            // bottom right
            if (graph[row][col][7]) {
                // may need a shift of 0.5
                reshapedPixels[row][col][4][0] = col + 0.75;
                reshapedPixels[row][col][4][1] = row + 0.25;
                reshapedPixels[row][col][5][0] = col + 0.25;
                reshapedPixels[row][col][5][1] = row + 0.75;
            } else if (col == wd - 1) {
                reshapedPixels[row][col][4][0] = -1.;
                reshapedPixels[row][col][4][1] = -1.;
                reshapedPixels[row][col][5][0] = col + 0.5;
                reshapedPixels[row][col][5][1] = row + 0.5;
            } else if (graph[row][col+1][5]) {
                // may need a shift of 0.5
                reshapedPixels[row][col][4][0] = -1.;
                reshapedPixels[row][col][4][1] = -1.;
                reshapedPixels[row][col][5][0] = col + 0.25;
                reshapedPixels[row][col][5][1] = row + 0.25;
            } else {
                reshapedPixels[row][col][4][0] = -1.;
                reshapedPixels[row][col][4][1] = -1.;
                reshapedPixels[row][col][5][0] = col + 0.5;
                reshapedPixels[row][col][5][1] = row + 0.5;
            }
            
            // bottom left
            if (graph[row][col][5]) {
                // may need a shift of 0.5
                reshapedPixels[row][col][6][0] = col - 0.25;
                reshapedPixels[row][col][6][1] = row + 0.75;
                reshapedPixels[row][col][7][0] = col - 0.75;
                reshapedPixels[row][col][7][1] = row + 0.25;
            } else if ((col > 0) && (graph[row][col-1][7])) {
                // may need a shift of 0.5
                reshapedPixels[row][col][6][0] = -1;
                reshapedPixels[row][col][6][1] = -1;
                reshapedPixels[row][col][7][0] = col - 0.25;
                reshapedPixels[row][col][7][1] = row + 0.25;
            } else if ((col > 0) && (!graph[row][col][5]) && (!graph[row][col-1][7])) {
                reshapedPixels[row][col][6][0] = -1.;
                reshapedPixels[row][col][6][1] = -1.;
                reshapedPixels[row][col][7][0] = col - 0.5;
                reshapedPixels[row][col][7][1] = row + 0.5;
            }
            
            // visible edges
            // top left to top right
            if (!graph[row][col][1] && row != 0) {
                if (graph[row][col][2]) {
                    float r = originalImage->getPixel(col, row).getColor(0);
                    float g = originalImage->getPixel(col, row).getColor(1);
                    float b = originalImage->getPixel(col, row).getColor(2);
                    vector<float> tempEdge;
                    tempEdge.push_back(reshapedPixels[row][col][1][0]);
                    tempEdge.push_back(reshapedPixels[row][col][1][1]);
                    tempEdge.push_back(reshapedPixels[row][col][2][0]);
                    tempEdge.push_back(reshapedPixels[row][col][2][1]);
                    tempEdge.push_back(r);
                    tempEdge.push_back(g);
                    tempEdge.push_back(b);
                    edges.push_back(tempEdge);
                } else if (((col < wd-1) && graph[row][col+1][0])||((col < wd-1) && (!graph[row][col][2]) && (!graph[row][col+1][0]))) {
                    float r = originalImage->getPixel(col, row).getColor(0);
                    float g = originalImage->getPixel(col, row).getColor(1);
                    float b = originalImage->getPixel(col, row).getColor(2);
                    vector<float> tempEdge;
                    tempEdge.push_back(reshapedPixels[row][col][1][0]);
                    tempEdge.push_back(reshapedPixels[row][col][1][1]);
                    tempEdge.push_back(reshapedPixels[row][col][3][0]);
                    tempEdge.push_back(reshapedPixels[row][col][3][1]);
                    tempEdge.push_back(r);
                    tempEdge.push_back(g);
                    tempEdge.push_back(b);
                    edges.push_back(tempEdge);
                }
            }
            
            
            // top right to bottom right
            if (!graph[row][col][4] && col != wd - 1) {
                if (graph[row][col][7]) {
                    float r = originalImage->getPixel(col, row).getColor(0);
                    float g = originalImage->getPixel(col, row).getColor(1);
                    float b = originalImage->getPixel(col, row).getColor(2);
                    vector<float> tempEdge;
                    tempEdge.push_back(reshapedPixels[row][col][3][0]);
                    tempEdge.push_back(reshapedPixels[row][col][3][1]);
                    tempEdge.push_back(reshapedPixels[row][col][4][0]);
                    tempEdge.push_back(reshapedPixels[row][col][4][1]);
                    tempEdge.push_back(r);
                    tempEdge.push_back(g);
                    tempEdge.push_back(b);
                    edges.push_back(tempEdge);
                } else if (((col < wd-1) && graph[row][col+1][5])||((col < wd-1) && (!graph[row][col][7]) && (!graph[row][col+1][5]))) {
                    float r = originalImage->getPixel(col, row).getColor(0);
                    float g = originalImage->getPixel(col, row).getColor(1);
                    float b = originalImage->getPixel(col, row).getColor(2);
                    vector<float> tempEdge;
                    tempEdge.push_back(reshapedPixels[row][col][3][0]);
                    tempEdge.push_back(reshapedPixels[row][col][3][1]);
                    tempEdge.push_back(reshapedPixels[row][col][5][0]);
                    tempEdge.push_back(reshapedPixels[row][col][5][1]);
                    tempEdge.push_back(r);
                    tempEdge.push_back(g);
                    tempEdge.push_back(b);
                    edges.push_back(tempEdge);
                }
            }
            
            // bottom right to bottom left
            if (!graph[row][col][6] && row != ht - 1) {
                if (graph[row][col][5]) {
                    float r = originalImage->getPixel(col, row).getColor(0);
                    float g = originalImage->getPixel(col, row).getColor(1);
                    float b = originalImage->getPixel(col, row).getColor(2);
                    vector<float> tempEdge;
                    tempEdge.push_back(reshapedPixels[row][col][5][0]);
                    tempEdge.push_back(reshapedPixels[row][col][5][1]);
                    tempEdge.push_back(reshapedPixels[row][col][6][0]);
                    tempEdge.push_back(reshapedPixels[row][col][6][1]);
                    tempEdge.push_back(r);
                    tempEdge.push_back(g);
                    tempEdge.push_back(b);
                    edges.push_back(tempEdge);
                } else if (((col > 0) && (graph[row][col-1][7]))||((col > 0) && (!graph[row][col-1][7]) && (!graph[row][col][5]))) {
                    float r = originalImage->getPixel(col, row).getColor(0);
                    float g = originalImage->getPixel(col, row).getColor(1);
                    float b = originalImage->getPixel(col, row).getColor(2);
                    vector<float> tempEdge;
                    tempEdge.push_back(reshapedPixels[row][col][5][0]);
                    tempEdge.push_back(reshapedPixels[row][col][5][1]);
                    tempEdge.push_back(reshapedPixels[row][col][7][0]);
                    tempEdge.push_back(reshapedPixels[row][col][7][1]);
                    tempEdge.push_back(r);
                    tempEdge.push_back(g);
                    tempEdge.push_back(b);
                    edges.push_back(tempEdge);
                }
            }

            
            // top right to bottom right
            if (!graph[row][col][3] && col != 0) {
                if (graph[row][col][0]) {
                    float r = originalImage->getPixel(col, row).getColor(0);
                    float g = originalImage->getPixel(col, row).getColor(1);
                    float b = originalImage->getPixel(col, row).getColor(2);
                    vector<float> tempEdge;
                    tempEdge.push_back(reshapedPixels[row][col][7][0]);
                    tempEdge.push_back(reshapedPixels[row][col][7][1]);
                    tempEdge.push_back(reshapedPixels[row][col][0][0]);
                    tempEdge.push_back(reshapedPixels[row][col][0][1]);
                    tempEdge.push_back(r);
                    tempEdge.push_back(g);
                    tempEdge.push_back(b);
                    edges.push_back(tempEdge);
                } else if (((col > 0) && graph[row][col-1][2])||((col > 0) && (!graph[row][col-1][2]) && (!graph[row][col][0]))) {
                    float r = originalImage->getPixel(col, row).getColor(0);
                    float g = originalImage->getPixel(col, row).getColor(1);
                    float b = originalImage->getPixel(col, row).getColor(2);
                    vector<float> tempEdge;
                    tempEdge.push_back(reshapedPixels[row][col][7][0]);
                    tempEdge.push_back(reshapedPixels[row][col][7][1]);
                    tempEdge.push_back(reshapedPixels[row][col][1][0]);
                    tempEdge.push_back(reshapedPixels[row][col][1][1]);
                    tempEdge.push_back(r);
                    tempEdge.push_back(g);
                    tempEdge.push_back(b);
                    edges.push_back(tempEdge);
                }
            }
            
        }
    }
}

/***********************End Graph********************/


/******************Begin Spline extraction***********/

// auxiliary method that compare float points
bool compareFloats(float x1, float y1, float x2, float y2){
    return abs(x1-x2) < 0.01 && abs(y1-y2) < 0.01;
}

bool compareColors(float r1, float g1, float b1, float r2, float g2, float b2) {
    double y1 = 0.299*r1 + 0.587*g1 + 0.114*b1;
    double u1 = -0.147*r1 - 0.289*g1 + 0.436*b1;
    double v1 = 0.615*r1 - 0.515*g1 - 0.100*b1;
    double y2 = 0.299*r2 + 0.587*g2 + 0.114*b2;
    double u2 = -0.147*r2 - 0.289*g2 + 0.436*b2;
    double v2 = 0.615*r2 - 0.515*g2 - 0.100*b2;
    return (abs(y1-y2) < 48./255 && abs(u1-u2) < 7./255 && abs(v1-v2) < 6./255);

}

bool connectedVertices(vector<float> a, vector<float> b){
    return compareFloats(a.at(2), a.at(3), b.at(0), b.at(1)) && compareColors(a.at(4), a.at(5), a.at(6), b.at(4), b.at(5), b.at(6));
}

bool sameVertices(vector<float> a, vector<float> b){
    return compareFloats(a.at(0), a.at(1), b.at(0), b.at(1)) && compareColors(a.at(4), a.at(5), a.at(6), b.at(4), b.at(5), b.at(6));
}

bool vertexVisited(list<vector<float>> &a, vector<float> b) {
    for (list<vector<float>>::iterator it=a.begin(); it != a.end(); ++it) {
        if (sameVertices(*it, b)) {
            return true;
        }
    }
    return false;
}

void splineVertices(){
    // initialize the visited vertex and start vertex
    list<vector<float>> visitedList;
    bool newList = true;
    
    for (list<vector<float>>::iterator it=edges.begin(); it != edges.end(); ++it) {
        newList = true;
        if (!vertexVisited(visitedList, *it)) {
            visitedList.push_back(*it);
            for (vector<vector<vector<float>>>::iterator iter=vertices.begin(); iter != vertices.end(); ++iter) {
                if (connectedVertices(iter->back(),*it)) {
                    iter->push_back(*it);
                    newList = false;
                    break;
                }
            }
            if (newList) {
                vector<vector<float> > tempList;
                tempList.push_back(*it);
                vertices.push_back(tempList);
            }
        }
    }
}

void mergeVertices() {
    
    int count = 0;
    // merge potential left-out vertices
    
    int n = (int) vertices.size();
    for (int i = 0; i < n; ++i) {
        flag.push_back(true);
    }
    
    for (int i = 0; i < n; ++i) {
        //if (!flag[i]) {
            for (int j = 0; j < n; ++j) {
                if (i != j){
                    int li = (int)(vertices[i].size());
                    int lj = (int)(vertices[j].size());
                    if (li > 0 && lj > 0) {
                        if (connectedVertices(vertices[i][li-1], vertices[j][0])) {
                            count++;
                            for (int k = 0; k < (vertices[j]).size(); ++k) {
                                vertices[i].push_back(vertices[j][k]);
                            }
                            flag[j] = false;
                        }
                    }
                }
            }
        //}
    }
}

void extractSpline(vector<vector<float>> vert) {
    
    //cout << "vertices size: " << vert.size() << endl;
    
    if (vert.size() > 2) {
        list<vector<float>> shape;
        glColor3f(0, 0, 0);
        for (vector<vector<float> >::iterator it = vert.begin(); it != --(--vert.end()); ++it){
            float coeff[2][3];

            /*  compute coefficients for the x and y quadratic polynomials */
            for (int d=0; d<2; d++) { // compute for dimension x and y
                for (int i=0; i< 3; i++) { // compute coeff[d][i]
                    coeff[d][i]=0;
                    coeff[d][i] += basisMatrix[i][0] * it->at(d);
                    coeff[d][i] += basisMatrix[i][1] * (next(it,1)->at(d));
                    coeff[d][i] += basisMatrix[i][2] * next(it,2)->at(d);
                }
            }
            glLineWidth(2.5f);
            /*  approximate the curve by a line strip through sample points	*/
            glBegin(GL_LINE_STRIP);
            
            float numSamples=20;
            float t=0;
            while(t<1) {
                /* TODO: compute X(t), Y(t) and create openGL vertex */
                float polyVal[2];
                for (int i=0;i<2;i++) {
                    polyVal[i] = coeff[i][0] + coeff[i][1]*t + coeff[i][2]*t*t;
                }
                glVertex2f((polyVal[0]+1)*scaleFactor/2, scaleFactor/2*(ht*2-polyVal[1]-1));
                t += 1.0/numSamples;
            }
            /* the curve ends at a control point when t=1  				*/
            /* because the increment 1.0/numSamples  has finite precision	*/
            /* t probably won't hit 1.0 exactly, so we force it			*/
            glEnd();
            glLineWidth(2.0f);
        }
    }

}

/***********************End Spline extraction******/



void initializeOffsets() {
    offset[0][0]=-1;
    offset[0][1]=-1;
    offset[1][0]=0;
    offset[1][1]=-1;
    offset[2][0]=1;
    offset[2][1]=-1;
    offset[3][0]=-1;
    offset[3][1]=0;
    offset[4][0]=1;
    offset[4][1]=0;
    offset[5][0]=-1;
    offset[5][1]=1;
    offset[6][0]=0;
    offset[6][1]=1;
    offset[7][0]=1;
    offset[7][1]=1;
}

int main (int argc, char** argv)
{
    // initialize parameters
    char* toLoad = init(argc, argv);
    
    //initialize offset
    initializeOffsets();
    
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
    // check if there have been any openGL problems
    GLenum errCode = glGetError();
    
    if (errCode != GL_NO_ERROR)
        
    {
        const GLubyte* errString = gluErrorString(errCode);
        cerr << "OpenGL error: " << errString << endl;
    }
    
    // clear the frame buffer
    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT);
    
    //if (displayMode==0) {
    // draw the similarity graph
    
    // draw the image
    if (currentImage)
        currentImage->glDrawPixelsWrapper();
    
    // draw edges
    if (displayMode) {
        
        glColor3f(0,0,0);
        
        glLineWidth(1);
        
        glDisable(GL_DEPTH_TEST);
        
        int width = originalImage->getWidth();
        int height = originalImage->getHeight();
        
        initializeGraph(originalImage, scaleFactor);
        removeCross(originalImage);
        heuristics(originalImage);
        reshapePic();
        
        splineVertices();
        mergeVertices();

        
        for (int col = 0; col < width; ++col) {
            for (int row = 0; row < height; ++row) {
                float red = originalImage->getPixel(col, row, 0);
                float green = originalImage->getPixel(col, row, 1);
                float blue = originalImage->getPixel(col, row, 2);
                
                glColor3f(red,green,blue);
                glBegin(GL_POLYGON);
                
                for (int k = 0; k < 8; k++) {
                    if (reshapedPixels[row][col][k][0] >= 0 && reshapedPixels[row][col][k][1] >= 0) {
                        glVertex2f(scaleFactor*(reshapedPixels[row][col][k][0]+0.5), scaleFactor*height - scaleFactor*(reshapedPixels[row][col][k][1]+0.5));
                    }
                }
                glEnd();
                
                // draw edges that connect pixels
//                glBegin(GL_LINES);
//                glColor3f(0,0,0);
//                for (int k = 0; k < 4; k++) {
//                    if (graph[row][col][k]) {
//                        int x1 = scaleFactor*col+scaleFactor/2.0;
//                        int y1 = scaleFactor*height - (scaleFactor*row +scaleFactor/2.0);
//                        int x2 = scaleFactor*(col+offset[k][0])+scaleFactor/2.0;
//                        int y2= scaleFactor*height - (scaleFactor*(row+offset[k][1])+scaleFactor/2.0);
//                        
//                        glVertex2d(x1,y1);
//                        glVertex2d(x2,y2);
//                        
//                    }
//                }
//                for (int k=4;k<8;k++) {
//                    if (graph[row][col][k]) {
//                        int x1 = scaleFactor*col+scaleFactor/2.0;
//                        int y1 = scaleFactor*height - (scaleFactor*row +scaleFactor/2.0);
//                        int x2 = scaleFactor*(col+offset[k][0])+scaleFactor/2.0;
//                        int y2= scaleFactor*height - (scaleFactor*(row+offset[k][1])+scaleFactor/2.0);
//                        
//                        glVertex2d(x1,y1);
//                        glVertex2d(x2,y2);
//                        
//                    }
//                }
//                glEnd();
                
                
            }
        }
        //glClear(GL_COLOR_BUFFER_BIT);
        
        glColor3f(0, 1, 0);
        
        for (int i = 0; i < vertices.size(); ++i) {
            if (flag[i]) extractSpline(vertices[i]);
        }
        
        glEnable(GL_DEPTH_TEST);

        
    }
    
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

