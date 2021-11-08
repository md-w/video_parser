#pragma once
#ifndef VtplVideoStream_h
#define VtplVideoStream_h
#include <string>
class VtplVideoStream
{
private:
  std::string _stream_url;
  void _get_stream_url(std::string& path);

public:
  VtplVideoStream(std::string path, uint16_t channel_id, uint32_t app_id, float suggested_fps = 20.0,
                  bool is_blocking = false, int gpu_id = 0, int seek_time = 100);
  ~VtplVideoStream();
};
#endif