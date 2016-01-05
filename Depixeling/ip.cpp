#include "ip.h"
#include "main.h"
#include <algorithm>
#include <cmath>
#include <math.h>
#include <time.h>
#include <list>
#include <array>
#include <stdio.h>
/*
 * By
 * Yaxi (Lucy) Gao
 * Zhu, Huangjian(Sean)
 */

//bool graph[ht][wd][8];
//int graphWeight[ht][wd][8];
//int adjacentRow;
//int adjacentCol;
//
//
///*
// * Decide if two adjacent pixels are considered "close"
// * in terms of their YUV clolrs.
// */
//
//bool similarColor(Pixel p1, Pixel p2) {
//    
//    double r1,g1,b1,r2,g2,b2;
//    r1 = p1.getColor(0);
//    g1 = p1.getColor(1);
//    b1 = p1.getColor(2);
//    r2 = p2.getColor(0);
//    g2 = p2.getColor(1);
//    b2 = p2.getColor(2);
//    double y1 = 0.299*r1+0.587*g1+0.114*b1;
//    double u1 = -0.147*r1-0.289*g1+0.436*b1;
//    double v1 = 0.615*r1-0.515*g1-0.100*b1;
//    double y2 = 0.299*r2+0.587*g2+0.114*b2;
//    double u2 = -0.147*r2-0.289*g2+0.436*b2;
//    double v2 = 0.615*r2-0.515*g2-0.100*b2;
//    
//    return (abs(y1-y2) < 48./255 && abs(u1-u2) < 7./255 && abs(v1-v2) < 6./255);
//    
//}
//
//
///*
// * return the kth adjacent pixel of pixel (j,i)
// */
//Pixel getAdjacentPixel(Image* src, int j, int i, int k) {
//    switch (k) {
//        case 0:
//            i--;
//            j--;
//            break;
//        case 1:
//            j--;
//            break;
//        case 2:
//            j--;
//            i++;
//            break;
//        case 3:
//            i--;
//            break;
//        case 4:
//            i++;
//            break;
//        case 5:
//            j++;
//            i--;
//            break;
//        case 6:
//            j++;
//            break;
//        case 7:
//            j++;
//            i++;
//            break;
//        default:
//            break;
//    }
//    adjacentRow = j;
//    adjacentCol = i;
//    return (src->getPixel(i, j));
//}
//
//
///*
// *  Initialize the connected graph
// */
//
//void initializeGraph(Image* src, int scale) {
//    
//    for (int j = 0; j < ht ; j++) {
//        for (int i = 0; i< wd; i++) {
//            for (int k =0; k<8; k++) {
//                graph[j][i][k] = false;
//                graphWeight[j][i][k] = 0;
//            }
//        }
//    }
//    
//    for (int j = 1; j < ht - 1; j++) {
//        for (int i = 1; i< wd - 1; i++) {
//            for (int k =0; k<8; k++) {
//                graph[j][i][k] = similarColor(src->getPixel(i, j), getAdjacentPixel(src,j,i,k));
//            }
//        }
//    }
//    
//    // initialize first row
//    graph[0][0][4] = similarColor(src->getPixel(0, 0), src->getPixel(1, 0));
//    graph[0][wd-1][3] = similarColor(src->getPixel(wd-1, 0), src->getPixel(wd-2, 0));
//    graph[0][0][6] = similarColor(src->getPixel(0, 0), src->getPixel(0, 1));
//    graph[0][0][7] = similarColor(src->getPixel(0, 0), src->getPixel(1, 1));
//    graph[0][wd-1][5] = similarColor(src->getPixel(wd-1, 0), src->getPixel(wd-2, 1));
//    graph[0][wd-1][6] = similarColor(src->getPixel(wd-1, 0), src->getPixel(wd-1, 1));
//    for (int i = 1; i < wd - 1; i++){
//        for (int k = 3; k < 8; ++k){
//            graph[0][i][k] = similarColor(src->getPixel(i, 0), getAdjacentPixel(src,0,i,k));
//        }
//    }
//    
//    // initialize bottom row
//    graph[ht-1][0][4] = similarColor(src->getPixel(0, ht-1), src->getPixel(1, ht-1));
//    graph[ht-1][wd-1][3] = similarColor(src->getPixel(wd-1, ht-1), src->getPixel(wd-2, ht-1));
//    graph[ht-1][0][1] = similarColor(src->getPixel(0, ht-1), src->getPixel(0, ht-2));
//    graph[ht-1][0][2] = similarColor(src->getPixel(0, ht-1), src->getPixel(1, ht-2));
//    graph[ht-1][wd-1][0] = similarColor(src->getPixel(wd-1, ht-1), src->getPixel(wd-2, ht-2));
//    graph[ht-1][wd-1][1] = similarColor(src->getPixel(wd-1, ht-1), src->getPixel(wd-1, ht-2));
//    for (int i = 1; i < wd - 1; i++){
//        for (int k = 0; k < 5; ++k){
//            graph[ht-1][i][k] = similarColor(src->getPixel(i, ht-1), getAdjacentPixel(src,ht-1,i,k));
//        }
//    }
//    
//    // initialize first column
//    for (int j = 1; j < ht-1; j++){
//        graph[j][0][1] = similarColor(src->getPixel(0, j), src->getPixel(0,j-1));
//        graph[j][0][2] = similarColor(src->getPixel(0, j), src->getPixel(1,j-1));
//        graph[j][0][4] = similarColor(src->getPixel(0, j), src->getPixel(1,j));
//        graph[j][0][6] = similarColor(src->getPixel(0, j), src->getPixel(0,j+1));
//        graph[j][0][7] = similarColor(src->getPixel(0, j), src->getPixel(1,j+1));
//    }
//    
//    // initialize last column
//    for (int j = 1; j < ht-1; j++){
//        graph[j][wd-1][0] = similarColor(src->getPixel(wd-1, j), src->getPixel(wd-2,j-1));
//        graph[j][wd-1][1] = similarColor(src->getPixel(wd-1, j), src->getPixel(wd-1,j-1));
//        graph[j][wd-1][3] = similarColor(src->getPixel(wd-1, j), src->getPixel(wd-2,j));
//        graph[j][wd-1][5] = similarColor(src->getPixel(wd-1, j), src->getPixel(wd-2,j+1));
//        graph[j][wd-1][6] = similarColor(src->getPixel(wd-1, j), src->getPixel(wd-1,j+1));
//    }
//}
//
//
///*
// * Remove crosses of 2x2 block with the exact same color.
// */
//void removeCross(Image* src) {
//    for (int j = 0; j < ht-1; j++) {
//        for (int i = 0; i < wd-1; i++) {
//            Pixel p0 = src->getPixel(i, j);
//            Pixel p1 = getAdjacentPixel(src, j, i, 4);
//            Pixel p2 = getAdjacentPixel(src, j, i, 6);
//            Pixel p3 = getAdjacentPixel(src, j, i, 7);
//            if (similarColor(p0, p1) && similarColor(p0, p2) && similarColor(p0, p3) && similarColor(p1, p2)) {
//                graph[j][i][7] = false;
//                graph[j+1][i+1][0] = false;
//                graph[j][i+1][5] = false;
//                graph[j+1][i][2] = false;
//            }
//        }
//    }
//}
//
///*
// * Return the count of valence of the given pixel
// */
//int countValence(int j, int i) {
//    int count = 0;
//    for (int k = 0; k < 8; k++) {
//        if (graph[j][i][k])
//            ++count;
//    }
//    return count;
//}
//
///*
// * Return the direction of the next pixel
// */
//int nextPixel(int j, int i, int dirFrom) {
//    for (int k = 0; k < 8; k++) {
//        if (graph[j][i][k] && (dirFrom != 7-k)) {
//            return k;
//        }
//    }
//    return -1;
//}
//
///*
// * Curves heuristics
// */
//void curves(Image* src, int j, int i, int k) {
//    graphWeight[j][i][k] += 1;
//    adjacentRow = j;
//    adjacentCol = i;
//    int direction = k;
//    while ((countValence(adjacentRow, adjacentCol) == 2) && (adjacentCol != i && adjacentRow != j)) {
//        direction = nextPixel(adjacentRow, adjacentCol, direction);
//        graphWeight[j][i][k] += 1;
//        getAdjacentPixel(src, adjacentRow, adjacentCol, direction);
//    }
//    
//    if (k == 7) {
//        adjacentRow = j + 1;
//        adjacentCol = i + 1;
//    } else {
//        adjacentRow = j + 1;
//        adjacentCol = i - 1;
//    }
//    direction = 7-k;
//    while ((countValence(adjacentRow, adjacentCol) == 2) && (adjacentCol != i && adjacentRow != j)) {
//        direction = nextPixel(adjacentRow, adjacentCol, direction);
//        graphWeight[j][i][k] += 1;
//        getAdjacentPixel(src, adjacentRow, adjacentCol, direction);
//    }
//}
//
///*
// * Sparse Pixels heuristics
// */
//void sparsePixels() {
//    
//}
//
//
///*
// * Islands heuristics
// */
//void islands(int j, int i) {
//    if (countValence(j, i) == 1) {
//        graphWeight[j][i][7] += 5;
//    }
//    if (countValence(j+1, i+1) ==1) {
//        graphWeight[j][i][7] += 5;
//    }
//    if (countValence(j, i+1) == 1) {
//        graphWeight[j][i+1][5] += 5;
//    }
//    if (countValence(j+1, i) == 1) {
//        graphWeight[j][i+1][5] += 5;
//    }
//}
//
//
///*
// * Aggregate the three heuristics
// */
//void heuristics(Image* src) {
//    for (int j = 0; j < ht-1; j++) {
//        for (int i = 0; i < wd-1; i++) {
//            if (graph[j][i][7] && graph[j+1][i][2]) {
//                if (!graph[j][i][4] && !graph[j][i][6] && !graph[j][i+1][6] && !graph[j+1][i+1][3]) {
//                    islands(j,i);
//                    curves(src, j, i, 7);
//                    curves(src, j, i+1, 5);
//                    sparsePixels();
//                    // keep the edge with the higher weight
//                    if (graphWeight[j][i][7] > graphWeight[j][i+1][5]) {
//                        graph[j][i+1][5] = false;
//                        graph[j+1][i][2] = false;
//                    } else {
//                        graph[j][i][7] = false;
//                        graph[j+1][i+1][0] = false;
//                    }
//                }
//            }
//        }
//    }
//}


/*
 * Draw the image based on the connected graph.
 */
Image* drawImage(Image* src, int scale, int wd, int ht) {
    
    Image* newImage = new Image(wd*scale,ht*scale,8);
    
    for (int j = 0; j<ht*scale; j++) {
        for(int i=0; i< wd*scale; i++) {
//            if(graph[j/scale][i/scale][4] && (j%scale==4) && (i%scale > 4)) {
//                // do nothing
//            } else if(graph[j/scale][i/scale][3] && (j%scale==4) && (i%scale <= 4)) {
//                // do nothing
//            } else if (graph[j/scale][i/scale][6] && (i%scale==4) && (j%scale > 4)) {
//                // do nothing to make it black
//            } else if (graph[j/scale][i/scale][1] && (i%scale==4) && (j%scale <= 4)) {
//                // do nothing to make it black
//            } else if (graph[j/scale][i/scale][7] && (i%scale==j%scale) && (j%scale >4)) {
//                // do nothing to make it black
//            } else if (graph[j/scale][i/scale][0] && (i%scale==j%scale) && (j%scale <=4)) {
//                // do nothing to make it black
//            } else if (graph[j/scale][i/scale][2] && (i%scale==(scale-j%scale-2)) && (i%scale > 4)) {
//                // do nothing to make it black
//            } else if (graph[j/scale][i/scale][5] && (i%scale==(scale-j%scale-2)) && (i%scale <=4)) {
//                // do nothing to make it black
//            } else {
                Pixel p = src->getPixel(i/scale, j/scale);
                newImage->setPixel(i, j, p);
//            }
        }
    }
    return newImage;
}



/*
 * Main function to depixelize the pixel art.
 */

Image* ip_depixelize(Image* src, int scale, int wd, int ht)
{
//    initializeGraph(src, scale);
//    removeCross(src);
//    heuristics(src);
    Image* newImage = drawImage(src, scale, wd, ht);
    return newImage;
}


