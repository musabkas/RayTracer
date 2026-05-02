#include <iostream>
#include <iomanip>

#include "materials/Cosine.hpp"

#include "samplers/Sampler.hpp"

#include "image/Image.hpp"
#include "utilities/RGBColor.hpp"
#include "utilities/Ray.hpp"
#include "utilities/ShadeInfo.hpp"

#include "world/World.hpp"
#include "world/ViewPlane.hpp"
#include "tracers/Tracer.hpp"

int main(int argc, char **argv) {
  World world;
  world.build();

  Sampler *sampler = world.sampler_ptr;
  ViewPlane &viewplane = world.vplane;
  Image image(viewplane);

  std::vector<Ray> rays;
  for (int x = 0; x < viewplane.hres; x++) {   // across.
    for (int y = 0; y < viewplane.vres; y++) { // down.
      // Get rays for the pixel from the sampler. The pixel color is the
      // weighted sum of the shades for each ray.
      RGBColor pixel_color(0);
      rays = sampler->get_rays(x, y);
      for (const auto &ray : rays) {
        float weight = ray.w; // ray weight for the pixel.
        RGBColor shade = world.tracer_ptr->trace_ray(ray, world);
        pixel_color += weight * shade;
      }
      // Save color to image.
      image.set_pixel(x, y, pixel_color);
      // std::cout << pixel_color << "\n";
      // --- PROGRESS BAR CODE START ---
        float progress = (float)(x + 1) / viewplane.hres;
        int barWidth = 40;

        std::cout << "\r["; // \r returns cursor to start of line
        int pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << int(progress * 100.0) << " %" << std::flush;
        // --- PROGRESS BAR CODE END ---
    }
  }
  // Write image to file.
  image.write_png("scene.png");

  std::cout << "Wrote image.\n";
  
  return 0;
}
