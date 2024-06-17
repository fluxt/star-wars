#include <iostream>
#include <memory>
#include <cmath>

#include <lodepng.h>
#include <glm/glm.hpp>

#include <camera.h>
#include <object.h>
#include <sphere.h>
#include <material.h>
#include <ray.h>

int main(int argc, char *argv[]) {
    std::string filename = "output.png";

    // image
    int32_t width = 400;
    int32_t height = 225;
    std::vector<uint8_t> image(height * width * 4); // rgba

    // camera
    glm::vec3 center(0.0, 0.0, 0.0);
    glm::vec3 direction(0.0, 0.0, -1.0);
    glm::vec3 up(0.0, 1.0, 0.0);
    float fov = 121.28f / 360.0f * 2.0f * M_PI;
    int32_t samples = 100;
    int32_t max_depth = 50;
    PerspectiveCamera perspectiveCamera(center, direction, up, height, width, fov, samples, max_depth);

    // materials
    std::shared_ptr<Material> material_ground = std::make_shared<Lambertian>(glm::vec3(0.8, 0.8, 0.0));
    std::shared_ptr<Material> material_center = std::make_shared<Lambertian>(glm::vec3(0.1, 0.2, 0.5));
    std::shared_ptr<Material> material_left   = std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.8));
    std::shared_ptr<Material> material_right  = std::make_shared<Metal>(glm::vec3(0.8, 0.6, 0.2));

    // objects
    ObjectList world;
    world.add(std::make_shared<Sphere>(Sphere(glm::vec3( 0.0, -100.5, -1.0), 100.0, material_ground)));
    world.add(std::make_shared<Sphere>(Sphere(glm::vec3( 0.0,    0.0, -1.2),   0.5, material_center)));
    world.add(std::make_shared<Sphere>(Sphere(glm::vec3(-1.0,    0.0, -1.0),   0.5, material_left)));
    world.add(std::make_shared<Sphere>(Sphere(glm::vec3( 1.0,    0.0, -1.0),   0.5, material_right)));

    // render
    perspectiveCamera.render(image, world);

    unsigned int error = lodepng::encode(filename, image, width, height);
    if (error) {
        std::cout << "encoder error " << error << ": "<< lodepng_error_text(error) << std::endl;
    }
}
