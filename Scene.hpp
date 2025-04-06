#ifndef SCENE_HEADER
#define SCENE_HEADER
#include "Vector3.hpp"
#include "Color.hpp"
#include <vector>
#include "Object.hpp"

struct Light {
    Vector3 position;
    Color color = WHITE;
    Light(Vector3 p): position {p} {}
    Light(double x, double y, double z): position {Vector3(x, y, z)} {}
    Light() {}
};

struct Scene {
    std::vector<Light> lights;
    std::vector<Object*> objects;
    Color ambient_light = Color(0.1, 0.1, 0.1);
    Color bacground_color = BLACK;
};

#endif