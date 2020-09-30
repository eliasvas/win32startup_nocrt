#version 330 core
layout(location = 0) in vec2 position;
layout(location = 1) in vec2 tex_coords;
layout(location = 2) in vec2 wps;
layout(location = 3) in vec2 scale;
layout(location = 4) in uint unit;
layout(location = 5) in float opacity;
layout(location = 6) in vec2 bl;
layout(location = 7) in vec2 dim;
layout(location = 8) in uint flip;

out float f_opacity;
out vec2 f_tex_coords;
out float f_iTime;
out vec2 f_bl;
out vec2 f_dim;
flat out uint tex_unit;
uniform float iTime;

uniform mat4 projection_matrix;

void main()
{
	vec2 real_position = position * scale;
	gl_Position = vec4(real_position.x + wps.x, real_position.y + wps.y,-1.0, 1.0);
	gl_Position = projection_matrix * gl_Position;
	tex_unit = unit;
	f_opacity = opacity;
	f_iTime = iTime;
	f_bl = bl;
	f_dim = dim;
	
	//very slow please change
	vec2 real_tex_coords = tex_coords;
	if (flip)
	{
		if (real_tex_coords.x  < 0.01f)
		{
			real_tex_coords.x = 1.0;
		}else
		{
			real_tex_coords.x = 0.0;
		}
	}
	f_tex_coords = real_tex_coords;
}