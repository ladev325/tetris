#include "opengl.h"
#include <iostream>

using namespace std;
using namespace glm;
unsigned int shaderProgram;

int screen_size[2] = { 0, 0 };
struct Color {
 unsigned char r, g, b, a;
 float fr, fg, fb, fa;
 void translateToFloat() {
  fr = (float)r / 255;
  fg = (float)g / 255;
  fb = (float)b / 255;
  fa = (float)a / 255;
 }
}; Color current_color;

mat4 projection = ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
 glViewport(0, 0, width, height);
 glfwGetFramebufferSize(window, &screen_size[0], &screen_size[1]);
 float aspect = (float)width / (float)height;
 projection = ortho(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
}

//-------------------------INIT OPENGL-------------------------

bool initOpenGL(int width, int height, int MSAAx) {
 if (!glfwInit()) {
  cerr << "Error initializing GLFW!" << endl;
  return -1;
 }

 screen_size[0] = width;
 screen_size[1] = height;
 if (MSAAx) glfwWindowHint(GLFW_SAMPLES, MSAAx);

 glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
 glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
 glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

 GLFWwindow* window = glfwCreateWindow(width, height, "Just window", NULL, NULL);
 if (window == NULL) {
  cerr << "Error creating GLFW window!" << endl;
  glfwTerminate();
  return -1;
 }

 glfwMakeContextCurrent(window);

 if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
  cerr << "Error initializing GLAD!" << endl;
  return -1;
 }

 framebuffer_size_callback(window, width, height);
 glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

 const char* vertexShaderSource = R"(
  #version 330 core
  layout(location = 0) in vec3 aPos;
  uniform mat4 MVP; // rotation matrix
  void main() {
   gl_Position = MVP * vec4(aPos, 1.0);
  }
 )";

 const char* fragmentShaderSource = R"(
  #version 330 core
  out vec4 FragColor;
  uniform vec4 color;
  void main() {
   FragColor = color;
  }
 )";

 unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
 glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
 glCompileShader(vertexShader);
 int success;
 char infoLog[512];
 glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
 if (!success) {
  glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
  cerr << "Error compiling vertex shader!\n" << infoLog << endl;
  return -1;
 }

 unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
 glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
 glCompileShader(fragmentShader);
 glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
 if (!success) {
  glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
  cerr << "Error compiling fragment shader!\n" << infoLog << endl;
  return -1;
 }

 shaderProgram = glCreateProgram();
 glAttachShader(shaderProgram, vertexShader);
 glAttachShader(shaderProgram, fragmentShader);
 glLinkProgram(shaderProgram);
 glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
 if (!success) {
  glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
  cerr << "Error linking shaders\n" << infoLog << endl;
  return -1;
 }

 glDeleteShader(vertexShader);
 glDeleteShader(fragmentShader);
 glEnable(GL_BLEND);
 glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
 return 0;
}

//-------------------------GET SCREEN SIZE-------------------------

int getScreenSize(bool index) {
 return screen_size[index];
}

//-------------------------ROTATE OBJECT-------------------------

void rotateObject(float rotation, float x, float y, float z) {
 mat4 model = rotate(mat4(1.0f), radians(rotation), vec3(x, y, z));
 mat4 MVP = projection * mat4(1.0f) * model;
 int mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
 glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, value_ptr(MVP));
}

//-------------------------SET COLOR-------------------------

void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
 Color color = { r, g, b, a };
 current_color = color;
 current_color.translateToFloat();
}

//-------------------------SET ALPHA-------------------------

void setAlpha(unsigned char a) {
 current_color.a = a;
 current_color.translateToFloat();
}

//-------------------------DRAW RECTANGLE-------------------------

void drawRectangle(int x, int y, int width, int height) {
 float ratio = (float)screen_size[0] / (float)screen_size[1];
 float left = (2.0f * (float)x / screen_size[0] - 1.0f) * ratio;
 float right = (2.0f * (float)(x + width) / screen_size[0] - 1.0f) * ratio;
 float bottom = 1.0f - 2.0f * (float)(y + height) / screen_size[1];
 float top = 1.0f - 2.0f * (float)y / screen_size[1];

 float vertices[] = {
  left, bottom, 0.0f,
  right, bottom, 0.0f,
  right, top, 0.0f,
  left, top, 0.0f
 };

 unsigned int indices[] = {
  0, 1, 2,
  2, 3, 0
 };

 unsigned int VAO, VBO, EBO;
 glGenVertexArrays(1, &VAO);
 glGenBuffers(1, &VBO);
 glGenBuffers(1, &EBO);
 glBindVertexArray(VAO);

 glBindBuffer(GL_ARRAY_BUFFER, VBO);
 glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
 glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

 glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
 glEnableVertexAttribArray(0);
 glBindBuffer(GL_ARRAY_BUFFER, 0);
 glBindVertexArray(0);

 glUseProgram(shaderProgram);
 int colorLocation = glGetUniformLocation(shaderProgram, "color");
 glUniform4f(colorLocation, current_color.fr, current_color.fg, current_color.fb, current_color.fa);

 glBindVertexArray(VAO);
 glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
 glDeleteVertexArrays(1, &VAO);
 glDeleteBuffers(1, &VBO);
 glDeleteBuffers(1, &EBO);
}

//-------------------------DRAW TRIANGLE-------------------------

void drawTriangle(int ax, int ay, int bx, int by, int cx, int cy) {
 float ratio = (float)screen_size[0] / (float)screen_size[1];
 float vertices[] = {
  (2.0f * (float)ax / screen_size[0] - 1.0f) * ratio, 1.0f - 2.0f * (float)ay / screen_size[1], 0.0f,
  (2.0f * (float)bx / screen_size[0] - 1.0f) * ratio, 1.0f - 2.0f * (float)by / screen_size[1], 0.0f,
  (2.0f * (float)cx / screen_size[0] - 1.0f) * ratio, 1.0f - 2.0f * (float)cy / screen_size[1], 0.0f
 };

 unsigned int indices[] = {
  0, 1, 2
 };

 unsigned int VAO, VBO, EBO;
 glGenVertexArrays(1, &VAO);
 glGenBuffers(1, &VBO);
 glGenBuffers(1, &EBO);
 glBindVertexArray(VAO);

 glBindBuffer(GL_ARRAY_BUFFER, VBO);
 glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
 glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
 glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

 glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
 glEnableVertexAttribArray(0);
 glBindBuffer(GL_ARRAY_BUFFER, 0);
 glBindVertexArray(0);

 glUseProgram(shaderProgram);
 int colorLocation = glGetUniformLocation(shaderProgram, "color");
 glUniform4f(colorLocation, current_color.fr, current_color.fg, current_color.fb, current_color.fa);

 glBindVertexArray(VAO);
 glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
 glDeleteVertexArrays(1, &VAO);
 glDeleteBuffers(1, &VBO);
 glDeleteBuffers(1, &EBO);
}

//-------------------------DRAW FRAME-------------------------

void drawFrame(int x, int y, int width, int height, int thickness) {
 drawRectangle(x, y, thickness, height);
 drawRectangle(x + width - thickness, y, thickness, height);
 drawRectangle(x + thickness, y, width - thickness * 2, thickness);
 drawRectangle(x + thickness, y + height - thickness, width - thickness * 2, thickness);
}
