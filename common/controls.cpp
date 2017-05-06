// Include GLFW
#include <glfw3.h>
extern GLFWwindow* window; // The "extern" keyword here is to access the variable "window" declared in tutorialXXX.cpp. This is a hack to keep the tutorials simple. Please avoid this.

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/quaternion.hpp>
using namespace glm;

#include "controls.hpp"

#include <cstdio>

glm::mat4 ViewMatrix;
glm::mat4 ProjectionMatrix;

glm::mat4 initViewMatrix;
glm::mat4 initProjectionMatrix;

glm::vec3 init_top = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 top = init_top;

glm::mat4 getViewMatrix(){
  return ViewMatrix;
}
glm::mat4 getProjectionMatrix(){
  return ProjectionMatrix;
}

void printVec3(const char* name, glm::vec3 v){
  printf("%s : %lf, %lf, %lf\n", name, v.x, v.y, v.z);
}

void printVec4(const char* name, glm::vec4 v){
  printf("%s : %lf, %lf, %lf, %lf\n", name, v.x, v.y, v.z, v.w);
}

// Initial position : on +Z
// glm::vec3 position = glm::vec3( 3.0f, 5.5f, 3.4f );
glm::vec3 position = glm::vec3( 100.0f, 0.0f, 0.0f );
// LookAt target
glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
// Initial Field of View
float FoV = 45.0f;

glm::vec3 init_position;

float speed = 30.0f; // 3 units / second

const int clicked = 0;
const int released = 1;
static int state = released;
double init_xpos, init_ypos;

glm::vec3 v1;

glm::vec3 screen2ball(double xpos, double ypos, int width, int height, glm::vec3 pos)
{
  glm::mat4 P = initProjectionMatrix;
  glm::mat4 V = initViewMatrix;
  float xp = (xpos / (double)width) * 2.0f - 1.0f;
  float yp = 1.0f - (ypos / (double)height) * 2.0f;

  glm::vec4 pivot = P * V * glm::vec4(target, 1.0f);
  pivot /= pivot.w;
  glm::vec4 sp = glm::inverse(V) * glm::inverse(P) * glm::vec4(xp, yp, pivot.z, 1.0f);
  sp /= sp.w;
  glm::vec3 ray = glm::normalize(glm::vec3(sp) - pos);

  glm::vec3 h = pos + glm::dot((target - pos), ray) * ray;
  float ratio = tan(3.141592f / 8.0f) / tan(glm::radians(FoV) / 2.0f) * 10.0f / glm::length(target - pos);
  float radius = 3.0f / ratio;
  float len = radius * radius - glm::length(h - target) * glm::length(h - target);
  if (len > 0.0f) {
    return h - ray * glm::sqrt(len);
  }
  else {
    return target + glm::normalize(h - target) * radius;
  }
}

void pseudo_seek(double xpos, double ypos, int width, int height, glm::vec3 pos) {
  glm::mat4 P = ProjectionMatrix;
  glm::mat4 V = ViewMatrix;
  float xp = (xpos / (double)width) * 2.0f - 1.0f;
  float yp = 1.0f - (ypos / (double)height) * 2.0f;

  glm::vec4 pivot = P * V * glm::vec4(target, 1.0f);
  pivot /= pivot.w;
  glm::vec4 sp = glm::inverse(V) * glm::inverse(P) * glm::vec4(xp, yp, pivot.z, 1.0f);
  sp /= sp.w;
  glm::vec3 ray = glm::normalize(glm::vec3(sp) - pos);

  glm::vec3 h = pos + glm::dot((target - pos), ray) * ray;
  float ratio = tan(3.141592f / 8.0f) / tan(glm::radians(FoV) / 2.0f) * 10.0f / glm::length(target - pos);
  float radius = 3.0f / ratio;
  float len = radius * radius - glm::length(h - target) * glm::length(h - target);
  if (len > 0.0f) {
    target = h;
  }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    if (state != clicked) {
      int width, height;
      glfwGetCursorPos(window, &init_xpos, &init_ypos);
      glfwGetWindowSize(window, &width, &height);
      glm::vec3 direction = glm::normalize(target - position);
      // Right vector
      glm::vec3 right = glm::normalize(glm::cross(direction, top));
      // Up vector
      glm::vec3 up = glm::cross( right, direction );
      ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.01f, 1000.0f);
      // Camera matrix
      ViewMatrix       = glm::lookAt(
          position,           // Camera is here
          target,
          up                  // Head is up (set to 0,-1,0 to look upside-down)
      );
      initProjectionMatrix = ProjectionMatrix;
      initViewMatrix = ViewMatrix;
      init_position = position;
      v1 = screen2ball(init_xpos, init_ypos, width, height, position);
    }
    state = clicked;
  }
  else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    int width, height;
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    glfwGetWindowSize(window, &width, &height);
    pseudo_seek(xpos, ypos, width, height, position);
  }
  if (action == GLFW_RELEASE) {
    state = released;
    init_top = top;
  }
}

void computeMatricesFromInputs(){

  // glfwGetTime is called only once, the first time this function is called
  static double lastTime = glfwGetTime();

  // Compute time difference between current and last frame
  double currentTime = glfwGetTime();
  float deltaTime = float(currentTime - lastTime);

  // Get mouse position
  double xpos, ypos;
  glfwGetCursorPos(window, &xpos, &ypos);

  // Reset mouse position for next frame
  glfwSetMouseButtonCallback(window, mouse_button_callback);

  // Direction : Spherical coordinates to Cartesian coordinates conversion
  glm::vec3 direction = glm::normalize(target - position);

  // Right vector
  glm::vec3 right = glm::normalize(glm::cross(direction, top));

  // Up vector
  glm::vec3 up = glm::cross( right, direction );

  float safety_margin = 0.01f;

  if (glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS){
    position += up * deltaTime * speed;
    target += up * deltaTime * speed;
  }
  if (glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS){
    position -= up * deltaTime * speed;
    target -= up * deltaTime * speed;
  }
  if (glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS){
    position += right * deltaTime * speed;
    target += right * deltaTime * speed;
  }
  if (glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS){
    position -= right * deltaTime * speed;
    target -= right * deltaTime * speed;
  }
  // Dolly in
  if (glfwGetKey( window, GLFW_KEY_PAGE_UP ) == GLFW_PRESS){
    if (glm::length(target - position) > glm::length(direction) * deltaTime * speed + safety_margin)
      position += direction * deltaTime * speed;
  }
  // Dolly out
  if (glfwGetKey( window, GLFW_KEY_PAGE_DOWN ) == GLFW_PRESS){
    position -= direction * deltaTime * speed;
  }
  // Zoom in
  if (glfwGetKey( window, GLFW_KEY_HOME ) == GLFW_PRESS){
    if (FoV > 2.0f) FoV -= 3.0f;
  }
  // Zoom out
  if (glfwGetKey( window, GLFW_KEY_END ) == GLFW_PRESS){
    if (FoV < 170.0f) FoV += 3.0f;
  }

  // Rotate
  if (state == clicked) {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    glm::vec3 v2 = screen2ball(xpos, ypos, width, height, init_position);
    if (glm::length(v1 - v2) > 1.0e-6f) {
      quat q = glm::rotation(glm::normalize(v2), glm::normalize(v1));
      glm::mat4 rotation = toMat4(q);
      position = glm::vec3(rotation * glm::vec4((init_position - target), 0.0f)) + target;
      top = glm::vec3(rotation * glm::vec4(init_top, 0.0f));
    }
  }

  // Projection matrix : 45* Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
  ProjectionMatrix = glm::perspective(glm::radians(FoV), 4.0f / 3.0f, 0.01f, 1000.0f);
  // Camera matrix
  ViewMatrix       = glm::lookAt(
      position,           // Camera is here
      target,
      up                  // Head is up (set to 0,-1,0 to look upside-down)
  );

  // For the next frame, the "last time" will be "now"
  lastTime = currentTime;
}