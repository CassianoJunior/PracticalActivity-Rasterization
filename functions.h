#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <stdio.h>

#define NONE 0
#define RIGHT 1
#define LEFT 2
#define UP 3
#define DOWN 4

struct point{
    int x;
    int y;
    bool reduced, drawn, isCirclePoint;
    point* next;
};

bool isFirstOctant(double x1, double y1, double x2, double y2);
point* pushPoint(int x, int y, bool reduced);
point* popPoint();
void retaImediata(double x1,double y1,double x2,double y2);
point* bresenham(double x1, double y1, double x2, double y2, bool declivity, bool simetric);
void reductionToFirstOctant(double &x1, double &y1, double &x2, double &y2, bool &declivity, bool &simetric);
void inverseTransformation(bool declivity, bool simetric);
point* bresenhamWithReductionToFirstOctant(double x1, double y1, double x2, double y2);
point* bresenhamAlgorithm(double x1, double y1, double x2, double y2);
point* resetList(void);
point* resetPolygonList(void);
point* pushPolygonVertice(int x, int y);
void translateToRealCoordinates(double originX, double originY);

point* firstPoint = NULL;
point* lastPoint = NULL;

point* firstPolygonPoint = NULL;

bool isFirstOctant(double x1, double y1, double x2, double y2) {
  double tangent = (y2 - y1) / (x2 - x1);
  return x1 < x2 && (tangent >= 0 && tangent <= 1);
}


point* pushPoint(int x, int y, bool reduced) {
  point* pnt = new point;
  pnt->x = x;
  pnt->y = y;
  pnt->reduced = reduced;
  pnt->drawn = false;
  pnt->next = NULL;
  if (firstPoint == NULL) {
    firstPoint = pnt;
    lastPoint = pnt;
  } else {
    lastPoint->next = pnt;
    lastPoint = pnt;
  }

	return firstPoint;
}

point* pushPolygonVertice(int x, int y) {
  point* pnt = new point;
  pnt->x = x;
  pnt->y = y;
  pnt->next = NULL;
  if (firstPolygonPoint == NULL) {
    firstPolygonPoint = pnt;
  } else {
    point* aux = firstPolygonPoint;
    while (aux->next != NULL) {
      aux = aux->next;
    }
    aux->next = pnt;
  }

	return firstPolygonPoint;
}

point* popPoint() {
	point* pnt = NULL;
  point* removed = NULL;
	if(firstPoint != NULL) {
		pnt = firstPoint->next;
    removed = firstPoint;
		delete firstPoint;
		firstPoint = pnt;
	}
	return removed;
}

void retaImediata(double x1,double y1,double x2,double y2) {
    double m, b, yd, xd;
    double xmin, xmax,ymin,ymax;
    firstPoint = pushPoint((int)x1,(int)y1, false);
    firstPoint = pushPoint((int)x2,(int)y2, false);

    if(x2-x1 != 0){ 
        m = (y2-y1)/(x2-x1);
        b = y1 - (m*x1);

        if(m >=-1 && m <= 1){
            xmin = (x1 < x2)? x1 : x2;
            xmax = (x1 > x2)? x1 : x2;

            for(int x = (int)xmin+1; x < xmax; x++){
                yd = (m*x)+b;
                yd = floor(0.5+yd);
                firstPoint = pushPoint(x,(int)yd, false);
            }
        } else {
            ymin = (y1 < y2)? y1 : y2;
            ymax = (y1 > y2)? y1 : y2;

            for(int y = (int)ymin + 1; y < ymax; y++){
                xd = (y - b)/m;
                xd = floor(0.5+xd);
                firstPoint = pushPoint((int)xd,y, false);
            }
        }

    } else { 
        ymin = (y1 < y2) ? y1 : y2;
        ymax = (y1 > y2) ? y1 : y2;
        for(int y = (int)ymin + 1; y < ymax; y++){
            firstPoint = pushPoint((int)x1,y, false);
        }
    }
}

point* bresenham(double x1, double y1, double x2, double y2, bool declivity, bool simetric) {
  bool reduced = declivity || simetric;
  firstPoint = pushPoint((int)x1,(int)y1, reduced);
  firstPoint = pushPoint((int)x2,(int)y2, reduced);

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
      firstPoint = pushPoint((int)x,(int)y, true);
    } else {
      firstPoint = pushPoint((int)x,(int)y, false);
    }
  }

  return firstPoint;
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
  point* pnt = firstPoint;
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

point* bresenhamWithReductionToFirstOctant(double x1, double y1, double x2, double y2){
  bool declivity = false, simetric = false;
  reductionToFirstOctant(x1, y1, x2, y2, declivity, simetric);
  firstPoint = bresenham(x1, y1, x2, y2, declivity, simetric);
  inverseTransformation(declivity, simetric);

  return firstPoint;
}

point* bresenhamAlgorithm(double x1, double y1, double x2, double y2) {
  return isFirstOctant(x1, y1, x2, y2) ? 
    bresenham(x1, y1, x2, y2, false, false) : 
    bresenhamWithReductionToFirstOctant(x1, y1, x2, y2);
}

point* resetList(void){
  firstPoint = NULL;
  return firstPoint;
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
  firstPoint = pushPoint(0,(int)radius, false);
  firstPoint = pushPoint(0,-(int)radius, false);
  firstPoint = pushPoint((int)radius, 0, false);
  firstPoint = pushPoint(-(int)radius, 0, false);

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
    firstPoint = pushPoint((int)x,(int)y, false);
    firstPoint = pushPoint(-(int)x,(int)y, false);
    firstPoint = pushPoint(-(int)y, (int)x, false);
    firstPoint = pushPoint(-(int)y,-(int)x, false);
    firstPoint = pushPoint(-(int)x,-(int)y, false);
    firstPoint = pushPoint((int)x,-(int)y, false);
    firstPoint = pushPoint((int)y,-(int)x, false);
    firstPoint = pushPoint((int)y,(int)x, false);
  }

  translateToRealCoordinates(originX, originY);

  return firstPoint;
}

void translate(int &x, int &y, int quantity, int direction) {
  if(direction == RIGHT) {
    x += quantity;
  } else if(direction == LEFT) {
    x -= quantity;
  } else if(direction == UP) {
    y += quantity;
  } else if(direction == DOWN) {
    y -= quantity;
  }
}

void translateToRealCoordinates(double originX, double originY) {
  point* pnt = firstPoint;
  while(pnt != NULL) {
    if(!pnt->drawn) {
      translate(pnt->x, pnt->y, originX, RIGHT);
      translate(pnt->x, pnt->y, originY, UP);
    }
    pnt = pnt->next;
  }
}