struct point{
    int x;
    int y;
    bool reduced, drawn, isVertice;
    int centroidCoordinateX;
    int centroidCoordinateY;
    point* next;
};

struct line{
  point* v1;
  point* v2;
  int centroidCoordinateX;
  int centroidCoordinateY;

  line* next;
};

struct square{
  point* v1;
  point* v2;
  point* v3;
  point* v4;
  int centroidCoordinateX;
  int centroidCoordinateY;

  square* next;
};

struct triangle{
  point* v1;
  point* v2;
  point* v3;
  int centroidCoordinateX;
  int centroidCoordinateY;

  triangle* next;
};

struct polygon{
  point* listOfVertices;
  int centroidCoordinateX;
  int centroidCoordinateY;
  polygon* next;
};

struct circle{
  point* center;
  int radius;

  circle* next;
};

point* pushPoint(int x, int y, bool reduced, bool isVertice);
point* popPoint();
point* firstPointDrawn = NULL;
point* lastPointDrawn = NULL;

line* pushLine(point* v1, point* v2, int centroidCoordinateX, int centroidCoordinateY);
line* firstLine = NULL;
line* lastLine = NULL;

square* pushSquare(point* v1, point* v2, point* v3, point* v4, int centroidCoordinateX, int centroidCoordinateY);
square* firstSquare = NULL;
square* lastSquare = NULL;

triangle* pushTriangle(point* v1, point* v2, point* v3, int centroidCoordinateX, int centroidCoordinateY);
triangle* firstTriangle = NULL;
triangle* lastTriangle = NULL;

polygon* pushPolygon(point* listOfVertices);
polygon* firstPolygon = NULL;
polygon* lastPolygon = NULL;

circle* pushCircle(point* center, int radius);
circle* firstCircle = NULL;
circle* lastCircle = NULL;

point* pushPolygonVertice(int x, int y);
point* firstPolygonPoint = NULL;

int getCentroidX(point* listOfVertices);
int getCentroidY(point* listOfVertices);

double getArea(point* listOfVertices);


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

line* pushLine(point* v1, point* v2, int centroidCoordinateX, int centroidCoordinateY) {
  line* lv = new line;
  lv->v1 = v1;
  lv->v2 = v2;
  lv->centroidCoordinateX = centroidCoordinateX;
  lv->centroidCoordinateY = centroidCoordinateY;
  lv->next = NULL;
  if (firstLine == NULL) {
    firstLine = lv;
    lastLine = lv;
  } else {
    lastLine->next = lv;
    lastLine = lv;
  }

  return firstLine;
}

square* pushSquare(point* v1, point* v2, point* v3, point* v4, int centroidCoordinateX, int centroidCoordinateY) {
  square* sv = new square;
  sv->v1 = v1;
  sv->v2 = v2;
  sv->v3 = v3;
  sv->v4 = v4;
  sv->centroidCoordinateX = centroidCoordinateX;
  sv->centroidCoordinateY = centroidCoordinateY;
  sv->next = NULL;
  if (firstSquare == NULL) {
    firstSquare = sv;
    lastSquare = sv;
  } else {
    lastSquare->next = sv;
    lastSquare = sv;
  }

  return firstSquare;
}

triangle* pushTriangle(point* v1, point* v2, point* v3, int centroidCoordinateX, int centroidCoordinateY) {
  triangle* tv = new triangle;
  tv->v1 = v1;
  tv->v2 = v2;
  tv->v3 = v3;
  tv->centroidCoordinateX = centroidCoordinateX;
  tv->centroidCoordinateY = centroidCoordinateY;
  tv->next = NULL;
  if (firstTriangle == NULL) {
    firstTriangle = tv;
    lastTriangle = tv;
  } else {
    lastTriangle->next = tv;
    lastTriangle = tv;
  }

  return firstTriangle;
}

polygon* pushPolygon(point* listOfVertices, int centroidCoordinateX, int centroidCoordinateY) {
  polygon* pv = new polygon;
  pv->listOfVertices = listOfVertices;
  pv->centroidCoordinateX = centroidCoordinateX;
  pv->centroidCoordinateY = centroidCoordinateY;
  pv->next = NULL;
  if (firstPolygon == NULL) {
    firstPolygon = pv;
    lastPolygon = pv;
  } else {
    lastPolygon->next = pv;
    lastPolygon = pv;
  }

  return firstPolygon;
}

circle* pushCircle(point* center, int radius) {
  circle* cv = new circle;
  cv->center = center;
  cv->radius = radius;
  cv->next = NULL;
  if (firstCircle == NULL) {
    firstCircle = cv;
    lastCircle = cv;
  } else {
    lastCircle->next = cv;
    lastCircle = cv;
  }

  return firstCircle;
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

int getCentroidX(point* listOfVertices) {
  if (listOfVertices == NULL) return 0;
  int sum = 0;
  int area = getArea(listOfVertices);
  point* aux = listOfVertices;
  point* control;
  while(aux->next != NULL){
    sum += (aux->x + aux->next->x) * (aux->x * aux->next->y - aux->next->x * aux->y);
    aux = aux->next;
    if(aux->next == NULL) control = aux;
  }
  sum += (aux->x + control->x) * (aux->x * control->y - control->x * aux->y);
  
  return (int) sum/(6*area);
}

int getCentroidY(point* listOfVertices) {
  if (listOfVertices == NULL) return 0;
  int sum = 0;
  int area = getArea(listOfVertices);
  point* aux = listOfVertices;
  point* control;
  while(aux->next != NULL){
    sum += (aux->y + aux->next->y) * (aux->x * aux->next->y - aux->next->x * aux->y);
    aux = aux->next;
    if(aux->next == NULL) control = aux;
  }
  sum += (aux->y + control->y) * (aux->x * control->y - control->x * aux->y);
  
  return (int) sum/(6*area);
}

double getArea(point* listOfVertices) {
  if (listOfVertices == NULL) return 0;
  int sum = 0;
  point* aux = listOfVertices;
  point* control;
  while(aux->next != NULL){
    sum += aux->x * aux->next->y - aux->next->x * aux->y;
    aux = aux->next;
    if(aux->next == NULL) control = aux;
  }
  sum += aux->x * control->y - control->x * aux->y;

  return sum / 2;
}