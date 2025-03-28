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
            Light(double x, double y, double z): position {Vector3(x, y, z)} {}
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
            std::cout << "P6\n" << screen_width << " " << screen_height << "\n255\n";
            for (int i = screen_height-1; i >= 0; i--) {
                for (int j = 0; j < screen_width; j++) {
                    Vector3 ray_direction = (screen_to_world(i, j) - position).normalized();
                    Color pixel_color = get_color(objects, position, ray_direction, 5);
                    unsigned char
                        r = static_cast<unsigned char>(std::min(255.0, pixel_color.r() * 255.99)),
                        g = static_cast<unsigned char>(std::min(255.0, pixel_color.g() * 255.99)),
                        b = static_cast<unsigned char>(std::min(255.0, pixel_color.b() * 255.99));
                    std::cout << r << g << b;
                }
            }
        }
        Color get_color(std::vector<Object*> objects, Vector3 p, Vector3 v, int recursions) {
            double min_dist = INFINITY;
            Object* hit_obj;
            Vector3 normal;
            Color color = BLACK;
            for (Object* o : objects) {
                Object::Intersection hit = o->raycast(p, v);
                double dist = hit.distance;
                if (dist < min_dist && dist > epsilon) {
                    min_dist = dist;
                    hit_obj = hit.object;
                }
            }
            if (min_dist == INFINITY) return color;
            v = v.normalized();
            Vector3 hit_point = p + v*min_dist;
            normal = hit_obj->get_normal(hit_point);
            Object::Material *material = hit_obj->material;

            // Como a reflexão é ortogonal, preferi calcular a reflexão da visão em n e então o cosseno com a direção da luz
            Vector3 reflected = reflection_vector(-v, normal);

            for (auto l : lights) {
                Vector3 light_direction = (l.position - hit_point).normalized();
                bool blocked = false;
                for (Object* o : objects) {
                    if (hit_obj == o) continue;
                    Object::Intersection obst = o->raycast(hit_point, light_direction);
                    double distance = obst.distance;
                    if(distance != INFINITY && distance > epsilon) {
                        blocked = true;
                        break;
                    }
                }
                if (blocked) continue;

                double cos_theta = (light_direction).dot(normal);
                if (cos_theta <= 0) cos_theta = 0;
                // Difusa
                else color += (l.color * material->diffuse) * cos_theta;

                double cos_alpha = reflected.dot(light_direction);
                if (cos_alpha <= 0) continue;
                
                cos_alpha = pow(cos_alpha, material->ns);
                // Especular
                color += (l.color * material->specular) * cos_alpha;
            }
            color += ambient_light * material->ambient;
            color *= material->opacity;
            if (!(material->specular == BLACK) && recursions > 0) {
                // std::clog << normal <<"\n";
                //hit_point += (normal * epsilon);
                color += material->specular * get_color(objects, hit_point + normal * epsilon, reflected, recursions-1);
            }
            
            if (material->opacity < 1 && recursions > 0) {
                //std::clog << "opacity: " << material->opacity << "\n";
                Vector3 N = normal;
                double n_it;
                if (N.dot(v) < 0) {
                    n_it = 1.0/material->ni;
                } else {
                    n_it = material->ni;
                    N = -N;
                }
                Vector3 refracted = refraction_vector(-v, N, n_it);
                color += std::max(0.0, 1 - material->opacity) * get_color(objects, hit_point - epsilon * N, refracted, recursions-1);
            }
            return color;
        }
        inline const Vector3 refraction_vector(const Vector3 &I, Vector3 &N, double n_it) const {
            const double &cos_i = N.dot(I);
            const double &sqrsin_i = 1 - cos_i*cos_i;
            const double &k = 1 - (n_it*n_it) * sqrsin_i;

            if (k < 0) {
                return reflection_vector(I, N);
            }
            return (n_it*(cos_i) - sqrt(k))*N + n_it*I;
        }
        inline const Vector3 reflection_vector(const Vector3 &I, const Vector3 &N) const {
            return 2 * N * N.dot(I) - I;
        }
};


#endif