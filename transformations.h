#define PI 3.14159265

#define NONE 0
#define RIGHT 1
#define LEFT 2
#define UP 3
#define DOWN 4

#define COUNTERCLOCKWISE 10010 // Sentido anti-horario
#define CLOCKWISE 10011 // Sentido horario

void translate(int &x, int &y, int quantity, int direction);
void rotate(int &x, int &y, float angle, int direction);
void scale(int &x, int &y, int quantity);


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