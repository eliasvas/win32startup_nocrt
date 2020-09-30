#version 330 core

struct Material {
    vec3 ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};
struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  


struct PointLight {    
    vec3 position;
    
    float constant;
    float linear;
    float quadratic;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define MAX_POINT_LIGHTS 1
#define MAX_DIR_LIGHTS 10
uniform sampler2D shadowMap;
uniform PointLight point_lights[MAX_POINT_LIGHTS];
uniform DirLight dir_light;
uniform DirLight dir_lights[MAX_DIR_LIGHTS];
uniform int point_light_count;
uniform int dir_light_count;

out vec4 FragColor;
  
in vec3 f_normal;
in vec3 w_frag_pos;
in vec2 f_texcoords;
in vec4 f_frag_pos_ls;



uniform vec3 view_pos; //camera position
uniform Material m;
float bias = 0.005;
vec3 light_position;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	//perspective devide so we go to clip-space [-1,1]
	vec3 projCoords = f_frag_pos_ls.xyz / f_frag_pos_ls.w;
	//we transform to NDC so we go to [0,1]
	projCoords = projCoords * 0.5 + 0.5;
	float closestDepth = texture(shadowMap, projCoords.xy).r;   
	float currentDepth = projCoords.z;  
	
	float shadow = 0.0;
	vec2 texel_size = 1.0 / vec2(1024,720); //instead of vec2(1024,720) must be the size of the tilemap..
	for (int x = -1; x <=1; ++x)
	{
		for (int y = -1; y <=1; ++y)
		{
			float pcf_depth = texture(shadowMap, projCoords.xy + vec2(x, y) * texel_size).r;
			shadow += currentDepth - bias > pcf_depth ? 1.0 : 0.0; //if 0, stay 0, if else add the adjacent shadow tiles
		}
		shadow /= 9.0;
	}
	
	//make the shadow 0 if it is outside the far plane
	if (projCoords.z > 1.0)
		shadow = 0.0;
	
	return shadow;
}

vec3 calculate_directional_lightOLD(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), m.shininess);
    // combine results
    vec3 ambient  = light.ambient * vec3(texture(m.diffuse, f_texcoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(m.diffuse, f_texcoords));
    vec3 specular = light.specular * spec * vec3(texture(m.specular, f_texcoords));
	
	vec3 color = texture(m.diffuse, f_texcoords).rgb;
	float shadow = ShadowCalculation(f_frag_pos_ls);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
	
    return (lighting);
}  
vec3 calculate_directional_light(DirLight light, vec3 normal, vec3 viewDir)
{
	vec3 light_pos = view_pos;
	
    vec3 lightDir = normalize(light_pos - w_frag_pos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), m.shininess);
    // combine results
    vec3 ambient  = light.ambient * vec3(texture(m.diffuse, f_texcoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(m.diffuse, f_texcoords));
    vec3 specular = light.specular * spec * vec3(texture(m.specular, f_texcoords));
	
	vec3 color = texture(m.diffuse, f_texcoords).rgb;
	float shadow = ShadowCalculation(f_frag_pos_ls);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
	
    return (lighting);
}  
vec3 calculate_point_light(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), m.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient  = light.ambient  * vec3(texture(m.diffuse, f_texcoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(m.diffuse, f_texcoords));
    vec3 specular = light.specular * spec * vec3(texture(m.specular, f_texcoords));
    //ambient  *= attenuation;
    //diffuse  *= attenuation;
    //specular *= attenuation;
	
	
	vec3 color = texture(m.diffuse, f_texcoords).rgb;
	float shadow = ShadowCalculation(f_frag_pos_ls);       
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * color;
	
    return (lighting);
} 


void main()
{
	vec3 norm = normalize(f_normal);
	//vec3 light_dir = normalize(light.position - w_frag_pos);
	vec3 view_dir = normalize(view_pos - w_frag_pos);
	
	vec3 output_color = vec3(0);
	//calculate ambient light contribution
	for(int i = 0; i < dir_light_count; ++i)
		output_color += calculate_directional_light(dir_lights[i], norm, view_dir);
	//calculate point light contributions
	for(int i = 0; i < point_light_count; ++i)
		output_color += calculate_point_light(point_lights[i], norm, w_frag_pos, view_dir);
	//calculatte spot light contributions
	//for(int i = 0; i < NR_SPOT_LIGHTS; ++i)
		//output_color += calculate_spot_light(spot_lights[i], normal, w_frag_pos, view_dir);
	
	FragColor = vec4(output_color, 1.0);
}

