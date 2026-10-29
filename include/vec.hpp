// NOTE: this is straight claude stuff.

#pragma once

#include <cmath>
#include <stdexcept>
#include <string>
#include <sstream>
#include <algorithm>

// ============================================================
//  vec.hpp  —  2D, 3D, and 4D vector template classes
//  Supports: arithmetic operators, comparison, dot/cross product,
//            length, normalization, lerp, reflect, swizzle,
//            and more.
// ============================================================

// ────────────────────────────────────────────────────────────
//  Forward declarations
// ────────────────────────────────────────────────────────────
template<typename T> struct Vec2;
template<typename T> struct Vec3;
template<typename T> struct Vec4;

// ============================================================
//  Vec2<T>
// ============================================================
template<typename T>
struct Vec2 {
    T x, y;

    // ── Constructors ─────────────────────────────────────────
    constexpr Vec2()                  : x(T(0)), y(T(0)) {}
    constexpr Vec2(T x, T y)          : x(x),    y(y)    {}
    explicit constexpr Vec2(T scalar) : x(scalar),y(scalar){}

    template<typename U>
    explicit constexpr Vec2(const Vec2<U>& v) : x(T(v.x)), y(T(v.y)) {}

    // Promote from lower dimension
    explicit constexpr Vec2(const T* arr) : x(arr[0]), y(arr[1]) {}

    // ── Array access ─────────────────────────────────────────
    T&       operator[](int i)       { if(i==0) return x; if(i==1) return y; throw std::out_of_range("Vec2 index"); }
    const T& operator[](int i) const { if(i==0) return x; if(i==1) return y; throw std::out_of_range("Vec2 index"); }

    // ── Unary ─────────────────────────────────────────────────
    Vec2 operator-() const { return {-x, -y}; }
    Vec2 operator+() const { return *this; }

    // ── Arithmetic (vector op vector) ────────────────────────
    Vec2 operator+(const Vec2& r) const { return {x+r.x, y+r.y}; }
    Vec2 operator-(const Vec2& r) const { return {x-r.x, y-r.y}; }
    Vec2 operator*(const Vec2& r) const { return {x*r.x, y*r.y}; }
    Vec2 operator/(const Vec2& r) const { return {x/r.x, y/r.y}; }

    Vec2& operator+=(const Vec2& r) { x+=r.x; y+=r.y; return *this; }
    Vec2& operator-=(const Vec2& r) { x-=r.x; y-=r.y; return *this; }
    Vec2& operator*=(const Vec2& r) { x*=r.x; y*=r.y; return *this; }
    Vec2& operator/=(const Vec2& r) { x/=r.x; y/=r.y; return *this; }

    // ── Arithmetic (vector op scalar) ────────────────────────
    Vec2 operator+(T s) const { return {x+s, y+s}; }
    Vec2 operator-(T s) const { return {x-s, y-s}; }
    Vec2 operator*(T s) const { return {x*s, y*s}; }
    Vec2 operator/(T s) const { return {x/s, y/s}; }

    Vec2& operator+=(T s) { x+=s; y+=s; return *this; }
    Vec2& operator-=(T s) { x-=s; y-=s; return *this; }
    Vec2& operator*=(T s) { x*=s; y*=s; return *this; }
    Vec2& operator/=(T s) { x/=s; y/=s; return *this; }

    // ── Comparison ───────────────────────────────────────────
    bool operator==(const Vec2& r) const { return x==r.x && y==r.y; }
    bool operator!=(const Vec2& r) const { return !(*this==r); }

    // ── Math ─────────────────────────────────────────────────
    T dot(const Vec2& r)   const { return x*r.x + y*r.y; }
    T cross(const Vec2& r) const { return x*r.y - y*r.x; }  // scalar "z" component

    T lengthSq()  const { return dot(*this); }
    T length()    const { return std::sqrt(lengthSq()); }
    T magnitude() const { return length(); }

    Vec2 normalized() const {
        T len = length();
        if(len == T(0)) return Vec2{};
        return *this / len;
    }
    void normalize() { *this = normalized(); }

    T distance(const Vec2& r)   const { return (*this - r).length(); }
    T distanceSq(const Vec2& r) const { return (*this - r).lengthSq(); }

    Vec2 lerp(const Vec2& r, T t) const { return *this * (T(1)-t) + r * t; }

    Vec2 reflect(const Vec2& normal) const {
        return *this - normal * (T(2) * dot(normal));
    }

    Vec2 abs()  const { return { std::abs(x), std::abs(y) }; }
    Vec2 floor()const { return { std::floor(x), std::floor(y) }; }
    Vec2 ceil() const { return { std::ceil(x),  std::ceil(y) }; }

    T min() const { return std::min(x, y); }
    T max() const { return std::max(x, y); }
    T sum() const { return x + y; }

    // Angle (radians) this vector makes with +X axis
    T angle() const { return std::atan2(y, x); }

    // Angle between two vectors
    /*T angleBetween(const Vec2& r) const {
        T denom = length() * r.length();
        if(denom == T(0)) return T(0);
        return std::acos(std::clamp(dot(r)/denom, T(-1), T(1)));
        }*/

    // Rotate by angle (radians)
    Vec2 rotated(T radians) const {
        T c = std::cos(radians), s = std::sin(radians);
        return { x*c - y*s, x*s + y*c };
    }

    // ── Swizzle helpers ──────────────────────────────────────
    Vec2 yx() const { return {y, x}; }
    Vec2 xx() const { return {x, x}; }
    Vec2 yy() const { return {y, y}; }

    // ── String ───────────────────────────────────────────────
    std::string toString() const {
        std::ostringstream ss;
        ss << "Vec2(" << x << ", " << y << ")";
        return ss.str();
    }

    // ── Static constants ─────────────────────────────────────
    static Vec2 zero()  { return {T(0), T(0)}; }
    static Vec2 one()   { return {T(1), T(1)}; }
    static Vec2 unitX() { return {T(1), T(0)}; }
    static Vec2 unitY() { return {T(0), T(1)}; }
};

// Scalar-on-left arithmetic
template<typename T> Vec2<T> operator+(T s, const Vec2<T>& v) { return v + s; }
template<typename T> Vec2<T> operator-(T s, const Vec2<T>& v) { return Vec2<T>(s) - v; }
template<typename T> Vec2<T> operator*(T s, const Vec2<T>& v) { return v * s; }
template<typename T> Vec2<T> operator/(T s, const Vec2<T>& v) { return Vec2<T>(s) / v; }

// Stream output
template<typename T>
std::ostream& operator<<(std::ostream& os, const Vec2<T>& v) {
    return os << v.toString();
}

// Free-function math helpers
template<typename T> T   dot(const Vec2<T>& a, const Vec2<T>& b)            { return a.dot(b); }
template<typename T> T   cross(const Vec2<T>& a, const Vec2<T>& b)          { return a.cross(b); }
template<typename T> T   length(const Vec2<T>& v)                           { return v.length(); }
template<typename T> Vec2<T> normalize(const Vec2<T>& v)                    { return v.normalized(); }
template<typename T> Vec2<T> lerp(const Vec2<T>& a, const Vec2<T>& b, T t) { return a.lerp(b, t); }
template<typename T> Vec2<T> reflect(const Vec2<T>& v, const Vec2<T>& n)   { return v.reflect(n); }


// ============================================================
//  Vec3<T>
// ============================================================
template<typename T>
struct Vec3 {
    T x, y, z;

    // ── Constructors ─────────────────────────────────────────
    constexpr Vec3()                       : x(T(0)), y(T(0)), z(T(0)) {}
    constexpr Vec3(T x, T y, T z)         : x(x),    y(y),    z(z)    {}
    explicit constexpr Vec3(T scalar)      : x(scalar),y(scalar),z(scalar){}
    constexpr Vec3(const Vec2<T>& v, T z) : x(v.x),  y(v.y),  z(z)   {}

    template<typename U>
    explicit constexpr Vec3(const Vec3<U>& v) : x(T(v.x)), y(T(v.y)), z(T(v.z)) {}

    // ── Array access ─────────────────────────────────────────
    T&       operator[](int i)       { if(i==0) return x; if(i==1) return y; if(i==2) return z; throw std::out_of_range("Vec3 index"); }
    const T& operator[](int i) const { if(i==0) return x; if(i==1) return y; if(i==2) return z; throw std::out_of_range("Vec3 index"); }

    // ── Unary ─────────────────────────────────────────────────
    Vec3 operator-() const { return {-x, -y, -z}; }
    Vec3 operator+() const { return *this; }

    // ── Arithmetic (vector op vector) ────────────────────────
    Vec3 operator+(const Vec3& r) const { return {x+r.x, y+r.y, z+r.z}; }
    Vec3 operator-(const Vec3& r) const { return {x-r.x, y-r.y, z-r.z}; }
    Vec3 operator*(const Vec3& r) const { return {x*r.x, y*r.y, z*r.z}; }
    Vec3 operator/(const Vec3& r) const { return {x/r.x, y/r.y, z/r.z}; }

    Vec3& operator+=(const Vec3& r) { x+=r.x; y+=r.y; z+=r.z; return *this; }
    Vec3& operator-=(const Vec3& r) { x-=r.x; y-=r.y; z-=r.z; return *this; }
    Vec3& operator*=(const Vec3& r) { x*=r.x; y*=r.y; z*=r.z; return *this; }
    Vec3& operator/=(const Vec3& r) { x/=r.x; y/=r.y; z/=r.z; return *this; }

    // ── Arithmetic (vector op scalar) ────────────────────────
    Vec3 operator+(T s) const { return {x+s, y+s, z+s}; }
    Vec3 operator-(T s) const { return {x-s, y-s, z-s}; }
    Vec3 operator*(T s) const { return {x*s, y*s, z*s}; }
    Vec3 operator/(T s) const { return {x/s, y/s, z/s}; }

    Vec3& operator+=(T s) { x+=s; y+=s; z+=s; return *this; }
    Vec3& operator-=(T s) { x-=s; y-=s; z-=s; return *this; }
    Vec3& operator*=(T s) { x*=s; y*=s; z*=s; return *this; }
    Vec3& operator/=(T s) { x/=s; y/=s; y/=s; return *this; }

    // ── Comparison ───────────────────────────────────────────
    bool operator==(const Vec3& r) const { return x==r.x && y==r.y && z==r.z; }
    bool operator!=(const Vec3& r) const { return !(*this==r); }

    // ── Math ─────────────────────────────────────────────────
    T dot(const Vec3& r) const { return x*r.x + y*r.y + z*r.z; }

    Vec3 cross(const Vec3& r) const {
        return { y*r.z - z*r.y,
                 z*r.x - x*r.z,
                 x*r.y - y*r.x };
    }

    T lengthSq()  const { return dot(*this); }
    T length()    const { return std::sqrt(lengthSq()); }
    T magnitude() const { return length(); }

    Vec3 normalized() const {
        T len = length();
        if(len == T(0)) return Vec3{};
        return *this / len;
    }
    void normalize() { *this = normalized(); }

    T distance(const Vec3& r)   const { return (*this - r).length(); }
    T distanceSq(const Vec3& r) const { return (*this - r).lengthSq(); }

    Vec3 lerp(const Vec3& r, T t) const { return *this * (T(1)-t) + r * t; }

    Vec3 reflect(const Vec3& normal) const {
        return *this - normal * (T(2) * dot(normal));
    }

    // Refract: eta = ratio of refractive indices (incident / transmitted)
    Vec3 refract(const Vec3& normal, T eta) const {
        Vec3 n  = normalized();
        Vec3 nn = normal.normalized();
        T cosI  = -n.dot(nn);
        T sinT2 = eta * eta * (T(1) - cosI * cosI);
        if(sinT2 > T(1)) return Vec3{}; // total internal reflection
        return n * eta + nn * (eta * cosI - std::sqrt(T(1) - sinT2));
    }

    Vec3 abs()  const { return { std::abs(x), std::abs(y), std::abs(z) }; }
    Vec3 floor()const { return { std::floor(x), std::floor(y), std::floor(z) }; }
    Vec3 ceil() const { return { std::ceil(x),  std::ceil(y),  std::ceil(z)  }; }

    T min() const { return std::min({x, y, z}); }
    T max() const { return std::max({x, y, z}); }
    T sum() const { return x + y + z; }

    /*T angleBetween(const Vec3& r) const {
        T denom = length() * r.length();
        if(denom == T(0)) return T(0);
        return std::acos(std::clamp(dot(r)/denom, T(-1), T(1)));
        }*/

    // Spherical linear interpolation
    Vec3 slerp(const Vec3& r, T t) const {
        T theta = angleBetween(r);
        if(std::abs(theta) < T(1e-6)) return lerp(r, t);
        T sinT = std::sin(theta);
        return *this * (std::sin((T(1)-t)*theta)/sinT)
             + r     * (std::sin(t*theta)/sinT);
    }

    // Project this vector onto r
    Vec3 project(const Vec3& r) const {
        T d = r.dot(r);
        if(d == T(0)) return Vec3{};
        return r * (dot(r) / d);
    }

    // Rejection (component perpendicular to r)
    Vec3 reject(const Vec3& r) const { return *this - project(r); }

    // ── Swizzle helpers ──────────────────────────────────────
    Vec2<T> xy() const { return {x, y}; }
    Vec2<T> xz() const { return {x, z}; }
    Vec2<T> yz() const { return {y, z}; }
    Vec3    xyz()const { return *this; }
    Vec3    xzy()const { return {x, z, y}; }
    Vec3    yxz()const { return {y, x, z}; }
    Vec3    yzx()const { return {y, z, x}; }
    Vec3    zxy()const { return {z, x, y}; }
    Vec3    zyx()const { return {z, y, x}; }

    // ── String ───────────────────────────────────────────────
    std::string toString() const {
        std::ostringstream ss;
        ss << "Vec3(" << x << ", " << y << ", " << z << ")";
        return ss.str();
    }

    // ── Static constants ─────────────────────────────────────
    static Vec3 zero()  { return {T(0), T(0), T(0)}; }
    static Vec3 one()   { return {T(1), T(1), T(1)}; }
    static Vec3 unitX() { return {T(1), T(0), T(0)}; }
    static Vec3 unitY() { return {T(0), T(1), T(0)}; }
    static Vec3 unitZ() { return {T(0), T(0), T(1)}; }
    static Vec3 up()    { return unitY(); }
    static Vec3 right() { return unitX(); }
    static Vec3 forward(){ return {T(0), T(0), T(-1)}; } // right-hand convention
};

template<typename T> Vec3<T> operator+(T s, const Vec3<T>& v) { return v + s; }
template<typename T> Vec3<T> operator-(T s, const Vec3<T>& v) { return Vec3<T>(s) - v; }
template<typename T> Vec3<T> operator*(T s, const Vec3<T>& v) { return v * s; }
template<typename T> Vec3<T> operator/(T s, const Vec3<T>& v) { return Vec3<T>(s) / v; }

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vec3<T>& v) {
    return os << v.toString();
}

template<typename T> T      dot(const Vec3<T>& a, const Vec3<T>& b)            { return a.dot(b); }
template<typename T> Vec3<T> cross(const Vec3<T>& a, const Vec3<T>& b)         { return a.cross(b); }
template<typename T> T      length(const Vec3<T>& v)                            { return v.length(); }
template<typename T> Vec3<T> normalize(const Vec3<T>& v)                        { return v.normalized(); }
template<typename T> Vec3<T> lerp(const Vec3<T>& a, const Vec3<T>& b, T t)     { return a.lerp(b, t); }
template<typename T> Vec3<T> slerp(const Vec3<T>& a, const Vec3<T>& b, T t)    { return a.slerp(b, t); }
template<typename T> Vec3<T> reflect(const Vec3<T>& v, const Vec3<T>& n)        { return v.reflect(n); }
template<typename T> Vec3<T> refract(const Vec3<T>& v, const Vec3<T>& n, T eta){ return v.refract(n, eta); }
template<typename T> Vec3<T> project(const Vec3<T>& v, const Vec3<T>& onto)     { return v.project(onto); }
template<typename T> Vec3<T> reject(const Vec3<T>& v, const Vec3<T>& from)      { return v.reject(from); }


// ============================================================
//  Vec4<T>
// ============================================================
template<typename T>
struct Vec4 {
    T x, y, z, w;

    // ── Constructors ─────────────────────────────────────────
    constexpr Vec4()                              : x(T(0)), y(T(0)), z(T(0)), w(T(0)) {}
    constexpr Vec4(T x, T y, T z, T w)           : x(x),    y(y),    z(z),    w(w)    {}
    explicit constexpr Vec4(T scalar)             : x(scalar),y(scalar),z(scalar),w(scalar){}
    constexpr Vec4(const Vec3<T>& v, T w)         : x(v.x),  y(v.y),  z(v.z),  w(w)   {}
    constexpr Vec4(const Vec2<T>& v, T z, T w)   : x(v.x),  y(v.y),  z(z),    w(w)   {}

    template<typename U>
    explicit constexpr Vec4(const Vec4<U>& v) : x(T(v.x)), y(T(v.y)), z(T(v.z)), w(T(v.w)) {}

    // ── Array access ─────────────────────────────────────────
    T&       operator[](int i)       { if(i==0) return x; if(i==1) return y; if(i==2) return z; if(i==3) return w; throw std::out_of_range("Vec4 index"); }
    const T& operator[](int i) const { if(i==0) return x; if(i==1) return y; if(i==2) return z; if(i==3) return w; throw std::out_of_range("Vec4 index"); }

    // ── Unary ─────────────────────────────────────────────────
    Vec4 operator-() const { return {-x, -y, -z, -w}; }
    Vec4 operator+() const { return *this; }

    // ── Arithmetic (vector op vector) ────────────────────────
    Vec4 operator+(const Vec4& r) const { return {x+r.x, y+r.y, z+r.z, w+r.w}; }
    Vec4 operator-(const Vec4& r) const { return {x-r.x, y-r.y, z-r.z, w-r.w}; }
    Vec4 operator*(const Vec4& r) const { return {x*r.x, y*r.y, z*r.z, w*r.w}; }
    Vec4 operator/(const Vec4& r) const { return {x/r.x, y/r.y, z/r.z, w/r.w}; }

    Vec4& operator+=(const Vec4& r) { x+=r.x; y+=r.y; z+=r.z; w+=r.w; return *this; }
    Vec4& operator-=(const Vec4& r) { x-=r.x; y-=r.y; z-=r.z; w-=r.w; return *this; }
    Vec4& operator*=(const Vec4& r) { x*=r.x; y*=r.y; z*=r.z; w*=r.w; return *this; }
    Vec4& operator/=(const Vec4& r) { x/=r.x; y/=r.y; z/=r.z; w/=r.w; return *this; }

    // ── Arithmetic (vector op scalar) ────────────────────────
    Vec4 operator+(T s) const { return {x+s, y+s, z+s, w+s}; }
    Vec4 operator-(T s) const { return {x-s, y-s, z-s, w-s}; }
    Vec4 operator*(T s) const { return {x*s, y*s, z*s, w*s}; }
    Vec4 operator/(T s) const { return {x/s, y/s, z/s, w/s}; }

    Vec4& operator+=(T s) { x+=s; y+=s; z+=s; w+=s; return *this; }
    Vec4& operator-=(T s) { x-=s; y-=s; z-=s; w-=s; return *this; }
    Vec4& operator*=(T s) { x*=s; y*=s; z*=s; w*=s; return *this; }
    Vec4& operator/=(T s) { x/=s; y/=s; z/=s; w/=s; return *this; }

    // ── Comparison ───────────────────────────────────────────
    bool operator==(const Vec4& r) const { return x==r.x && y==r.y && z==r.z && w==r.w; }
    bool operator!=(const Vec4& r) const { return !(*this==r); }

    // ── Math ─────────────────────────────────────────────────
    T dot(const Vec4& r) const { return x*r.x + y*r.y + z*r.z + w*r.w; }

    T lengthSq()  const { return dot(*this); }
    T length()    const { return std::sqrt(lengthSq()); }
    T magnitude() const { return length(); }

    Vec4 normalized() const {
        T len = length();
        if(len == T(0)) return Vec4{};
        return *this / len;
    }
    void normalize() { *this = normalized(); }

    T distance(const Vec4& r)   const { return (*this - r).length(); }
    T distanceSq(const Vec4& r) const { return (*this - r).lengthSq(); }

    Vec4 lerp(const Vec4& r, T t) const { return *this * (T(1)-t) + r * t; }

    Vec4 reflect(const Vec4& normal) const {
        return *this - normal * (T(2) * dot(normal));
    }


    Vec4 abs()  const { return { std::abs(x), std::abs(y), std::abs(z), std::abs(w) }; }
    Vec4 floor()const { return { std::floor(x), std::floor(y), std::floor(z), std::floor(w) }; }
    Vec4 ceil() const { return { std::ceil(x),  std::ceil(y),  std::ceil(z),  std::ceil(w)  }; }

    T min() const { return std::min({x, y, z, w}); }
    T max() const { return std::max({x, y, z, w}); }
    T sum() const { return x + y + z + w; }

    /*T angleBetween(const Vec4& r) const {
        T denom = length() * r.length();
        if(denom == T(0)) return T(0);
        return std::acos(std::clamp(dot(r)/denom, T(-1), T(1)));
        }*/

    // Homogeneous perspective divide (for use as homogeneous 3D point)
    Vec3<T> perspectiveDivide() const {
        if(w == T(0)) return {x, y, z};
        return {x/w, y/w, z/w};
    }

    // Project this 4D vector onto r
    Vec4 project(const Vec4& r) const {
        T d = r.dot(r);
        if(d == T(0)) return Vec4{};
        return r * (dot(r) / d);
    }

    // ── Swizzle helpers ──────────────────────────────────────
    Vec2<T> xy()   const { return {x, y}; }
    Vec2<T> zw()   const { return {z, w}; }
    Vec3<T> xyz()  const { return {x, y, z}; }
    Vec3<T> xyw()  const { return {x, y, w}; }
    Vec4    xyzw() const { return *this; }
    Vec4    wzyx() const { return {w, z, y, x}; }

    // ── RGBA / XYZW aliases ──────────────────────────────────
    T& r() { return x; }  T r() const { return x; }
    T& g() { return y; }  T g() const { return y; }
    T& b() { return z; }  T b() const { return z; }
    T& a() { return w; }  T a() const { return w; }

    // ── String ───────────────────────────────────────────────
    std::string toString() const {
        std::ostringstream ss;
        ss << "Vec4(" << x << ", " << y << ", " << z << ", " << w << ")";
        return ss.str();
    }

    // ── Static constants ─────────────────────────────────────
    static Vec4 zero()  { return {T(0), T(0), T(0), T(0)}; }
    static Vec4 one()   { return {T(1), T(1), T(1), T(1)}; }
    static Vec4 unitX() { return {T(1), T(0), T(0), T(0)}; }
    static Vec4 unitY() { return {T(0), T(1), T(0), T(0)}; }
    static Vec4 unitZ() { return {T(0), T(0), T(1), T(0)}; }
    static Vec4 unitW() { return {T(0), T(0), T(0), T(1)}; }
};

template<typename T> Vec4<T> operator+(T s, const Vec4<T>& v) { return v + s; }
template<typename T> Vec4<T> operator-(T s, const Vec4<T>& v) { return Vec4<T>(s) - v; }
template<typename T> Vec4<T> operator*(T s, const Vec4<T>& v) { return v * s; }
template<typename T> Vec4<T> operator/(T s, const Vec4<T>& v) { return Vec4<T>(s) / v; }

template<typename T>
std::ostream& operator<<(std::ostream& os, const Vec4<T>& v) {
    return os << v.toString();
}

template<typename T> T      dot(const Vec4<T>& a, const Vec4<T>& b)            { return a.dot(b); }
template<typename T> T      length(const Vec4<T>& v)                            { return v.length(); }
template<typename T> Vec4<T> normalize(const Vec4<T>& v)                        { return v.normalized(); }
template<typename T> Vec4<T> lerp(const Vec4<T>& a, const Vec4<T>& b, T t)     { return a.lerp(b, t); }
template<typename T> Vec4<T> reflect(const Vec4<T>& v, const Vec4<T>& n)        { return v.reflect(n); }


// ============================================================
//  Convenient type aliases
// ============================================================
using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;
using Vec2i = Vec2<int>;

using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;
using Vec3i = Vec3<int>;

using Vec4f = Vec4<float>;
using Vec4d = Vec4<double>;
using Vec4i = Vec4<int>;

// GLSL-style aliases
using vec2  = Vec2f;
using vec3  = Vec3f;
using vec4  = Vec4f;
using dvec2 = Vec2d;
using dvec3 = Vec3d;
using dvec4 = Vec4d;
using ivec2 = Vec2i;
using ivec3 = Vec3i;
using ivec4 = Vec4i;
