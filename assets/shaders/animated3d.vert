#version 330 core

const int MAX_JOINTS = 6; //max joints in skeleton
const int MAX_BONES = 3; //max bones for each vertex

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex_coords;
layout(location = 3) in ivec3 joint_ids;
layout(location = 4) in vec3 weights;

out vec2 f_tex_coords;
out vec3 f_normal;

uniform mat4 joint_transforms[MAX_JOINTS];
uniform mat4 view_matrix;
uniform mat4 projection_matrix;
//maybe add a model

void main(){
	vec4 total_local_pos = vec4(0.0);
	vec4 total_normal = vec4(0.0);
	
	for (int i = 0; i < MAX_BONES; ++i)
	{
		mat4 joint_transform = joint_transforms[joint_ids[i]];
		
		vec4 base_pos = joint_transform * vec4(position, 1.0);
		total_local_pos += base_pos * weights[i];

		vec4 world_normal = joint_transform * vec4(normal,1.0);
		total_normal += world_normal * weights[i];
	}
	//these two lines are just for viewing the model without animation
	total_local_pos = vec4(position, 1.0);
	total_normal = vec4(normal,1.0);
	gl_Position = projection_matrix * view_matrix * total_local_pos;
	f_normal = total_normal.xyz;
	f_tex_coords = tex_coords;
}

