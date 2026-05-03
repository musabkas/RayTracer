#include <iostream>
#include <iomanip>
#include <chrono>
#include <atomic> // Included for thread-safe progress tracking
#include <omp.h>  // Included for OpenMP

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
  
  // Create a thread-safe counter to track how many rows are done
  std::atomic<int> completed_rows(0);

  // Tell OpenMP to parallelize this loop. dynamic scheduling is best for 
  // raytracing since some parts of the image (like empty sky) render faster than others.
  #pragma omp parallel for schedule(dynamic)
  for (int x = 0; x < viewplane.hres; x++) {   // across (rows).
    
    // CRITICAL FIX: The rays vector MUST be inside the loop so each thread has its own copy!
    std::vector<Ray> rays; 

    for (int y = 0; y < viewplane.vres; y++) { // down (columns).
      RGBColor pixel_color(0);
      rays = sampler->get_rays(x, y);
      
      for (const auto &ray : rays) {
        float weight = ray.w; 
        RGBColor shade = world.tracer_ptr->trace_ray(ray, world);
        pixel_color += weight * shade;
      }
      // image.set_pixel is thread-safe as long as no two threads write to the exact same (x, y)
      image.set_pixel(x, y, pixel_color);
    }
    
    // --- MULTITHREADED PROGRESS BAR CODE START ---
    int current_progress = ++completed_rows; // Atomically increment the total rows finished
    
    // The critical block ensures only ONE thread executes this printing code at a time.
    // Without this, your console output will turn into garbled text.
    #pragma omp critical
    {
        float progress = (float)current_progress / viewplane.hres;
        int barWidth = 40;
        
        auto current_time = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(current_time - start_time).count();
        
        double eta_seconds = 0.0;
        if (progress > 0.0 && progress < 1.0) {
          eta_seconds = (elapsed / progress) * (1.0 - progress);
        }
        
        int eta_minutes = (int)(eta_seconds / 60.0);
        int eta_secs = (int)(eta_seconds) % 60;
        
        std::cout << "\r[";
        int pos = barWidth * progress;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] Row " << current_progress << "/" << viewplane.hres 
                  << " (" << int(progress * 100.0) << "%) "
                  << "ETA: " << std::setfill('0') << std::setw(2) << eta_minutes 
                  << ":" << std::setw(2) << eta_secs << "    " << std::flush;
    }
    // --- PROGRESS BAR CODE END ---
  }
  
  std::cout << "\n";
  
  // Write image to file.
  image.write_png("scene.png");

  std::cout << "Wrote image.\n";
  
  return 0;
}