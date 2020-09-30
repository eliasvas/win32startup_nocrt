#version 330 core

out vec4 FragColor;

flat in uint tex_unit;

in vec2 f_tex_coords;
in float f_opacity;
in float f_iTime;
in vec2 f_bl;
in vec2 f_dim;

uniform sampler2D slots[32];
uniform vec2 tex_sizes[32];

void main(){
	vec2 bottom_left = f_bl;//vec2(1.0/6.0,0.0);
	vec2 tex_length = f_dim;//vec2 (1.0/6.0,1.0); 
	
	
	FragColor = texture(slots[tex_unit], vec2(bottom_left.x + tex_length.x*(f_tex_coords.x),bottom_left.y + f_tex_coords.y * tex_length.y));
	
	if (FragColor.a < 0.1)
	{
		discard;
	}else
	{
		FragColor.a = min(f_opacity, FragColor.a);
	}

}  