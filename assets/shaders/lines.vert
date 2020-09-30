#version 330 core

layout (location = 0) in vec3 aPos;
  
out vec4 vertexColor;

uniform mat4 MVP;
uniform float red;

void main()
{
    gl_Position = vec4(aPos, 1.0); // see how we directly give a vec3 to vec4's constructor
    gl_Position = MVP * gl_Position;
	vertexColor = vec4(1.0,0.0,0.0,1.0); // set the output variable to a dark-red color
}
