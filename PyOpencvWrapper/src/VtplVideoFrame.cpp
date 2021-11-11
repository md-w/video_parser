#include "VtplVideoFrame.h"
#include <iostream>

bool VtplVideoFrame::updateWidthHeight(int32_t w, int32_t h)
{
  if ((w * h * 4) == (width * height * 4)) {
    if (rgb_buffer != nullptr)
      return true;
  }
  if (rgb_buffer) {
    std::free(rgb_buffer);
  }
  int pitch = ((w * 4) / 64 + 1) * 64;
  rgb_buffer = static_cast<uint8_t*>(std::malloc(pitch * h));
  width = w;
  height = h;
  for (size_t i = 0; i < width; i++)
  {
    rgb_buffer[i] = i;
  }

  return (rgb_buffer != nullptr);
}

uint8_t* VtplVideoFrame::data() { return rgb_buffer; }
int VtplVideoFrame::rows() { return height; }
int VtplVideoFrame::cols() { return width; }

VtplVideoFrame::VtplVideoFrame()
    : channel_id(0), app_id(0), frame_id(0), time_stamp(0), fps(0.0), width(1920), height(1080), is_first_frame(false),
      is_end_of_stream(false), rgb_buffer(nullptr)
{
  updateWidthHeight(width, height);
  std::cout << "VtplVideoFrame ctor\n";
}
VtplVideoFrame::~VtplVideoFrame()
{
  if (rgb_buffer) {
    std::free(rgb_buffer);
    rgb_buffer = nullptr;
    width = 0;
    height = 0;
  }
  std::cout << "VtplVideoFrame dtor\n";
}
