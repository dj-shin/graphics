#ifndef GRAPHICS_SURFACE_H
#define GRAPHICS_SURFACE_H

#include <cstdio>
#include <cstring>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "spline.h"

static const int N_SPLINE = 20;

class RawSection {
public:
    std::vector<glm::vec2> control_points;
    double scale;
    glm::quat rotate;
    glm::vec3 position;
};

class Section {
public:
    std::vector<glm::vec3> points;
    Section(RawSection &);
};

Section::Section(RawSection & rawSection) {
  // Generate n points from Control points
  points.clear();

  // Move raw control points to world space
  std::vector<glm::vec3> control_points;
  for (int i = 0; i < rawSection.control_points.size(); ++i) {
    glm::vec3 point = glm::vec3(rawSection.control_points[i].x, 0.0f, rawSection.control_points[i].y);
    point = point * (float)rawSection.scale;
    point = glm::vec3(glm::toMat4(rawSection.rotate) * glm::vec4(point, 1.0f));
    point += rawSection.position;
    control_points.push_back(point);
  }

  // Generate closed curve
  size_t cnt = control_points.size();
  for (int i = 0; i < cnt; ++i) {
    glm::vec3 p0 = control_points[i];
    glm::vec3 p1 = control_points[(i + 1) % cnt];
    glm::vec3 p2 = control_points[(i + 2) % cnt];
    glm::vec3 p3 = control_points[(i + 3) % cnt];
    // B-Spline
    for (int k = 0; k < N_SPLINE; ++k) {
      double t = (double) k / N_SPLINE;
      double b0 = (1.0 - t) * (1.0 - t) * (1.0 - t) / 6.0;
      double b1 = (3.0 * t * t * t - 6.0 * t * t + 4.0) / 6.0;
      double b2 = (-3.0 * t * t * t + 3.0 * t * t + 3.0 * t + 1.0) / 6.0;
      double b3 = t * t * t / 6.0;
      glm::vec3 point = (float) b0 * p0 + (float) b1 * p1 + (float) b2 * p2 + (float) b3 * p3;
      points.push_back(point);
    }
  }
}

enum class SurfaceType {
    BSpline,
    CatmullRom
};

class RawSurface {
public:
    std::vector<RawSection> sections;
    SurfaceType surfaceType;
    static RawSurface createFromFile(const char *);
};

class Surface {
public:
    std::vector<Section> sections;
    SurfaceType surfaceType;
    Surface(RawSurface &);
    void fillVertices(GLfloat *);
    size_t dataSize();
    size_t section_count();
    size_t per_section_point_count();
};

Surface::Surface(RawSurface & rawSurface) {
  // With overall Raw Sections, generate spline for scaling, rotation, translation
  // Generate Sections with interpolating splines, each containing interpolating closed curve
  surfaceType = rawSurface.surfaceType;

  const size_t rawSectionCnt = rawSurface.sections.size();
  for (int i = 0; i < rawSectionCnt; ++i) {
    // For each 4 Raw Sections, create subsurface by generating interpolating sections
    RawSection s0 = rawSurface.sections[i];
    RawSection s1 = rawSurface.sections[(i + 1) % rawSectionCnt];
    RawSection s2 = rawSurface.sections[(i + 2) % rawSectionCnt];
    RawSection s3 = rawSurface.sections[(i + 3) % rawSectionCnt];

    for (int j = 0; j < N_SPLINE; ++j) {
      float t = (float)j / N_SPLINE;
      // Scaling
      float scale = bspline(s0.scale, s1.scale, s2.scale, s3.scale, t);
      // Rotate
      glm::quat rotate = bspline(s0.rotate, s1.rotate, s2.rotate, s3.rotate, t);
      // Translate
      glm::vec3 position = bspline(s0.position, s1.position, s2.position, s2.position, t);

      // Generate interpolating section
      std::vector<glm::vec2> control_points;
      for (int k = 0; k < s0.control_points.size(); ++k) {
        control_points.push_back(bspline(s0.control_points[k], s1.control_points[k], s2.control_points[k], s3.control_points[k], t));
      }
      RawSection rawSection;
      rawSection.control_points = control_points;
      rawSection.scale = scale;
      rawSection.rotate = rotate;
      rawSection.position = position;
      Section section = Section(rawSection);

      sections.push_back(section);
    }
  }
}

RawSurface RawSurface::createFromFile(const char * path) {
  FILE * file = fopen(path, "r");
  RawSurface rawSurface;
  if (file == NULL) {
    printf("Failed to open spline data file\n");
    return rawSurface;
  }

  char curve_type_string[32];
  SurfaceType curve_type;
  fscanf(file, "%s", curve_type_string);
  if (strcmp(curve_type_string, "BSPLINE") == 0) curve_type = SurfaceType::BSpline;
  else if (strcmp(curve_type_string, "CATMULL_ROM") == 0) curve_type = SurfaceType::CatmullRom;
  else {
    printf("Invalid curve type\n");
    return rawSurface;
  }
  rawSurface.surfaceType = curve_type;

  int num_cross_section, num_control_point;
  fscanf(file, "%d %d", &num_cross_section, &num_control_point);
  for (int i = 0; i < num_cross_section; ++i) {
    RawSection cs;
    for (int j = 0; j < num_control_point; ++j) {
      double x, z;
      fscanf(file, "%lf %lf", &x, &z);
      cs.control_points.push_back(glm::vec2(x, z));
    }
    double scale;
    double angle;
    int x, y, z;
    fscanf(file, "%lf", &scale);
    fscanf(file, "%lf %d %d %d", &angle, &x, &y, &z);
    cs.rotate = glm::quat(glm::cos(angle / 2.0), x * glm::sin(angle / 2.0), y * glm::sin(angle / 2.0), z * glm::sin(angle / 2.0));
    fscanf(file, "%d %d %d", &x, &y, &z);
    glm::vec3 position = glm::vec3(x, y, z);

    cs.scale = scale;
    cs.position = position;
    rawSurface.sections.push_back(cs);
  }
  return rawSurface;
}

void Surface::fillVertices(GLfloat * vertices) {
  // Fill given vertices memory for OpenGL
  for (int i = 0; i < sections.size(); ++i) {
    for (int j = 0; j < sections[i].points.size(); ++j) {
      *(vertices + (i * sections[i].points.size() + j) * 3 + 0) = sections[i].points[j].x;
      *(vertices + (i * sections[i].points.size() + j) * 3 + 1) = sections[i].points[j].y;
      *(vertices + (i * sections[i].points.size() + j) * 3 + 2) = sections[i].points[j].z;
    }
  }
}

size_t Surface::dataSize() {
  // Total number of points * 3(xyz)
  return sections.size() * sections[0].points.size() * 3;
}

size_t Surface::section_count() {
  return sections.size();
}

size_t Surface::per_section_point_count() {
  return sections[0].points.size();
}

#endif //GRAPHICS_SURFACE_H
