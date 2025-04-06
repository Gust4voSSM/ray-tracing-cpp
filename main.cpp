#include <iostream>
#include "Raytracing.hpp"
#include "Scene.hpp"
using namespace std;

int main() {    
    Object::Material marrom, branco, madeira;
    marrom.diffuse = Color(0.6, 0.3, 0);
    marrom.ambient = Color(0.1, 0.05, 0);
    
    madeira.diffuse = Color(0.5, 0.25, 0);
    madeira.specular = Color(0.9, 0.9, 0.9);
    madeira.ns = 10;

    Object::Material reflexivo;
    reflexivo.diffuse = Color(0, 0, 0);
    reflexivo.specular = Color(0.5, 0.5, 0.5);
    reflexivo.ns = 100;
    reflexivo.opacity = 0.5;
    reflexivo.ni = 1.5;
    
    Plane plano(Vector3(0, 1, 0));
    plano.material = &madeira;
    
    // Cena esfera
    Vector3 
        cam_pos(-2, 3, 0),
        target(6, 1.5, 0);
    Camera sphereCam(cam_pos, target);
    Scene sphereScene;
        Sphere esfera(2);
        esfera.position = Vector3(6, 1.5, 0);
        esfera.material = &reflexivo;
        sphereScene.objects.push_back(&plano);
        sphereScene.objects.push_back(&esfera);
        sphereScene.lights.emplace_back(Vector3(0, 8, -5));

    // Cena icosaedro
    cam_pos = Vector3(-2, 3, 0);
    target = Vector3(6, 1.5, 0);
    Camera icosCam(cam_pos, target);
    Scene icosScene;
        TriangleMesh ico("inputs/icosahedron.obj");
        icosScene.lights.emplace_back(Vector3(0, 8, 5));
        icosScene.objects.push_back(&plano);
        icosScene.objects.push_back(&ico);

    // Cena macaco
    cam_pos = Vector3(-2, 3, 0);
    target = Vector3(6, 3, 0);
    Camera macacoCam(cam_pos, target);
    Scene macacoScene;
        TriangleMesh macaco("inputs/macaco.obj");
        macaco.position = Vector3(3, 3, 0);
        macaco.scale = Vector3(6, 6, 6);
        macaco.rotation.y() = 90;
        macacoScene.lights.emplace_back(Vector3(0, 8, 5));
        macacoScene.objects.push_back(&plano);
        macacoScene.objects.push_back(&macaco);

    macacoCam.draw(macacoScene);
}

