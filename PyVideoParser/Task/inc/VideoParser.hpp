#pragma once
#ifndef VideoParser_hpp
#define VideoParser_hpp
#include <map>
#include <string>

#include "CodecSupport.hpp"
#include "Core.hpp"

namespace vp {

class VideoParser final : public Task {
 public:
  VideoParser() = delete;
  VideoParser(const VideoParser &other) = delete;
  VideoParser &operator=(const VideoParser &other) = delete;
  void GetParams(struct MuxingParams &params);
  TaskExecStatus Execute() final;
  ~VideoParser() final;
  static VideoParser *Make(const std::string &url, const std::map<std::string, std::string> &ffmpeg_options);

 private:
  VideoParser(const std::string &url, const std::map<std::string, std::string> &ffmpeg_options);
  static const uint32_t numInputs = 2U;
  static const uint32_t numOutputs = 4U;
  struct VideoParser_Impl *pImpl = nullptr;
};

}  // namespace vp
#endif