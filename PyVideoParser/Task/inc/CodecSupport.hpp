#pragma once
#ifndef CodecSupport_hpp
#define CodecSupport_hpp
#include <cstdint>
#include <string>

namespace vp {
struct PacketData {
  int64_t pts;
  int64_t dts;
  uint64_t poc;
  uint64_t pos;
  uint64_t duration;
};

struct VideoContext {
  uint32_t width;
  uint32_t height;
  uint32_t gop_size;
  uint32_t num_frames;
  uint32_t is_vfr;
  double frameRate;
  double avgFrameRate;
  double timeBase;
  uint32_t streamIndex;
  std::string codec;
  // cudaVideoCodec codec;
  // Pixel_Format format;
  // ColorSpace color_space;
  // ColorRange color_range;
};

struct AudioContext {
  // Reserved for future use;
};

struct MuxingParams {
  VideoContext videoContext;
  AudioContext audioContext;
};
}  // namespace vp
#endif