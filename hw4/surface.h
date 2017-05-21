#ifndef GRAPHICS_SURFACE_H
#define GRAPHICS_SURFACE_H

#include <cstdio>
#include <cstring>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "spline.h"

static const int N_SPLINE = 20;

enum class CurveType {
    BSpline,
    CatmullRom
};

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
    std::vector<glm::vec3> tangents;
    Section(RawSection &, CurveType);
};

Section::Section(RawSection & rawSection, CurveType curveType) {
  // Generate n points from Control points
  points.clear();
  tangents.clear();

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
  if (curveType == CurveType::BSpline) {
    for (int i = 0; i < cnt; ++i) {
      glm::vec3 p0 = control_points[i];
      glm::vec3 p1 = control_points[(i + 1) % cnt];
      glm::vec3 p2 = control_points[(i + 2) % cnt];
      glm::vec3 p3 = control_points[(i + 3) % cnt];
      // B-Spline
      for (int k = 0; k < N_SPLINE; ++k) {
        float t = (float) k / N_SPLINE;
        glm::vec3 point = bspline(p0, p1, p2, p3, t);
        glm::vec3 tangent = bspline_tanget(p0, p1, p2, p3, t);
        points.push_back(point);
        tangents.push_back(tangent);
      }
    }
  }
  else {
    for (int i = 0; i < cnt; ++i) {
      glm::vec3 p0 = control_points[i];
      glm::vec3 p1 = control_points[(i + 1) % cnt];
      glm::vec3 p2 = control_points[(i + 2) % cnt];
      glm::vec3 p3 = control_points[(i + 3) % cnt];
      // B-Spline
      for (int k = 0; k < N_SPLINE; ++k) {
        float t = (float) k / N_SPLINE;
        glm::vec3 t1 = (p2 - p0) / 2.0f;
        glm::vec3 t2 = (p3 - p1) / 2.0f;
        glm::vec3 point = catmullrom(p1, p2, t1, t2, t);
        glm::vec3 tangent = catmullrom_tangent(p1, p2, t1, t2, t);
        points.push_back(point);
        tangents.push_back(tangent);
      }
    }
  }
}

class RawSurface {
public:
    std::vector<RawSection> sections;
    CurveType surfaceType;
    static RawSurface createFromFile(const char *);
};

class Surface {
public:
    std::vector<Section> sections;
    std::vector<glm::vec3> normals;
    CurveType curveType;
    Surface(RawSurface &);
    void fillVertices(GLfloat *);
    void fillNormalVertices(GLfloat *);
    void fillMeshVertices(GLfloat *);
    size_t dataSize();
    size_t meshDataSize();
    size_t section_count();
    size_t per_section_point_count();
};

Surface::Surface(RawSurface & rawSurface) {
  // With overall Raw Sections, generate spline for scaling, rotation, translation
  // Generate Sections with interpolating splines, each containing interpolating closed curve
  curveType = rawSurface.surfaceType;

  const size_t rawSectionCnt = rawSurface.sections.size();
  for (int i = 0; i < rawSectionCnt - 1; ++i) {
    // For each consecutive Raw Sections, create subsurface by generating interpolating sections
    RawSection s1 = rawSurface.sections[i];
    RawSection s2 = rawSurface.sections[i + 1];

    for (int j = 0; j < N_SPLINE; ++j) {
      float t = (float)j / N_SPLINE;
      // Scaling
      float st1 = (i == 0) ? (s2.scale - s1.scale) / 2.0f : (s2.scale - rawSurface.sections[i - 1].scale) / 2.0f;
      float st2 = (i == rawSectionCnt - 2) ? (s2.scale - s1.scale) / 2.0f : (rawSurface.sections[i + 2].scale - s1.scale) / 2.0f;
      float scale = catmullrom(s1.scale, s2.scale, st1, st2, t);
      // Rotate
      glm::quat rt1 = (i == 0) ?
                      glm::log(glm::inverse(s1.rotate) * s1.rotate) / 2.0f :
                      glm::log(glm::inverse(rawSurface.sections[i - 1].rotate) * s2.rotate) / 2.0f;
      glm::quat rt2 = (i == rawSectionCnt - 2) ?
                      glm::log(glm::inverse(s1.rotate) * s2.rotate) / 2.0f :
                      glm::log(glm::inverse(s1.rotate) * rawSurface.sections[i + 2].rotate) / 2.0f;
      glm::quat rotate = catmullrom(s1.rotate, s2.rotate, rt1, rt2, t);
      // Translate
      glm::vec3 pt1 = (i == 0) ? (s2.position - s1.position) / 2.0f : (s2.position - rawSurface.sections[i - 1].position) / 2.0f;
      glm::vec3 pt2 = (i == rawSectionCnt - 2) ? (s2.position - s1.position) / 2.0f : (rawSurface.sections[i + 2].position - s1.position) / 2.0f;
      glm::vec3 position = catmullrom(s1.position, s2.position, pt1, pt2, t);

      // Generate interpolating section
      std::vector<glm::vec2> control_points;
      for (int k = 0; k < s1.control_points.size(); ++k) {
        glm::vec2 cpt1 = (i == 0) ? (s2.control_points[k] - s1.control_points[k]) / 2.0f : (s2.control_points[k] - rawSurface.sections[i - 1].control_points[k]) / 2.0f;
        glm::vec2 cpt2 = (i == rawSectionCnt - 2) ? (s2.control_points[k] - s1.control_points[k]) / 2.0f : (rawSurface.sections[i + 2].control_points[k] - s1.control_points[k]) / 2.0f;
        control_points.push_back(catmullrom(s1.control_points[k], s2.control_points[k], cpt1, cpt2, t));
      }
      RawSection rawSection;
      rawSection.control_points = control_points;
      rawSection.scale = scale;
      rawSection.rotate = rotate;
      rawSection.position = position;
      Section section = Section(rawSection, curveType);

      sections.push_back(section);
    }
  }
  for (int i = 0; i < rawSectionCnt; ++i) {
    const size_t pcnt = sections[i].points.size();
    for (int j = 0; j < pcnt; ++j) {
      glm::vec3 pl = sections[i].points[(j - 1 + pcnt) % pcnt];
      glm::vec3 pr = sections[i].points[(j + 1 + pcnt) % pcnt];
      glm::vec3 pu = sections[(i - 1 + rawSectionCnt) % rawSectionCnt].points[j];
      glm::vec3 pd = sections[(i + 1 + rawSectionCnt) % rawSectionCnt].points[j];
      glm::vec3 normal = glm::cross(pr - pl, pu - pd);
      normals.push_back(normal);
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
  CurveType curve_type;
  fscanf(file, "%s", curve_type_string);
  if (strcmp(curve_type_string, "BSPLINE") == 0) curve_type = CurveType::BSpline;
  else if (strcmp(curve_type_string, "CATMULL_ROM") == 0) curve_type = CurveType::CatmullRom;
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
    double x, y, z;
    fscanf(file, "%lf", &scale);
    fscanf(file, "%lf %lf %lf %lf", &angle, &x, &y, &z);
    cs.rotate = glm::quat(glm::cos(angle / 2.0), x * glm::sin(angle / 2.0), y * glm::sin(angle / 2.0), z * glm::sin(angle / 2.0));
    fscanf(file, "%lf %lf %lf", &x, &y, &z);
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

void Surface::fillNormalVertices(GLfloat * vertices) {
  // Fill given normals memory for OpenGL
  size_t scnt = sections.size();
  int idx = 0;
  for (int i = 0; i < scnt; ++i) {
    size_t cnt = sections[i].points.size();
    for (int j = 0; j < cnt; ++j) {
      // Upper Triangle
      if (i > 0) {
        glm::vec3 p11 = sections[i].points[j];
        glm::vec3 p01 = sections[(i - 1 + scnt) % scnt].points[j];
        glm::vec3 p02 = sections[(i - 1 + scnt) % scnt].points[(j + 1) % cnt];

        glm::vec3 n1 = glm::normalize(glm::cross(p02 - p11, p01 - p11));

        *(vertices + (idx++)) = n1.x;
        *(vertices + (idx++)) = n1.y;
        *(vertices + (idx++)) = n1.z;

        *(vertices + (idx++)) = n1.x;
        *(vertices + (idx++)) = n1.y;
        *(vertices + (idx++)) = n1.z;

        *(vertices + (idx++)) = n1.x;
        *(vertices + (idx++)) = n1.y;
        *(vertices + (idx++)) = n1.z;
      }

      // Lower Triangle
      if (i < scnt - 1) {
        glm::vec3 p11 = sections[i].points[j];
        glm::vec3 p21 = sections[(i + 1 + scnt) % scnt].points[j];
        glm::vec3 p20 = sections[(i + 1 + scnt) % scnt].points[(j - 1 + cnt) % cnt];

        glm::vec3 n2 = glm::normalize(glm::cross(p20 - p11, p21 - p11));

        *(vertices + (idx++)) = n2.x;
        *(vertices + (idx++)) = n2.y;
        *(vertices + (idx++)) = n2.z;

        *(vertices + (idx++)) = n2.x;
        *(vertices + (idx++)) = n2.y;
        *(vertices + (idx++)) = n2.z;

        *(vertices + (idx++)) = n2.x;
        *(vertices + (idx++)) = n2.y;
        *(vertices + (idx++)) = n2.z;
      }
    }
  }
}

void Surface::fillMeshVertices(GLfloat * vertices) {
  // Fill given vertices memory for OpenGL
  size_t scnt = sections.size();
  int idx = 0;
  for (int i = 0; i < scnt; ++i) {
    size_t cnt = sections[i].points.size();
    for (int j = 0; j < cnt; ++j) {
      // Upper Triangle
      if (i > 0) {
        *(vertices + (idx++)) = sections[i].points[j].x;
        *(vertices + (idx++)) = sections[i].points[j].y;
        *(vertices + (idx++)) = sections[i].points[j].z;

        *(vertices + (idx++)) = sections[(i - 1 + scnt) % scnt].points[j].x;
        *(vertices + (idx++)) = sections[(i - 1 + scnt) % scnt].points[j].y;
        *(vertices + (idx++)) = sections[(i - 1 + scnt) % scnt].points[j].z;

        *(vertices + (idx++)) = sections[(i - 1 + scnt) % scnt].points[(j + 1) % cnt].x;
        *(vertices + (idx++)) = sections[(i - 1 + scnt) % scnt].points[(j + 1) % cnt].y;
        *(vertices + (idx++)) = sections[(i - 1 + scnt) % scnt].points[(j + 1) % cnt].z;
      }

      // Lower Triangle
      if (i < scnt - 1) {
        *(vertices + (idx++)) = sections[i].points[j].x;
        *(vertices + (idx++)) = sections[i].points[j].y;
        *(vertices + (idx++)) = sections[i].points[j].z;

        *(vertices + (idx++)) = sections[(i + 1) % scnt].points[j].x;
        *(vertices + (idx++)) = sections[(i + 1) % scnt].points[j].y;
        *(vertices + (idx++)) = sections[(i + 1) % scnt].points[j].z;

        *(vertices + (idx++)) = sections[(i + 1) % scnt].points[(j - 1 + cnt) % cnt].x;
        *(vertices + (idx++)) = sections[(i + 1) % scnt].points[(j - 1 + cnt) % cnt].y;
        *(vertices + (idx++)) = sections[(i + 1) % scnt].points[(j - 1 + cnt) % cnt].z;
      }
    }
  }
}

size_t Surface::dataSize() {
  // Total number of points * 3(xyz)
  return sections.size() * sections[0].points.size() * 3;
}

size_t Surface::meshDataSize() {
  // Total number of points * 3(xyz) * 2
  return (sections.size() - 1) * sections[0].points.size() * 3 * 6;
}

size_t Surface::section_count() {
  return sections.size();
}

size_t Surface::per_section_point_count() {
  return sections[0].points.size();
}

#endif //GRAPHICS_SURFACE_H
