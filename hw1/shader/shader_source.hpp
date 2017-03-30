#include <string>
using namespace std;

std::string FragmentShaderCode = 
"#version 330 core\n\
\n\
// Interpolated values from the vertex shaders\n\
in vec2 UV;\n\
\n\
// Ouput data\n\
out vec3 color;\n\
\n\
// Values that stay constant for the whole mesh.\n\
uniform sampler2D myTextureSampler;\n\
\n\
void main(){\n\
\n\
	// Output color = color of the texture at the specified UV\n\
	color = texture( myTextureSampler, UV ).rgb;\n\
}";

std::string VertexShaderCode = 
"#version 330 core\n\
\n\
// Input vertex data, different for all executions of this shader.\n\
layout(location = 0) in vec3 vertexPosition_modelspace;\n\
layout(location = 1) in vec2 vertexUV;\n\
\n\
// Output data ; will be interpolated for each fragment.\n\
out vec2 UV;\n\
\n\
// Values that stay constant for the whole mesh.\n\
uniform mat4 MVP;\n\
\n\
void main(){\n\
\n\
	// Output position of the vertex, in clip space : MVP * position\n\
	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);\n\
	\n\
	// UV of the vertex. No special space for this one.\n\
	UV = vertexUV;\n\
}";

