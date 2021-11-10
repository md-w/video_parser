#pragma once
#ifndef VtplVideoFrame_h
#define VtplVideoFrame_h

#include <cstdio>
#include <cstdlib>
#include <stdint.h>

class VtplVideoFrame
{
public:
  int16_t channel_id;
  uint16_t app_id;
  uint64_t frame_id;
  uint64_t time_stamp;
  float fps;
  int32_t width;
  int32_t height;
  bool is_first_frame;
  bool is_end_of_stream;
  uint8_t* rgb_buffer;
  bool updateWidthHeight(int32_t w, int32_t h)
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
    return (rgb_buffer != nullptr);
  }

  VtplVideoFrame();
  ~VtplVideoFrame();
};

#endif