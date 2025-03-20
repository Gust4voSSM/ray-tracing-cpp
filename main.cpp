#include <iostream>
#include "Raytracing.hpp"
using namespace std;

int main() {
    vector<Object*> objs;
    
    Object::Material rosa, branco;
    rosa.diffuse = Color(1, 0.7, 0.75);
    
    Plane plano(Vector3(0, -1, 0), Vector3(0, 1, 0));
    plano.material = &branco;
    
    Sphere esfera(Vector3(0, 1, 0), 2);
    esfera.material = &branco;
    
    Vector3
        cam_pos(0, 2, 0),
        target(6, 2, 0);

    Camera cam(cam_pos, target);
    cam.lights.emplace_back(Vector3(0, 8, -3));
    cam.lights.emplace_back(Vector3(0, 8, 3));
    cam.lights[0].color = Color(1, 0.2, 0.1);
    cam.lights[1].color = Color(0.1, 0.2, 1);

    //cam.light_sources.emplace_back(0, 5, 0);
    TriangleMesh mesh("inputs/icosahedron.obj");
    
    Vector3 a(0, 1, 1);
    Vector3 b(1, 2, 2);
    Vector3 c(-1, 2, 2);

    Triangle tri = Triangle(&a, &b, &c);
    tri.material = &branco;

    objs.push_back(&plano);
    objs.push_back(&mesh);
    //cout << mesh;
    cam.draw(objs);
}

