#ifndef __VKDF_UTIL_H__
#define __VKDF_UTIL_H__

#include "vkdf-deps.hpp"

#define PI ((float) M_PI)
#define DEG_TO_RAD(x) ((float)((x) * PI / 180.0f))
#define RAD_TO_DEG(x) ((float)((x) * 180.0f / PI))

#define RAND_NEG(n) (random() % (2*n+1) - (2*n+1) / 2)
#define RAND(n) (random() % (n+1))

#define MIN2(a, b) (((a) < (b)) ? (a) : (b))
#define MAX2(a, b) (((a) > (b)) ? (a) : (b))

#define ALIGN(value, align) (((value) + (align) - 1) & ~((align) - 1))

#define JOIN(a,b) (a b)

inline float
rand_float(float min, float max)
{
   float range = max - min;
   float r = random() / ((float) RAND_MAX);
   return  r * range + min;
}

inline float
lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

inline glm::vec3
vec3(glm::vec4 v)
{
   return glm::vec3(v.x, v.y, v.z);
}

inline glm::vec4
vec4(glm::vec3 v, float w)
{
   return glm::vec4(v.x, v.y, v.z, w);
}

static inline float
vkdf_vec3_module(glm::vec3 p, int xaxis, int yaxis, int zaxis)
{
   return sqrtf(p.x * p.x * xaxis + p.y * p.y * yaxis + p.z * p.z * zaxis);
}

static inline void
vkdf_vec3_normalize(glm::vec3 *p)
{
   float m = vkdf_vec3_module(*p, 1, 1, 1);
   if (m > 0.0f) {
      p->x /= m;
      p->y /= m;
      p->z /= m;
   }
}

static inline float
vkdf_vec3_dot(glm::vec3 v1, glm::vec3 v2)
{
   return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

static inline glm::vec3
vkdf_vec3_cross(glm::vec3 v1, glm::vec3 v2)
{
   glm::vec3 r;
   r.x = v1.y * v2.z - v1.z * v2.y;
   r.y = v1.z * v2.x - v1.x * v2.z;
   r.z = v1.x * v2.y - v1.y * v2.x;
   return r;
}

glm::vec3
vkdf_compute_view_rotation(glm::vec3 origin, glm::vec3 target);

glm::mat4
vkdf_compute_view_matrix(glm::vec3 origin, glm::vec3 target);

glm::mat4
vkdf_compute_view_matrix_for_rotation(glm::vec3 origin, glm::vec3 rot);

glm::mat4
vkdf_compute_view_matrix_for_direction(glm::vec3 dir);

glm::mat4
vkdf_compute_rotation_matrix(glm::vec3 rot);

glm::vec3
vkdf_compute_viewdir(glm::vec3 rot);

glm::mat4
vkdf_compute_model_matrix(glm::vec3 pos,
                          glm::vec3 rot,
                          glm::vec3 scale,
                          glm::vec3 rot_origin_offset = glm::vec3(0.0f));

inline void
vkdf_color_clear_set(VkClearValue *value, const glm::vec4 &color)
{
   value->color.float32[0] = color.x;
   value->color.float32[1] = color.y;
   value->color.float32[2] = color.z;
   value->color.float32[3] = color.w;
}

inline void
vkdf_depth_stencil_clear_set(VkClearValue *value, float d, float s)
{
   value->depthStencil.depth = d;
   value->depthStencil.stencil = s;
}

inline void
bitfield_set(uint32_t *bitfield, uint32_t bits)
{
   *bitfield |= bits;
}

inline void
bitfield_unset(uint32_t *bitfield, uint32_t bits)
{
   *bitfield &= ~bits;
}

inline uint32_t
bitfield_get(uint32_t bitfield, uint32_t bits)
{
   return bitfield & bits;
}

#endif
