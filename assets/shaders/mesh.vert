#version 330 core

layout (location = 0) in vec3 vertex_pos; 
layout (location = 1) in vec3 n;
layout (location = 2) in vec2 tex_coord;
  
out vec4 vertexColor;

uniform mat4 MVP;

void main()
{
    gl_Position = vec4(vertex_pos,1.0);
	gl_Position = MVP * gl_Position;

    vertexColor = vec4(normalize(n), 1.0);
	//if (n.x < 0.03, n.y < 0.03, n.z < 0.03)vertexColor = vec4(1.0);
}
