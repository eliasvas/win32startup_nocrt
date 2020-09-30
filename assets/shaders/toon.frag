#version 330 core
struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
struct Light {

    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
out vec4 FragColor;
  
in vec3 f_normal;
in vec3 w_frag_pos;

uniform vec3 view_pos; //camera position
uniform Material m;
uniform Light light;

vec3 rim_color = vec3(1.0,1.0,1.0);

void main()
{
	vec3 ambient = light.ambient * m.ambient;
	
	vec3 norm = normalize(f_normal);
	vec3 light_dir = normalize(light.position - w_frag_pos);
	
	float diff = max(dot(norm,light_dir),0.0);
	vec3 diffuse =light.diffuse * diff * m.diffuse;
	float intensity;
	if (diff > 0.85)
		intensity = 1.0;
	else if(diff > 0.30)
		intensity = 0.7;
	else 
		intensity = 0.2;
	diffuse = diffuse * intensity;
	
	vec3 view_dir = normalize(view_pos - w_frag_pos);
	vec3 reflect_dir = reflect(-light_dir, norm);
	float spec = pow(max(dot(view_dir, reflect_dir), 0.0), m.shininess);
	if (spec > 0.9)
		spec = 1.0 * spec * 2;
	else
		spec = 0.0;
	spec*= intensity;
	
	vec3 specular = light.specular * spec * m.specular;
	
	
	
	//float rim = 1.0 - dot(view_dir, norm);
	//if (rim < 0.99)rim = 0.0;
	//vec3 rim_vec3 = rim * rim_color;
	
    FragColor = vec4((diffuse + ambient + specular), 1.0);
} 

