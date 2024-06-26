#pragma once

#include <cmath>
#include <cassert>

#include <glm/glm.hpp>

#include <random.h>
#include <object.h>
#include <material.h>
#include <ray.h>


class PerspectiveCamera {
    int32_t height, width, samples, max_depth;
    float focal_distance, defocus_angle;
    glm::vec3 center, pixel00, du, dv, disk_u, disk_v;

public:
    PerspectiveCamera(
        glm::vec3 &center,
        glm::vec3& direction,
        glm::vec3& up,
        int32_t height,
        int32_t width,
        float fov,
        float focal_distance,
        float defocus_angle,
        int32_t samples,
        int32_t max_depth
    ) : center(center), height(height), width(width), focal_distance(focal_distance), samples(samples), max_depth(max_depth), defocus_angle(defocus_angle) {
        float widthf = static_cast<float>(width);
        float heightf = static_cast<float>(height);

        float magnitude = 2.0f * focal_distance * std::tan(fov / 2.0f) / widthf;
        du = glm::normalize(glm::cross(direction, up)) * magnitude;
        dv = glm::normalize(glm::cross(direction, du)) * magnitude;

        float disk_radius = focal_distance * std::tan(defocus_angle / 2.0f);
        disk_u = glm::normalize(glm::cross(direction, up)) * disk_radius;
        disk_v = glm::normalize(glm::cross(direction, du)) * disk_radius;

        pixel00 = center + focal_distance * glm::normalize(direction)
                    - du * (widthf / 2.0f)
                    - dv * (heightf / 2.0f);
    }

    void render(std::vector<uint8_t> &image, const ObjectList& world) {
        for (int32_t h = 0; h < height; ++h) {
            for (int32_t w = 0; w < width; ++w) {
                std::clog << "\rPixels remaining: " << h * width + w << " out of " << height * width << std::flush;

                glm::vec3 pixel(0.0, 0.0, 0.0);

                // if (h >= 20 && h < 50 && w >= 200 && w < 210) {
                // if (h == 25 && w == 200) {
                if (true) {
                    for (int32_t s = 0; s < samples; ++s) {
                        Ray r = this->get_ray(h, w);
                        glm::vec3 sampled = get_color(r, world, 50);
                        pixel += sampled;
                    }

                    pixel /= samples;
                } else {
                    pixel.x = 0.0f;
                    pixel.y = 1.0f;
                    pixel.z = 0.0f;
                }

                // linear to gamma
                pixel.x = pixel.x > 0.0f ? std::sqrt(pixel.x) : 0.0f;
                pixel.y = pixel.y > 0.0f ? std::sqrt(pixel.y) : 0.0f;
                pixel.z = pixel.z > 0.0f ? std::sqrt(pixel.z) : 0.0f;

                // clamp
                pixel.x = pixel.x < 0.0f ? 0.0f : pixel.x;
                pixel.x = pixel.x > 1.0f ? 1.0f : pixel.x;
                pixel.y = pixel.y < 0.0f ? 0.0f : pixel.y;
                pixel.y = pixel.y > 1.0f ? 1.0f : pixel.y;
                pixel.z = pixel.z < 0.0f ? 0.0f : pixel.z;
                pixel.z = pixel.z > 1.0f ? 1.0f : pixel.z;

                uint8_t ir = static_cast<uint8_t>(255.999 * pixel.x);
                uint8_t ig = static_cast<uint8_t>(255.999 * pixel.y);
                uint8_t ib = static_cast<uint8_t>(255.999 * pixel.z);

                image[h * width * 4 + w * 4 + 0] = ir;
                image[h * width * 4 + w * 4 + 1] = ig;
                image[h * width * 4 + w * 4 + 2] = ib;
                image[h * width * 4 + w * 4 + 3] = 255;
            }
        }

        std::cout << std::endl;
    }

    Ray get_ray(int32_t h, int32_t w) {
        // float random_h = std::rand() / (RAND_MAX + 1.0f) - 0.5f;
        // float random_w = std::rand() / (RAND_MAX + 1.0f) - 0.5f;
        float random_h = static_cast<float>(h) + random_float();
        float random_w = static_cast<float>(w) + random_float();
        glm::vec3 origin;

        if (defocus_angle <= 0) {
            origin = center;
        } else {
            glm::vec2 p = random_disk();
            origin = center + p.x * disk_u + p.y * disk_v;
        }

        glm::vec3 direction = glm::normalize(
            pixel00 + dv * random_h + du * random_w - origin
        );
        return Ray(origin, direction);
    }

    glm::vec3 get_color(const Ray &r, const ObjectList &world, int32_t depth) const {
        if (depth <= 0) {
            return glm::vec3(0.0, 0.0, 0.0);
        }

        Hit hit = world.hit(r, 0.001f, 1000.0f);

        if (hit.is_hit) {
            const auto& [is_scatter, attenuation, scattered] = hit.mat->scatter(r, hit);

            if (is_scatter) {
                return attenuation * get_color(scattered, world, depth-1);
            }
            return glm::vec3(0.0, 0.0, 0.0);
        }

        // background
        float alpha = 0.5f * (r.direction().y + 1.0f);
        return (1.0f - alpha) * glm::vec3(1.0, 1.0, 1.0) + alpha * glm::vec3(0.5, 0.7, 1.0);
    }
};
