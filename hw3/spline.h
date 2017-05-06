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

#endif //GRAPHICS_SPLINE_H
