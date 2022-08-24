#include <cmath>
#include <cstdio>
#include <cstdlib>

#include <stdio.h>

#define PI 3.14159265

#define NONE 0
#define RIGHT 1
#define LEFT 2
#define UP 3
#define DOWN 4

#define COUNTERCLOCKWISE 10010 // Sentido anti-horario
#define CLOCKWISE 10011 // Sentido horario

struct point{
    int x;
    int y;
    bool reduced, drawn, isVertice;
    int centroidCoordinateX;
    int centroidCoordinateY;
    point* next;
};

struct lineVertices{
  point* v1;
  point* v2;
  int centroidCoordinateX;
  int centroidCoordinateY;

  lineVertices* next;
};

struct squareVertices{
  point* v1;
  point* v2;
  point* v3;
  point* v4;
  int centroidCoordinateX;
  int centroidCoordinateY;

  squareVertices* next;
};

struct triangleVertices{
  point* v1;
  point* v2;
  point* v3;
  int centroidCoordinateX;
  int centroidCoordinateY;

  triangleVertices* next;
};

bool isFirstOctant(double x1, double y1, double x2, double y2);
point* pushPoint(int x, int y, bool reduced, bool isVertice);
lineVertices* pushLineVertices(point* v1, point* v2, int centroidCoordinateX, int centroidCoordinateY);
squareVertices* pushSquareVertices(point* v1, point* v2, point* v3, point* v4, int centroidCoordinateX, int centroidCoordinateY);
triangleVertices* pushTriangleVertices(point* v1, point* v2, point* v3, int centroidCoordinateX, int centroidCoordinateY);
point* popPoint();
void retaImediata(double x1,double y1,double x2,double y2);
point* bresenham(double x1, double y1, double x2, double y2, bool declivity, bool simetric);
void reductionToFirstOctant(double &x1, double &y1, double &x2, double &y2, bool &declivity, bool &simetric);
void inverseTransformation(bool declivity, bool simetric);
point* bresenhamWithReductionToFirstOctant(double x1, double y1, double x2, double y2 );
point* bresenhamAlgorithm(double x1, double y1, double x2, double y2 );
point* resetList(void);
point* resetPolygonList(void);
point* pushPolygonVertice(int x, int y);
void translateToRealCoordinates(double originX, double originY);

point* firstPointDrawn = NULL;
point* lastPointDrawn = NULL;

lineVertices* firstLineVertices = NULL;
lineVertices* lastLineVertices = NULL;

squareVertices* firstSquareVertices = NULL;
squareVertices* lastSquareVertices = NULL;

triangleVertices* firstTriangleVertices = NULL;
triangleVertices* lastTriangleVertices = NULL;

point* firstPolygonPoint = NULL;

bool isFirstOctant(double x1, double y1, double x2, double y2) {
  double tangent = (y2 - y1) / (x2 - x1);
  return x1 < x2 && (tangent >= 0 && tangent <= 1);
}


point* pushPoint(int x, int y, bool reduced, bool isVertice) {
  point* pnt = new point;
  pnt->x = x;
  pnt->y = y;
  pnt->reduced = reduced;
  pnt->drawn = false;
  pnt->isVertice = isVertice;
  pnt->next = NULL;
  if (firstPointDrawn == NULL) {
    firstPointDrawn = pnt;
    lastPointDrawn = pnt;
  } else {
    lastPointDrawn->next = pnt;
    lastPointDrawn = pnt;
  }

	return firstPointDrawn;
}

lineVertices* pushLineVertices(point* v1, point* v2, int centroidCoordinateX, int centroidCoordinateY) {
  lineVertices* lv = new lineVertices;
  lv->v1 = v1;
  lv->v2 = v2;
  lv->centroidCoordinateX = centroidCoordinateX;
  lv->centroidCoordinateY = centroidCoordinateY;
  lv->next = NULL;
  if (firstLineVertices == NULL) {
    firstLineVertices = lv;
    lastLineVertices = lv;
  } else {
    lastLineVertices->next = lv;
    lastLineVertices = lv;
  }

  return firstLineVertices;
}

squareVertices* pushSquareVertices(point* v1, point* v2, point* v3, point* v4, int centroidCoordinateX, int centroidCoordinateY) {
  squareVertices* sv = new squareVertices;
  sv->v1 = v1;
  sv->v2 = v2;
  sv->v3 = v3;
  sv->v4 = v4;
  sv->centroidCoordinateX = centroidCoordinateX;
  sv->centroidCoordinateY = centroidCoordinateY;
  sv->next = NULL;
  if (firstSquareVertices == NULL) {
    firstSquareVertices = sv;
    lastSquareVertices = sv;
  } else {
    lastSquareVertices->next = sv;
    lastSquareVertices = sv;
  }

  return firstSquareVertices;
}

triangleVertices* pushTriangleVertices(point* v1, point* v2, point* v3, int centroidCoordinateX, int centroidCoordinateY) {
  triangleVertices* tv = new triangleVertices;
  tv->v1 = v1;
  tv->v2 = v2;
  tv->v3 = v3;
  tv->centroidCoordinateX = centroidCoordinateX;
  tv->centroidCoordinateY = centroidCoordinateY;
  tv->next = NULL;
  if (firstTriangleVertices == NULL) {
    firstTriangleVertices = tv;
    lastTriangleVertices = tv;
  } else {
    lastTriangleVertices->next = tv;
    lastTriangleVertices = tv;
  }

  return firstTriangleVertices;
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
	if(firstPointDrawn != NULL) {
		pnt = firstPointDrawn->next;
    removed = firstPointDrawn;
		delete firstPointDrawn;
		firstPointDrawn = pnt;
	}
	return removed;
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

lineVertices* resetLineList(){
  firstLineVertices = NULL;
  return firstLineVertices;
}

squareVertices* resetSquareList(){
  firstSquareVertices = NULL;
  return firstSquareVertices;
}

triangleVertices* resetTriangleList(){
  firstTriangleVertices = NULL;
  return firstTriangleVertices;
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

void rotate(int &x, int &y, float angle, int direction) {
  double rad = angle * PI / 180;
  if(direction == CLOCKWISE) {
    x = (int)(x * cos(-rad) - y * sin(-rad));
    y = (int)(x * sin(-rad) + y * cos(-rad));
  } else if(direction == COUNTERCLOCKWISE) {
    x = (int)(x * cos(rad) - y * sin(rad));
    y = (int)(x * sin(rad) + y * cos(rad));
  }
}

void scale(int &x, int &y, int quantity) {
  x *= quantity;
  y *= quantity;
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