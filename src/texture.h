#ifndef TEXTURE_H
#define TEXTURE_H

#include "tools.h"
//NOTE: fuck this shit
#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_NO_HDR
#define STBI_ASSERT
#define STBI_NO_GIF
#define STBI_NO_LINEAR
#define STBI_NO_SIMD
//TODO: do these alright
#define STBI_MALLOC(x) (VirtualAlloc(NULL, x, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE))
#define STBI_FREE(x) (VirtualFree(x, sizeof(x),MEM_DECOMMIT))
#define STBI_REALLOC(x, new_size) (VirtualAlloc(NULL, x, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE))

#include "ext/stb_image.h"



typedef struct Texture 
{
    GLuint id;
    u32 width;
    u32 height;
}Texture;

static b32 load_texture(Texture* tex,const char *filename)
{
    b32 result = 0;
    glGenTextures(1, &tex->id);
    glBindTexture(GL_TEXTURE_2D, tex->id);
    // set the texture wrapping/filtering options (on the currently bound texture object)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //TODO FIX
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //TODO FIX
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load and generate the texture
    int width = 0, height = 0, nrChannels;
    stbi_set_flip_vertically_on_load(TRUE);
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels,STBI_rgb_alpha);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        result = 1;
    }
    //stbi_image_free(data);
    tex->width = width;
    tex->height = height;
    return result;
}

typedef struct Image 
{
    unsigned char *data;
    i32 width;
    i32 height;
}Image;

static Image 
load_image_bytes(const char *filename)
{
    Image res;

    i32 width = 0, height = 0, nrChannels;
    stbi_set_flip_vertically_on_load(TRUE);
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels,STBI_rgb_alpha);
    res.data = data;
    res.width = width;
    res.height = height;
    return res;
}


typedef struct TextureContainer
{
    Texture t;
    char *filename;
    b32 has_mipmaps;
}TextureContainer;

typedef struct TextureManager
{
    TextureContainer textures[32];
    u32 size;
    u32 max_size;
}TextureManager;

static Texture* 
find_texture(TextureManager* manager, const char* filename)
{
    for (u32 i = 0; i < manager->size; ++i)
    {
        if (strcmp(manager->textures[i].filename, filename) == 0)
            return &manager->textures[i].t;
    }
    return 0; //texture not found
}

static GLuint 
push_texture(TextureManager* manager, const char* filepath)
{
    Texture dummy;
    load_texture(&dummy,filepath);
    char *filename = getFileName(filepath);
    TextureContainer to_insert = {dummy, getFileName(filepath), 1};
    
    //putting TextureContainer in textures array (if it fits)
    if (manager->size >= manager->max_size || dummy.id == 0)
        return 0;
    manager->textures[manager->size++] = to_insert;
    return dummy.id;
}

#endif
