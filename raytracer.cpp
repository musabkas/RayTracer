#include <iostream>
#include <iomanip>
#include <chrono>

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

  auto start_time = std::chrono::high_resolution_clock::now();
  
  std::vector<Ray> rays;
  for (int x = 0; x < viewplane.hres; x++) {   // across (rows).
    for (int y = 0; y < viewplane.vres; y++) { // down (columns).
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
    }
    
    // --- PROGRESS BAR CODE START (Row-by-row) ---
    float progress = (float)(x + 1) / viewplane.hres;
    int barWidth = 40;
    
    // Elapsed time
    auto current_time = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
    
    // Estimate remaining time
    double eta_seconds = 0.0;
    if (progress > 0.0 && progress < 1.0) {
      eta_seconds = (elapsed / progress) * (1.0 - progress);
    }
    
    int eta_minutes = (int)(eta_seconds / 60.0);
    int eta_secs = (int)(eta_seconds) % 60;
    
    // Progress bar
    std::cout << "\r[";
    int pos = barWidth * progress;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] Row " << (x + 1) << "/" << viewplane.hres 
              << " (" << int(progress * 100.0) << "%) "
              << "ETA: " << std::setfill('0') << std::setw(2) << eta_minutes 
              << ":" << std::setw(2) << eta_secs << "    " << std::flush;
    // --- PROGRESS BAR CODE END ---
  }
  
  std::cout << "\n";
  
  // Write image to file.
  image.write_png("scene.png");

  std::cout << "Wrote image.\n";
  
  return 0;
}
