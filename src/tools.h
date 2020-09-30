#ifndef TOOLS_H
#define TOOLS_H

#include "stdint.h" //for types
#include "assert.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INLINE static inline
#define INTERNAL static

#define kilobytes(val) (val * 1024)
#define megabytes(val) (kilobytes(val) * 1024)
#define gigabytes(val) (megabytes(val) * 1024)

#define PI 3.1415926535897f
//NOTE: this argument controls whether the C
//Runtime Library is included or not.
#if !defined(BUILD_CRTLESS)
#define BUILD_CRTLESS 0
#endif

#if !defined(BUILD_WIN32)
#define BUILD_WIN32 0
#endif



//TODO: add more compile options?
#if BUILD_WIN32 
#include "windows.h"
    #if BUILD_CRTLESS 
        #include "win32_crt_impl.c"
    #endif
#endif


typedef uint8_t   u8;
typedef int8_t    i8;
typedef uint16_t  u16;
typedef int16_t   i16;
typedef uint32_t  u32;
typedef int32_t   i32;
typedef uint64_t  u64;
typedef int64_t   i64;
typedef float     f32;
typedef double    f64;
typedef int32_t   b32;
typedef char      b8;

#define INLINE static inline
#define INTERNAL static

#define kilobytes(val) (val * 1024)
#define megabytes(val) (kilobytes(val) * 1024)
#define gigabytes(val) (megabytes(val) * 1024)

#define PI 3.1415926535897f

#define ALLOC(x) (VirtualAlloc(NULL, x, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE))

//here we implement some crt functionality
#if BUILD_CRTLESS

#define offsetof(type, member) ((unsigned int)((unsigned char*)&((type*)0)->member - (unsigned char*)0)) 

#pragma function(abs)
INLINE i32 abs(i32 x)
{
  return x < 0 ? -x : x;
}
/*
#pragma function(pow)
INLINE f32 pow(double x,double y)
{
    for (i32 i = 0; i < (i32)y; ++i)
    {
        x = x * x;
    }
}
#pragma function(fabs)
INLINE f32 fabs(f32 x)
{
    return x < 0 ? -x : x;
}
*/

INLINE i32 isnan(f32 x) { return x != x; }
INLINE i32 isinf(f32 x) { return !isnan(x) && isnan(x - x); }

//cos_32 computes a cosine to about 3.2 decimal digits of accuracy
//this is a horrible implementation change pls
#endif
INLINE f32 
cos32s(f32 x)
{
    const float c1= 0.99940307;
    const float c2=-0.49558072;
    const float c3= 0.03679168;
    float x2; // The input argument squared
    x2= x * x;
    return (c1 + x2*(c2 + c3 * x2));
}


#pragma function(fmodf)
INLINE f32 fmodf(f32 a, f32 b)
{
    f32 div = a/b;
    i32 mod = (i32)div;
    return (f32)mod;
}

INLINE f32
cos32(f32 x){
    i32 quad; // what quadrant are we in?
    x= fmodf(x,(2.f*PI)); // Get rid of values > 2* pi
    if(x<0)x=-x; // cos(-x) = cos(x)
    quad=(i32)(x/(PI/2.f)); // Get quadrant # (0 to 3) switch (quad){
    switch(quad){
        case 0: return cos32s(x);
        case 1: return -cos32s(PI-x);
        case 2: return -cos32s(x-PI);
        case 3: return cos32s((2.f*PI)-x);
        default: return cos32s(x);
    }
}

INLINE f32 sin32(f32 x)
{
    return cos32(PI/2.f - x);
}
#define sinf(x) sin32(x)
#define cosf(x) cos32(x)


//Math library implementation
typedef union vec2
{
    struct
    {
        f32 x,y;
    };
    struct 
    {
        f32 u,v;
    };
    struct
    {
        f32 r,g;
    };
    f32 elements[2];
#ifdef __cplusplus
    inline f32 &operator[](i32 &index)
    {
        return elements[index];
    }
#endif

}vec2;


typedef union vec3
{
    struct
    {
        f32 x,y,z;
    };
    struct
    {
        f32 r,g,b;
    };
    f32 elements[3];
#ifdef __cplusplus
    inline f32 &operator[](i32 &index)
    {
        return elements[index];
    }
#endif
}vec3;

typedef vec3 color3;
typedef vec3 float3;

typedef union vec4
{
    struct
    {
        f32 x,y,z,w;
    };
    struct
    {
        f32 r,g,b,a;
    };
    f32 elements[4];
#ifdef TOOLS_SSE
    __m128 elements_sse; //because __m128 = 128 = 4 * float = 4*32 = 128 bits
#endif
#ifdef __cplusplus
    inline f32 &operator[](i32 &index)
    {
        return elements[index];
    }
#endif

}vec4;

typedef vec4 color4;
typedef vec4 float4;

typedef union mat4
{
    f32 elements[4][4];//{x.x,x.y,x.z,0,y.x,y.y,y.z,0,z.x,z.y,z.z,0,p.x,p.y,p.z,1} 

    f32 raw[16]; //{x.x,x.y,x.z,0,y.x,y.y,y.z,0,z.x,z.y,z.z,0,p.x,p.y,p.z,1} 

#ifdef TOOLS_SSE
    __m128 cols[4]; //same as elements (our matrices are column major)
#endif
#ifdef __cplusplus
    inline vec4 operator[](i32 &Index)
    {
        f32* col = elements[Index];

        vec4 res;
        res.elements[0] = col[0];
        res.elements[1] = col[1];
        res.elements[2] = col[2];
        res.elements[3] = col[3];

        return res;
    }
#endif
}mat4;


typedef union ivec3
{
    struct
    {
        i32 x,y,z;
    };
    struct
    {
        i32 r,g,b;
    };
    i32 elements[3];
}ivec3;

INLINE b32 equals_ivec3(ivec3 l, ivec3 r)
{
    i32 res = ((l.x == r.x) && (l.y == r.y) && (l.z == r.z));
    return res;
}

INLINE f32 to_radians(float degrees)
{
    f32 res = degrees * (PI / 180.0f);
    return(res);
}

INLINE f32 lerp(f32 A, f32 B, f32 t)
{
    f32 res = (1.0f - t)*A + t*B;
    return res;
}

INLINE vec2 v2(f32 x, f32 y)
{
    vec2 res;
    res.x = x;
    res.y = y;
    return res;
}

INLINE vec3 v3(f32 x, f32 y, f32 z)
{
    vec3 res;
    res.x = x;
    res.y = y;
    res.z = z;
    return res;
}

INLINE vec4 v4(f32 x, f32 y, f32 z, f32 w)
{
    vec4 res;
    res.x = x;
    res.y = y;
    res.z = z;
    res.w = w;
    return res;
}

INLINE vec2 add_vec2(vec2 l, vec2 r)
{
    vec2 res;
    res.x = l.x + r.x;
    res.y = l.y + r.y;
    return res;
}

INLINE vec2 add_vec2f(vec2 v,f32 val)
{
    vec2 res;
    res.x = v.x + val;
    res.y = v.y + val;
    return res;
}


INLINE vec2 sub_vec2(vec2 l, vec2 r)
{
    vec2 res;
    res.x = l.x - r.x;
    res.y = l.y - r.y;
    return res;
}

INLINE vec2 sub_vec2f(vec2 v,f32 val)
{
    vec2 res;
    res.x = v.x - val;
    res.y = v.y - val;
    return res;
}

INLINE vec2 mul_vec2(vec2 l, vec2 r)
{
    vec2 res;
    res.x = l.x * r.x;
    res.y = l.y * r.y;
    return res;
}

INLINE vec2 mul_vec2f(vec2 l, f32 r)
{
    vec2 res;
    res.x = l.x * r;
    res.y = l.y * r;
    return res;
}

INLINE vec2 div_vec2(vec2 l, vec2 r)
{
    vec2 res;
    res.x = l.x / r.x;
    res.y = l.y / r.y;
    return res;
}

INLINE vec2 div_vec2f(vec2 l, f32 r)
{
    vec2 res;
    res.x = l.x / r;
    res.y = l.y / r;
    return res;
}

INLINE f32 dot_vec2(vec2 l, vec2 r)
{
    f32 res = (l.x + r.x)+(l.y + r.y); // Σ(Ai*Bi)
    return res;
}

INLINE vec2 sqrt_vec2(vec2 v)
{
    vec2 res;
    res.x = sqrt(v.x);
    res.y = sqrt(v.y);
    return res;
}

INLINE vec2 rotate_vec2(vec2 v, f32 a) {
	vec2 res;
    f32 sn = sin(a);
	f32 cs = cos(a);
    res.x = v.x * cs - v.y * sn;
    res.y = v.x * sn + v.y * cs;
    return res;
} 

INLINE f32 length_vec2(vec2 v)
{
    f32 res = sqrt(dot_vec2(v,v)); // (x^2 + y^2)^(1/2)
    return res;
}

INLINE vec2 abs_vec2(vec2 v)
{
    vec2 res = v2(abs(v.x), abs(v.y));
    return res;
}
   
INLINE vec2 normalize_vec2(vec2 v)
{
    vec2 res = {0}; //in case length is zero we return zero vector
    f32 vec_length = length_vec2(v);
    if (vec_length > 0.1)
    {
        res.x = v.x * (1.0f/vec_length);
        res.y = v.y * (1.0f/vec_length);
    }
    return res;
}

INLINE vec3 add_vec3(vec3 l, vec3 r)
{
    vec3 res;
    res.x = l.x + r.x;
    res.y = l.y + r.y;
    res.z = l.z + r.z;
    return res;
}

INLINE vec3 sub_vec3(vec3 l, vec3 r)
{
    vec3 res;
    res.x = l.x - r.x;
    res.y = l.y - r.y;
    res.z = l.z - r.z;
    return res;
}

INLINE vec3 mul_vec3(vec3 l, vec3 r)
{
    vec3 res;
    res.x = l.x * r.x;
    res.y = l.y * r.y;
    res.z = l.z * r.z;
    return res;
}

INLINE vec3 mul_vec3f(vec3 l, f32 r)
{
    vec3 res;
    res.x = l.x * r;
    res.y = l.y * r;
    res.z = l.z * r;
    return res;
}

INLINE vec3 div_vec3(vec3 l, vec3 r)
{
    vec3 res;
    res.x = l.x / r.x;
    res.y = l.y / r.y;
    res.z = l.z / r.z;
    return res;
}

INLINE vec3 div_vec3f(vec3 l, f32 r)
{
    vec3 res;
    res.x = l.x / r;
    res.y = l.y / r;
    res.z = l.z / r;
    return res;
}

INLINE f32 dot_vec3(vec3 l, vec3 r)
{
    f32 res = (l.x * r.x)+(l.y * r.y)+(l.z * r.z); // Σ(Ai*Bi)
    return res;
}

INLINE f32 length_vec3(vec3 v)
{
    f32 res = sqrt(dot_vec3(v,v)); // (x^2 + y^2)^(1/2)
    return res;
}

INLINE vec3 rotate_vec3(vec3 v, f32 a)
{
    vec3 res;
    //TBA
    return res;
}


INLINE vec3 normalize_vec3(vec3 v)
{
    vec3 res = {0}; //in case length is zero we return zero vector
    f32 vec_length = length_vec3(v);
    if (vec_length != 0)
    {
        res.x = v.x * (1.0f/vec_length);
        res.y = v.y * (1.0f/vec_length);
        res.z = v.z * (1.0f/vec_length);
    }
    return res;
}

INLINE vec3 lerp_vec3(vec3 l, vec3 r, f32 time)
{
    vec3 res;

    f32 x = lerp(l.x, r.x, time);
    f32 y = lerp(l.y, r.y, time);
    f32 z = lerp(l.z, r.z, time);
    res = v3(x,y,z); 
    
    return res;
}

INLINE vec3 cross_vec3(vec3 l,vec3 r)
{
    vec3 res;
    res.x = (l.y*r.z) - (l.z*r.y);
    res.y = (l.z * r.x) - (l.x*r.z);
    res.z = (l.x * r.y) - (l.y * r.x);
    return (res);
}

INLINE vec3 blender_to_opengl_vec3(vec3 v)
{
    return v3(v.x, -v.z, v.y);
}

INLINE vec4 add_vec4(vec4 l, vec4 r)
{
    vec4 res;
#ifdef TOOLS_SSE
    res.elements_sse = _mm_add_ps(l.elements_sse, r.elements_sse);
#else
    res.x = l.x + r.x;
    res.y = l.y + r.y;
    res.z = l.z + r.z;
    res.w = l.w + r.w;
#endif
    return res;
}

INLINE vec4 sub_vec4(vec4 l, vec4 r)
{
    vec4 res;
#ifdef TOOLS_SSE
    res.elements_sse = _mm_sub_ps(l.elements_sse, r.elements_sse);
#else
    res.x = l.x - r.x;
    res.y = l.y - r.y;
    res.z = l.z - r.z;
    res.w = l.w - r.w;
#endif
    return res;
}

INLINE vec4 mul_vec4(vec4 l, vec4 r)
{
    vec4 res;
#ifdef TOOLS_SSE
    res.selements_sse = _mm_mul_ps(l.elements_sse, r.elements_sse);
#else
    res.x = l.x * r.x;
    res.y = l.y * r.y;
    res.z = l.z * r.z;
    res.w = l.w * r.w;
#endif
    return res;
}

INLINE vec4 mul_vec4f(vec4 l, f32 r)
{
    vec4 res;
#ifdef TOOLS_SSE
    __m128 scalar = _mm_set1_ps(r); // [r r r r]
    res.elements_sse = _mm_mul_ps(l.elements_sse, scalar);
#else
    res.x = l.x * r;
    res.y = l.y * r;
    res.z = l.z * r;
    res.w = l.w * r;
#endif
    return res;
}

INLINE vec4 div_vec4(vec4 l, vec4 r)
{
    vec4 res;
#ifdef TOOLS_SSE
    res.elements_sse = _mm_div_ps(l.elements_sse, r.elements_sse);
#else
    res.x = l.x / r.x;
    res.y = l.y / r.y;
    res.z = l.z / r.z;
    res.w = l.w / r.w;
#endif
    return res;
}

INLINE vec4 div_vec4f(vec4 l, f32 r)
{
    vec4 res;
#ifdef TOOLS_SSE
    __m128 scalar = _mm_set1_ps(r);
    res.elements_sse = _mm_div_ps(l.elements_sse, r.elements_sse);
#else
    res.x = l.x / r;
    res.y = l.y / r;
    res.z = l.z / r;
    res.w = l.w / r;
#endif
    return res;
}

INLINE f32 dot_vec4(vec4 l, vec4 r)
{
    f32 res = (l.x + r.x)+(l.y + r.y)+(l.z + r.z)+(l.w + r.w); // Σ(Ai*Bi)
    return res;
}

INLINE f32 length_vec4(vec4 v)
{
    f32 res = sqrt(dot_vec4(v,v)); // (x^2 + y^2)^(1/2)
    return res;
}
   
INLINE vec4 normalize_vec4(vec4 v)
{
    vec4 res = {0}; //in case length is zero we return zero vector
    f32 vec_length = length_vec4(v);
    if (!equalf(vec_length, 0.f, 0.01))
    {
        res.x = v.x * (1.0f/vec_length);
        res.y = v.y * (1.0f/vec_length);
        res.z = v.z * (1.0f/vec_length);
        res.w = v.w * (1.0f/vec_length);
    }
    return res;
}


INLINE mat4 m4(void)
{
    mat4 res = {0};
    return res;
}

INLINE mat4 m4d(f32 d)
{
    mat4 res = m4();
    res.elements[0][0] = d;
    res.elements[1][1] = d;
    res.elements[2][2] = d;
    res.elements[3][3] = d;
    return res;
}

INLINE mat4 transpose_mat4(mat4 m)
{
    mat4 res;
    for (u32 i = 0; i < 4;++i)
    {
        for (u32 j = 0; j< 4;++j)
        {
            res.elements[j][i] = m.elements[i][j];
        }
    }
    return res;
}
INLINE mat4 mul_mat4f(mat4 m, f32 s)
{
    mat4 res;
    for (u32 i = 0; i < 4; ++i)
    {
        for (u32 j = 0; j < 4; ++j)
        {
            res.elements[i][j] = (f32)m.elements[i][j] * s;
        }
    }
    return res;
}


INLINE mat4 div_mat4f(mat4 m, f32 s)
{
    mat4 res = {0};
    
    if (s != 0.0f)
    {
        for (u32 i = 0; i < 4; ++i)
        {
            for (u32 j = 0; j < 4; ++j)
            {
                res.elements[i][j] = m.elements[i][j] / s;
            }
        }
    }
    return res;
}

INLINE mat4 add_mat4(mat4 l, mat4 r)
{
    mat4 res;
    for (u32 i = 0; i < 4; ++i)
    {
        for (u32 j = 0; j < 4; ++j)
        {
            res.elements[i][j] = (f32)l.elements[i][j] + (f32)r.elements[i][j];
        }
    }
    return res;
}


INLINE mat4 sub_mat4(mat4 l, mat4 r)
{
    mat4 res;
    for (u32 i = 0; i < 4; ++i)
    {
        for (u32 j = 0; j < 4; ++j)
        {
            res.elements[i][j] = l.elements[i][j] - r.elements[i][j];
        }
    }
    return res;
}

INLINE mat4 mul_mat4(mat4 l, mat4 r)
{
    mat4 res;
    for (u32 col = 0; col < 4; ++col)
    {
        for (u32 row = 0; row < 4; ++row)
        {
            f32 sum = 0;
            for (u32 current_index = 0; current_index < 4; ++current_index)
            {
                sum += (f32)l.elements[current_index][row] * (f32)r.elements[col][current_index];
            }
            res.elements[col][row] = sum;
        }
    }
    return res;
}


INLINE mat4 translate_mat4(vec3 t) //TODO(ilias): check handedness
{
    mat4 res = m4d(1.0f);
    res.elements[3][0] = t.x;
    res.elements[3][1] = t.y;
    res.elements[3][2] = t.z;
    return res;
}

INLINE mat4 rotate_mat4(float angle, vec3 axis)
{
    mat4 res = m4d(1.0f);

    axis = normalize_vec3(axis);

    float sinA = sin(to_radians(angle));
    float cosA = cos(to_radians(angle));
    float cos_val = 1.0f - cosA;

    res.elements[0][0] = (axis.x * axis.x * cos_val) + cosA;
    res.elements[0][1] = (axis.x * axis.y * cos_val) + (axis.z * sinA);
    res.elements[0][2] = (axis.x * axis.z * cos_val) - (axis.y * sinA);

    res.elements[1][0] = (axis.y * axis.x * cos_val) - (axis.z * sinA);
    res.elements[1][1] = (axis.y * axis.y * cos_val) + cosA;
    res.elements[1][2] = (axis.y * axis.z * cos_val) + (axis.x * sinA);

    res.elements[2][0] = (axis.z * axis.x * cos_val) + (axis.y * sinA);
    res.elements[2][1] = (axis.z * axis.y * cos_val) - (axis.x * sinA);
    res.elements[2][2] = (axis.z * axis.z * cos_val) + cosA;

    return (res);
}

INLINE mat4 scale_mat4(vec3 s)
{
    mat4 res = m4d(1.f);
    res.elements[0][0] *= s.x;
    res.elements[1][1] *= s.y;
    res.elements[2][2] *= s.z;
    return res;
}
//what the fuuuuuuuck is this shit??
INLINE mat4 inv_mat4(mat4 m)
{
    mat4 res;
    f32 det;
    mat4 inv, inv_out;
    i32 i;

    inv.raw[0] = m.raw[5]  * m.raw[10] * m.raw[15] - 
             m.raw[5]  * m.raw[11] * m.raw[14] - 
             m.raw[9]  * m.raw[6]  * m.raw[15] + 
             m.raw[9]  * m.raw[7]  * m.raw[14] +
             m.raw[13] * m.raw[6]  * m.raw[11] - 
             m.raw[13] * m.raw[7]  * m.raw[10];

    inv.raw[4] = -m.raw[4]  * m.raw[10] * m.raw[15] + 
              m.raw[4]  * m.raw[11] * m.raw[14] + 
              m.raw[8]  * m.raw[6]  * m.raw[15] - 
              m.raw[8]  * m.raw[7]  * m.raw[14] - 
              m.raw[12] * m.raw[6]  * m.raw[11] + 
              m.raw[12] * m.raw[7]  * m.raw[10];

    inv.raw[8] = m.raw[4]  * m.raw[9] * m.raw[15] - 
             m.raw[4]  * m.raw[11] * m.raw[13] - 
             m.raw[8]  * m.raw[5] * m.raw[15] + 
             m.raw[8]  * m.raw[7] * m.raw[13] + 
             m.raw[12] * m.raw[5] * m.raw[11] - 
             m.raw[12] * m.raw[7] * m.raw[9];

    inv.raw[12] = -m.raw[4]  * m.raw[9] * m.raw[14] + 
               m.raw[4]  * m.raw[10] * m.raw[13] +
               m.raw[8]  * m.raw[5] * m.raw[14] - 
               m.raw[8]  * m.raw[6] * m.raw[13] - 
               m.raw[12] * m.raw[5] * m.raw[10] + 
               m.raw[12] * m.raw[6] * m.raw[9];

    inv.raw[1] = -m.raw[1]  * m.raw[10] * m.raw[15] + 
              m.raw[1]  * m.raw[11] * m.raw[14] + 
              m.raw[9]  * m.raw[2] * m.raw[15] - 
              m.raw[9]  * m.raw[3] * m.raw[14] - 
              m.raw[13] * m.raw[2] * m.raw[11] + 
              m.raw[13] * m.raw[3] * m.raw[10];

    inv.raw[5] = m.raw[0]  * m.raw[10] * m.raw[15] - 
             m.raw[0]  * m.raw[11] * m.raw[14] - 
             m.raw[8]  * m.raw[2] * m.raw[15] + 
             m.raw[8]  * m.raw[3] * m.raw[14] + 
             m.raw[12] * m.raw[2] * m.raw[11] - 
             m.raw[12] * m.raw[3] * m.raw[10];

    inv.raw[9] = -m.raw[0]  * m.raw[9] * m.raw[15] + 
              m.raw[0]  * m.raw[11] * m.raw[13] + 
              m.raw[8]  * m.raw[1] * m.raw[15] - 
              m.raw[8]  * m.raw[3] * m.raw[13] - 
              m.raw[12] * m.raw[1] * m.raw[11] + 
              m.raw[12] * m.raw[3] * m.raw[9];

    inv.raw[13] = m.raw[0]  * m.raw[9] * m.raw[14] - 
              m.raw[0]  * m.raw[10] * m.raw[13] - 
              m.raw[8]  * m.raw[1] * m.raw[14] + 
              m.raw[8]  * m.raw[2] * m.raw[13] + 
              m.raw[12] * m.raw[1] * m.raw[10] - 
              m.raw[12] * m.raw[2] * m.raw[9];

    inv.raw[2] = m.raw[1]  * m.raw[6] * m.raw[15] - 
             m.raw[1]  * m.raw[7] * m.raw[14] - 
             m.raw[5]  * m.raw[2] * m.raw[15] + 
             m.raw[5]  * m.raw[3] * m.raw[14] + 
             m.raw[13] * m.raw[2] * m.raw[7] - 
             m.raw[13] * m.raw[3] * m.raw[6];

    inv.raw[6] = -m.raw[0]  * m.raw[6] * m.raw[15] + 
              m.raw[0]  * m.raw[7] * m.raw[14] + 
              m.raw[4]  * m.raw[2] * m.raw[15] - 
              m.raw[4]  * m.raw[3] * m.raw[14] - 
              m.raw[12] * m.raw[2] * m.raw[7] + 
              m.raw[12] * m.raw[3] * m.raw[6];

    inv.raw[10] = m.raw[0]  * m.raw[5] * m.raw[15] - 
              m.raw[0]  * m.raw[7] * m.raw[13] - 
              m.raw[4]  * m.raw[1] * m.raw[15] + 
              m.raw[4]  * m.raw[3] * m.raw[13] + 
              m.raw[12] * m.raw[1] * m.raw[7] - 
              m.raw[12] * m.raw[3] * m.raw[5];

    inv.raw[14] = -m.raw[0]  * m.raw[5] * m.raw[14] + 
               m.raw[0]  * m.raw[6] * m.raw[13] + 
               m.raw[4]  * m.raw[1] * m.raw[14] - 
               m.raw[4]  * m.raw[2] * m.raw[13] - 
               m.raw[12] * m.raw[1] * m.raw[6] + 
               m.raw[12] * m.raw[2] * m.raw[5];

    inv.raw[3] = -m.raw[1] * m.raw[6] * m.raw[11] + 
              m.raw[1] * m.raw[7] * m.raw[10] + 
              m.raw[5] * m.raw[2] * m.raw[11] - 
              m.raw[5] * m.raw[3] * m.raw[10] - 
              m.raw[9] * m.raw[2] * m.raw[7] + 
              m.raw[9] * m.raw[3] * m.raw[6];

    inv.raw[7] = m.raw[0] * m.raw[6] * m.raw[11] - 
             m.raw[0] * m.raw[7] * m.raw[10] - 
             m.raw[4] * m.raw[2] * m.raw[11] + 
             m.raw[4] * m.raw[3] * m.raw[10] + 
             m.raw[8] * m.raw[2] * m.raw[7] - 
             m.raw[8] * m.raw[3] * m.raw[6];

    inv.raw[11] = -m.raw[0] * m.raw[5] * m.raw[11] + 
               m.raw[0] * m.raw[7] * m.raw[9] + 
               m.raw[4] * m.raw[1] * m.raw[11] - 
               m.raw[4] * m.raw[3] * m.raw[9] - 
               m.raw[8] * m.raw[1] * m.raw[7] + 
               m.raw[8] * m.raw[3] * m.raw[5];

    inv.raw[15] = m.raw[0] * m.raw[5] * m.raw[10] - 
              m.raw[0] * m.raw[6] * m.raw[9] - 
              m.raw[4] * m.raw[1] * m.raw[10] + 
              m.raw[4] * m.raw[2] * m.raw[9] + 
              m.raw[8] * m.raw[1] * m.raw[6] - 
              m.raw[8] * m.raw[2] * m.raw[5];

    det = m.raw[0] * inv.raw[0] + m.raw[1] * inv.raw[4] + 
        m.raw[2] * inv.raw[8] + m.raw[3] * inv.raw[12];

    if (equalsf(det,0.f)){ //in case the matrix is non-invertible
        return m4d(0.f);
    }

    det = 1.f / det;

    for (i = 0; i < 16; ++i)
        inv_out.raw[i] = inv.raw[i] * det;

    return inv_out;
}

INLINE mat4 orthographic_proj(f32 l, f32 r, f32 b, f32 t, f32 n, f32 f)
{
    mat4 res = m4();

    //the quotents are in reverse because we were supposed to do one more matrix multiplication to negate z..
    //its basically two steps in one..
    res.elements[0][0] = 2.0f / (r - l);
    res.elements[1][1] = 2.0f / (t - b);
    res.elements[2][2] = 2.0f / (n - f);
    res.elements[3][3] = 1.0f;

    res.elements[3][0] = (l + r) / (l - r);
    res.elements[3][1] = (b + t) / (b - t);
    res.elements[3][2] = (f + n) / (n - f);

    return res;
}

INLINE mat4 perspective_proj(f32 fov, f32 aspect, f32 n, f32 f)
{
    mat4 res = m4();

    f32 cot = 1.0f / tanf(fov * (PI / 360.0f));

    res.elements[0][0] = cot / aspect;
    res.elements[1][1] = cot;
    res.elements[2][3] = -1.0f;

    res.elements[2][2] = (n + f)/(n - f);

    res.elements[3][2] = (2.f * n * f) / (n - f);
    res.elements[3][3] = 0.0f;

    return res;
}

INLINE mat4 look_at(vec3 eye, vec3 center, vec3 fake_up)
{
    mat4 res = m4();

    vec3 f = normalize_vec3(sub_vec3(center, eye));
    vec3 r = normalize_vec3(cross_vec3(f, fake_up));
    vec3 up = cross_vec3(r, f);

    res.elements[0][0] = r.x;
    res.elements[0][1] = up.x;
    res.elements[0][2] = -f.x;
    res.elements[0][3] = 0.0f;

    res.elements[1][0] = r.y;
    res.elements[1][1] = up.y;
    res.elements[1][2] = -f.y;
    res.elements[1][3] = 0.0f;

    res.elements[2][0] = r.z;
    res.elements[2][1] = up.z;
    res.elements[2][2] = -f.z;
    res.elements[2][3] = 0.0f;

    res.elements[3][0] = -dot_vec3(r, eye);
    res.elements[3][1] = -dot_vec3(up, eye);
    res.elements[3][2] = dot_vec3(f, eye);
    res.elements[3][3] = 1.0f;

    return res;
}

INLINE mat4 
swap_cols_mat4(mat4 mat, u32 col1, u32 col2)
{
    vec4 second_column = v4(mat.elements[col2][0],mat.elements[col2][1], mat.elements[col2][2], mat.elements[col2][3]);
    for (u32 i = 0; i < 4; ++i)
        mat.elements[col2][i] = mat.elements[col1][i];
    for (u32 i = 0; i < 4; ++i)
        mat.elements[col1][i] = second_column.elements[i];
    return mat;
}

INLINE mat4
swap_rows_mat4(mat4 mat, u32 row1, u32 row2)
{
   vec4 second_row = v4(mat.elements[0][row2], mat.elements[1][row2], mat.elements[2][row2], mat.elements[3][row2]); 
   for (u32 i = 0; i < 4; ++i)
       mat.elements[i][row2] = mat.elements[i][row1];
   for (u32 i = 0; i < 4; ++i)
       mat.elements[i][row1] = second_row.elements[i];
   return mat;
}

INLINE mat4 negate_row_mat4(mat4 mat, u32 row)
{
    for (u32 i = 0; i < 4; ++i)
        mat.elements[i][row] = -1.f * mat.elements[i][row];
    return mat;
}

INLINE mat4 negate_col_mat4(mat4 mat, u32 col)
{
    for (u32 i = 0; i < 4; ++i)
        mat.elements[col][i] = -1.f * mat.elements[col][i];
    return mat;
}

//some operator overloading (NOT NEEDED)
#ifdef __cplusplus
INLINE vec2 operator+(vec2 l, vec2 r)
{
    vec2 res = add_vec2(l, r);

    return res;
}
INLINE vec2 operator-(vec2 l, vec2 r)
{
    vec2 res = sub_vec2(l, r);

    return res;
}
INLINE vec2 operator*(vec2 l, vec2 r)
{
    vec2 res = mul_vec2(l, r);

    return res;
}
INLINE vec2 operator*(vec2 l, f32 x)
{
    vec2 res = mul_vec2f(l, x);

    return res;
}

INLINE vec2 operator/(vec2 l, vec2 r)
{
    vec2 res = div_vec2(l, r);

    return res;
}

INLINE vec2 operator/(vec2 l,f32 r)
{
    vec2 res = div_vec2f(l, r);

    return res;
}

INLINE vec3 operator+(vec3 l, vec3 r)
{
    vec3 res = add_vec3(l, r);

    return res;
}
INLINE vec3 operator-(vec3 l, vec3 r)
{
    vec3 res = sub_vec3(l, r);

    return res;
}
INLINE vec3 operator*(vec3 l, vec3 r)
{
    vec3 res = mul_vec3(l, r);

    return res;
}
INLINE vec3 operator*(vec3 l, f32 r)
{
    vec3 res = mul_vec3f(l, r);

    return res;
}
INLINE vec3 operator/(vec3 l, vec3 r)
{
    vec3 res = div_vec3(l, r);

    return res;
}
INLINE vec3 operator/(vec3 l,f32 r)
{
    vec3 res = div_vec3f(l, r);

    return res;
}

INLINE vec4 operator+(vec4 l, vec4 r)
{
    vec4 res = add_vec4(l, r);

    return res;
}
INLINE vec4 operator-(vec4 l, vec4 r)
{
    vec4 res = sub_vec4(l, r);

    return res;
}
INLINE vec4 operator*(vec4 l, vec4 r)
{
    vec4 res = mul_vec4(l, r);

    return res;
}
INLINE vec4 operator*(vec4 l, f32 r)
{
    vec4 res = mul_vec4f(l, r);

    return res;
}
INLINE vec4 operator/(vec4 l, vec4 r)
{
    vec4 res = div_vec4(l, r);

    return res;
}
INLINE vec4 operator/(vec4 l,f32 r)
{
    vec4 res = div_vec4f(l, r);

    return res;
}


INLINE mat4 operator+(mat4 l, mat4 r)
{
    mat4 res = add_mat4(l,r);

    return res;
}
INLINE mat4 operator-(mat4 l, mat4 r)
{
    mat4 res = sub_mat4(l,r);

    return res;
}

INLINE mat4 operator*(mat4 l, mat4 r)
{
    mat4 res = mul_mat4(l,r);

    return res;
}

INLINE mat4 operator*(mat4 l,f32 r)
{
    mat4 res = mul_mat4f(l,r);

    return res;
}
#endif



//An arena memory allocator
typedef struct Arena
{
    void *memory;
    u32 memory_size;
    u32 current_offset;
} Arena;


static Arena 
arena_init(void* memory, u32 size)
{
    Arena a = {0};
    a.memory = memory;
    a.memory_size = size;
    a.current_offset = 0;

    return a;
}

static void
arena_free(Arena* arena, u32 size)
{
    //do nothing
}

static void * 
arena_alloc(Arena* arena, u32 size)
{
    void* mem = 0;

    if (arena->current_offset + size <= arena->memory_size)
    {
        //position of next available byte
        mem = (void *)((u8*)arena->memory + arena->current_offset); 
        arena->current_offset += size;
    }
    //we return a pointer to size free bytes of memory
    return mem;
}

static void
arena_clear(Arena* arena)
{
    arena->current_offset = 0;
}

static void 
arena_zero(Arena* arena)
{
    memset(arena->memory, 0, arena->memory_size);
}


#ifdef __cplusplus
}
#endif

#endif
