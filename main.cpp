#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>

#include "algorithms.h"

#define REGION_TO_CLOSE_POLYGON 3
#define DEFAULT_DISPLACEMENT 5
#define DEFAULT_ANGLE 5
#define DEFAULT_INCREASE_SCALE 1.2
#define DEFAULT_DECREASE_SCALE 0.8
#define DEFAULT_SHEAR 1

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
    // #define INCREASE 10020 // Aumentar
    // #define DECREASE 10021 // Diminuir
  #define MIRROR 1003 // Espelhamento / reflexao
    // #define X_AXIS 10030
    // #define Y_AXIS 10031
  #define SHEAR 1004 // Cisalhamento
#define r 114 // Aplicar rotacoes no modo de transformacoes geometricas
#define S 83 // Aplicar escala no modo de transformacoes geometricas
// #define t 116 // Aplicar translacao no modo de transformacoes gerometricas
#define c 99 // Aplicar cisalhamento no modo de transformacoes geometricas
#define m 109 // Aplicar espelhamento no modo de transformacoes geometricas

#define SPACE 32 // Acionar algoritmo flood fill

double xA, yA, xB, yB, xC, yC;
double polygonStartVerticeX, polygonStartVerticeY;
bool isFirstLine = true;
bool freeMode = false;
bool isToApplyGeometricTransformations = false;
int activeTransformation = TRANSLATE;
int activeMode = DRAW_LINES;

bool click1 = false, click2 = false, click3 = false;

int width = 800, height = 600;

point* start = NULL; // Inicio da lista de pontos que estao desenhados na tela em determinado momento
point* polygonPoints = NULL; // Inicio da lista de vertices dos poligonos, utilizada para guardar os vertices. 

void init(void);
void reshape(int w, int h);

void displayLines(void);
void displaySquares(void);
void displayTriangles(void);
void displayPolygons(void);
  bool coordinateIsInClosingArea(double x, double y);
void displayCircles(void);
  double calculateCircleRadius(double xA, double yA, double xB, double yB);
void displayToFloodFill(void);

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
void resetLists(void);
void resetClicks(void);
point* createPoint(double x, double y, int centroidCoordinateX, int centroidCoordinateY);
line* createLine(point* v1, point* v2);

void applyTranslation(point* aux, int direction);
void applyRotation(point* aux, int direction);
void applyScale(double quantity);
void applyMirror(int axis, int direction);
  line* mirrorLine(line* lineToMirror, int axis, int direction);
void applyShearing(int quantity);

void floodFill(int x, int y, float* oldColor, float* newColor);
void mouseToFloodFill(int button, int state, int x, int y);

void keyMapMessage(void);

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

    keyMapMessage();

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
}

void keyboard(unsigned char key, int x, int y){
  switch (key) { 
    case ESCAPE: 
      exit(0);
      break;
    case BACKSPACE:
      resetLists();
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
          resetLists();
          glutPostRedisplay();
        }
        resetClicks();
      } else {
        printf("Invalid key!\n");
        keyMapMessage();
      }
      break;
    case p:
      if(!isToApplyGeometricTransformations){
        activeMode = DRAW_POLYGONS;
        printf("Mode changed to draw polygons!\n");
        glutDisplayFunc(displayPolygons);
        glutMouseFunc(mouseToDrawPolygons);
        if(!freeMode) {
          resetLists();
          glutPostRedisplay();
        }
        resetClicks();
      } else {
        printf("Invalid key!\n");
        keyMapMessage();
      }
      break;
    case s:
      if(!isToApplyGeometricTransformations) {
        activeMode = DRAW_SQUARES;
        printf("Mode changed to draw squares!\n");
        glutDisplayFunc(displaySquares);
        glutMouseFunc(mouseToDrawLinesAndSquares);
        if(!freeMode) {
          resetLists();
          glutPostRedisplay();
        }
        resetClicks();
      } else {
        activeTransformation = SCALE;
        printf("Ready to scale objects\n");
        printf("Use arrow up to increase scale or arrow down to decrease scale!\n");
      }
      break;
    case t:
      if(!isToApplyGeometricTransformations) {
        activeMode = DRAW_TRIANGLES;
        printf("Mode changed to draw triangles!\n");
        glutDisplayFunc(displayTriangles);
        glutMouseFunc(mouseToDrawTriangles);
        if(!freeMode) {
          resetLists();
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
        printf("Use arrow left to rotate counterclockwise or arrow right to rotate clockwise!\n");
      } else {
        printf("Invalid key!\n");
        keyMapMessage();
      }
      break;
    case c:
      if(!isToApplyGeometricTransformations) {
        activeMode = DRAW_CIRCLES;
        printf("Mode changed to draw circles!\n");
        glutDisplayFunc(displayCircles);
        glutMouseFunc(mouseToDrawCircles);
        if(!freeMode) {
          resetLists();
          glutPostRedisplay();
        }
        resetClicks();
      } else {
        activeTransformation = SHEAR;
        printf("Ready to shear objects\n");
        printf("Use arrow up to increase shear or arrow down to decrease shear!\n");
      }
      break;
    case m:
      if(isToApplyGeometricTransformations) {
        activeTransformation = MIRROR;
        printf("Ready to mirror objects\n");
        printf("Use up or down arrows to mirror in the X axis and right or left arrows to mirror in the Y axis!\n");
      } else {
        printf("Invalid key!\n");
        keyMapMessage();
      }
      break;
    case SPACE:
      printf("Select one object drawned to apply flood fill!\n");
      glutMouseFunc(mouseToFloodFill);
      glutDisplayFunc(displayToFloodFill);
      resetClicks();
      break;
    default:
      printf("Invalid key!\n");
      keyMapMessage();
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
            glClear(GL_COLOR_BUFFER_BIT);
            clearScreen();
            applyTranslation(aux, UP);
            drawPoints(start);
            break;
          case ROTATE:
            printf("Invalid move!\n");
            printf("Use arrow left to rotate clockwise or arrow right to rotate counterclockwise!\n");
            break;
          case SCALE:
            glClear(GL_COLOR_BUFFER_BIT);
            clearScreen();
            applyScale(DEFAULT_INCREASE_SCALE);
            drawPoints(start);
            break;
          case SHEAR:
            glClear(GL_COLOR_BUFFER_BIT);
            clearScreen();
            applyShearing(DEFAULT_SHEAR);
            drawPoints(start);
            break;
          case MIRROR:
            glClear(GL_COLOR_BUFFER_BIT);
            clearScreen();
            applyMirror(X_AXIS, UP);
            drawPoints(start);
            break;
          
          default:
            break;
        }
        glutSwapBuffers();
      }
      break;
    case GLUT_KEY_DOWN:
      if(isToApplyGeometricTransformations) {
        point* aux = start;
        switch (activeTransformation) {
          case TRANSLATE:
            glClear(GL_COLOR_BUFFER_BIT);
            clearScreen();
            applyTranslation(aux, DOWN);
            drawPoints(start);
            break;
          case ROTATE:
            printf("Invalid move!\n");
            printf("Use arrow left to rotate clockwise or arrow right to rotate counterclockwise!\n");
            break;
          case SCALE:
            glClear(GL_COLOR_BUFFER_BIT);
            clearScreen();
            applyScale(DEFAULT_DECREASE_SCALE);
            drawPoints(start);
            break;
          case SHEAR:
            glClear(GL_COLOR_BUFFER_BIT);
            clearScreen();
            applyShearing(-DEFAULT_SHEAR);
            drawPoints(start);
            break;
          case MIRROR:
            glClear(GL_COLOR_BUFFER_BIT);
            clearScreen();
            applyMirror(X_AXIS, DOWN);
            drawPoints(start);
            break;
          
          default:
            break;
        }
        glutSwapBuffers();
      }
      break;
    case GLUT_KEY_LEFT:
      if(isToApplyGeometricTransformations) {
        point* aux = start;
        switch (activeTransformation) {
          case TRANSLATE:
            glClear(GL_COLOR_BUFFER_BIT);
            clearScreen();
            applyTranslation(aux, LEFT);
            drawPoints(start);
            break;
          case ROTATE:
            glClear(GL_COLOR_BUFFER_BIT);
            clearScreen();
            applyRotation(aux, COUNTERCLOCKWISE);
            drawPoints(start);
            break;
          case SCALE:
            printf("Invalid move!\n");
            printf("Use arrow up to increase scaling or arrow down to decrease scaling!\n");
            break;
          case SHEAR:
            printf("Invalid move!\n");
            printf("Use arrow up to increase shear or arrow down to decrease shear!\n");
            break;
          case MIRROR:
            glClear(GL_COLOR_BUFFER_BIT);
            clearScreen();
            applyMirror(Y_AXIS, LEFT);
            drawPoints(start);
            break;
          
          default:
            break;
        }
        glutSwapBuffers();  
      }
      break;
    case GLUT_KEY_RIGHT:
      if(isToApplyGeometricTransformations) {
          point* aux = start;
          switch (activeTransformation) {
            case TRANSLATE:
              glClear(GL_COLOR_BUFFER_BIT);
              clearScreen();
              applyTranslation(aux, RIGHT);
              drawPoints(start);
              break;
            case ROTATE:
              glClear(GL_COLOR_BUFFER_BIT);
              clearScreen();
              applyRotation(aux, CLOCKWISE);
              drawPoints(start);
              break;
            case SCALE:
              printf("Invalid move!\n");
              printf("Use arrow up to increase scaling or arrow down to decrease scaling!\n");
              break;
            case SHEAR:
              printf("Invalid move!\n");
              printf("Use arrow up to increase shear or arrow down to decrease shear!\n");
              break;
            case MIRROR:
              glClear(GL_COLOR_BUFFER_BIT);
              clearScreen();
              applyMirror(Y_AXIS, RIGHT);
              drawPoints(start);
              break;
            
            default:
              break;
          }
          glutSwapBuffers();
        }
      break;
    default:
      break;
    

  }
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

void mouseToFloodFill(int button, int state, int x, int y){
  switch (button) {
    case GLUT_LEFT_BUTTON:
      if (state == GLUT_DOWN) {
        if(!click1){
          click1 = true;
          xA = x;
          yA = height - y;
          printf("click: xAyA(%.0lf,%.0lf)\n",xA,yA);
          glutPostRedisplay();
        }
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
      int centroidCoordinateX = (int)(xA + xB) / 2;
      int centroidCoordinateY = (int)(yA + yB) / 2;

      point* p1 = createPoint(xA, yA, centroidCoordinateX, centroidCoordinateY);
      point* p2 = createPoint(xB, yB, centroidCoordinateX, centroidCoordinateY);
      firstLine = pushLine(p1, p2, centroidCoordinateX, centroidCoordinateY);

      start = bresenhamAlgorithm(xA, yA, xB, yB);
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
      int centroidCoordinateX = (int)(xA + xB) / 2;
      int centroidCoordinateY = (int)(yA + yB) / 2;

      point* p1 = createPoint(xA, yA, centroidCoordinateX, centroidCoordinateY);
      point* p2 = createPoint(xB, yB, centroidCoordinateX, centroidCoordinateY);
      point* p3 = createPoint(xB, yA, centroidCoordinateX, centroidCoordinateY);
      point* p4 = createPoint(xA, yB, centroidCoordinateX, centroidCoordinateY);

      firstSquare = pushSquare(p1, p2, p3, p4, centroidCoordinateX, centroidCoordinateY);

      start = bresenhamAlgorithm(xA, yA, xB, yA);
      drawPoints(start);
      start = bresenhamAlgorithm(xB, yA, xB, yB);
      drawPoints(start);
      start = bresenhamAlgorithm(xB, yB, xA, yB);
      drawPoints(start);
      start = bresenhamAlgorithm(xA, yB, xA, yA);
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
    int centroidCoordinateX = (int)(xA + xB + xC) / 3;
    int centroidCoordinateY = (int)(yA + yB + yC) / 3;

    point* p1 = createPoint(xA, yA, centroidCoordinateX, centroidCoordinateY);
    point* p2 = createPoint(xB, yB, centroidCoordinateX, centroidCoordinateY);
    point* p3 = createPoint(xC, yC, centroidCoordinateX, centroidCoordinateY);

    firstTriangle = pushTriangle(p1, p2, p3, centroidCoordinateX, centroidCoordinateY);

    start = bresenhamAlgorithm(xA, yA, xB, yB);
    drawPoints(start);
    start = bresenhamAlgorithm(xB, yB, xC, yC);
    drawPoints(start);
    start = bresenhamAlgorithm(xC, yC, xA, yA);
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
      if(coordinateIsInClosingArea(xB, yB)){
        click1 = false;
        isFirstLine = true;
        start = bresenhamAlgorithm(polygonStartVerticeX, polygonStartVerticeY, xA, yA);
        printf("Line poins: xAyA(%.0lf,%.0lf) | xByB(%.0lf,%.0lf)\n",xA, yA, polygonStartVerticeX,polygonStartVerticeY);
        drawPoints(start);
        point* pnt = polygonPoints;
        printf("Polygon vertices:\n");
        int i = 1;
        while(pnt != NULL){
          printf("Point %d: xy(%d,%d)\n",i++,pnt->x,pnt->y);
          pnt = pnt->next;
        }
        int centroidCoordinateX = getCentroidX(polygonPoints);
        int centroidCoordinateY = getCentroidY(polygonPoints);
        firstPolygon = pushPolygon(polygonPoints, centroidCoordinateX, centroidCoordinateY);
        polygonPoints = resetPolygonList();
      } else {
        start = bresenhamAlgorithm(xA, yA, xB, yB);
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

    point* center = createPoint(xA, yA, xA, yA);
    firstCircle = pushCircle(center, (int)radius);

    start = bresenhamToRasterizeCircles(xA, yA, radius);
    drawPoints(start);
    printf("Circle center: xy(%.0lf,%.0lf)\n",xA,yA);
    printf("Circle radius: %.0lf\n", radius);
    click1 = false;
    click2 = false;
  }

  glutSwapBuffers();
}

void displayToFloodFill(void){
  glClear(GL_COLOR_BUFFER_BIT);
  glLoadIdentity();
  
  glColor3f(1.0, 1.0, 1.0);
  if(click1){
    printf("Applying flood fill!\n");
    click1 = false;
    float oldColor[] = {0, 0, 0};
    glPointSize(1);
    float newColor[] = {0, 1, 1};
    floodFill(xA, yA, oldColor, newColor);
    printf("Flood fill applied!\n");
  }
}

void drawPoints(point* firstPoint){
  point* pnt = firstPoint;
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

void resetLists(){
  clearScreen();
  polygonPoints = resetPolygonList();
  firstTriangle = resetTriangleList();
  firstSquare= resetSquareList();
  firstLine= resetLineList();
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
  line* line = firstLine;
  while(line != NULL){
    translate(line->v1->x, line->v1->y, DEFAULT_DISPLACEMENT, direction);
    translate(line->v2->x, line->v2->y, DEFAULT_DISPLACEMENT, direction);
    line->centroidCoordinateX = (int)(line->v1->x + line->v2->x) / 2;
    line->centroidCoordinateY = (int)(line->v1->y + line->v2->y) / 2;

    start = bresenhamAlgorithm(line->v1->x, line->v1->y, line->v2->x, line->v2->y);
    drawPoints(start);
    line = line->next;
  }

  square* square = firstSquare;
  while(square != NULL){
    translate(square->v1->x, square->v1->y, DEFAULT_DISPLACEMENT, direction);
    translate(square->v2->x, square->v2->y, DEFAULT_DISPLACEMENT, direction);
    translate(square->v3->x, square->v3->y, DEFAULT_DISPLACEMENT, direction);
    translate(square->v4->x, square->v4->y, DEFAULT_DISPLACEMENT, direction);
    square->centroidCoordinateX = (int)(square->v1->x + square->v2->x) / 2;
    square->centroidCoordinateY = (int)(square->v1->y + square->v2->y) / 2;

    start = bresenhamAlgorithm(square->v1->x, square->v1->y, square->v3->x, square->v3->y);
    drawPoints(start);
    start = bresenhamAlgorithm(square->v3->x, square->v3->y, square->v2->x, square->v2->y);
    drawPoints(start);
    start = bresenhamAlgorithm(square->v2->x, square->v2->y, square->v4->x, square->v4->y);
    drawPoints(start);
    start = bresenhamAlgorithm(square->v4->x, square->v4->y, square->v1->x, square->v1->y);
    drawPoints(start);

    square = square->next;
  }

  triangle* triangle = firstTriangle;
  while(triangle != NULL){
    translate(triangle->v1->x, triangle->v1->y, DEFAULT_DISPLACEMENT, direction);
    translate(triangle->v2->x, triangle->v2->y, DEFAULT_DISPLACEMENT, direction);
    translate(triangle->v3->x, triangle->v3->y, DEFAULT_DISPLACEMENT, direction);
    triangle->centroidCoordinateX = (int)(triangle->v1->x + triangle->v2->x + triangle->v3->x) / 3;
    triangle->centroidCoordinateY = (int)(triangle->v1->y + triangle->v2->y + triangle->v3->y) / 3;

    start = bresenhamAlgorithm(triangle->v1->x, triangle->v1->y, triangle->v2->x, triangle->v2->y);
    drawPoints(start);
    start = bresenhamAlgorithm(triangle->v2->x, triangle->v2->y, triangle->v3->x, triangle->v3->y);
    drawPoints(start);
    start = bresenhamAlgorithm(triangle->v3->x, triangle->v3->y, triangle->v1->x, triangle->v1->y);
    drawPoints(start);

    triangle = triangle->next;
  }

  polygon* polygon = firstPolygon;
  while(polygon != NULL){
    point* pnt = polygon->listOfVertices;
    while(pnt != NULL){
      translate(pnt->x, pnt->y, DEFAULT_DISPLACEMENT, direction);
      pnt = pnt->next;
    }

    pnt = polygon->listOfVertices;
    point* aux = pnt;
    while(pnt->next != NULL){
      start = bresenhamAlgorithm(pnt->x, pnt->y, pnt->next->x, pnt->next->y);
      drawPoints(start);
      if(pnt->next->next == NULL) aux = pnt->next;
      pnt = pnt->next;
    }

    start = bresenhamAlgorithm(polygon->listOfVertices->x, polygon->listOfVertices->y, aux->x, aux->y);
    drawPoints(start);

    polygon = polygon->next;
  }

  circle* circle = firstCircle;
  while(circle != NULL){
    translate(circle->center->x, circle->center->y, DEFAULT_DISPLACEMENT, direction);

    start = bresenhamToRasterizeCircles(circle->center->x, circle->center->y, circle->radius);
    drawPoints(start);

    circle = circle->next;
  }
}

// A rotacao esta com algum problema, ela diminui a figura desenhada a cada rotacao 
void applyRotation(point* pnt, int direction) {
  line* line = firstLine;
  while(line != NULL) {
    int displacementX = line->centroidCoordinateX;
    int displacementY = line->centroidCoordinateY;
    translate(line->v1->x, line->v1->y, displacementX, LEFT);
    translate(line->v1->x, line->v1->y, displacementY, DOWN);
    translate(line->v2->x, line->v2->y, displacementX, LEFT);
    translate(line->v2->x, line->v2->y, displacementY, DOWN);
    rotate(line->v1->x, line->v1->y, DEFAULT_ANGLE, direction);
    rotate(line->v2->x, line->v2->y, DEFAULT_ANGLE, direction);
    translate(line->v1->x, line->v1->y, displacementX, RIGHT);
    translate(line->v1->x, line->v1->y, displacementY, UP);
    translate(line->v2->x, line->v2->y, displacementX, RIGHT);
    translate(line->v2->x, line->v2->y, displacementY, UP);

    start = bresenhamAlgorithm(line->v1->x, line->v1->y, line->v2->x, line->v2->y);
    drawPoints(start);
    line = line->next;
  }

  square* square = firstSquare;
  while(square != NULL) {
    int displacementX = square->centroidCoordinateX;
    int displacementY = square->centroidCoordinateY;
    translate(square->v1->x, square->v1->y, displacementX, LEFT);
    translate(square->v1->x, square->v1->y, displacementY, DOWN);
    translate(square->v2->x, square->v2->y, displacementX, LEFT);
    translate(square->v2->x, square->v2->y, displacementY, DOWN);
    translate(square->v3->x, square->v3->y, displacementX, LEFT);
    translate(square->v3->x, square->v3->y, displacementY, DOWN);
    translate(square->v4->x, square->v4->y, displacementX, LEFT);
    translate(square->v4->x, square->v4->y, displacementY, DOWN);
    rotate(square->v1->x, square->v1->y, DEFAULT_ANGLE, direction);
    rotate(square->v2->x, square->v2->y, DEFAULT_ANGLE, direction);
    rotate(square->v3->x, square->v3->y, DEFAULT_ANGLE, direction);
    rotate(square->v4->x, square->v4->y, DEFAULT_ANGLE, direction);
    translate(square->v1->x, square->v1->y, displacementX, RIGHT);
    translate(square->v1->x, square->v1->y, displacementY, UP);
    translate(square->v2->x, square->v2->y, displacementX, RIGHT);
    translate(square->v2->x, square->v2->y, displacementY, UP);
    translate(square->v3->x, square->v3->y, displacementX, RIGHT);
    translate(square->v3->x, square->v3->y, displacementY, UP);
    translate(square->v4->x, square->v4->y, displacementX, RIGHT);
    translate(square->v4->x, square->v4->y, displacementY, UP);

    start = bresenhamAlgorithm(square->v1->x, square->v1->y, square->v3->x, square->v3->y);
    drawPoints(start);
    start = bresenhamAlgorithm(square->v3->x, square->v3->y, square->v2->x, square->v2->y);
    drawPoints(start);
    start = bresenhamAlgorithm(square->v2->x, square->v2->y, square->v4->x, square->v4->y);
    drawPoints(start);
    start = bresenhamAlgorithm(square->v4->x, square->v4->y, square->v1->x, square->v1->y);
    drawPoints(start);

    square = square->next;
  }

  triangle* triangle = firstTriangle;
  while(triangle != NULL) {
    int displacementX = triangle->centroidCoordinateX;
    int displacementY = triangle->centroidCoordinateY;
    translate(triangle->v1->x, triangle->v1->y, displacementX, LEFT);
    translate(triangle->v1->x, triangle->v1->y, displacementY, DOWN);
    translate(triangle->v2->x, triangle->v2->y, displacementX, LEFT);
    translate(triangle->v2->x, triangle->v2->y, displacementY, DOWN);
    translate(triangle->v3->x, triangle->v3->y, displacementX, LEFT);
    translate(triangle->v3->x, triangle->v3->y, displacementY, DOWN);
    rotate(triangle->v1->x, triangle->v1->y, DEFAULT_ANGLE, direction);
    rotate(triangle->v2->x, triangle->v2->y, DEFAULT_ANGLE, direction);
    rotate(triangle->v3->x, triangle->v3->y, DEFAULT_ANGLE, direction);
    translate(triangle->v1->x, triangle->v1->y, displacementX, RIGHT);
    translate(triangle->v1->x, triangle->v1->y, displacementY, UP);
    translate(triangle->v2->x, triangle->v2->y, displacementX, RIGHT);
    translate(triangle->v2->x, triangle->v2->y, displacementY, UP);
    translate(triangle->v3->x, triangle->v3->y, displacementX, RIGHT);
    translate(triangle->v3->x, triangle->v3->y, displacementY, UP);

    start = bresenhamAlgorithm(triangle->v1->x, triangle->v1->y, triangle->v2->x, triangle->v2->y);
    drawPoints(start);
    start = bresenhamAlgorithm(triangle->v2->x, triangle->v2->y, triangle->v3->x, triangle->v3->y);
    drawPoints(start);
    start = bresenhamAlgorithm(triangle->v3->x, triangle->v3->y, triangle->v1->x, triangle->v1->y);
    drawPoints(start);

    triangle = triangle->next;
  }

  polygon* polygon = firstPolygon;
  while(polygon != NULL) {
    int displacementX = polygon->centroidCoordinateX;
    int displacementY = polygon->centroidCoordinateY;
    point* vertice = polygon->listOfVertices;
    while(vertice != NULL) {
      translate(vertice->x, vertice->y, displacementX, LEFT);
      translate(vertice->x, vertice->y, displacementY, DOWN);
      rotate(vertice->x, vertice->y, DEFAULT_ANGLE, direction);
      translate(vertice->x, vertice->y, displacementX, RIGHT);
      translate(vertice->x, vertice->y, displacementY, UP);
      vertice = vertice->next;
    }

    vertice = polygon->listOfVertices;
    point* aux = vertice;
    while(vertice->next != NULL){
      start = bresenhamAlgorithm(vertice->x, vertice->y, vertice->next->x, vertice->next->y);
      drawPoints(start);
      if(vertice->next->next == NULL) aux = vertice->next;
      vertice = vertice->next;
    }

    start = bresenhamAlgorithm(polygon->listOfVertices->x, polygon->listOfVertices->y, aux->x, aux->y);
    drawPoints(start);

    polygon = polygon->next;
  }

}

void applyScale(double quantity){
  line* line = firstLine;
  while(line != NULL) {
    translate(line->v1->x, line->v1->y, line->centroidCoordinateX, LEFT);
    translate(line->v1->x, line->v1->y, line->centroidCoordinateY, DOWN);
    translate(line->v2->x, line->v2->y, line->centroidCoordinateX, LEFT);
    translate(line->v2->x, line->v2->y, line->centroidCoordinateY, DOWN);
    scale(line->v1->x, line->v1->y, quantity);
    scale(line->v2->x, line->v2->y, quantity);
    translate(line->v1->x, line->v1->y, line->centroidCoordinateX, RIGHT);
    translate(line->v1->x, line->v1->y, line->centroidCoordinateY, UP);
    translate(line->v2->x, line->v2->y, line->centroidCoordinateX, RIGHT);
    translate(line->v2->x, line->v2->y, line->centroidCoordinateY, UP);

    start = bresenhamAlgorithm(line->v1->x, line->v1->y, line->v2->x, line->v2->y);
    drawPoints(start);
    
    line = line->next;
  } 

  square* square = firstSquare;
  while(square != NULL) {
    translate(square->v1->x, square->v1->y, square->centroidCoordinateX, LEFT);
    translate(square->v1->x, square->v1->y, square->centroidCoordinateY, DOWN);
    translate(square->v2->x, square->v2->y, square->centroidCoordinateX, LEFT);
    translate(square->v2->x, square->v2->y, square->centroidCoordinateY, DOWN);
    translate(square->v3->x, square->v3->y, square->centroidCoordinateX, LEFT);
    translate(square->v3->x, square->v3->y, square->centroidCoordinateY, DOWN);
    translate(square->v4->x, square->v4->y, square->centroidCoordinateX, LEFT);
    translate(square->v4->x, square->v4->y, square->centroidCoordinateY, DOWN);
    scale(square->v1->x, square->v1->y, quantity);
    scale(square->v2->x, square->v2->y, quantity);
    scale(square->v3->x, square->v3->y, quantity);
    scale(square->v4->x, square->v4->y, quantity);
    translate(square->v1->x, square->v1->y, square->centroidCoordinateX, RIGHT);
    translate(square->v1->x, square->v1->y, square->centroidCoordinateY, UP);
    translate(square->v2->x, square->v2->y, square->centroidCoordinateX, RIGHT);
    translate(square->v2->x, square->v2->y, square->centroidCoordinateY, UP);
    translate(square->v3->x, square->v3->y, square->centroidCoordinateX, RIGHT);
    translate(square->v3->x, square->v3->y, square->centroidCoordinateY, UP);
    translate(square->v4->x, square->v4->y, square->centroidCoordinateX, RIGHT);
    translate(square->v4->x, square->v4->y, square->centroidCoordinateY, UP);

    start = bresenhamAlgorithm(square->v1->x, square->v1->y, square->v3->x, square->v3->y);
    drawPoints(start);
    start = bresenhamAlgorithm(square->v3->x, square->v3->y, square->v2->x, square->v2->y);
    drawPoints(start);
    start = bresenhamAlgorithm(square->v2->x, square->v2->y, square->v4->x, square->v4->y);
    drawPoints(start);
    start = bresenhamAlgorithm(square->v4->x, square->v4->y, square->v1->x, square->v1->y);
    drawPoints(start);
    
    square = square->next;
  }

  triangle* triangle = firstTriangle;
  while(triangle != NULL) {
    translate(triangle->v1->x, triangle->v1->y, triangle->centroidCoordinateX, LEFT);
    translate(triangle->v1->x, triangle->v1->y, triangle->centroidCoordinateY, DOWN);
    translate(triangle->v2->x, triangle->v2->y, triangle->centroidCoordinateX, LEFT);
    translate(triangle->v2->x, triangle->v2->y, triangle->centroidCoordinateY, DOWN);
    translate(triangle->v3->x, triangle->v3->y, triangle->centroidCoordinateX, LEFT);
    translate(triangle->v3->x, triangle->v3->y, triangle->centroidCoordinateY, DOWN);
    scale(triangle->v1->x, triangle->v1->y, quantity);
    scale(triangle->v2->x, triangle->v2->y, quantity);
    scale(triangle->v3->x, triangle->v3->y, quantity);
    translate(triangle->v1->x, triangle->v1->y, triangle->centroidCoordinateX, RIGHT);
    translate(triangle->v1->x, triangle->v1->y, triangle->centroidCoordinateY, UP);
    translate(triangle->v2->x, triangle->v2->y, triangle->centroidCoordinateX, RIGHT);
    translate(triangle->v2->x, triangle->v2->y, triangle->centroidCoordinateY, UP);
    translate(triangle->v3->x, triangle->v3->y, triangle->centroidCoordinateX, RIGHT);
    translate(triangle->v3->x, triangle->v3->y, triangle->centroidCoordinateY, UP);

    start = bresenhamAlgorithm(triangle->v1->x, triangle->v1->y, triangle->v2->x, triangle->v2->y);
    drawPoints(start);
    start = bresenhamAlgorithm(triangle->v2->x, triangle->v2->y, triangle->v3->x, triangle->v3->y);
    drawPoints(start);
    start = bresenhamAlgorithm(triangle->v3->x, triangle->v3->y, triangle->v1->x, triangle->v1->y);
    drawPoints(start);
    
    triangle = triangle->next;
  }

  circle* circle = firstCircle;
  while(circle != NULL) {
    circle->radius = (int)(circle->radius*quantity);

    start = bresenhamToRasterizeCircles(circle->center->x, circle->center->y, circle->radius);
    drawPoints(start);

    circle = circle->next;
  }

  polygon* polygon = firstPolygon;
  while(polygon != NULL) {
    point* vertice = polygon->listOfVertices;
    while(vertice != NULL) {
      translate(vertice->x, vertice->y, polygon->centroidCoordinateX, LEFT);
      translate(vertice->x, vertice->y, polygon->centroidCoordinateY, DOWN);
      scale(vertice->x, vertice->y, quantity);
      translate(vertice->x, vertice->y, polygon->centroidCoordinateX, RIGHT);
      translate(vertice->x, vertice->y, polygon->centroidCoordinateY, UP);
      vertice = vertice->next;
    }

    point* pnt = polygon->listOfVertices;
    point* aux = pnt;
    while(pnt->next != NULL){
      start = bresenhamAlgorithm(pnt->x, pnt->y, pnt->next->x, pnt->next->y);
      drawPoints(start);
      if(pnt->next->next == NULL) aux = pnt->next;
      pnt = pnt->next;
    }

    start = bresenhamAlgorithm(polygon->listOfVertices->x, polygon->listOfVertices->y, aux->x, aux->y);
    drawPoints(start);

    polygon = polygon->next;
  }
}

void applyMirror(int axis, int direction){
  line* lines = firstLine;
  while(lines != NULL) {
    lines = mirrorLine(lines, axis, direction);
    
    start = bresenhamAlgorithm(lines->v1->x, lines->v1->y, lines->v2->x, lines->v2->y);
    lines->centroidCoordinateX = (lines->v1->x + lines->v2->x) / 2;
    lines->centroidCoordinateY = (lines->v1->y + lines->v2->y) / 2;
    drawPoints(start);

    lines = lines->next;
  }

  square* square = firstSquare;
  while(square != NULL){
    switch(direction){
      case UP:
        if(square->v1->y > square->v2->y){
          int YDistance = square->v1->y - square->v2->y; 
          mirror(square->v2->x, square->v2->y, axis, YDistance);
          mirror(square->v4->x, square->v4->y, axis, YDistance);
        } else {
          int YDistance = square->v2->y - square->v1->y;
          mirror(square->v3->x, square->v3->y, axis, YDistance);
          mirror(square->v1->x, square->v1->y, axis, YDistance);
        }
        break;
      case DOWN:
        if(square->v1->y < square->v2->y){
          int YDistance = square->v1->y - square->v2->y;
          mirror(square->v2->x, square->v2->y, axis, YDistance);
          mirror(square->v4->x, square->v4->y, axis, YDistance);
        } else {
          int YDistance = square->v2->y - square->v1->y;
          mirror(square->v3->x, square->v3->y, axis, YDistance);
          mirror(square->v1->x, square->v1->y, axis, YDistance);
        }
        break;
      case LEFT:
        if(square->v1->x < square->v2->x){
          int XDistance = square->v1->x - square->v2->x;
          mirror(square->v2->x, square->v2->y, axis, XDistance);
          mirror(square->v3->x, square->v3->y, axis, XDistance);
        } else {
          int XDistance = square->v2->x - square->v1->x;
          mirror(square->v4->x, square->v4->y, axis, XDistance);
          mirror(square->v1->x, square->v1->y, axis, XDistance);
        }
        break;
      case RIGHT:
        if(square->v1->x > square->v2->x){
          int XDistance = square->v1->x - square->v2->x;
          mirror(square->v2->x, square->v2->y, axis, XDistance);
          mirror(square->v3->x, square->v3->y, axis, XDistance);
        } else {
          int XDistance = square->v2->x - square->v1->x;
          mirror(square->v4->x, square->v4->y, axis, XDistance);
          mirror(square->v1->x, square->v1->y, axis, XDistance);
        }
        break;
      default:
        break;
    }

    start = bresenhamAlgorithm(square->v1->x, square->v1->y, square->v3->x, square->v3->y);
    drawPoints(start);
    start = bresenhamAlgorithm(square->v3->x, square->v3->y, square->v2->x, square->v2->y);
    drawPoints(start);
    start = bresenhamAlgorithm(square->v2->x, square->v2->y, square->v4->x, square->v4->y);
    drawPoints(start);
    start = bresenhamAlgorithm(square->v4->x, square->v4->y, square->v1->x, square->v1->y);
    drawPoints(start);

    square->centroidCoordinateX = (square->v1->x + square->v2->x + square->v3->x + square->v4->x) / 4;
    square->centroidCoordinateY = (square->v1->y + square->v2->y + square->v3->y + square->v4->y) / 4;

    square = square->next;
  }

  triangle* triangle = firstTriangle;
  line* lineA;
  line* lineB;
  while(triangle != NULL) {
    switch(direction){
      case UP:
        {
          int Ymax = triangle->v1->y;
          if(triangle->v2->y > Ymax) Ymax = triangle->v2->y;
          if(triangle->v3->y > Ymax) Ymax = triangle->v3->y;

          if(Ymax == triangle->v1->y) {
            lineA = createLine(triangle->v1, triangle->v2);
            lineB = createLine(triangle->v1, triangle->v3);
          } else if(Ymax == triangle->v2->y) {
            lineA = createLine(triangle->v2, triangle->v1);
            lineB = createLine(triangle->v2, triangle->v3);
          } else {
            lineA = createLine(triangle->v3, triangle->v1);
            lineB = createLine(triangle->v3, triangle->v2);
          }

          mirrorLine(lineA, axis, direction);
          mirrorLine(lineB, axis, direction);
          break;
        }
      case DOWN:
        {
          int Ymin = triangle->v1->y;
          if(triangle->v2->y < Ymin) Ymin = triangle->v2->y;
          if(triangle->v3->y < Ymin) Ymin = triangle->v3->y;

          if(Ymin == triangle->v1->y) {
            lineA = createLine(triangle->v1, triangle->v2);
            lineB = createLine(triangle->v1, triangle->v3);
          } else if(Ymin == triangle->v2->y) {
            lineA = createLine(triangle->v2, triangle->v1);
            lineB = createLine(triangle->v2, triangle->v3);
          } else {
            lineA = createLine(triangle->v3, triangle->v1);
            lineB = createLine(triangle->v3, triangle->v2);
          }

          mirrorLine(lineA, axis, direction);
          mirrorLine(lineB, axis, direction);
          break;
        }
      case LEFT:
        {
          int Xmin = triangle->v1->x;
          if(triangle->v2->x < Xmin) Xmin = triangle->v2->x;
          if(triangle->v3->x < Xmin) Xmin = triangle->v3->x;

          if(Xmin == triangle->v1->x) {
            lineA = createLine(triangle->v1, triangle->v2);
            lineB = createLine(triangle->v1, triangle->v3);
          } else if(Xmin == triangle->v2->x) {
            lineA = createLine(triangle->v2, triangle->v1);
            lineB = createLine(triangle->v2, triangle->v3);
          } else {
            lineA = createLine(triangle->v3, triangle->v1);
            lineB = createLine(triangle->v3, triangle->v2);
          }

          mirrorLine(lineA, axis, direction);
          mirrorLine(lineB, axis, direction);
          break;
        }
      case RIGHT:
        {
          int Xmax = triangle->v1->x;
          if(triangle->v2->x > Xmax) Xmax = triangle->v2->x;
          if(triangle->v3->x > Xmax) Xmax = triangle->v3->x;

          if(Xmax == triangle->v1->x) {
            lineA = createLine(triangle->v1, triangle->v2);
            lineB = createLine(triangle->v1, triangle->v3);
          } else if(Xmax == triangle->v2->x) {
            lineA = createLine(triangle->v2, triangle->v1);
            lineB = createLine(triangle->v2, triangle->v3);
          } else {
            lineA = createLine(triangle->v3, triangle->v1);
            lineB = createLine(triangle->v3, triangle->v2);
          }

          mirrorLine(lineA, axis, direction);
          mirrorLine(lineB, axis, direction);
          break;
        }
      default:
        break;
    } 


    start = bresenhamAlgorithm(triangle->v1->x, triangle->v1->y, triangle->v2->x, triangle->v2->y);
    drawPoints(start);
    start = bresenhamAlgorithm(triangle->v2->x, triangle->v2->y, triangle->v3->x, triangle->v3->y);
    drawPoints(start);
    start = bresenhamAlgorithm(triangle->v3->x, triangle->v3->y, triangle->v1->x, triangle->v1->y);
    drawPoints(start);

    triangle = triangle->next;
  }

  circle* circle = firstCircle;
  while(circle != NULL) {
    switch(direction){
      case UP:
        mirror(circle->center->x, circle->center->y, axis, circle->radius);
        break;
      case DOWN:
        mirror(circle->center->x, circle->center->y, axis, -(circle->radius));
        break;
      case LEFT:
        mirror(circle->center->x, circle->center->y, axis, -(circle->radius));
        break;
      case RIGHT:
        mirror(circle->center->x, circle->center->y, axis, circle->radius);
        break;
      default:
        break;
    }

    start = bresenhamToRasterizeCircles(circle->center->x, circle->center->y, circle->radius);
    drawPoints(start);

    circle = circle->next;
  }

  polygon* polygon = firstPolygon;
  while(polygon != NULL) {
    switch(direction){
      case UP:
        {
          int Ymax = polygon->listOfVertices->y;
          point* vertice = polygon->listOfVertices;
          while(vertice != NULL) {
            if(vertice->y > Ymax) Ymax = vertice->y;
            vertice = vertice->next;
          }
          vertice = polygon->listOfVertices;
          while(vertice != NULL) {
            int distance = Ymax - vertice->y;
            mirror(vertice->x, vertice->y, axis, distance);
            vertice = vertice->next;
          }

          break;
        }
      case DOWN:
        {
          int Ymin = polygon->listOfVertices->y;
          point* vertice = polygon->listOfVertices;
          while(vertice != NULL) {
            if(vertice->y < Ymin) Ymin = vertice->y;
            vertice = vertice->next;
          }
          vertice = polygon->listOfVertices;
          while(vertice != NULL) {
            int distance = Ymin - vertice->y;
            mirror(vertice->x, vertice->y, axis, distance);
            vertice = vertice->next;
          }

          break;
        }
      case LEFT:
        {
          int Xmin = polygon->listOfVertices->x;
          point* vertice = polygon->listOfVertices;
          while(vertice != NULL) {
            if(vertice->x < Xmin) Xmin = vertice->x;
            vertice = vertice->next;
          }
          vertice = polygon->listOfVertices;
          while(vertice != NULL) {
            int distance = Xmin - vertice->x;
            mirror(vertice->x, vertice->y, axis, distance);
            vertice = vertice->next;
          }

          break;
        }
      case RIGHT:
        {
          int Xmax = polygon->listOfVertices->x;
          point* vertice = polygon->listOfVertices;
          while(vertice != NULL) {
            if(vertice->x > Xmax) Xmax = vertice->x;
            vertice = vertice->next;
          }
          vertice = polygon->listOfVertices;
          while(vertice != NULL) {
            int distance = Xmax - vertice->x;
            mirror(vertice->x, vertice->y, axis, distance);
            vertice = vertice->next;
          }

          break;
        }
      default:
        break;
    }

    point* pnt = polygon->listOfVertices;
    point* aux = pnt;
    while(pnt->next != NULL){
      start = bresenhamAlgorithm(pnt->x, pnt->y, pnt->next->x, pnt->next->y);
      drawPoints(start);
      if(pnt->next->next == NULL) aux = pnt->next;
      pnt = pnt->next;
    }

    start = bresenhamAlgorithm(polygon->listOfVertices->x, polygon->listOfVertices->y, aux->x, aux->y);
    drawPoints(start);

    polygon = polygon->next;
  }

}

line* mirrorLine(line* lineToMirror, int axis, int direction){
  switch(direction){
      case UP:
        if(lineToMirror->v1->y > lineToMirror->v2->y){
          int YDistance = lineToMirror->v1->y - lineToMirror->v2->y; 
          mirror(lineToMirror->v2->x, lineToMirror->v2->y, axis, YDistance);
        } else {
          int YDistance = lineToMirror->v2->y - lineToMirror->v1->y;
          mirror(lineToMirror->v1->x, lineToMirror->v1->y, axis, YDistance);
        }
        break;
      case DOWN:
        if(lineToMirror->v1->y < lineToMirror->v2->y){
          int YDistance = lineToMirror->v1->y - lineToMirror->v2->y;
          mirror(lineToMirror->v2->x, lineToMirror->v2->y, axis, YDistance);
        } else {
          int YDistance = lineToMirror->v2->y - lineToMirror->v1->y;
          mirror(lineToMirror->v1->x, lineToMirror->v1->y, axis, YDistance);
        }
        break;
      case LEFT:
        if(lineToMirror->v1->x < lineToMirror->v2->x){
          int XDistance = lineToMirror->v1->x - lineToMirror->v2->x;
          mirror(lineToMirror->v2->x, lineToMirror->v2->y, axis, XDistance);
        } else {
          int XDistance = lineToMirror->v2->x - lineToMirror->v1->x;
          mirror(lineToMirror->v1->x, lineToMirror->v1->y, axis, XDistance);
        }
        break;
      case RIGHT:
        if(lineToMirror->v1->x > lineToMirror->v2->x){
          int XDistance = lineToMirror->v1->x - lineToMirror->v2->x;
          mirror(lineToMirror->v2->x, lineToMirror->v2->y, axis, XDistance);
        } else {
          int XDistance = lineToMirror->v2->x - lineToMirror->v1->x;
          mirror(lineToMirror->v1->x, lineToMirror->v1->y, axis, XDistance);
        }
        break;
      default:
        break;
    }

    return lineToMirror;
}

line* createLine(point* v1, point* v2){
  line* newLine = new line;
  newLine->v1 = v1;
  newLine->v2 = v2;
  newLine->next = NULL;
  newLine->centroidCoordinateX = (v1->x + v2->x) / 2;
  newLine->centroidCoordinateY = (v1->y + v2->y) / 2;

  return newLine;
}

// Nao consegui terminar a tempo
void applyShearing(int quantity){
  line* line = firstLine;
  while(line != NULL){
    printf("v1: (%d %d) v2: (%d %d)\n", line->v1->x, line->v1->y, line->v2->x, line->v2->y);
    shear(line->v1->x, line->v1->y, quantity);
    shear(line->v2->x, line->v2->y, quantity);
    printf("v1: (%d %d) v2: (%d %d)\n", line->v1->x, line->v1->y, line->v2->x, line->v2->y);

    start = bresenhamAlgorithm(line->v1->x, line->v1->y, line->v2->x, line->v2->y);
    drawPoints(start);

    line = line->next;
  }
}

void keyMapMessage() {
  printf("Keymap:\n");
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

point* createPoint(double x, double y, int centroidCoordinateX, int centroidCoordinateY) {
  point* pnt = new point;
  pnt->x = x;
  pnt->y = y;
  pnt->next = NULL;
  pnt->drawn = false;
  pnt->isVertice = true;
  pnt->centroidCoordinateX = centroidCoordinateX;
  pnt->centroidCoordinateY = centroidCoordinateY;

  return pnt;
}

void floodFill(int x, int y, float* oldColor, float* newColor){
  float color[3];
  glReadPixels(x, y, 1, 1, GL_RGB, GL_FLOAT, color);
  drawPoints(start);
  if(color[0] != newColor[0] || color[1] != newColor[1] || color[2] != newColor[2]){
    glColor3f(newColor[0], newColor[1], newColor[2]);
    glBegin(GL_POINTS);
      glVertex2i(x, y);
    glEnd();
    start = pushPoint(x, y, false, false);
    glutSwapBuffers();
    floodFill(x+1, y, oldColor, newColor);
    floodFill(x-1, y, oldColor, newColor);
    floodFill(x, y+1, oldColor, newColor);
    floodFill(x, y-1, oldColor, newColor);
  } 
  drawPoints(start);
}