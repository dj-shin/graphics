// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include <common/shader.hpp>
#include <common/controls.hpp>

#include "surface.h"

int init_glfw() {
  // Initialise GLFW
  if( !glfwInit() )
  {
    fprintf( stderr, "Failed to initialize GLFW\n" );
    getchar();
    return -1;
  }

  glfwWindowHint(GLFW_SAMPLES, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Open a window and create its OpenGL context
  window = glfwCreateWindow( 1024, 768, "Graphics hw3", NULL, NULL);
  if( window == NULL ){
    fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible.\n" );
    getchar();
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // Initialize GLEW
  glewExperimental = true; // Needed for core profile
  if (glewInit() != GLEW_OK) {
    fprintf(stderr, "Failed to initialize GLEW\n");
    getchar();
    glfwTerminate();
    return -1;
  }

  // Ensure we can capture the escape key being pressed below
  glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
  glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);

  // Set the mouse at the center of the screen
  glfwPollEvents();
  return 0;
}

void exit_glfw() {
  // Close OpenGL window and terminate GLFW
  glfwTerminate();
}

int main( void )
{
  if (init_glfw()) {
    return -1;
  }
  RawSurface rawSurface = RawSurface::createFromFile("./sample.txt");
  Surface surface = Surface(rawSurface);

  // GLfloat *vertices = (GLfloat*)malloc(sizeof(GLfloat) * surface.dataSize());
  // surface.fillVertices(vertices);

  GLfloat *vertices = (GLfloat*)malloc(sizeof(GLfloat) * surface.meshDataSize());
  surface.fillMeshVertices(vertices);

  GLfloat *normal_vertices = (GLfloat*)malloc(sizeof(GLfloat) * surface.meshDataSize());
  surface.fillNormalVertices(normal_vertices);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  // Cull triangles which normal is not towards the camera
  //glEnable(GL_CULL_FACE);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint programID = LoadShaders( "./shader/Basic.vert", "./shader/LightShading.frag" );

  // Get a handle for our "MVP" uniform
  GLuint MatrixID = glGetUniformLocation(programID, "MVP");
  GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
  GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

  /*
  GLuint* bspline_vbo = (GLuint*)malloc(sizeof(GLuint) * surface.section_count());
  glGenBuffers(surface.section_count(), bspline_vbo);
  for (int i = 0; i < surface.section_count(); ++i) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *(bspline_vbo + i));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 sizeof(GLfloat) * surface.per_section_point_count() * 3,
                 &vertices[surface.per_section_point_count() * 3 * i],
                 GL_STATIC_DRAW);
  }
   */
  GLuint bspline_vbo;
  glGenBuffers(1, &bspline_vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bspline_vbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(GLfloat) * surface.meshDataSize(),
               &vertices[0],
               GL_STATIC_DRAW);

  GLuint bspline_normal;
  glGenBuffers(1, &bspline_normal);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bspline_normal);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(GLfloat) * surface.meshDataSize(),
               &normal_vertices[0],
               GL_STATIC_DRAW);

  // Get a handle for our "LightPosition" uniform
  glUseProgram(programID);
  GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

  double initTime = glfwGetTime();
  do{
    float elapsedTime = float(glfwGetTime() - initTime);
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(programID);

    // Compute the BodyMVP matrix from keyboard and mouse input
    computeMatricesFromInputs();
    glm::mat4 ProjectionMatrix = getProjectionMatrix();
    glm::mat4 ViewMatrix = getViewMatrix();
    glm::mat4 BodyModelMatrix = glm::mat4(1.0);
    glm::mat4 BodyMVP = ProjectionMatrix * ViewMatrix * BodyModelMatrix;

    // Send our transformation to the currently bound shader,
    // in the "BodyMVP" uniform
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &BodyMVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &BodyModelMatrix[0][0]);
    glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

    glm::vec3 lightPos = glm::vec3(-80,0,0);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

    glEnableVertexAttribArray(0);
    // Bspline
    /*
    for (int i = 0; i < surface.section_count(); ++i) {
      glBindBuffer(GL_ARRAY_BUFFER, bspline_vbo[i]);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
      glDrawArrays(GL_LINE_LOOP, 0, surface.per_section_point_count());
    }
    */
    glBindBuffer(GL_ARRAY_BUFFER, bspline_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, bspline_normal);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, surface.meshDataSize() / 3);

    glDisableVertexAttribArray(0);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

    glFlush();

  } // Check if the ESC key was pressed or the window was closed
  while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0 );

  glFinish();
  exit_glfw();
}
