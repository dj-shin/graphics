#ifndef GRAPHICS_SPLINE_H
#define GRAPHICS_SPLINE_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

float bspline(float p0, float p1, float p2, float p3, float t)
{
  float b0 = (1.0f - t) * (1.0f - t) * (1.0f - t) / 6.0f;
  float b1 = (3.0f * t * t * t - 6.0f * t * t + 4.0f) / 6.0f;
  float b2 = (-3.0f * t * t * t + 3.0f * t * t + 3.0f * t + 1.0f) / 6.0f;
  float b3 = t * t * t / 6.0f;

  return p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;
}

glm::vec3 bspline(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
{
  float b0 = (1.0f - t) * (1.0f - t) * (1.0f - t) / 6.0f;
  float b1 = (3.0f * t * t * t - 6.0f * t * t + 4.0f) / 6.0f;
  float b2 = (-3.0f * t * t * t + 3.0f * t * t + 3.0f * t + 1.0f) / 6.0f;
  float b3 = t * t * t / 6.0f;

  return p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;
}

glm::vec3 bspline_tanget(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
{
  float b0 = -3.0f * (1.0f - t) * (1.0f - t) / 6.0f;
  float b1 = (3.0f * t * t - 4.0f * t) / 2.0f;
  float b2 = (-3.0f * t * t + 2.0f * t + 1.0f) / 2.0f;
  float b3 = t * t / 2.0f;

  return p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;
}

glm::vec2 bspline(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float t)
{
  float b0 = (1.0f - t) * (1.0f - t) * (1.0f - t) / 6.0f;
  float b1 = (3.0f * t * t * t - 6.0f * t * t + 4.0f) / 6.0f;
  float b2 = (-3.0f * t * t * t + 3.0f * t * t + 3.0f * t + 1.0f) / 6.0f;
  float b3 = t * t * t / 6.0f;

  return p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;
}

glm::quat bspline(glm::quat p0, glm::quat p1, glm::quat p2, glm::quat p3, float t)
{
  float b0 = (1.0f - t) * (1.0f - t) * (1.0f - t) / 6.0f;
  float b1 = (3.0f * t * t * t - 6.0f * t * t + 4.0f) / 6.0f;
  float b2 = (-3.0f * t * t * t + 3.0f * t * t + 3.0f * t + 1.0f) / 6.0f;
  float b3 = t * t * t / 6.0f;

  return glm::exp(b0 * glm::log(p0)) *
      glm::exp(b1 * glm::log(p1)) *
      glm::exp(b2 * glm::log(p2)) *
      glm::exp(b3 * glm::log(p3));
}

glm::vec3 bezier(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
{
  float b0 = (1.0f - t) * (1.0f - t) * (1.0f - t);
  float b1 = 3.0f * t * (1.0f - t) * (1.0f - t);
  float b2 = 3.0f * t * t * (1.0f - t);
  float b3 = t * t * t;

  return p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;
}

glm::vec3 catmullrom(glm::vec3 p0, glm::vec3 p3, glm::vec3 t0, glm::vec3 t3, float t)
{
  glm::vec3 p1 = p0 + (t0 / 3.0f);
  glm::vec3 p2 = p3 - (t3 / 3.0f);
  return bezier(p0, p1, p2, p3, t);
}

glm::vec2 bezier(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, glm::vec2 p3, float t)
{
  float b0 = (1.0f - t) * (1.0f - t) * (1.0f - t);
  float b1 = 3.0f * t * (1.0f - t) * (1.0f - t);
  float b2 = 3.0f * t * t * (1.0f - t);
  float b3 = t * t * t;

  return p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;
}

glm::vec2 catmullrom(glm::vec2 p0, glm::vec2 p3, glm::vec2 t0, glm::vec2 t3, float t)
{
  glm::vec2 p1 = p0 + (t0 / 3.0f);
  glm::vec2 p2 = p3 - (t3 / 3.0f);
  return bezier(p0, p1, p2, p3, t);
}

float bezier(float p0, float p1, float p2, float p3, float t)
{
  float b0 = (1.0f - t) * (1.0f - t) * (1.0f - t);
  float b1 = 3.0f * t * (1.0f - t) * (1.0f - t);
  float b2 = 3.0f * t * t * (1.0f - t);
  float b3 = t * t * t;

  return p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;
}

float catmullrom(float p0, float p3, float t0, float t3, float t)
{
  float p1 = p0 + (t0 / 3.0f);
  float p2 = p3 - (t3 / 3.0f);
  return bezier(p0, p1, p2, p3, t);
}

glm::quat bezier(glm::quat p0, glm::quat p1, glm::quat p2, glm::quat p3, float t)
{
  float b0 = (1.0f - t) * (1.0f - t) * (1.0f - t);
  float b1 = 3.0f * t * (1.0f - t) * (1.0f - t);
  float b2 = 3.0f * t * t * (1.0f - t);
  float b3 = t * t * t;

  return glm::exp(b0 * glm::log(p0)) *
         glm::exp(b1 * glm::log(p1)) *
         glm::exp(b2 * glm::log(p2)) *
         glm::exp(b3 * glm::log(p3));
}

glm::quat catmullrom(glm::quat p0, glm::quat p3, glm::quat t0, glm::quat t3, float t)
{
  glm::quat p1 = p0 * glm::exp(t0 / 3.0f);
  glm::quat p2 = p3 * glm::inverse(glm::exp(t3 / 3.0f));
  return bezier(p0, p1, p2, p3, t);
}

glm::vec3 bezier_tangent(glm::vec3 p0, glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float t)
{
  float b0 = -3.0f * (1.0f - t) * (1.0f - t);
  float b1 = 3.0f * (1.0f - t) * (1.0f - t) - 6.0f * t * (1.0f - t);
  float b2 = 6.0f * t * (1.0f - t) - 3.0f * t * t;
  float b3 = 3.0f * t * t;

  return p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;
}

glm::vec3 catmullrom_tangent(glm::vec3 p0, glm::vec3 p3, glm::vec3 t0, glm::vec3 t3, float t)
{
  glm::vec3 p1 = p0 + (t0 / 3.0f);
  glm::vec3 p2 = p3 - (t3 / 3.0f);
  return bezier_tangent(p0, p1, p2, p3, t);
}

#endif //GRAPHICS_SPLINE_H
