#ifndef SHADER_HPP
#define SHADER_HPP
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <algorithm>
using namespace std;

#include <cstdlib>
#include <cstring>

#include <GL/glew.h>


GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
GLuint LoadShadersSource(std::string VertexShaderCode, std::string FragmentShaderCode);

#endif
