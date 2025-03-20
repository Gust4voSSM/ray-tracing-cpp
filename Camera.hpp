#ifndef CAMERA
#define CAMERA
#include "Vector3.hpp"
#include "Object.hpp"
#include <cassert>
#include <vector>

class Camera {
    private:
        Vector3 forward;
        Vector3 right;
        Vector3 upwards;
    public:
        Camera(Vector3 position, Vector3 target):
            position {position}, target {target},
            screen_height {900}, screen_width {1600},
            global_up {Vector3(0, 1, 0)},
            screen_distance {1}
            {
                forward = (target - position).normalized();
                right = global_up.cross(forward);
                upwards = forward.cross(right).normalized();
            };
        Vector3 position;
        Vector3 target;
        Vector3 global_up;

        struct Light {
            Vector3 position;
            Color color = WHITE;
            Light(Vector3 p): position {p} {}
            Light() {}
        };

        std::vector<Light> lights;
        Color ambient_light = WHITE;
        Color bacground_color = BLACK;

        double screen_distance;
        int screen_height;
        int screen_width;
        double global_height = 0.9;
        double global_width = 1.6;
        
        Vector3 screen_to_world(int i, int j) {
            assert (i >= 0 && i < screen_height);
            assert (j >= 0 && j < screen_width);

            Vector3 screen_center = position + forward * screen_distance;

            double pixel_height = global_height/screen_height;
            double pixel_width = global_width/screen_width;

            Vector3 dh = upwards * (i - (screen_height - 1) / 2) * pixel_height;
            Vector3 dw = right * ((screen_width)/ 2 - j) * pixel_width;

            return screen_center + dh + dw;
        }
        void draw(std::vector<Object*> objects) {
            std::cout << "P3\n" << screen_width << " " << screen_height << "\n255\n";
            for (int i = screen_height-1; i >= 0; i--) {
                for (int j = 0; j < screen_width; j++) {
                    Vector3 ray_direction = (screen_to_world(i, j) - position).normalized();
                    Color pixel_color = get_color(objects, position, ray_direction);
                    std::cout << pixel_color * 255.99 << "\n";
                }
            }
        }
        Color get_color(std::vector<Object*> objects, Vector3 p, Vector3 v) {
            double min_dist = INFINITY;
            Object* hit_obj;
            Vector3 normal;
            Color color = BLACK;
            for (Object* o : objects) {
                Object::Intersection hit = o->raycast(p, v);
                double dist = hit.distance;
                if (dist < min_dist) {
                    min_dist = dist;
                    hit_obj = hit.object;
                }
            }
            if (min_dist == INFINITY) return color;
            v = v.normalized();
            Vector3 hit_point = p + v*min_dist;
            normal = hit_obj->get_normal(hit_point);
            Object::Material *material = hit_obj->material;
            for (auto l : lights) {
                Vector3 light_direction = (l.position - hit_point).normalized();
                bool blocked = false;
                for (Object* o : objects) {
                    double distance = o->raycast(hit_point, light_direction).distance;
                    if(distance != INFINITY && distance > epsilon) {
                        blocked = true;
                        break;
                    }
                }
                if (blocked) continue;
                double cos = (light_direction).dot(normal);
                if (cos < 0) cos = 0;
                color += (l.color * material->diffuse) * cos; // Difusa
                Vector3 r = (2 * cos * normal) - light_direction;
                double cos2 = r.dot(v);
                if (cos2 < 0) cos2 = 0;
                cos2 = pow(cos2, material->ns);
                color += (l.color * material->specular) * cos2; //Especular
            }
            return color + ambient_light * material->ambient;
        } 
};
#endif