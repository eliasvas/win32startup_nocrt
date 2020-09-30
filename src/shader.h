#ifndef SHADER_H
#define SHADER_H
#include <windows.h>
#include <gl/gl.h>
#include "ext/glext.h"
#include "ext/wglext.h"

#include "platform.h"
#include "tools.h"
#include <stdlib.h>
#include <stdio.h>

//char infoLog[512];
typedef struct Shader
{
    GLuint ID;   
    const char* vertex_str;
    const char* fragment_str;
}Shader;

static void use_shader(Shader* shader)
{
    glUseProgram(shader->ID);
}

static GLuint load_shader_from_strings (const char * vertex_str, const char * fragment_str)
{ 
    GLuint ID;
    // 2. compile shaders
    unsigned int vertex, fragment;
    int success;
       
    // vertex Shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertex_str, NULL);
    glCompileShader(vertex);
    // print compile errors if any
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        return 356;
    };
      
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragment_str, NULL);
    glCompileShader(fragment);
    // print compile errors if any
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        return 666;
    };
      
    
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    // print linking errors if any
    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        return 666;
    }
      
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return ID;
}
static void shader_load (Shader* s, const char * vertex_path, const char * fragment_path)
{
    s->vertex_str = vertex_path;
    s->fragment_str = fragment_path;
    const char* vs = read_whole_file(vertex_path);
    const char* fs = read_whole_file(fragment_path); 
    s->ID = load_shader_from_strings(vs,fs);

}

static void reload_shader_from_files( GLuint* program, const char* vertex_shader_filename, const char* fragment_shader_filename ) {
  Shader new_shader;
  shader_load(&new_shader,vertex_shader_filename, fragment_shader_filename );
  if ( new_shader.ID ) {
    glDeleteProgram( *program );
    *program = new_shader.ID;
  }
}
static void 
setBool(Shader* shader, const char* name, int value)
{         
    glUniform1i(glGetUniformLocation(shader->ID, name ), (int)value); 
}
static void 
setInt(Shader* shader, const char* name, i32 value)
{ 
    glUniform1i(glGetUniformLocation(shader->ID, name ), value); 
}
static void 
setFloat(Shader * shader, const char* name, f32 value)
{ 
    glUniform1f(glGetUniformLocation(shader->ID, name ), value); 
} 
static void
setVec3(Shader * shader, const char* name, vec3 value)
{ 
    glUniform3f(glGetUniformLocation(shader->ID, name ), value.x,value.y,value.z); 
} 
static void 
setMat4fv(Shader * shader, const char* name, f32* value)
{ 
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, name ),1,GL_FALSE, value);  //NOTE(ilias): check
}
#endif
