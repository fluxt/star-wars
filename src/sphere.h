#pragma once

#include <cmath>
#include <numbers>

#include <glm/glm.hpp>

#include <object.h>

class Sphere : public Object {
    glm::vec3 origin;
    float radius;
    std::shared_ptr<Material> mat;

public:
    Sphere(const glm::vec3 &origin, float radius, std::shared_ptr<Material> mat) : origin(origin), radius(radius), mat(mat) {}

    ColorHit hit(const BVHHit &bvhhit, const Ray &r, float tmin, float tmax) const override {
        ColorHit ret;
        ret.point = r.at(bvhhit.t);
        glm::vec3 outward_normal = glm::normalize((ret.point - origin) / radius);
        ret.is_front = glm::dot(r.direction, outward_normal) < 0.0f;
        ret.normal = ret.is_front ? outward_normal : -outward_normal;
        ret.direction = random_hemisphere(ret.normal);
        ret.mat = mat;

        float theta = std::acos(-outward_normal.y);
        float phi = std::atan2(-outward_normal.z, outward_normal.x) + std::numbers::pi_v<float>;

        ret.u = phi / (2.0f * std::numbers::pi_v<float>);
        ret.v = theta / std::numbers::pi_v<float>;

        return ret;
    }

    BVHHit bvh_hit(const Ray &r, float tmin, float tmax) const override {
        glm::vec3 oc = origin - r.origin;
        float a = 1.0f;
        float h = glm::dot(r.direction, oc);
        float c = oc.x * oc.x + oc.y * oc.y + oc.z * oc.z - radius * radius;
        float discriminant = h * h - a * c;

        BVHHit ret;
        ret.is_hit = false;

        if (discriminant < 0.0f) {
            return ret;
        }

        float sqrtd = std::sqrt(discriminant);

        float t = (h - sqrtd) / a;
        if (t <= tmin || t > tmax) {
            t = (h + sqrtd) / a;
            if (t <= tmin || t > tmax) {
                return ret;
            }
        }

        ret.is_hit = true;
        ret.t = t;

        return ret;
    }

    AABB aabb() const override {
        glm::vec3 rvec(radius, radius, radius);
        return AABB(origin - rvec, origin + rvec);
    }
};