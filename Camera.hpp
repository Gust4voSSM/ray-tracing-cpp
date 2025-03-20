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
            screen_distance {2}
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
        void draw(std::vector<Object*> objects) {
            std::cout << "P3\n" << screen_width << " " << screen_height << "\n255\n";
            for (int i = screen_height-1; i >= 0; i--) {
                for (int j = 0; j < screen_width; j++) {
                    //std::cout << "i = " << i << ", j = " << j <<"\n";
                    Vector3 ray_direction = (screen_to_world(i, j) - position).normalized();
                    double min_dist = INFINITY;
                    Color pixel_color;
                    for (Object* o : objects) {
                        double dist = o->raycast(position, ray_direction).distance;
                        if (dist < min_dist) {
                            min_dist = dist;
                            pixel_color = o->material->diffuse;
                        }
                    }
                    std::cout << pixel_color * 255.99 << "\n";
                }
            }
        }
};
#endif