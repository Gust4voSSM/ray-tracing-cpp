#ifndef COLOR
#define COLOR
#include "Vector3.hpp"
#define R e[0]
#define G e[1]
#define B e[2]

class Color: public Vector3 {
    public:
        Color() { R = 0; G = 0; B = 0; }
        explicit Color(Vector3 &v) { R = v.R; G = v.G; B = v.B; } 
        Color(double r, double g, double b) { R = r; G = g; B = b; };
        inline double r() const { return R; }
        inline double g() const { return G; }
        inline double b() const { return B; }
};

const Color
    BLACK = Color(0, 0, 0),
    WHITE = Color(1, 1, 1);

#undef R
#undef G
#undef B
#endif