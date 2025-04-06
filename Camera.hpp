#ifndef CAMERA
#define CAMERA
#include "Vector3.hpp"
#include "Object.hpp"
#include <cassert>
#include <vector>
#include "Scene.hpp"

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
        void draw(Scene scene) {
            for (auto &o : scene.objects) {
                o->apply_transformations();
            }
            std::cout << "P6\n" << screen_width << " " << screen_height << "\n255\n";
            for (int i = screen_height-1; i >= 0; i--) {
                for (int j = 0; j < screen_width; j++) {
                    Vector3 ray_direction = (screen_to_world(i, j) - position).normalized();
                    Color pixel_color = get_color(scene, position, ray_direction, 3);
                    unsigned char
                        r = static_cast<unsigned char>(std::min(255.0, pixel_color.r() * 255.99)),
                        g = static_cast<unsigned char>(std::min(255.0, pixel_color.g() * 255.99)),
                        b = static_cast<unsigned char>(std::min(255.0, pixel_color.b() * 255.99));
                    std::cout << r << g << b;
                }
            }
        }
        Color get_color(Scene scene, Vector3 p, Vector3 v, int recursions) {
            double min_dist = INFINITY;
            Object::Material* material;
            Vector3 normal;
            Color color = BLACK;
            for (Object* o : scene.objects) {
                Object::Intersection hit = o->raycast(p, v);
                double dist = hit.distance;
                if (dist < min_dist && dist > epsilon) {
                    min_dist = dist;
                    material = hit.material;
                    normal = hit.normal;
                }
            }
            if (min_dist == INFINITY) return color;
            Vector3 hit_point = p + v*min_dist;

            // Como a reflexão é ortogonal, preferi calcular a reflexão da visão em n e então o cosseno com a direção da luz
            Vector3 reflected = reflection_vector(-v, normal);

            for (const auto &l : scene.lights) {
                Vector3 light_direction = (l.position - hit_point).normalized();
                bool blocked = false;
                for (Object* o : scene.objects) {
                    Object::Intersection obst = o->raycast(hit_point + normal*epsilon, light_direction);
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
            color += scene.ambient_light * material->ambient;
            //color *= material->opacity;
            if (recursions <= 0) return color;
            
            if (material->opacity < 1) {
                //std::clog << "opacity: " << material->opacity << "\n";
                Vector3 N = normal;
                double n_it;
                if (N.dot(v) < 0) {
                    if (!(material->specular == BLACK)) {
                        // std::clog << normal <<"\n";
                        //hit_point += (normal * epsilon);
                        color += material->specular * get_color(scene, hit_point + normal * epsilon, reflected, recursions-1);
                    }
                    n_it = 1.0/material->ni;
                } else {
                    n_it = material->ni;
                    //N = -N;
                }
                const double &cos_i = N.dot(-v);
                const double &sqrsin_i = 1 - cos_i*cos_i;
                const double &k = 1 - (n_it*n_it) * sqrsin_i;
                //if (k < 0) N = -N;

                Vector3 refracted = refraction_vector(-v, N, n_it);
                // TODO: ADD OFFSET
                color += std::max(0.0, 1 - material->opacity) * get_color(scene, hit_point -N *epsilon, refracted, recursions-1);
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