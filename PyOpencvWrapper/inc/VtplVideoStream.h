#pragma once
#ifndef VtplVideoStream_h
#define VtplVideoStream_h
#include "IVtplStreamFrameSrc.h"
#include "VtplVideoFrame.h"
#include "zero_copy_queue.h"
#include <atomic>
#include <memory>
#include <string>
#include <thread>
#include <vector>
class VtplVideoStream
{
private:
  std::string _stream_type;
  std::string _stream_url;

  uint16_t _channel_id;
  uint16_t _app_id;

  bool _reconnect_requested;
  bool _is_already_shutting_down = false;
  std::atomic_bool _is_shutdown = false;
  bool _is_eos = false;
  int _reconnect_retry_time = 1;
  float _suggested_fps = 25.0;
  int _consecutive_grabbing_error = 0;

  std::unique_ptr<std::thread> _thread;
  std::unique_ptr<IVtplStreamFrameSrc> _stream;
  zero_copy_queue<VtplVideoFrame, 5> _q;

  void _get_stream_url(std::string& path);
  void _release();
  void _reconnect();

public:
  VtplVideoStream(std::string path, uint16_t channel_id = 0, uint32_t app_id = 0, float suggested_fps = 20.0,
                  bool is_blocking = false, int gpu_id = 0, int seek_time = 100);
  ~VtplVideoStream();
  void start();
  void _do_task();
  bool read(std::vector<uint8_t>& data_out);
  void stop();
};
#endif