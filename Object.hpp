#ifndef OBJECT_HPP
#define OBJECT_HPP
#include "Vector3.hpp"
#include "Color.hpp"
#include <math.h>
#include <vector>

const double epsilon = 1.0E-8;

class Object {

public:
    Object() : material(default_material) {}
    virtual ~Object() {}
    
    struct Material {
        Color  diffuse  = Color (1.0, 1.0, 1.0),
        specular = Color (0.0, 0.0, 0.0),
        ambient  = Color (0.1, 0.1, 0.1),
        emissive = Color (0.0, 0.0, 0.0);
        double opacity  = 1,
        ni       = 0.5,
        ns       = 0;
    } *material;
    
    struct Intersection {
        double distance;
        Object *object;
        Intersection(double d): distance {d},  object {nullptr} {};
        Intersection(double d, Object *o): distance {d}, object {o} {};
    };
    virtual Intersection raycast(Vector3 p, Vector3 v) = 0;
    virtual Vector3 get_normal(const Vector3 &p) = 0;
    
    static Material *default_material;

    //void rotate(Vector3 v) { rotate(v.x(), v.y(), v.z()); }
    //virtual void rotate(double x, double y, double z) = 0;
    //void translate(Vector3 v) { translate(v.x(), v.y(), v.z()); }
    //virtual void translate(double x, double y, double z) = 0;

};

Object::Material* Object::default_material = new Object::Material();

class Plane: public Object {

public:
    Plane(Vector3 point, Vector3 normal): point {point}, normal {normal.normalized()} {}
    Vector3 normal;
    Vector3 point;

    Vector3 get_normal(const Vector3 &p) { return normal; }
    Intersection raycast(Vector3 p, Vector3 v) {

        double sin = normal.dot(v);

        if (fabs(sin) <= epsilon) {
            return INFINITY;
        }
        
        double distance = (point - p).dot(normal) / sin;

        return (distance >= 0)? Intersection(distance, this) : INFINITY;
    }
};

class Sphere: public Object {

public:
    Sphere() {}
    Sphere(Vector3 center, double radius): center {center}, radius {radius} {}
    Vector3 get_normal(const Vector3 &p) { return (p - center).normalized(); }
    Intersection raycast(Vector3 p, Vector3 v) {
        Vector3 d = center - p;
        
        const double proj_lenght = d.dot(v);
        
        if (proj_lenght < 0) return INFINITY;

        const double
            square_distance = d.sqr_lenght() - (proj_lenght*proj_lenght),
            square_radius = radius*radius;
        
        if (square_distance > square_radius) return INFINITY;

        double
            half_chord = sqrt(square_radius - square_distance),

            d_1 = proj_lenght - half_chord,
            d_2 = proj_lenght + half_chord;

        //std::cerr << "Interseção!\n";

        return  (d_1 > 0)? Intersection(d_1, this) :
                (d_2 > 0)? Intersection(d_2, this) : INFINITY;

    }
    Vector3 center;
    double radius;
};

class Triangle: public Object {
    public:
        Triangle(Vector3* v0, Vector3* v1, Vector3* v2) {
            v[0] = v0;
            v[1] = v1;
            v[2] = v2;
            normal = (*v[1] - *v[0]).cross(*v[2] - *v[0]).normalized();
        }
        Vector3 get_normal(const Vector3 &p) { return normal; }
        Intersection raycast(Vector3 origin, Vector3 direction) {
            double dist = Plane(*v[0], normal).raycast(origin, direction).distance;
            if (dist < 0) return INFINITY;
            Vector3 P = origin + (direction * dist);
            const Vector3 &A = *v[0], &B = *v[1], &C = *v[2];

            Vector3
                AB = *v[1] - *v[0],
                AC = *v[2] - *v[0],
                AP = P-*v[0];

            double a, b, c;

            // # Calcula os coeficientes baricêntricos usando produto escalar
            double denom = AB.dot(AB) * AC.dot(AC) - AB.dot(AC) * AB.dot(AC);
            if (fabs(denom) < epsilon) return INFINITY;
            // Triângulo degenerado

            a = (AC.dot(AC) * AP.dot(AB) - AB.dot(AC) * AP.dot(AC)) / denom;
            b = (AB.dot(AB) * AP.dot(AC) - AB.dot(AC) * AP.dot(AB)) / denom;
            c = 1 - b - a;


            if (a >= 0 && b >= 0 && c > 0 && a < 1 && b < 1 && c < 1) return Intersection(dist, this);
            return INFINITY;
        }
        Vector3* v[3];
        Vector3 normal;
};
#endif