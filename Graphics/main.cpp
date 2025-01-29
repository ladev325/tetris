#include "opengl.h"
#include "tetris_drawer.h"
#include "Account.h"
#include "donut.h"
#include <iostream>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <limits>
using namespace std;

int speed = 0;
int cols = 0;
int rows = 0;
int score = 0;
int erased = 0;
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

bool in_account = false;
Account* acc_array = nullptr;
Account curr_account;
int num_accounts = 0;
FILE* file;
int this_id = -1;


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
   erased++;
   cout << "Score: " << score << endl;
   curr_account.erased = erased;
   curr_account.score = score;

   fopen_s(&file, "data.bin", "wb");
   if (file != nullptr) {
    int new_num = num_accounts + 1;
    fwrite(&new_num, sizeof(int), 1, file);

    for (int i = -1; i < num_accounts; i++) {
     Account this_acc;
     if (i == -1) this_acc = curr_account;
     else this_acc = acc_array[i];

     int name_len = strlen(this_acc.name);
     fwrite(&name_len, sizeof(int), 1, file);
     fwrite(this_acc.name, sizeof(char), name_len, file);

     int pass_len = strlen(this_acc.password);
     fwrite(&pass_len, sizeof(int), 1, file);
     fwrite(this_acc.password, sizeof(char), pass_len, file);

     fwrite(&this_acc.score, sizeof(int), 1, file);
     fwrite(&this_acc.erased, sizeof(int), 1, file);
    }

    fclose(file);
   }

   else cout << "Some errors occured durinc saving!";

  }
}

void tetrisCheckOver() {
 for(int x = 0; x < cols; x ++)
  if(tetrisGetPixel(x, 0) > 0) tetris_over = true;
}


int main() {
  using namespace chrono;
  srand(time(0));

  fopen_s(&file, "data.bin", "rb");
  if(file != nullptr) {
   fread(&num_accounts, sizeof(int), 1, file);
   acc_array = new Account[num_accounts];

   for(int i = 0; i < num_accounts; i ++) {
    int name_len = 0;
    fread(&name_len, sizeof(int), 1, file);
    fread(acc_array[i].name, sizeof(char), name_len, file);
    int pass_len = 0;
    fread(&pass_len, sizeof(int), 1, file);
    fread(acc_array[i].password, sizeof(char), pass_len, file);
    fread(&acc_array[i].score, sizeof(int), 1, file);
    fread(&acc_array[i].erased, sizeof(int), 1, file);
   }
   fclose(file);

   for(int i = 0; i < num_accounts; i ++)
       cout << num_accounts << " " << acc_array[i].name << " " << acc_array[i].password << " " << acc_array[i].score << " " << acc_array[i].erased << endl;
  }

  int input = 0;
  while(input != 1 && input != 2) {
   if (!in_account) {
      cout << "<-----------Welcome------------>" << endl;
      cout << "| 1 - " << "sign in" << "                  |" << endl;
      cout << "| 2 - " << "sign up" << "                  |" << endl;
      cout << "<------------------------------>" << endl;
   }
    cin >> input;
    if (cin.fail()) {   
     cin.clear();
     cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    if (input != 1 && input != 2) cout << "Wrong number! Try again:" << endl;
  }

  if(input == 2) {
   bool success = false;
   cout << "Enter account name (unic ID, max 20 symbols):" << endl;
   cin.ignore();
   while (!success) {
    cin.getline(curr_account.name, 20);

    if (cin.fail()) {
     cin.clear();
     cin.ignore(numeric_limits<streamsize>::max(), '\n');
     cout << "Too long name or something wrong with symbols! Try again:" << endl;
    }

    else {
     success = true;
     for(int i = 0; i < num_accounts; i ++) {
      if (strcmp(curr_account.name, acc_array[i].name) == 0) {
        success = false;
        cout << "We already have such an account! Try another name: " << endl;
      }
     }
    }

   }

   success = false;
   cout << "Enter account password (max 30 symbols):" << endl;
   while (!success) {
    cin.getline(curr_account.password, 30);

    if (cin.fail()) {
     cin.clear();
     cin.ignore(numeric_limits<streamsize>::max(), '\n');
     cout << "Too long pass or something wrong with symbols! Try again:" << endl;
    }
    else success = true;
   }

   success = false;
   cout << "Repeat your password:" << endl;
   while (!success) {
    Account buf;
    cin.clear();
    cin.getline(buf.password, 30);

    if (cin.fail()) {
     cin.clear();
     cin.ignore(numeric_limits<streamsize>::max(), '\n');
     cout << "The password doesn`t match! Try again:" << endl;
    }

    else if (strcmp(buf.password, curr_account.password) != 0) {
     cin.clear();
     cout << "The password doesn`t match! Try again:" << endl;
    }
    
    else success = true;
   }

   fopen_s(&file, "data.bin", "wb");
   if (file != nullptr) {
    int new_num = num_accounts + 1;
    fwrite(&new_num, sizeof(int), 1, file);

    for (int i = -1; i < num_accounts; i++) {
     Account this_acc;
     if (i == -1) this_acc = curr_account;
     else this_acc = acc_array[i];

     int name_len = strlen(this_acc.name);
     fwrite(&name_len, sizeof(int), 1, file);
     fwrite(this_acc.name, sizeof(char), name_len, file);

     int pass_len = strlen(this_acc.password);
     fwrite(&pass_len, sizeof(int), 1, file);
     fwrite(this_acc.password, sizeof(char), pass_len, file);

     fwrite(&this_acc.score, sizeof(int), 1, file);
     fwrite(&this_acc.erased, sizeof(int), 1, file);
    }

    fclose(file);
    cout << "Your account has been successfully created!" << endl;
   }

   else cout << "Some errors occured durinc account creation!";
  }

  if (input == 1) {
   bool success = false;
   this_id = -1;
   cin.ignore();
   cout << "Enter username:" << endl;
   while (!success) {
    cin.clear();
    cin.getline(curr_account.name, 20);

    if (cin.fail()) {
     cin.clear();
     cin.ignore(numeric_limits<streamsize>::max(), '\n');
     cout << "We don`t have such a username! Try again:" << endl;
    }
    else {
     for(int i = 0; i < num_accounts; i ++) {
      if (strcmp(curr_account.name, acc_array[i].name) == 0) {
        cin.clear();
        success = true;
        this_id = i;
      }
     }
     if (!success) {
       cout << "We don`t have such a username! Try again:" << endl;
     }
    }
   }

   success = false;
   int num_try = 0;
   cout << "Enter password:" << endl;
   while (!success) {
    cin.clear();
    cin.getline(curr_account.password, 30);

    if (cin.fail()) {
     cin.clear();
     cin.ignore(numeric_limits<streamsize>::max(), '\n');
     cout << "Wrong password! Try again:" << endl;
     num_try ++;
    }
    else {
     if (strcmp(curr_account.password, acc_array[this_id].password) == 0) {
      cin.clear();
      success = true;
     }
     else {
      cout << "Wrong password! Try again:" << endl;
      num_try++;
     }
    }

    if (num_try > 10) {
     donut();
    }
   }
  }

  score = acc_array[this_id].score;
   erased = acc_array[this_id].erased;

 cout << "Logged in as: " << curr_account.name << endl;
 cout << "Score: " << score << endl;
 cout << "Erased: " << erased << endl;
 input = 0;
  while(input != 1 && input != 2) {
   if (!in_account) {
      cout << "<-----------Welcome------------>" << endl;
      cout << "| 1 - " << "play" << "                  |" << endl;
      cout << "<------------------------------>" << endl;
   }
    cin >> input;
    if (cin.fail()) {
     cin.clear();
     cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    if (input != 1 && input != 2) cout << "Wrong number! Try again:" << endl;
  }


if(input == 1) {
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
 }
 return 0;
}