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
#include <common/objloader.hpp>

#include "surface.h"
#include "bsp.h"

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
  RawSurface rawSurface = RawSurface::createFromFile("./knight.txt");
  Surface surface = Surface(rawSurface);

  GLfloat *vertices = (GLfloat*)malloc(sizeof(GLfloat) * surface.meshDataSize());
  surface.fillMeshVertices(vertices);

  GLfloat *normal_vertices = (GLfloat*)malloc(sizeof(GLfloat) * surface.meshDataSize());
  surface.fillNormalVertices(normal_vertices);

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  GLuint programID = LoadShaders( "./shader/Basic.vert", "./shader/LightShading.frag" );

  // Get a handle for our "MVP" uniform
  GLuint MatrixID = glGetUniformLocation(programID, "MVP");
  GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
  GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

  // Opaque object
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

  std::vector<Polygon> cube;

  auto f1 = std::vector<glm::vec3>{
          glm::vec3(1.0f, 1.0f,  1.0f),
          glm::vec3(1.0f, -1.0f,  1.0f),
          glm::vec3(1.0f, -1.0f, -1.0f),
          glm::vec3(1.0f, 1.0f, -1.0f)
      };
  auto f2 = std::vector<glm::vec3>{
          glm::vec3(-1.0f, 1.0f,  1.0f),
          glm::vec3(-1.0f, 1.0f, -1.0f),
          glm::vec3(-1.0f, -1.0f, -1.0f),
          glm::vec3(-1.0f, -1.0f,  1.0f)
      };
  auto f3 = std::vector<glm::vec3>{
          glm::vec3(1.0f, 1.0f,  1.0f),
          glm::vec3(1.0f, 1.0f, -1.0f),
          glm::vec3(-1.0f, 1.0f, -1.0f),
          glm::vec3(-1.0f, 1.0f,  1.0f)
      };
  auto f4 = std::vector<glm::vec3>{
          glm::vec3(1.0f, -1.0f,  1.0f),
          glm::vec3(-1.0f, -1.0f,  1.0f),
          glm::vec3(-1.0f, -1.0f, -1.0f),
          glm::vec3(1.0f, -1.0f, -1.0f)
      };
  auto f5 = std::vector<glm::vec3>{
          glm::vec3(1.0f, 1.0f,  1.0f),
          glm::vec3(-1.0f, 1.0f,  1.0f),
          glm::vec3(-1.0f, -1.0f,  1.0f),
          glm::vec3(1.0f, -1.0f,  1.0f)
      };
  auto f6 = std::vector<glm::vec3>{
          glm::vec3(1.0f, 1.0f, -1.0f),
          glm::vec3(1.0f, -1.0f, -1.0f),
          glm::vec3(-1.0f, -1.0f, -1.0f),
          glm::vec3(-1.0f, 1.0f, -1.0f)
      };
  // Material Coefficients from http://www.it.hiof.no/~borres/j3d/explain/light/p-materials.html
  cube.push_back(Polygon(f1,
                         glm::vec3(0.5f, 0.5f, 0.4f),
                         glm::vec3(0.05f, 0.05f, 0.0f),
                         glm::vec3(0.7f, 0.7f, 0.04f),
                         10.0f, glm::vec4(1,1,1,0.8f))); // Yellow Rubber
  cube.push_back(Polygon(f2,
                         glm::vec3(0.4f),
                         glm::vec3(0.25f),
                         glm::vec3(0.774597f),
                         78.6f, glm::vec4(1,1,1,0.8f))); // Chrome
  cube.push_back(Polygon(f3,
                         glm::vec3(0.7038f, 0.27048f, 0.0828f),
                         glm::vec3(0.19125f, 0.0735f, 0.0225f),
                         glm::vec3(0.256777f, 0.137622f, 0.086014f),
                         12.8f, glm::vec4(1,1,1,0.8f))); // Copper
  cube.push_back(Polygon(f4,
                         glm::vec3(0.07568f, 0.61424f, 0.07568f),
                         glm::vec3(0.0215f, 0.1745f, 0.0215f),
                         glm::vec3(0.633f, 0.727811f, 0.633f),
                         76.8f, glm::vec4(1,1,1,0.8f))); // Emerald
  cube.push_back(Polygon(f5,
                         glm::vec3(0.61424f, 0.04136f, 0.04136f),
                         glm::vec3(0.1745f, 0.01175f, 0.01175f),
                         glm::vec3(0.727811f, 0.626959f, 0.626959f),
                         76.8f, glm::vec4(1,1,1,0.8f))); // Ruby
  cube.push_back(Polygon(f6,
                         glm::vec3(0.0f,0.50980392f,0.50980392f),
                         glm::vec3(0.0f,0.1f,0.06f),
                         glm::vec3(0.50196078f,0.50196078f,0.50196078f),
                         32.0f, glm::vec4(1,1,1,0.8f))); // Cyan Plastic

  BSPTree cube_bsp = BSPTree(cube);
  cube_bsp.apply(glm::translate(glm::vec3(5.0f, 8.0f, 0.0f)) * glm::scale(glm::vec3(8.0f, 8.0f, 8.0f)));

  std::vector<glm::vec3> test_vertices;
  std::vector<glm::vec3> test_normals;
  loadOBJ("./polygon.obj", test_vertices, test_normals);
  BSPTree test_bsp = BSPTree(test_vertices, glm::vec3(0.5f), glm::vec3(0.0f), glm::vec3(0.8f), 5.0f, glm::vec4(1,1,1,0.3f));
  test_bsp.apply(glm::translate(glm::vec3(-3.5f, 1.5f, -2.5f)) * glm::scale(glm::vec3(3.0f, 3.0f, 3.0f)));

  std::vector<Polygon> cube_polygons = cube_bsp.getPolygons();
  std::vector<Polygon> test_polygons = test_bsp.getPolygons();
  std::vector<Polygon> merge_polygons;
  merge_polygons.reserve(cube_polygons.size() + test_polygons.size());
  merge_polygons.insert(merge_polygons.end(), cube_polygons.begin(), cube_polygons.end());
  merge_polygons.insert(merge_polygons.end(), test_polygons.begin(), test_polygons.end());
  BSPTree merge_bsp = BSPTree(merge_polygons);

  // Get a handle for our "LightPosition" uniform
  glUseProgram(programID);
  GLuint LightID1 = glGetUniformLocation(programID, "LightPosition_worldspace1");
  GLuint LightID2 = glGetUniformLocation(programID, "LightPosition_worldspace2");
  GLuint LightID3 = glGetUniformLocation(programID, "LightPosition_worldspace3");
  GLuint LightID4 = glGetUniformLocation(programID, "LightPosition_worldspace4");
  GLuint LightID5 = glGetUniformLocation(programID, "LightPosition_worldspace5");
  GLuint LightID6 = glGetUniformLocation(programID, "LightPosition_worldspace6");
  GLuint ColorID = glGetUniformLocation(programID, "material_color");

  GLuint Kd = glGetUniformLocation(programID, "Kd");
  GLuint Ka = glGetUniformLocation(programID, "Ka");
  GLuint Ks = glGetUniformLocation(programID, "Ks");
  GLuint n = glGetUniformLocation(programID, "n");

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

    glUniform3f(LightID1, 37.0f, 8.0f, 0.0f);
    glUniform3f(LightID2, 5.0f, 40.0f, 0.0f);
    glUniform3f(LightID3, 5.0f, 8.0f, 32.0f);
    glUniform3f(LightID4, -27.0f, 8.0f, 0.0f);
    glUniform3f(LightID5, 5.0f, -24.0f, 0.0f);
    glUniform3f(LightID6, 5.0f, 8.0f, -32.0f);

    glUniform3f(Kd, 1.0f, 1.0f, 1.0f);
    glUniform3f(Ka, 0.2f, 0.2f, 0.2f);
    glUniform3f(Ks, 0.5f, 0.5f, 0.5f);
    glUniform1f(n, 5.0f);

    glm::vec4 knightColor = glm::vec4(1,1,1,1);
    glUniform4f(ColorID, knightColor.x, knightColor.y, knightColor.z, knightColor.w);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, bspline_vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, bspline_normal);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, surface.meshDataSize() / 3);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    glm::vec3 v = getEye();
    merge_bsp.draw(v, programID);

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
