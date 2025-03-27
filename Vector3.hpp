#ifndef VECTOR3
#define VECTOR3
#include <math.h>
#include <stdlib.h>
#include <iostream>
#define X e[0]
#define Y e[1]
#define Z e[2]

class Vector3 {

    public:
        double e[3];
        Vector3(): e {0, 0, 0} {}
        Vector3(double e0, double e1, double e2) {
            X = e0;
            Y = e1;
            Z = e2;
        }
        inline double x() const {return X; }
        inline double y() const {return Y; }
        inline double z() const {return Z; }

        inline const Vector3& operator +() const { return *this; }
        inline const Vector3 operator -() const { return Vector3(-X, -Y, -Z); }

        inline double operator [](int i) const { return e[i]; }
        inline double& operator [](int i) { return e[i]; }
    
        inline Vector3
            &operator +=(const Vector3 &other),
            &operator -=(const Vector3 &other),
            &operator *=(const double k),
            &operator /=(const double k);
    
        inline double length() const { return sqrt(X*X + Y*Y + Z*Z) ; }
        inline double sqr_lenght() const { return X*X + Y*Y + Z*Z; }
        inline Vector3 normalized();
        inline Vector3 cross(const Vector3 &other);
        inline const double dot(const Vector3 &other) const;

};

inline bool operator ==(Vector3 u, Vector3 v) {
    return (u.X == v.X) && (u.Y == v.Y) && (u.Z == v.Z);
}

inline Vector3 Vector3::cross(const Vector3 &other) {
    return Vector3( this->y() * other.z() - this->z() * other.y(),
                    this->z() * other.x() - this->x() * other.z(),
                    this->x() * other.y() - this->y() * other.x() );
}

inline const double Vector3::dot(const Vector3 &other) const{
    return  this->x() * other.x() +
            this->y() * other.y() +
            this->z() * other.z();
}

inline Vector3 operator +(const Vector3 &u, const Vector3 &v) {
    return Vector3( u.x() + v.x(),
                    u.y() + v.y(),
                    u.z() + v.z() );
}

inline Vector3 operator -(const Vector3 &u, const Vector3 &v) {
    return Vector3( u.x() - v.x(),
                    u.y() - v.y(),
                    u.z() - v.z() );
}

inline Vector3 operator *(const Vector3 &u, const Vector3 &v) {
    return Vector3( u.x() * v.x(),
                    u.y() * v.y(),
                    u.z() * v.z());
}

inline Vector3 operator *(const Vector3 &v, const double k) {
    return Vector3( v.x() * k,
                    v.y() * k,
                    v.z() * k );
}

inline Vector3 operator *(const double k, const Vector3 &v) {return v*k; }

inline Vector3 operator /(const Vector3 &v, double k) {
    return Vector3( v.x() / k,
                    v.y() / k,
                    v.z() / k );
}

inline Vector3 Vector3::normalized() {
    return *this / this->length();
}

inline Vector3& Vector3::operator+=(const Vector3 &other) {
    X += other.x();
    Y += other.y();
    Z += other.z();
    return *this;
}

inline Vector3& Vector3::operator-=(const Vector3 &other) {
    X -= other.x();
    Y -= other.y();
    Z -= other.z();
    return *this;
}

inline Vector3& Vector3::operator*=(const double k) {
    Z *= k;
    Y *= k;
    Z *= k;
    return *this;
}

inline Vector3& Vector3::operator/=(const double k) {
    Z /= k;
    Y /= k;
    Z /= k;
    return *this;
}

inline std::istream& operator>>(std::istream &is, Vector3 &v) {
    is >> v.X >> v.Y >> v.Z;
    return is;
}

inline std::ostream& operator<<(std::ostream &os, const Vector3 &v) {
    os << v.X << " " << v.Y << " " << v.Z;
    return os;
}

#undef X
#undef Y
#undef Z
#endif
