#pragma once
 
#include <cmath>
#include <cassert>
#include <string>
#include <algorithm>
#include "vec.hpp"
 
// ---------------------------------------------------------------------------
// Quaternion  –  q = w + xi + yj + zk
//
//  w  : scalar (real) part
//  x,y,z : vector (imaginary) part
//
//  Conventions
//  -----------
//  - Unit quaternions represent rotations.
//  - Multiplication is RIGHT-to-LEFT composition (same as matrices):
//      (p * q).rotate(v)  ==  p.rotate( q.rotate(v) )
//  - All rotation helpers expect / return RADIANS.
// ---------------------------------------------------------------------------
  
class Quaternion
{
public:
    float w, x, y, z;
 
    // -----------------------------------------------------------------------
    // Constructors
    // -----------------------------------------------------------------------
 
    // Identity  (no rotation)
    Quaternion() : w(1.f), x(0.f), y(0.f), z(0.f) {}
 
    // Raw components  (w first – "scalar, vector" order)
    Quaternion(float w, float x, float y, float z)
        : w(w), x(x), y(y), z(z) {}
 
    // -----------------------------------------------------------------------
    // Factory helpers
    // -----------------------------------------------------------------------
 
    // Build from an axis + angle in radians.
    // The axis does NOT need to be pre-normalised.
    static Quaternion fromAxisAngle(Vec3f axis, float angleRad)
    {
        float len = std::sqrt(axis.x*axis.x + axis.y*axis.y + axis.z*axis.z);
        assert(len > 1e-10f && "axis must be non-zero");
        float inv = 1.f / len;
        float s   = std::sin(angleRad * 0.5f);
        return Quaternion(
            std::cos(angleRad * 0.5f),
            axis.x * inv * s,
            axis.y * inv * s,
            axis.z * inv * s
        );
    }
 
    // Build from Euler angles (radians).
    // Rotation order: Z (yaw) → Y (pitch) → X (roll)  (extrinsic XYZ)
    static Quaternion fromEuler(float rollX, float pitchY, float yawZ)
    {
        float cr = std::cos(rollX  * 0.5f),  sr = std::sin(rollX  * 0.5f);
        float cp = std::cos(pitchY * 0.5f),  sp = std::sin(pitchY * 0.5f);
        float cy = std::cos(yawZ   * 0.5f),  sy = std::sin(yawZ   * 0.5f);
 
        return Quaternion(
            cr*cp*cy + sr*sp*sy,
            sr*cp*cy - cr*sp*sy,
            cr*sp*cy + sr*cp*sy,
            cr*cp*sy - sr*sp*cy
        );
    }
 
    // -----------------------------------------------------------------------
    // Basic properties
    // -----------------------------------------------------------------------
 
    float normSq() const { return w*w + x*x + y*y + z*z; }
    float norm()   const { return std::sqrt(normSq()); }
    bool  isUnit(float eps = 1e-5f) const { return std::abs(normSq() - 1.f) < eps; }
 
    // -----------------------------------------------------------------------
    // Arithmetic operators
    // -----------------------------------------------------------------------
 
    Quaternion operator+(const Quaternion& o) const { return {w+o.w, x+o.x, y+o.y, z+o.z}; }
    Quaternion operator-(const Quaternion& o) const { return {w-o.w, x-o.x, y-o.y, z-o.z}; }
    Quaternion operator-()                    const { return {-w, -x, -y, -z}; }
 
    // Hamilton product  (rotation composition: this THEN o applied first)
    Quaternion operator*(const Quaternion& o) const
    {
        return Quaternion(
            w*o.w - x*o.x - y*o.y - z*o.z,
            w*o.x + x*o.w + y*o.z - z*o.y,
            w*o.y - x*o.z + y*o.w + z*o.x,
            w*o.z + x*o.y - y*o.x + z*o.w
        );
    }
 
    Quaternion operator*(float s) const { return {w*s, x*s, y*s, z*s}; }
    Quaternion operator/(float s) const { float inv=1.f/s; return {w*inv, x*inv, y*inv, z*inv}; }
 
    friend Quaternion operator*(float s, const Quaternion& q) { return q * s; }
 
    Quaternion& operator+=(const Quaternion& o) { *this = *this + o; return *this; }
    Quaternion& operator-=(const Quaternion& o) { *this = *this - o; return *this; }
    Quaternion& operator*=(const Quaternion& o) { *this = *this * o; return *this; }
    Quaternion& operator*=(float s)             { *this = *this * s; return *this; }
    Quaternion& operator/=(float s)             { *this = *this / s; return *this; }
 
    bool operator==(const Quaternion& o) const { return w==o.w && x==o.x && y==o.y && z==o.z; }
    bool operator!=(const Quaternion& o) const { return !(*this == o); }
 
    // -----------------------------------------------------------------------
    // Quaternion operations
    // -----------------------------------------------------------------------
 
    float dot(const Quaternion& o) const { return w*o.w + x*o.x + y*o.y + z*o.z; }
 
    // Conjugate  q* = (w, -x, -y, -z)
    Quaternion conjugate() const { return {w, -x, -y, -z}; }
 
    // Inverse  q^-1 = q* / |q|^2  (for unit quaternions this is just conjugate)
    Quaternion inverse() const { return conjugate() / normSq(); }
 
    // Normalise to unit length
    Quaternion normalised() const
    {
        float n = norm();
        assert(n > 1e-10f && "cannot normalise zero quaternion");
        return *this / n;
    }
 
    void normalise() { *this = normalised(); }
 
    // -----------------------------------------------------------------------
    // Rotation
    // -----------------------------------------------------------------------
 
    // Rotate a 3-D point/vector by this quaternion.
    // q must be a unit quaternion (asserted in debug builds).
    Vec3f rotate(Vec3f v) const
    {
        assert(isUnit() && "quaternion must be normalised before rotating");
        // Optimised sandwich product: q * (0,v) * q*
        // Using the formula: v' = v + 2w(q_vec × v) + 2(q_vec × (q_vec × v))
        float tx = 2.f * (y*v.z - z*v.y);
        float ty = 2.f * (z*v.x - x*v.z);
        float tz = 2.f * (x*v.y - y*v.x);
        return Vec3f{
            v.x + w*tx + y*tz - z*ty,
            v.y + w*ty + z*tx - x*tz,
            v.z + w*tz + x*ty - y*tx
        };
    }
 
    // Inverse-rotate (rotate by conjugate)
    Vec3f rotateInverse(Vec3f v) const { return conjugate().rotate(v); }
 
    // -----------------------------------------------------------------------
    // Interpolation
    // -----------------------------------------------------------------------
 
    // Linear interpolation (not length-preserving; fast, ok for small angles)
    static Quaternion lerp(const Quaternion& a, const Quaternion& b, float t)
    {
        return (a * (1.f - t) + b * t).normalised();
    }
 
    // Spherical linear interpolation – smooth rotation between two orientations
    static Quaternion slerp(Quaternion a, Quaternion b, float t)
    {
        float cosTheta = a.dot(b);
 
        // Ensure we take the shortest arc
        if (cosTheta < 0.f) { b = -b; cosTheta = -cosTheta; }
 
        // Fall back to lerp when the quaternions are very close (avoids divide-by-zero)
        if (cosTheta > 0.9995f) return lerp(a, b, t);
 
        float theta    = std::acos(cosTheta);
        float sinTheta = std::sin(theta);
        float wa       = std::sin((1.f - t) * theta) / sinTheta;
        float wb       = std::sin(       t  * theta) / sinTheta;
        return (a * wa + b * wb).normalised();
    }
 
    // -----------------------------------------------------------------------
    // Conversion
    // -----------------------------------------------------------------------
 
    // Decompose back to axis + angle (radians).
    // Returns identity axis (0,0,1) and angle 0 for the identity quaternion.
    void toAxisAngle(Vec3f& outAxis, float& outAngleRad) const
    {
        Quaternion q = isUnit() ? *this : normalised();
        outAngleRad  = 2.f * std::acos(std::clamp(q.w, -1.f, 1.f));
        float s      = std::sqrt(std::max(0.f, 1.f - q.w*q.w));
        if (s < 1e-10f)
            outAxis = {0.f, 0.f, 1.f};   // angle ≈ 0, axis is arbitrary
        else
            outAxis = {q.x/s, q.y/s, q.z/s};
    }
 
    // Convert to Euler angles (radians): {rollX, pitchY, yawZ}
    // Same convention as fromEuler() above.
    Vec3f toEuler() const
    {
        // Roll (x-axis)
        float sinr_cosp =  2.f * (w*x + y*z);
        float cosr_cosp =  1.f - 2.f * (x*x + y*y);
        float roll  = std::atan2(sinr_cosp, cosr_cosp);
 
        // Pitch (y-axis) – clamped to avoid NaN at poles
        float sinp  = 2.f * (w*y - z*x);
        float pitch = std::abs(sinp) >= 1.f
                    ? std::copysign(3.14159265f * 0.5f, sinp)
                    : std::asin(sinp);
 
        // Yaw (z-axis)
        float siny_cosp =  2.f * (w*z + x*y);
        float cosy_cosp =  1.f - 2.f * (y*y + z*z);
        float yaw   = std::atan2(siny_cosp, cosy_cosp);
 
        return {roll, pitch, yaw};
    }
 
    // Row-major 3×3 rotation matrix (stored in a flat 9-element array)
    void toMatrix3x3(float out[9]) const
    {
        float xx=x*x, yy=y*y, zz=z*z;
        float xy=x*y, xz=x*z, yz=y*z;
        float wx=w*x, wy=w*y, wz=w*z;
 
        out[0]=1-2*(yy+zz); out[1]=  2*(xy-wz); out[2]=  2*(xz+wy);
        out[3]=  2*(xy+wz); out[4]=1-2*(xx+zz); out[5]=  2*(yz-wx);
        out[6]=  2*(xz-wy); out[7]=  2*(yz+wx); out[8]=1-2*(xx+yy);
    }
 
    // -----------------------------------------------------------------------
    // Utilities
    // -----------------------------------------------------------------------
 
    std::string toString() const
    {
        return "Quaternion(" +
               std::to_string(w) + ", " +
               std::to_string(x) + "i, " +
               std::to_string(y) + "j, " +
               std::to_string(z) + "k)";
    }
 
    // Static identity constant
    static Quaternion identity() { return Quaternion(); }
};