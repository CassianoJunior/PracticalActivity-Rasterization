#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <stdio.h>

#include "DataStructures.h"
#include "transformations.h"

void retaImediata(double x1,double y1,double x2,double y2);

bool isFirstOctant(double x1, double y1, double x2, double y2);
void reductionToFirstOctant(double &x1, double &y1, double &x2, double &y2, bool &declivity, bool &simetric);
void inverseTransformation(bool declivity, bool simetric);
point* bresenhamAlgorithm(double x1, double y1, double x2, double y2 );
point* bresenham(double x1, double y1, double x2, double y2, bool declivity, bool simetric);
point* bresenhamWithReductionToFirstOctant(double x1, double y1, double x2, double y2 );
point* resetList(void);
point* resetPolygonList(void);
void translateToRealCoordinates(double originX, double originY);

bool isFirstOctant(double x1, double y1, double x2, double y2) {
  double tangent = (y2 - y1) / (x2 - x1);
  return x1 < x2 && (tangent >= 0 && tangent <= 1);
}

void retaImediata(double x1,double y1,double x2,double y2) {
    double m, b, yd, xd;
    double xmin, xmax,ymin,ymax;
    firstPointDrawn = pushPoint((int)x1,(int)y1, false, false);
    firstPointDrawn = pushPoint((int)x2,(int)y2, false, false);

    if(x2-x1 != 0){ 
        m = (y2-y1)/(x2-x1);
        b = y1 - (m*x1);

        if(m >=-1 && m <= 1){
            xmin = (x1 < x2)? x1 : x2;
            xmax = (x1 > x2)? x1 : x2;

            for(int x = (int)xmin+1; x < xmax; x++){
                yd = (m*x)+b;
                yd = floor(0.5+yd);
                firstPointDrawn = pushPoint(x,(int)yd, false, false);
            }
        } else {
            ymin = (y1 < y2)? y1 : y2;
            ymax = (y1 > y2)? y1 : y2;

            for(int y = (int)ymin + 1; y < ymax; y++){
                xd = (y - b)/m;
                xd = floor(0.5+xd);
                firstPointDrawn = pushPoint((int)xd,y, false, false);
            }
        }

    } else { 
        ymin = (y1 < y2) ? y1 : y2;
        ymax = (y1 > y2) ? y1 : y2;
        for(int y = (int)ymin + 1; y < ymax; y++){
            firstPointDrawn = pushPoint((int)x1,y, false, false);
        }
    }
}

point* bresenham(double x1, double y1, double x2, double y2, bool declivity, bool simetric) {
  bool reduced = declivity || simetric;
  firstPointDrawn = pushPoint((int)x1,(int)y1, reduced, true);
  firstPointDrawn = pushPoint((int)x2,(int)y2, reduced, true);

  double deltax = x2 - x1;
  double deltay = y2 - y1;
  double d = 2*deltay - deltax;
  double eastInc = 2*deltay;
  double northeastInc = 2*(deltay - deltax);
  double x = x1;
  double y = y1;

  while(x < x2){
    if(d <= 0){
      d += eastInc;
    } else {
      d += northeastInc;
      y++;
    }
    x++;
    if(declivity || simetric){
      firstPointDrawn = pushPoint((int)x,(int)y, true, false);
    } else {
      firstPointDrawn = pushPoint((int)x,(int)y, false, false);
    }
  }

  return firstPointDrawn;
}

void reductionToFirstOctant(double &x1, double &y1, double &x2, double &y2, bool &declivity, bool &simetric) {
  double deltax = x2 - x1;
  double deltay = y2 - y1;

  if (deltax * deltay < 0) {
    simetric = true;
    y1 *= -1;
    y2 *= -1;

    deltay = y2 - y1;
  }

  if (fabs(deltax) < fabs(deltay)) {
    declivity = true;
    double aux = x1;
    x1 = y1;
    y1 = aux;
    aux = x2;
    x2 = y2;
    y2 = aux;
    deltax = x2 - x1;
    deltay = y2 - y1;
  }

  if (x1 > x2) {
    double aux = x1;
    x1 = x2;
    x2 = aux;
    aux = y1;
    y1 = y2;
    y2 = aux;
  }
}

void inverseTransformation(bool declivity, bool simetric) {
  point* pnt = firstPointDrawn;
  while(pnt != NULL) {
    if(pnt->reduced && !pnt->drawn) {
      if (declivity) {
        int aux = pnt->x;
        pnt->x = pnt->y;
        pnt->y = aux;
      }
      if(simetric) {
        pnt->y *= -1;
      }
    }
    pnt = pnt->next;
  }
}

point* bresenhamWithReductionToFirstOctant(double x1, double y1, double x2, double y2 ) {
  bool declivity = false, simetric = false;
  reductionToFirstOctant(x1, y1, x2, y2, declivity, simetric);
  firstPointDrawn = bresenham(x1, y1, x2, y2, declivity, simetric);
  inverseTransformation(declivity, simetric);

  return firstPointDrawn;
}

point* bresenhamAlgorithm(double x1, double y1, double x2, double y2 ) {
  return isFirstOctant(x1, y1, x2, y2) ? 
    bresenham(x1, y1, x2, y2, false, false) : 
    bresenhamWithReductionToFirstOctant(x1, y1, x2, y2);
}

point* resetList(void){
  firstPointDrawn = NULL;
  return firstPointDrawn;
}

line* resetLineList(){
  firstLine = NULL;
  return firstLine;
}

square* resetSquareList(){
  firstSquare = NULL;
  return firstSquare;
}

triangle* resetTriangleList(){
  firstTriangle = NULL;
  return firstTriangle;
}

polygon* resetPolygonsList(){
  firstPolygon = NULL;
  return firstPolygon;
}

circle* resetCircleList(){
  firstCircle = NULL;
  return firstCircle;
}

point* resetPolygonList(void){
  firstPolygonPoint = NULL;
  return firstPolygonPoint;
}

point* bresenhamToRasterizeCircles(double originX, double originY, double radius) {
  double d = 1 - radius;
  double eastInc = 3;
  double southeastInc = 5 - 2*radius;

  double x = 0;
  double y = radius;
  
  firstPointDrawn = pushPoint(0,(int)radius, false, false);
  firstPointDrawn = pushPoint(0,-(int)radius, false, false);
  firstPointDrawn = pushPoint((int)radius, 0, false, false);
  firstPointDrawn = pushPoint(-(int)radius, 0, false, false);

  while(y > x){
    if(d < 0){
      d += eastInc;
      eastInc += 2;
      southeastInc += 2;
    } else {
      d += southeastInc;
      eastInc += 2;
      southeastInc += 4;
      y--;
    }
    
    x++;
    firstPointDrawn = pushPoint((int)x,(int)y, false, false);
    firstPointDrawn = pushPoint(-(int)x,(int)y, false, false);
    firstPointDrawn = pushPoint(-(int)y, (int)x, false, false);
    firstPointDrawn = pushPoint(-(int)y,-(int)x, false, false);
    firstPointDrawn = pushPoint(-(int)x,-(int)y, false, false);
    firstPointDrawn = pushPoint((int)x,-(int)y, false, false);
    firstPointDrawn = pushPoint((int)y,-(int)x, false, false);
    firstPointDrawn = pushPoint((int)y,(int)x, false, false);
  }

  translateToRealCoordinates(originX, originY);

  return firstPointDrawn;
}

void translateToRealCoordinates(double originX, double originY) {
  point* pnt = firstPointDrawn;
  while(pnt != NULL) {
    if(!pnt->drawn) {
      translate(pnt->x, pnt->y, originX, RIGHT);
      translate(pnt->x, pnt->y, originY, UP);
    }
    pnt = pnt->next;
  }
}

void floodFill(){

}