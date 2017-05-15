#ifndef GRAPHICS_BSP_H
#define GRAPHICS_BSP_H

#include <vector>
#include <cstdio>
using namespace std;

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <memory>

#include <GL/glew.h>

class Polygon {
    // Simple, convex polygon
public:
    vector<glm::vec3> points;
    glm::vec3 normal;

    Polygon(vector<glm::vec3>);
    vector<Polygon> slice(glm::vec3 point, glm::vec3 normal);
    vector<Polygon> slice(Polygon& plane);
    bool isFront(Polygon& other) const;
    bool isBehind(Polygon& other) const;
    bool isOnSamePlane(Polygon& other) const;
    void draw() const;
};

/**
 * Initializing Polygon with vertices
 * @param point
 */
Polygon::Polygon(vector<glm::vec3> point) {
  this->points.clear();
  assert(point.size() >= 3);
  glm::vec3 e1 = point[1] - point[0];
  glm::vec3 e2 = point[2] - point[1];
  normal = glm::normalize(glm::cross(e1, e2));

  for (int i = 1; i < point.size(); ++i) {
    glm::vec3 ea = point[(i + 1) % point.size()] - point[i];
    glm::vec3 eb = point[(i + 2) % point.size()] - point[(i + 1) % point.size()];
    glm::vec3 n = glm::normalize(glm::cross(ea, eb));
    // Check all points are on same plane
    assert(glm::all(glm::equal(n, normal)));
  }
  for(auto const& p: point) {
    this->points.push_back(p);
  }
}

/**
 * Slice polygon by plane with given point and normal vector
 * @param point
 * @param normal
 * @return
 */
vector<Polygon> Polygon::slice(glm::vec3 p, glm::vec3 n) {
  vector<Polygon> result;
  vector<glm::vec3> polygon[2];
  int pidx = 0;
  for (int i = 0; i < points.size(); ++i) {
    auto p1 = points[i];
    auto p2 = points[(i+1) % points.size()];

    polygon[pidx].push_back(p1);
    if (glm::dot(p1 - p, n) * glm::dot(p2 - p, n) < 0.0f) {
      float s = glm::dot(n, p - p1) / glm::dot(n, p2 - p1);
      glm::vec3 pi = p1 + s * (p2 - p1);
      polygon[pidx % 2].push_back(pi);
      pidx++;
      polygon[pidx % 2].push_back(pi);
    }
  }
  result.push_back(Polygon(polygon[0]));
  if (pidx != 0) {
    result.push_back(Polygon(polygon[1]));
  }
  return result;
}

vector<Polygon> Polygon::slice(Polygon &plane) {
  return slice(plane.points[0], plane.normal);
}

bool Polygon::isFront(Polygon &other) const {
  // Is this in front of (on normal side) other
  for (auto const& p: points) {
    if (glm::dot(p - other.points[0], other.normal) < 0.0f) return false;
  }
  return true;
}

bool Polygon::isBehind(Polygon &other) const {
  // Is this behind (on opposite of normal side) other
  for (auto const& p: points) {
    if (glm::dot(p - other.points[0], other.normal) > 0.0f) return false;
  }
  return true;
}

bool Polygon::isOnSamePlane(Polygon &other) const {
  if (isFront(other) || isBehind(other)) return false;
  return glm::all(glm::equal(normal, other.normal));
}

void Polygon::draw() const {
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(glm::vec3) * points.size(),
               &points[0],
               GL_STATIC_DRAW);

  std::vector<glm::vec3> normals(points.size(), normal);
  GLuint normal_vbo;
  glGenBuffers(1, &normal_vbo);
  glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
  glBufferData(GL_ARRAY_BUFFER,
               sizeof(glm::vec3) * normals.size(),
               &normals[0],
               GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, normal_vbo);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

  glDrawArrays(GL_TRIANGLE_FAN, 0, points.size());
}

class BSPNode {
    using node_ptr = std::unique_ptr<BSPNode>;
    node_ptr front, behind;
    std::vector<Polygon> polygons;
    glm::vec3 normal;
    glm::vec3 point;

public:
    BSPNode(std::vector<Polygon>);
    void print(int indent, int index);
    void draw(glm::vec3);
    bool isFront(glm::vec3&) const;
    bool isBehind(glm::vec3&) const;
};

void BSPNode::print(int indent, int index) {
  printf("%*sindex : %d\n", indent, " ", index);
  for(int i = 0; i < polygons.size(); ++i) {
    auto polygon = polygons[i];
    printf("%*s(%d\n", indent, " ", i);
    for(auto const& point: polygon.points) {
      printf("%*s%s\n", indent, " ", glm::to_string(point).c_str());
    }
    printf("%*s)\n", indent, " ");
  }
  if (front) {
    front->print(indent + 1, index * 2 + 1);
  }
  if (behind) {
    behind->print(indent + 1, index * 2 + 2);
  }
}

BSPNode::BSPNode(std::vector<Polygon> polygons) {
  assert(!polygons.empty());
  // Choose a polygon P from the list
  Polygon& p = polygons[0];
  this->polygons.push_back(p);
  // For each other polygons in the list
  std::vector<Polygon> front, behind;
  this->normal = p.normal;
  this->point = p.points[0];
  for (int i = 1; i < polygons.size(); ++i) {
    if (polygons[i].isFront(p)) front.push_back(polygons[i]);
    else if (polygons[i].isBehind(p)) behind.push_back(polygons[i]);
    else if (polygons[i].isOnSamePlane(p)) this->polygons.push_back(polygons[i]);
    else {
      // Split it into two polygons
      std::vector<Polygon> split = polygons[i].slice(p);
      if (split[0].isFront(p) && split[1].isBehind(p)) {
        front.push_back(split[0]);
        behind.push_back(split[1]);
      }
      else if (split[1].isFront(p) && split[0].isBehind(p)) {
        front.push_back(split[1]);
        behind.push_back(split[0]);
      }
      else assert(false);
    }
  }
  if (!front.empty()) {
    this->front = std::make_unique<BSPNode>(BSPNode(front));
  }
  else {
    this->front = nullptr;
  }
  if (!behind.empty()) {
    this->behind = std::make_unique<BSPNode>(BSPNode(behind));
  }
  else {
    this->behind = nullptr;
  }
}

bool BSPNode::isFront(glm::vec3 &p) const {
  // Is this p in front of (on normal side) this
  return (glm::dot(p - point, normal) > 0.0f);
}

bool BSPNode::isBehind(glm::vec3 &p) const {
  // Is this p in front of (on normal side) this
  return (glm::dot(p - point, normal) < 0.0f);
}

void BSPNode::draw(glm::vec3 v) {
  if (!front && !behind) {
    for(auto const& polygon: polygons) {
      polygon.draw();
    }
  }
  else if (isFront(v)) {
    if (behind) behind->draw(v);
    for(auto const& polygon: polygons) {
      polygon.draw();
    }
    if (front) front->draw(v);
  }
  else if (isBehind(v)) {
    if (front) front->draw(v);
    for(auto const& polygon: polygons) {
      polygon.draw();
    }
    if (behind) behind->draw(v);
  }
  else {
    if (front) front->draw(v);
    if (behind) behind->draw(v);
  }
}

class BSPTree {
    using node_ptr = std::unique_ptr<BSPNode>;
    node_ptr root;
public:
    BSPTree(std::vector<Polygon> polygons);
    void print();
    void draw(glm::vec3);
};

BSPTree::BSPTree(std::vector<Polygon> polygons) {
  assert(!polygons.empty());
  root = std::make_unique<BSPNode>(BSPNode(polygons));
}

void BSPTree::print() {
  root->print(0, 0);
}

void BSPTree::draw(glm::vec3 v) {
  root->draw(v);
}

#endif //GRAPHICS_BSP_H
