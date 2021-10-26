#include "UltraFace.h"
int main(int argc, char const* argv[])
{
  std::cout << "UltraFace" << std::endl;
  std::string imageFilepath = "1.jpg";
  cv::Mat image_bgr = cv::imread(imageFilepath, cv::ImreadModes::IMREAD_COLOR);
  UltraFace ultraface;
  int numTests{1000};
  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();
  for (int i = 0; i < numTests; i++) {
    ultraface.getResult(image_bgr);
  }
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
  std::cout << "Inference FPS: "
            << (static_cast<float>(numTests) * 1000.0) /
                   std::chrono::duration_cast<std::chrono::milliseconds>(end -
                                                                         begin)
                       .count()
            << " FPS" << std::endl;
  return 0;
}
