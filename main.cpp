#include <iostream>
#include "Raytracing.hpp"
using namespace std;

int main() {
    vector<Object*> objs;
    
    Object::Material marrom, branco, madeira;
    marrom.diffuse = Color(0.6, 0.3, 0);
    marrom.ambient = Color(0.1, 0.05, 0);
    
    
    madeira.diffuse = Color(0.5, 0.25, 0);
    madeira.specular = Color(0.9, 0.9, 0.9);
    madeira.ns = 10;

    Object::Material meuMat;
    meuMat.diffuse = Color(0.9, 0.9, 0.9);
    meuMat.specular = Color(0.5, 0.5, 0.5);
    meuMat.ns = 10;
    
    Plane plano(Vector3(0, 0, 0), Vector3(0, 1, 0));
    plano.material = &madeira;
    
    Sphere esfera(Vector3(6, 2, 0), 2);
    esfera.material = &meuMat;
    
    Vector3
        cam_pos(-2, 3, 0),
        target(6, 2, 0);

    Camera cam(cam_pos, target);
    cam.lights.emplace_back(Vector3(0, 8, -5));
    //cam.lights.emplace_back(Vector3(0, 8, 5));
    cam.lights[0].color = Color(1, 1, 1);
    //cam.lights[1].color = Color(0.3, 0.3, 0.3);
    cam.ambient_light = Color(0.1, 0.1, 0.4);

    //cam.light_sources.emplace_back(0, 5, 0);
    //TriangleMesh mesh("inputs/icosahedron.obj");
    
    // Vector3 a(0, 1, 1);
    // Vector3 b(1, 2, 2);
    // Vector3 c(-1, 2, 2);

    // Triangle tri = Triangle(&a, &b, &c);
    // tri.material = &branco;

    objs.push_back(&plano);
    objs.push_back(&esfera);
    //cout << mesh;
    cam.draw(objs);
}

