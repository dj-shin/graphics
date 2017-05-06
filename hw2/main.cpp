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

int main( void )
{
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
  window = glfwCreateWindow( 1024, 768, "Graphics hw2", NULL, NULL);
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

  // Dark blue background
  glClearColor(0.2f, 0.2f, 0.2f, 0.0f);

  // Enable depth test
  glEnable(GL_DEPTH_TEST);
  // Accept fragment if it closer to the camera than the former one
  glDepthFunc(GL_LESS);

  // Cull triangles which normal is not towards the camera
  glEnable(GL_CULL_FACE);

  GLuint VertexArrayID;
  glGenVertexArrays(1, &VertexArrayID);
  glBindVertexArray(VertexArrayID);

  // Create and compile our GLSL program from the shaders
  GLuint programID = LoadShaders( "/home/lastone817/graphics/hw2/shader/Basic.vert", "/home/lastone817/graphics/hw2/shader/LightShading.frag" );

  // Get a handle for our "MVP" uniform
  GLuint MatrixID = glGetUniformLocation(programID, "MVP");
  GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
  GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

  // Read our .obj file
  std::vector<glm::vec3> body_vertices;
  std::vector<glm::vec3> body_normals;
  loadOBJ("./model/body.obj", body_vertices, body_normals);

  std::vector<glm::vec3> rwing1_vertices;
  std::vector<glm::vec3> rwing1_normals;
  loadOBJ("./model/rwing1.obj", rwing1_vertices, rwing1_normals);

  std::vector<glm::vec3> rwing2_vertices;
  std::vector<glm::vec3> rwing2_normals;
  loadOBJ("./model/rwing2.obj", rwing2_vertices, rwing2_normals);

  std::vector<glm::vec3> rwing3_vertices;
  std::vector<glm::vec3> rwing3_normals;
  loadOBJ("./model/rwing3.obj", rwing3_vertices, rwing3_normals);

  std::vector<glm::vec3> lwing1_vertices;
  std::vector<glm::vec3> lwing1_normals;
  loadOBJ("./model/lwing1.obj", lwing1_vertices, lwing1_normals);

  std::vector<glm::vec3> lwing2_vertices;
  std::vector<glm::vec3> lwing2_normals;
  loadOBJ("./model/lwing2.obj", lwing2_vertices, lwing2_normals);

  std::vector<glm::vec3> lwing3_vertices;
  std::vector<glm::vec3> lwing3_normals;
  loadOBJ("./model/lwing3.obj", lwing3_vertices, lwing3_normals);

  // Load it into a VBO

  GLuint body_vertexbuffer;
  glGenBuffers(1, &body_vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, body_vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, body_vertices.size() * sizeof(glm::vec3), &body_vertices[0], GL_STATIC_DRAW);

  GLuint body_normalbuffer;
  glGenBuffers(1, &body_normalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, body_normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, body_normals.size() * sizeof(glm::vec3), &body_normals[0], GL_STATIC_DRAW);

  GLuint rwing1_vertexbuffer;
  glGenBuffers(1, &rwing1_vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, rwing1_vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, rwing1_vertices.size() * sizeof(glm::vec3), &rwing1_vertices[0], GL_STATIC_DRAW);

  GLuint rwing1_normalbuffer;
  glGenBuffers(1, &rwing1_normalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, rwing1_normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, rwing1_normals.size() * sizeof(glm::vec3), &rwing1_normals[0], GL_STATIC_DRAW);

  GLuint rwing2_vertexbuffer;
  glGenBuffers(1, &rwing2_vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, rwing2_vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, rwing2_vertices.size() * sizeof(glm::vec3), &rwing2_vertices[0], GL_STATIC_DRAW);

  GLuint rwing2_normalbuffer;
  glGenBuffers(1, &rwing2_normalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, rwing2_normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, rwing2_normals.size() * sizeof(glm::vec3), &rwing2_normals[0], GL_STATIC_DRAW);

  GLuint rwing3_vertexbuffer;
  glGenBuffers(1, &rwing3_vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, rwing3_vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, rwing3_vertices.size() * sizeof(glm::vec3), &rwing3_vertices[0], GL_STATIC_DRAW);

  GLuint rwing3_normalbuffer;
  glGenBuffers(1, &rwing3_normalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, rwing3_normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, rwing3_normals.size() * sizeof(glm::vec3), &rwing3_normals[0], GL_STATIC_DRAW);

  GLuint lwing1_vertexbuffer;
  glGenBuffers(1, &lwing1_vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, lwing1_vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, lwing1_vertices.size() * sizeof(glm::vec3), &lwing1_vertices[0], GL_STATIC_DRAW);

  GLuint lwing1_normalbuffer;
  glGenBuffers(1, &lwing1_normalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, lwing1_normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, lwing1_normals.size() * sizeof(glm::vec3), &lwing1_normals[0], GL_STATIC_DRAW);

  GLuint lwing2_vertexbuffer;
  glGenBuffers(1, &lwing2_vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, lwing2_vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, lwing2_vertices.size() * sizeof(glm::vec3), &lwing2_vertices[0], GL_STATIC_DRAW);

  GLuint lwing2_normalbuffer;
  glGenBuffers(1, &lwing2_normalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, lwing2_normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, lwing2_normals.size() * sizeof(glm::vec3), &lwing2_normals[0], GL_STATIC_DRAW);

  GLuint lwing3_vertexbuffer;
  glGenBuffers(1, &lwing3_vertexbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, lwing3_vertexbuffer);
  glBufferData(GL_ARRAY_BUFFER, lwing3_vertices.size() * sizeof(glm::vec3), &lwing3_vertices[0], GL_STATIC_DRAW);

  GLuint lwing3_normalbuffer;
  glGenBuffers(1, &lwing3_normalbuffer);
  glBindBuffer(GL_ARRAY_BUFFER, lwing3_normalbuffer);
  glBufferData(GL_ARRAY_BUFFER, lwing3_normals.size() * sizeof(glm::vec3), &lwing3_normals[0], GL_STATIC_DRAW);

  // Get a handle for our "LightPosition" uniform
  glUseProgram(programID);
  GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

  double initTime = glfwGetTime();
  do{
    float elapsedTime = float(glfwGetTime() - initTime);
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use our shader
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

    glm::vec3 lightPos = glm::vec3(6,8,0);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);


    // Body
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, body_vertexbuffer);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, body_normalbuffer);
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    glDrawArrays(GL_TRIANGLES, 0, body_vertices.size() );

    // Right wing 1
    glm::mat4 RWing1ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.13f, 0.84f, 0.46f))
                                  * glm::rotate(glm::mat4(1.0), 0.3f * sin(5.0f * elapsedTime), glm::vec3(0,0,1))
                                  * glm::translate(glm::mat4(1.0), glm::vec3(-0.13f, -0.84f, -0.46f));
    glm::mat4 RWing1MVP = ProjectionMatrix * ViewMatrix * RWing1ModelMatrix;
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &RWing1MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &RWing1ModelMatrix[0][0]);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, rwing1_vertexbuffer);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, rwing1_normalbuffer);
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    glDrawArrays(GL_TRIANGLES, 0, rwing1_vertices.size() );

    // Right wing 2
    glm::mat4 RWing2ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.97f))
                                  * glm::rotate(glm::mat4(1.0), -0.3f * sin(5.0f * elapsedTime), glm::vec3(0,1,0))
                                  * glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, -0.97f))
                                  * RWing1ModelMatrix;
    glm::mat4 RWing2MVP = ProjectionMatrix * ViewMatrix * RWing2ModelMatrix;
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &RWing2MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &RWing2ModelMatrix[0][0]);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, rwing2_vertexbuffer);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, rwing2_normalbuffer);
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    glDrawArrays(GL_TRIANGLES, 0, rwing2_vertices.size() );

    glm::mat4 RWing3ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(-0.18f, -0.0f, 1.79f))
                                  * glm::rotate(glm::mat4(1.0), 0.2f * sin(5.0f * elapsedTime), glm::vec3(0,1,0))
                                  * glm::translate(glm::mat4(1.0), glm::vec3(0.18f, 0.0f, -1.79f))
                                  * RWing2ModelMatrix;
    glm::mat4 RWing3MVP = ProjectionMatrix * ViewMatrix * RWing3ModelMatrix;
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &RWing3MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &RWing3ModelMatrix[0][0]);

    // Right wing 3
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, rwing3_vertexbuffer);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, rwing3_normalbuffer);
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    glDrawArrays(GL_TRIANGLES, 0, rwing3_vertices.size() );

    glm::mat4 LWing1ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.13f, 0.84f, -0.46f))
                                  * glm::rotate(glm::mat4(1.0), 0.3f * sin(5.0f * elapsedTime), glm::vec3(0,0,1))
                                  * glm::translate(glm::mat4(1.0), glm::vec3(-0.13f, -0.84f, 0.46f));
    glm::mat4 LWing1MVP = ProjectionMatrix * ViewMatrix * LWing1ModelMatrix;
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &LWing1MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &LWing1ModelMatrix[0][0]);
    // Left wing 1
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, lwing1_vertexbuffer);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, lwing1_normalbuffer);
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    glDrawArrays(GL_TRIANGLES, 0, lwing1_vertices.size() );

    glm::mat4 LWing2ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, -0.97f))
                                  * glm::rotate(glm::mat4(1.0), 0.2f * sin(5.0f * elapsedTime), glm::vec3(0,1,0))
                                  * glm::translate(glm::mat4(1.0), glm::vec3(0.0f, 0.0f, 0.97f))
                                  * LWing1ModelMatrix;
    glm::mat4 LWing2MVP = ProjectionMatrix * ViewMatrix * LWing2ModelMatrix;
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &LWing2MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &LWing2ModelMatrix[0][0]);
    // Left wing 2
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, lwing2_vertexbuffer);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, lwing2_normalbuffer);
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    glDrawArrays(GL_TRIANGLES, 0, lwing2_vertices.size() );

    glm::mat4 LWing3ModelMatrix = glm::translate(glm::mat4(1.0), glm::vec3(-0.18f, -0.0f, -1.79f))
                                  * glm::rotate(glm::mat4(1.0), -0.3f * sin(5.0f * elapsedTime), glm::vec3(0,1,0))
                                  * glm::translate(glm::mat4(1.0), glm::vec3(0.18f, 0.0f, 1.79f))
                                  * LWing2ModelMatrix;
    glm::mat4 LWing3MVP = ProjectionMatrix * ViewMatrix * LWing3ModelMatrix;
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &LWing3MVP[0][0]);
    glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &LWing3ModelMatrix[0][0]);
    // Left wing 3
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, lwing3_vertexbuffer);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, lwing3_normalbuffer);
    glVertexAttribPointer( 1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    glDrawArrays(GL_TRIANGLES, 0, lwing3_vertices.size() );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);

    // Swap buffers
    glfwSwapBuffers(window);
    glfwPollEvents();

  } // Check if the ESC key was pressed or the window was closed
  while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0 );

  // Cleanup VBO and shader
  glDeleteBuffers(1, &body_vertexbuffer);
  glDeleteBuffers(1, &body_normalbuffer);
  glDeleteProgram(programID);
  glDeleteVertexArrays(1, &VertexArrayID);

  // Close OpenGL window and terminate GLFW
  glfwTerminate();

  return 0;
}

