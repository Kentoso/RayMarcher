#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>

using std::sqrt;
class vec3;

inline vec3 cross(const vec3 &u, const vec3 &v);
inline float dot(const vec3 &u, const vec3 &v);

class vec3 {
    public:
        vec3() : e{0,0,0} {}
        vec3(float e0, float e1, float e2) : e{e0, e1, e2} {}

        float x() const { return e[0]; }
        float y() const { return e[1]; }
        float z() const { return e[2]; }

        
    
        vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
        float operator[](int i) const { return e[i]; }
        float& operator[](int i) { return e[i]; }

        vec3& operator+=(const vec3 &v) {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        vec3 operator+(const vec3 &v) {
            return {e[0] + v[0], e[1] + v[1], e[2] + v[2]};
        }

        vec3& operator*=(const float t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this;
        }

        vec3 operator*(const float t) {
            return {e[0] * t, e[1] * t, e[2] * t};
        }

        vec3& operator/=(const float t) {
            return *this *= 1/t;
        }

        vec3 abs() const
        {
            return {fabs(e[0]), fabs(e[1]), fabs(e[2])};
        }

        vec3 rotate_by(vec3 axis, float angle)
        {
            return *this * cos(angle) + cross(axis, *this) * sin(angle) + axis * dot(axis, *this) * (1 - cos(angle));
        }
        
        bool near_zero() const {
            const auto epsilon = 10e-8f;
            return (fabs(e[0]) < epsilon) && (fabs(e[1]) < epsilon) && (fabs(e[2]) < epsilon);
        }

        float length() const {
            return sqrt(length_squared());
        }

        float length_squared() const {
            return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
        }

        // inline static vec3 random() {
        //     return vec3(random_float(), random_float(), random_float());
        // }
        //
        // inline static vec3 random(float min, float max) {
        //     return vec3(random_float(min, max), random_float(min, max), random_float(min, max));
        // }

    public:
        float e[3];
};

// Type aliases for vec3
using point3 = vec3;   // 3D point
using color = vec3;    // RGB color

inline std::ostream& operator<<(std::ostream &out, const vec3 &v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator+(const vec3 &u, float c) {
    return vec3(u.e[0] + c, u.e[1] + c, u.e[2] + c);
}

inline vec3 operator-(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(float t, const vec3 &v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3 &v, float t) {
    return t * v;
}

inline vec3 operator/(vec3 v, float t) {
    return (1/t) * v;
}

inline float dot(const vec3 &u, const vec3 &v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

// vec3 random_in_unit_sphere() {
//     while (true) {
//         auto p = vec3::random(-1, 1);
//         if (p.length_squared() >= 1) continue;
//         return p;
//     }
// }
//
// vec3 random_unit_vector() {
//     return unit_vector(random_in_unit_sphere());
// }
//
// vec3 random_in_hemisphere(const vec3& normal) {
//     vec3 in_unit_sphere = random_in_unit_sphere();
//     if (dot(in_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
//         return in_unit_sphere;
//     else
//         return -in_unit_sphere;
// }
//
// vec3 reflect(const vec3& v, const vec3& n) {
//     return v - 2 * dot(v, n) * n;
// }
//
// vec3 refract(const vec3& uv, const vec3& n, float eta_over_etap) {
//     auto cos_theta = fmin(dot(-uv, n), 1.0);
//     vec3 r_out_perp = eta_over_etap * (uv + cos_theta * n);
//     vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
//     return r_out_parallel + r_out_perp;
// }

#endif

