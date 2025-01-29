#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

bool initOpenGL(int width, int height, int MSAAx = 8);
int getScreenSize(bool index);
void rotateObject(float rotation, float x, float y, float z);
void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a = 255);
void setAlpha(unsigned char a);
void drawRectangle(int x, int y, int width, int height);
void drawTriangle(int ax, int ay, int bx, int by, int cx, int cy);
void drawFrame(int x, int y, int width, int height, int thickness = 1);