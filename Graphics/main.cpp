#include "opengl.h"
#include "tetris_drawer.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <cstdlib>
using namespace std;

int speed = 0;
int cols = 0;
int rows = 0;
int score = 0;
int figure_x;
int figure_y;
int tetris_figure;
int tetris_rotation;
int tetris_color;
bool figure_generated = false;
bool tetris_over = false;
bool was_key = false;
bool new_figure = false;
bool use_effects = false;

void tetrisDrawFigure(int x, int y, int figure, int rotation, int color) {
 tetrisDrawPixel(x, y, color);
 for(int i = 0; i < 3; i ++)
  tetrisDrawPixel(x + tetris_figures[figure][rotation][i][0], y + tetris_figures[figure][rotation][i][1], color);
}

void tetrisNewFigure() {
 figure_generated = true;
 figure_x = rand() % (cols - 2 * FIGURES_SPAWN_SPACE) + FIGURES_SPAWN_SPACE;
 figure_y = -2;
 tetris_figure = rand() % 7;
 tetris_rotation = rand() % 4;
 tetris_color = rand() % TETRIS_COLORS;
 new_figure = true;
}

bool tetrisWrongPos(int x, int y, int figure, int rotation) {
 bool wrong = false;
 if(tetrisGetPixel(x, y) > TETRIS_COLORS || y > rows - 1 || x > cols - 1 || x < 0) wrong = true;
 for(int i = 0; i < 3; i ++) {
  int this_x = x + tetris_figures[figure][rotation][i][0];
  int this_y = y + tetris_figures[figure][rotation][i][1];
  if(tetrisGetPixel(this_x, this_y) > TETRIS_COLORS || this_y > rows - 1 ||
     this_x > cols - 1 || this_x < 0) wrong = true;
 }
  return wrong;
}

void tetrisCheckFill() {
 int fills_amount = 0;
 for(int y = 0; y < rows; y ++) {
  bool fill = true;
  for(int x = 0; x < cols; x ++) 
   if(!tetrisGetPixel(x, y)) fill = false;
  if(fill) {
   fills_amount ++;
   for(int x = 0; x < cols; x ++) tetrisDrawPixel(x, y, 0);
   for(int fy = y; fy > -1; fy --) {
    for(int fx = 0; fx < cols; fx ++) {
     if(fy > 0) tetrisDrawPixel(fx, fy, tetrisGetPixel(fx, fy - 1));
     else tetrisDrawPixel(fx, fy, 0);
    }
   }
  }
 }

 if(fills_amount > 0) {
   score += score_divider[fills_amount - 1];
   cout << "Score: " << score << endl;
  }
}

void tetrisCheckOver() {
 for(int x = 0; x < cols; x ++)
  if(tetrisGetPixel(x, 0) > 0) tetris_over = true;
}


int main() {
 using namespace chrono;
 srand(time(0));
 while (1) {
  tetrisNewFigure();
  tetris_over = false;
  for (int x = 0; x < cols; x ++) {
   for (int y = 0; y < rows; y ++) {
    tetrisDrawPixel(x, y, 0);
   }
  }

  cout << "Enter grid columns (10-20): ";
  cin >> cols;
  if (cols < 10 || cols > 20) {
   cout << "Wrong number!" << endl;
   continue;
  }
  cout << "Enter grid rows (10-20): ";
  cin >> rows;
  if (rows < 10 || rows > 20) {
   cout << "Wrong number!" << endl;
   continue;
  }
  cout << "Enter game speed (1-10): ";
  cin >> speed;
  if (speed < 1 || speed > 10) {
   cout << "Wrong number!" << endl;
   continue;
  }

  bool use_glowing = false;
  cout << "Use glowing? (1 - yes, 0 - no): ";
  cin >> use_glowing;
  if (use_glowing != 1 && use_glowing != 0) {
   cout << "Wrong number!" << endl;
   continue;
  }
 
  cout << "Use effects (rotation)? (1 - yes, 0 - no): ";
  cin >> use_effects;
  if (use_effects != 1 && use_effects != 0) {
   cout << "Wrong number!" << endl;
   continue;
  }

  initOpenGL(800, 600, 8);
  GLFWwindow* window = glfwGetCurrentContext();
  steady_clock::time_point loop_tmr = steady_clock::now();
  steady_clock::time_point key_tmr = steady_clock::now();
  tetrisSetGrid(cols, rows, use_glowing);

  while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
      steady_clock::time_point current_time = steady_clock::now();
      milliseconds elapsed_looptime = duration_cast<milliseconds>(current_time - loop_tmr);
      glfwPollEvents();
      glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);

      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_RELEASE && 
          glfwGetKey(window, GLFW_KEY_D) == GLFW_RELEASE && 
          glfwGetKey(window, GLFW_KEY_W) == GLFW_RELEASE &&
          glfwGetKey(window, GLFW_KEY_S) == GLFW_RELEASE &&
          glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
          was_key = false;
      }

      int key = 0;
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {if (!was_key) key = 1;}
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {if (!was_key) key = 2;}
      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {if (!was_key) key = 3;}
      if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {if (!was_key) key = 5;}
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {if (!new_figure) key = 4;}
      else new_figure = false;
   
   if (key > 0) {
    was_key = true;
    tetrisDrawFigure(figure_x, figure_y, tetris_figure, tetris_rotation, 0);
    int this_rot = 0;
    switch (key) {
     case 1: if (!tetrisWrongPos(figure_x - 1, figure_y, tetris_figure, tetris_rotation))
             figure_x --;
     break;
     case 2: if (!tetrisWrongPos(figure_x + 1, figure_y, tetris_figure, tetris_rotation))
             figure_x ++;
     break;
     case 3: if (tetris_rotation + 1 <= 3) this_rot = tetris_rotation + 1;
             else this_rot = 0;
             if (!tetrisWrongPos(figure_x, figure_y, tetris_figure, this_rot))
              tetris_rotation = this_rot;
     break;
     case 4: if (!tetrisWrongPos(figure_x, figure_y + 1, tetris_figure, tetris_rotation))
              figure_y ++;
     break;
     case 5: use_glowing = !use_glowing;
             use_effects = use_glowing;
             tetrisSetGrid(0, 0, use_glowing);
     break;
    }
     tetrisDrawFigure(figure_x, figure_y, tetris_figure, tetris_rotation, tetris_color + 1);
   }

   if (elapsed_looptime.count() > (11 - speed) * 100) {
    if (tetris_over) break;

    if(tetrisWrongPos(figure_x, figure_y + 1, tetris_figure, tetris_rotation)) {
     tetrisDrawFigure(figure_x, figure_y, tetris_figure, tetris_rotation, tetris_color + 9);
     tetrisNewFigure();
     tetrisCheckFill();
     tetrisCheckOver();
    }
    else {
     tetrisDrawFigure(figure_x, figure_y, tetris_figure, tetris_rotation, 0);
     figure_y ++;
     tetrisDrawFigure(figure_x, figure_y, tetris_figure, tetris_rotation, tetris_color + 1);
    }

    loop_tmr = steady_clock::now();  
   }

    if(use_effects) rotateObject(1, 0, 0, 1.0f);
    else rotateObject(0, 0, 0, 1.0f);
    tetrisShow();
    glfwSwapBuffers(window);
  }
   glfwTerminate();
   cin.ignore();
   cin.get();
   system("cls");
 }
 return 0;
}