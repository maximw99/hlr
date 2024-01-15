#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <immintrin.h>
#include <iostream>
#include <omp.h>

#include "complex.h"
#include "lodepng.h"
#include "Stopwatch.h"

int mandelbrot(const complex &c, const float max_dist, const int max_iter) {
  int n = 0;
  complex zn(0., 0.);
  float dist = 0.;
  while (n < max_iter && dist <= max_dist) {
    n = n + 1;
    zn = zn * zn + c;
    dist = zn.abs();
  }
  return n;
}

int main(int argc, char **argv) {
  // const int width = 4048, height = 4048;
  const int width = 8000, height = 8000; // changed size for better results
  const float sx = 2.f / width;
  const float sy = 2.f / height;
  float m = 1.;
  float x0 = -.5, y0 = .0;
  const float max_dist = 2.;
  const int max_iter = 300;
  const int max_color = 255;

  Stopwatch stopwatch_calc;
  
  unsigned char *image = NULL;
  image = (unsigned char *)malloc(width * height * 4);

  if (argc > 1) {
    m = std::strtof(argv[1], 0);
  }
  if (argc > 2) {
    x0 = std::strtof(argv[2], 0);
  }
  if (argc > 3) {
    y0 = std::strtof(argv[3], 0);
  }

  // precalculate constants out of the loop
  const float cx = sx / m;
  const float cy = sy / m;

  const float color_scale = ((float)max_color / (float)max_iter);

  const int thread_val = 150;
  
  stopwatch_calc.Start();

  // iterate over image pixels and calculate their value
  // #pragma omp parallel for schedule(static, 8)
  // #pragma omp parallel for schedule(dynamic, 8)
  #pragma omp parallel for
  for (int x = 0; x < width; x++) {
    for (int y = 0; y < height; y++) {
      complex c(x0 + cx * (x - width / 2), y0 + cy * (y - height / 2));

      // scale to color pallet (value between 0 and 255 and invert)
      int iter = mandelbrot(c, max_dist, max_iter);

      // get threak number
      int thread_id = omp_get_thread_num();

      int color_val = max_color - nearbyint(((float)iter * color_scale));
      // optinal integer scaling
      // int color_val = max_color - (mandelbrot(c, max_dist, max_iter) * max_color / max_iter);

      image[4 * width * y + 4 * x + 0] = color_val; // R
      image[4 * width * y + 4 * x + 1] = color_val; // G
      image[4 * width * y + 4 * x + 2] = thread_val - thread_id * 30; // B value by thread num
      image[4 * width * y + 4 * x + 3] = 255;       // Alpha
    }
  }

  stopwatch_calc.Stop();

  /*Encode the image*/
  unsigned error =
      lodepng_encode32_file("mandelbrot.png", image, width, height);
  if (error) {
    std::cout << "error " << error << " : " << lodepng_error_text(error)
              << std::endl;
  }

  std::cout << "The duration to calculate mandelbrot was: ";
  stopwatch_calc.Print("m");
  
  /*cleanup*/
  free(image);

  return 0;
}
