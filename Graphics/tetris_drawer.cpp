#include "opengl.h"
#include "tetris_drawer.h"
#include <vector>
#include <iostream>
using namespace std;

int grid_cols = 1; // x
int grid_rows = 1; // y
vector<int> row_arr(grid_rows, 0);
vector<vector<int>> tetris_grid(grid_cols, row_arr);
bool glowing_on = false;

long map(long x, long in_min, long in_max, long out_min, long out_max) {
 return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void tetrisSetGrid(int cols, int rows, bool glowing) {
 glowing_on = glowing;
 if(cols != 0 && rows != 0) {
  grid_cols = cols;
  grid_rows = rows;

  tetris_grid.resize(cols);
  for (int i = 0; i < cols; ++i) {
   tetris_grid[i].resize(rows, 0);
  }
 }
}

void tetrisShow() {
 int grid_width = getScreenSize(0) - 80;
 int grid_height = getScreenSize(1) - 80;
 int rect_width = grid_width / grid_cols - 1;
 int rect_height = grid_height / grid_rows - 1;

 setColor(0, 255, 255);
 drawFrame(40, 40 , grid_width + 1, grid_height + 1);
 if(glowing_on) {
  for (int i = 1; i < 10; i ++) {
   setAlpha((10 - i) * 8);
   drawFrame(40 - i*2, 40 - i*2, grid_width + 1 + i*4, grid_height + 1 + i*4, 2);
  }
 }

 setColor(0, 255, 255, 140);
 for (int x = 1; x < grid_cols; x ++) {
  int this_x = map(x, 0, grid_cols, 40, grid_width + 40);
  drawRectangle(this_x, 40, 1, grid_height);
 }

 for (int y = 1; y < grid_rows; y ++) {
  int this_y = map(y, 0, grid_rows, 40, grid_height + 40);
  drawRectangle(40, this_y, grid_width, 1);
 }

 for (int x = 0; x < grid_cols; x ++) {
  for (int y = 0; y < grid_rows; y ++) {
   switch(tetris_grid[x][y]) {
    case 1:case 9: setColor(255, 0, 0); break;
    case 2:case 10: setColor(0, 255, 0); break;
    case 3:case 11: setColor(0, 100, 255); break;
    case 4:case 12: setColor(255, 255, 0); break;
    case 5:case 13: setColor(0, 255, 255); break;
    case 6:case 14: setColor(255, 0, 255); break;
    case 7:case 15: setColor(255, 255, 255); break;
    case 8:case 16: setColor(255, 120, 0); break;
    default: setColor(0, 0, 0, 0);
   }
   drawRectangle(map(x, 0, grid_cols, 40, grid_width + 40) + 1, 
	             map(y, 0, grid_rows, 40, grid_height + 40) + 1, 
	             rect_width, rect_height);
   if(glowing_on && tetris_grid[x][y] != 0)
   for (int i = 0; i < 10; i ++) {
    setAlpha((10 - i) * 10);
    drawRectangle(map(x, 0, grid_cols, 40, grid_width + 40) + 1, 
	              map(y, 0, grid_rows, 40, grid_height + 40) + 1 - i*2, 
	              rect_width, 2);
    drawRectangle(map(x, 0, grid_cols, 40, grid_width + 40) + 1, 
	              map(y, 0, grid_rows, 40, grid_height + 40) + 1 + rect_height + i*2, 
	              rect_width, 2);
    drawRectangle(map(x, 0, grid_cols, 40, grid_width + 40) + 1 + rect_width + i*2,
	              map(y, 0, grid_rows, 40, grid_height + 40) + 1, 
	              2, rect_height);
    drawRectangle(map(x, 0, grid_cols, 40, grid_width + 40) + 1 - i*2,
	              map(y, 0, grid_rows, 40, grid_height + 40) + 1, 
	              2, rect_height);
   }
  }
 }
}

void tetrisDrawPixel(int x, int y, int state) {
 if (x >= grid_cols || x < 0 || y >= grid_rows || y < 0)
  return;
 tetris_grid[x][y] = state;
}

int tetrisGetPixel(int x, int y) {
 if (x >= grid_cols || x < 0 || y >= grid_rows || y < 0)
  return 0;
 return tetris_grid[x][y];
}