#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "functions.h"

#define REGION_TO_CLOSE_POLYGON 3
#define DEFAULT_DISPLACEMENT 5
#define DEFAULT_ANGLE 5

#define f 102  // Alternar modo de desenho livre (Nao limpa a tela quando troca o modo de desenho)
#define BACKSPACE 8 // Limpar a tela
#define ESCAPE 27 // Sair do programa

#define DRAW_LINES 0
#define DRAW_SQUARES 1
#define DRAW_TRIANGLES 2
#define DRAW_POLYGONS 3
#define DRAW_CIRCLES 4

#define l 108 // Desenhar as linhas (padrao)
#define p 112 // Desenhar os poligonos
#define s 115 // Desenhar os quadrados 
#define t 116 // Desenhar os triangulos | Ativar translacao se tiver no modo de transformacoes geometricas

#define T 84 // Mudar o modo para aplicar tranformacoes geometricas
  // #define NONE 0
  // #define RIGHT 1
  // #define LEFT 2
  // #define UP 3
  // #define DOWN 4 
  #define TRANSLATE 1000 // Translacao
  #define ROTATE 1001 // Rotacao
  #define SCALE 1002 // Escala
  #define MIRROR 1003 // Espelhamento / reflexao
  #define SHEAR 1004 // Cisalhamento
#define r 114 // Aplicar rotacoes no modo de transformacoes geometricas
#define S 83 // Aplicar escala no modo de transformacoes geometricas
// #define t 116 // Aplicar translacao no modo de transformacoes gerometricas
#define c 99 // Aplicar cisalhamento no modo de transformacoes geometricas
#define m 109 // Aplicar espelhamento no modo de transformacoes geometricas


double xA, yA, xB, yB, xC, yC;
double polygonStartVerticeX, polygonStartVerticeY;
bool isFirstLine = true;
bool freeMode = false;
bool isToApplyGeometricTransformations = false;
int activeTransformation = TRANSLATE;
int activeMode = DRAW_LINES;

bool click1 = false, click2 = false, click3 = false;

int width = 800, height = 600;

point* start = NULL;
point* polygonPoints = NULL;

void init(void);
void reshape(int w, int h);

void displayLines(void);
void displaySquares(void);
  int* getLineOrSquareCentroid(double xA, double yA, double xB, double yB);
void displayTriangles(void);
  int* getTriangleCentroid(double xA, double yA, double xB, double yB, double xC, double yC);
void displayPolygons(void);
  bool coordinateIsInClosingArea(double x, double y);
void displayCircles(void);
  double calculateCircleRadius(double xA, double yA, double xB, double yB);

void keyboard(unsigned char key, int x, int y);
  void backToLastMode(void);
void processSpecialKeys(int key, int x, int y);

void mouseToDrawLinesAndSquares(int button, int state, int x, int y);
void mouseToDrawTriangles(int button, int state, int x, int y);
void mouseToDrawPolygons(int button, int state, int x, int y);
void mouseToDrawCircles(int button, int state, int x, int y);
void mouseWhileGeometricTranformationsIsOn(int button, int state, int x, int y);

void drawPoints(point* firstPoint);
void clearScreen(void);
void resetClicks(void);

void applyTranslation(point* aux, int direction);
void applyRotation(point* aux, int direction);

void invalidKeyMessage(void);

int main(int argc, char** argv) {
    glutInit(&argc, argv); 
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB); 
    glutInitWindowSize (width, height);  
    glutInitWindowPosition (100, 100); 
    glutCreateWindow ("Rasterizacao"); 
    init(); 
    glutReshapeFunc(reshape); 
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(processSpecialKeys);
    glutMouseFunc(mouseToDrawLinesAndSquares); 
    glutDisplayFunc(displayLines); 

    freeMode 
      ? printf("Free mode active!\n")
      : printf("Free mode desactive!\n");

    glutMainLoop(); 
    
    return 0;
}

void init(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0); 
}

void reshape(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, w, h);
	
	width = w;
	height = h;
  glOrtho (0, w, 0, h, -1 ,1);  

	glMatrixMode(GL_MODELVIEW);
  click1 = true; 
  click2 = true;
}

void keyboard(unsigned char key, int x, int y){
  switch (key) { 
    case ESCAPE: 
      exit(0);
      break;
    case BACKSPACE:
      clearScreen();
      glutPostRedisplay();
      break;
    case f:
      freeMode = !freeMode;
      freeMode 
        ? printf("Free mode ON!\n")
        : printf("Free mode OFF!\n");
      break;
    case T: 
      isToApplyGeometricTransformations = !isToApplyGeometricTransformations;
      if(isToApplyGeometricTransformations){
        glutMouseFunc(mouseWhileGeometricTranformationsIsOn);
        printf("Ready to apply geometric transformations!\n");
        printf("Press 't' to apply translations\n");
        printf("Press 'r' to apply rotations\n");
        printf("Press 's' to apply scales\n");
        printf("Press 'c' to apply shears\n");
        printf("Press 'm' to apply mirrors\n");
      } else {
        printf("Geometric transformations OFF!\n");
        backToLastMode();
      }
      break;
    case l:
      if(!isToApplyGeometricTransformations) {
        activeMode = DRAW_LINES;
        printf("Mode changed to draw lines!\n");
        glutDisplayFunc(displayLines);
        glutMouseFunc(mouseToDrawLinesAndSquares);
        if(!freeMode) {
          clearScreen();
          glutPostRedisplay();
        }
        resetClicks();
      } else {

      }
      break;
    case p:
      if(!isToApplyGeometricTransformations){
        activeMode = DRAW_POLYGONS;
        printf("Mode changed to draw polygons!\n");
        glutDisplayFunc(displayPolygons);
        glutMouseFunc(mouseToDrawPolygons);
        if(!freeMode) {
          clearScreen();
          glutPostRedisplay();
        }
        resetClicks();
      } else {

      }
      break;
    case s:
      if(!isToApplyGeometricTransformations) {
        activeMode = DRAW_SQUARES;
        printf("Mode changed to draw squares!\n");
        glutDisplayFunc(displaySquares);
        glutMouseFunc(mouseToDrawLinesAndSquares);
        if(!freeMode) {
          clearScreen();
          glutPostRedisplay();
        }
        resetClicks();
      } else {
        activeTransformation = SCALE;
        printf("Ready to scale objects\n");
      }
      break;
    case t:
      if(!isToApplyGeometricTransformations) {
        activeMode = DRAW_TRIANGLES;
        printf("Mode changed to draw triangles!\n");
        glutDisplayFunc(displayTriangles);
        glutMouseFunc(mouseToDrawTriangles);
        if(!freeMode) {
          clearScreen();
          glutPostRedisplay();
        }
        resetClicks();
      } else {
        activeTransformation = TRANSLATE;
        printf("Ready to translate objects\n");
        printf("Use the arrow keys to move!\n");
      }
      break;
    case r:
      if(isToApplyGeometricTransformations) {
        activeTransformation = ROTATE;
        printf("Ready to rotate objects\n");
        printf("Use arrow left to rotate clockwise or arrow right to rotate counterclockwise!\n");
      } else {
        invalidKeyMessage();
      }
      break;
    case c:
      if(!isToApplyGeometricTransformations) {
        activeMode = DRAW_CIRCLES;
        printf("Mode changed to draw circles!\n");
        glutDisplayFunc(displayCircles);
        glutMouseFunc(mouseToDrawCircles);
        if(!freeMode) {
          clearScreen();
          glutPostRedisplay();
        }
        resetClicks();
      } else {
        activeTransformation = SHEAR;
        printf("Ready to shear objects\n");
      }
      break;
    case m:
      if(isToApplyGeometricTransformations) {
        activeTransformation = MIRROR;
        printf("Ready to mirror objects\n");
      }
      break;
    default:
      invalidKeyMessage();
      break;
  }
}

void processSpecialKeys(int key, int x, int y) {
  switch (key) {
    case GLUT_KEY_UP:
      if(isToApplyGeometricTransformations) {
        point* aux = start;
        switch (activeTransformation) {
          case TRANSLATE:
            applyTranslation(aux, UP);
            glClear(GL_COLOR_BUFFER_BIT);
            drawPoints(start);
            break;
          case ROTATE:
            printf("Invalid move!\n");
            printf("Use arrow left to rotate clockwise or arrow right to rotate counterclockwise!\n");
            break;
          // case SCALE:
          //   applyScale(aux);
          //   break;
          // case SHEAR:
          //   applyShear(aux);
          //   break;
          // case MIRROR:
          //   applyMirror(aux);
          //   break;
          
          default:
            break;
        }
      }
      break;
    case GLUT_KEY_DOWN:
      if(isToApplyGeometricTransformations) {
        point* aux = start;
        switch (activeTransformation) {
          case TRANSLATE:
            applyTranslation(aux, DOWN);
            glClear(GL_COLOR_BUFFER_BIT);
            drawPoints(start);
            break;
          case ROTATE:
            printf("Invalid move!\n");
            printf("Use arrow left to rotate clockwise or arrow right to rotate counterclockwise!\n");
            break;
          // case SCALE:
          //   applyScale(aux);
          //   break;
          // case SHEAR:
          //   applyShear(aux);
          //   break;
          // case MIRROR:
          //   applyMirror(aux);
          //   break;
          
          default:
            break;
        }
      }
      break;
    case GLUT_KEY_LEFT:
      if(isToApplyGeometricTransformations) {
        point* aux = start;
        switch (activeTransformation) {
          case TRANSLATE:
            applyTranslation(aux, LEFT);
            glClear(GL_COLOR_BUFFER_BIT);
            drawPoints(start);
            break;
          case ROTATE:
            applyRotation(aux, COUNTERCLOCKWISE);
            glClear(GL_COLOR_BUFFER_BIT);
            drawPoints(start);
            break;
          // case SCALE:
          //   applyScale(aux);
          //   break;
          // case SHEAR:
          //   applyShear(aux);
          //   break;
          // case MIRROR:
          //   applyMirror(aux);
          //   break;
          
          default:
            break;
        }
      }
      break;
    case GLUT_KEY_RIGHT:
      if(isToApplyGeometricTransformations) {
          point* aux = start;
          switch (activeTransformation) {
            case TRANSLATE:
              applyTranslation(aux, RIGHT);
              glClear(GL_COLOR_BUFFER_BIT);
              drawPoints(start);
              break;
            // case ROTATE:
            //   applyRotation(aux);
            //   break;
            // case SCALE:
            //   applyScale(aux);
            //   break;
            // case SHEAR:
            //   applyShear(aux);
            //   break;
            // case MIRROR:
            //   applyMirror(aux);
            //   break;
            
            default:
              break;
          }
        }
      break;
    default:
      break;
  }
  glutSwapBuffers();
}

void mouseToDrawLinesAndSquares(int button, int state, int x, int y) {
  switch (button) {
    case GLUT_LEFT_BUTTON:
      if (state == GLUT_DOWN) {
        if(click1){
          click2 = true;
          xB = x;
          yB = height - y;
          printf("click2: xByB(%.0lf,%.0lf)\n",xB,yB);
          glutPostRedisplay();
        }else{
          click1 = true;
          xA = x;
          yA = height - y;
          printf("click1: xAyA(%.0lf,%.0lf)\n",xA,yA);
        }
      }
      break;
    default:
        break;
  }
}

void mouseToDrawTriangles(int button, int state, int x, int y) {
  switch (button) {
    case GLUT_LEFT_BUTTON:
      if (state == GLUT_DOWN) {
        if(click1 || click2) {
          if (click2) {
            click3 = true;
            xC = x;
            yC = height - y;
            printf("click3: xCyC(%.0lf,%.0lf)\n",xC,yC);
            glutPostRedisplay();
          } else {
            click2 = true;
            xB = x;
            yB = height - y;
            printf("click2: xByB(%.0lf,%.0lf)\n",xB,yB);
          }
        }else{
          click1 = true;
          xA = x;
          yA = height - y;
          printf("click1: xAyA(%.0lf,%.0lf)\n",xA,yA);
        }
      }
      break;
    default:
        break;
  }
}

void mouseToDrawPolygons(int button, int state, int x, int y) {
  switch (button) {
    case GLUT_LEFT_BUTTON:
      if (state == GLUT_DOWN) {
        if(isFirstLine) {
          isFirstLine = false;
          if(click1){
            click2 = true;
            xB = x;
            yB = height - y;
            printf("click2: xByB(%.0lf,%.0lf)\n",xB,yB);
            glutPostRedisplay();
          }else{
            click1 = true;
            xA = x;
            yA = height - y;
            xB = xA;
            yB = yA;
            polygonStartVerticeX = xA;
            polygonStartVerticeY = yA;
            polygonPoints = pushPolygonVertice(polygonStartVerticeX, polygonStartVerticeY);
            printf("click1: xAyA(%.0lf,%.0lf)\n",xA,yA);
          }
        } else {
          click2 = true;
          xA = xB;
          yA = yB;
          xB = x;
          yB = height - y;
          printf("click: xy(%.0lf,%.0lf)\n", xB, yB);
          glutPostRedisplay();
        }
      } 
        
      break;
    default:
      break;
  }
}

void mouseToDrawCircles(int button, int state, int x, int y) {
  switch (button) {
    case GLUT_LEFT_BUTTON:
      if (state == GLUT_DOWN) {
        if(click1){
          click2 = true;
          xB = x;
          yB = height - y;
          printf("click2: xByB(%.0lf,%.0lf)\n",xB,yB);
          glutPostRedisplay();
        }else{
          click1 = true;
          xA = x;
          yA = height - y;
          printf("click1: xAyA(%.0lf,%.0lf)\n",xA,yA);
        }
      }
      break;
    default:
        break;
  }
}

void mouseWhileGeometricTranformationsIsOn(int button, int state, int x, int y){
  switch (button) {
    case GLUT_LEFT_BUTTON:
      if (state == GLUT_DOWN) {
        printf("Press 'T' to turn off geometric transformations mode and then back to draw!\n");
      }
      break;
    default:
      break;
  }
}

void displayLines(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    glColor3f (1.0, 1.0, 1.0); 
    
    if(click1 && click2){
      int* centroidCoordinates = getLineOrSquareCentroid(xA, yA, xB, yB);

      start = bresenhamAlgorithm(xA, yA, xB, yB, LINE, centroidCoordinates);
      printf("Line vertices: xAyA(%.0lf,%.0lf) | xByB(%.0lf,%.0lf)\n",xA,yA,xB,yB);
      drawPoints(start);
      click1 = false;
      click2 = false;
    }

    glutSwapBuffers();

}

void displaySquares(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    glColor3f (1.0, 1.0, 1.0); 
    
    if(click1 && click2){
      int* centroidCoordinates = getLineOrSquareCentroid(xA, yA, xB, yB);

      start = bresenhamAlgorithm(xA, yA, xB, yA, SQUARE, centroidCoordinates);
      drawPoints(start);
      start = bresenhamAlgorithm(xB, yA, xB, yB, SQUARE, centroidCoordinates);
      drawPoints(start);
      start = bresenhamAlgorithm(xB, yB, xA, yB, SQUARE, centroidCoordinates);
      drawPoints(start);
      start = bresenhamAlgorithm(xA, yB, xA, yA, SQUARE, centroidCoordinates);
      drawPoints(start);
      printf("Square vertices:\nxAyA(%.0lf,%.0lf) | xAyB(%.0lf,%.0lf)\nxAyB(%.0lf,%.0lf) | xByB(%.0lf,%.0lf)\nxByB(%.0lf,%.0lf) | xAyB(%.0lf,%.0lf)\nxAyB(%.0lf,%.0lf) | xAyA(%.0lf,%.0lf)\n",xA,yA,xB,yA,xB,yA,xB,yB,xB,yB,xA,yB,xA,yB,xA,yA);
      click1 = false;
      click2 = false;
    }

    glutSwapBuffers();

}

void displayTriangles(void){
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  
  glColor3f (1.0, 1.0, 1.0); 
  
  if(click1 && click2 && click3){
    int* centroidCoordinates = getTriangleCentroid(xA, yA, xB, yB, xC, yC);

    start = bresenhamAlgorithm(xA, yA, xB, yB, TRIANGLE, centroidCoordinates);
    drawPoints(start);
    start = bresenhamAlgorithm(xB, yB, xC, yC, TRIANGLE, centroidCoordinates);
    drawPoints(start);
    start = bresenhamAlgorithm(xC, yC, xA, yA, TRIANGLE, centroidCoordinates);
    drawPoints(start);
    printf("Triangle vertices:\nxAyA(%.0lf,%.0lf) | xAyB(%.0lf,%.0lf)\nxAyB(%.0lf,%.0lf) | xByB(%.0lf,%.0lf)\nxByB(%.0lf,%.0lf) | xAyB(%.0lf,%.0lf)\n",xA,yA,xB,yB,xB,yB,xC,yC,xC,yC,xA,yA);
    click1 = false;
    click2 = false;
    click3 = false;
  }

  glutSwapBuffers();
}

void displayPolygons(void){
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    
    glColor3f (1.0, 1.0, 1.0); 
    
    if(click1 && click2){
      int centroidCoordinates[2] = {0, 0};
      if(coordinateIsInClosingArea(xB, yB)){
        click1 = false;
        isFirstLine = true;
        start = bresenhamAlgorithm(polygonStartVerticeX, polygonStartVerticeY, xA, yA, POLYGON, centroidCoordinates);
        printf("Line poins: xAyA(%.0lf,%.0lf) | xByB(%.0lf,%.0lf)\n",xA, yA, polygonStartVerticeX,polygonStartVerticeY);
        drawPoints(start);
        point* pnt = polygonPoints;
        printf("Polygon vertices:\n");
        int i = 1;
        while(pnt != NULL){
          printf("Point %d: xy(%d,%d)\n",i++,pnt->x,pnt->y);
          pnt = pnt->next;
        }
        polygonPoints = resetPolygonList();
      } else {
        start = bresenhamAlgorithm(xA, yA, xB, yB, POLYGON, centroidCoordinates);
        polygonPoints = pushPolygonVertice(xB, yB);
        drawPoints(start);
        printf("Line poins: xAyA(%.0lf,%.0lf) | xByB(%.0lf,%.0lf)\n",xA,yA,xB,yB);
      }
      click2 = false;
    }

    glutSwapBuffers();

}

void displayCircles(void){
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  
  glColor3f (1.0, 1.0, 1.0); 
  
  if(click1 && click2){
    double radius = calculateCircleRadius(xA, yA, xB, yB);
    start = bresenhamToRasterizeCircles(xA, yA, radius);
    drawPoints(start);
    printf("Circle center: xy(%.0lf,%.0lf)\n",xA,yA);
    printf("Circle radius: %.0lf\n", radius);
    click1 = false;
    click2 = false;
  }

  glutSwapBuffers();
}



void drawPoints(point* firstPoint){
  point* pnt;
  pnt = firstPoint;
  glBegin(GL_POINTS); 
    while(pnt != NULL){
      glVertex2i(pnt->x,pnt->y);
      pnt->drawn = true;
      pnt = pnt->next;
    }
  glEnd();  
}

void clearScreen(){
  start = resetList();
  drawPoints(start);
}

void resetClicks(){
  click1 = false;
  click2 = false;
  click3 = false;
}

bool coordinateIsInClosingArea(double x, double y){
  return 
    ((x >= polygonStartVerticeX && x <= polygonStartVerticeX + REGION_TO_CLOSE_POLYGON) || (x <= polygonStartVerticeX && x >= polygonStartVerticeX - REGION_TO_CLOSE_POLYGON)) && 
    ((y >= polygonStartVerticeY && y <= polygonStartVerticeY + REGION_TO_CLOSE_POLYGON) || (y <= polygonStartVerticeY && y >= polygonStartVerticeY - REGION_TO_CLOSE_POLYGON));
}

double calculateCircleRadius(double xA, double yA, double xB, double yB) {
  return sqrt(pow(xA - xB, 2) + pow(yA - yB, 2));
}

void applyTranslation(point* pnt, int direction) {
  while(pnt != NULL){
    translate(pnt->x, pnt->y, DEFAULT_DISPLACEMENT, direction);
    pnt = pnt->next;
  }
}

void applyRotation(point* pnt, int direction) {
  while(pnt != NULL){
    translate(pnt->x, pnt->y, pnt->centroidCoordinateX, LEFT);
    translate(pnt->x, pnt->y, pnt->centroidCoordinateY, DOWN);
    rotate(pnt->x, pnt->y, DEFAULT_ANGLE, direction);
    translate(pnt->x, pnt->y, pnt->centroidCoordinateX, RIGHT);
    translate(pnt->x, pnt->y, pnt->centroidCoordinateY, UP);
    pnt = pnt->next;
  }
}

int* getLineOrSquareCentroid(double xA, double yA, double xB, double yB) {
  int centroidCoordinates[2];
  centroidCoordinates[0] = (int)(xA + xB) / 2;
  centroidCoordinates[1] = (int)(yA + yB) / 2;

  return centroidCoordinates;
}

int* getTriangleCentroid(double xA, double yA, double xB, double yB, double xC, double yC) {
  int centroidCoordinates[2];
  centroidCoordinates[0] = (int)(xA + xB + xC) / 3;
  centroidCoordinates[1] = (int)(yA + yB + yC) / 3;

  return centroidCoordinates;
}

void invalidKeyMessage() {
  printf("Invalid key!\n");
  printf("Press 'l' to draw lines\n");
  printf("Press 's' to draw squares\n");
  printf("Press 't' to draw triagles\n");
  printf("Press 'p' to draw polygons\n");
  printf("Press 'F' to toggle free mode\n");
  printf("Press 'T' to toggle mode of apply geometric transformations\n");
  printf("Press 'backspace' to clear screen\n");
  printf("Press 'esc' to exit\n");
}

void backToLastMode() {
  switch(activeMode) {
    case DRAW_LINES:
      activeMode = DRAW_LINES;
      glutMouseFunc(mouseToDrawLinesAndSquares);
      glutDisplayFunc(displayLines);
      break;
    case DRAW_SQUARES:
      activeMode = DRAW_SQUARES;
      glutMouseFunc(mouseToDrawLinesAndSquares);
      glutDisplayFunc(displaySquares);
      break;
    case DRAW_TRIANGLES:
      activeMode = DRAW_TRIANGLES;
      glutMouseFunc(mouseToDrawTriangles);
      glutDisplayFunc(displayTriangles);
      break;
    case DRAW_POLYGONS:
      activeMode = DRAW_POLYGONS;
      glutMouseFunc(mouseToDrawPolygons);
      glutDisplayFunc(displayPolygons);
      break;
    case DRAW_CIRCLES:
      activeMode = DRAW_CIRCLES;
      glutMouseFunc(mouseToDrawCircles);
      glutDisplayFunc(displayCircles);
      break;
    default:
      activeMode = DRAW_LINES;
      glutMouseFunc(mouseToDrawLinesAndSquares);
      glutDisplayFunc(displayLines);
      break;
  }
}