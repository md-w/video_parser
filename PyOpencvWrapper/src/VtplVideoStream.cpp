#include "VtplVideoStream.h"
#include "VtplVmsStreamGrabberFrameSrc.h"
#include <Poco/URI.h>
#include <iostream>
#include <logging.h>

VtplVideoStream::VtplVideoStream(std::string path, uint16_t channel_id, uint32_t app_id, float suggested_fps,
                                 bool is_blocking, int gpu_id, int seek_time)
    : _channel_id(channel_id), _app_id(app_id), _reconnect_requested(false), _is_already_shutting_down(false),
      _is_shutdown(false)
{
  _get_stream_url(path);
}

void VtplVideoStream::_get_stream_url(std::string& path)
{
  if (path.size() > 0) {
    Poco::URI uri(path);
    _stream_type = uri.getScheme();
    _stream_url = uri.toString();
    if (_stream_type.find("file") != std::string::npos) {
#ifdef WIN32
      _stream_url = _stream_url.substr(std::string("file://").size());
#endif
    }
    if (_stream_type == "") {
      _stream_type = "file";
      _stream_url = "file://" + _stream_url;
      std::cout << "Stream type is Not set, defaulting to file " << _stream_url << std::endl;
    }
    if (_stream_type == "folder") {
      _stream_url = uri.getPath();
    }
    std::cout << "_stream_type : " << _stream_type << " _stream_url : " << _stream_url
              << " uri.getPath(): " << uri.getPath() << std::endl;
  }
}

void VtplVideoStream::_do_task()
{
  RAY_LOG(INFO) << "do_task started";
  while (!_is_shutdown) {
    if (_reconnect_requested) {
      _reconnect_requested = false;
      _consecutive_grabbing_error = 0;
      _reconnect();
    }
    if (_is_eos)
      break;
    if (_stream == nullptr)
      continue;
    // if (!_q.full()) {
    //   int write_idx = _q.getWriteIdx();
    //   VtplVideoFrame* frame = _q.getWritable(write_idx);
    // }
    std::vector<uint8_t> data;
    if (_stream->read(data)) {
      // put in the q
    } else {
      _is_shutdown = true;
      break;
    }
  }

  _release();
  RAY_LOG(INFO) << "do_task finished";
}
void VtplVideoStream::start()
{
  std::cout << "Start: " << _channel_id << " " << _app_id << std::endl;
  _reconnect_requested = true;
  _thread.reset(new std::thread(&VtplVideoStream::_do_task, this));
}
bool VtplVideoStream::read(std::vector<uint8_t>& data_out) { return false; }
void VtplVideoStream::_release()
{
  if (_stream) {
    RAY_LOG(INFO) << ("Before release");
    _stream.release();
  }
}
void VtplVideoStream::_reconnect()
{
  RAY_LOG(INFO) << fmt::format("Reconnect {}", _stream_url);
  _is_eos = false;
  _release();
  int time_out_in_sec = _reconnect_retry_time;
  const std::chrono::time_point<std::chrono::system_clock> entry_time = std::chrono::system_clock::now();
  while (!_is_shutdown) {
    std::chrono::time_point<std::chrono::system_clock> current_time = std::chrono::system_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(current_time - entry_time).count() > time_out_in_sec)
      break;
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
  if (!_is_shutdown && !_stream_url.empty()) {
    RAY_LOG(INFO) << fmt::format("reconnect_retry_time {}", _reconnect_retry_time);
    _reconnect_retry_time = 10;
    if (_stream_type.find("vms") != std::string::npos) {
      _stream.reset(new VtplVmsStreamGrabberFrameSrc(_stream_url));
    }
    float f = _stream->get(0);
    if ((f > 0) && (f < 100)) {
      _suggested_fps = f;
    }
    RAY_LOG(INFO) << fmt::format("Suggested FPS {} {}", _suggested_fps, f);
  }
}
void VtplVideoStream::stop()
{
  if (_is_already_shutting_down)
    return;
  _is_already_shutting_down = true;
  RAY_LOG(INFO) << fmt::format("Stop set {} {}", _channel_id, _app_id);
  _is_shutdown = true;
  if (_stream)
    _stream->release();
  std::cout << "MONO Here 1" << std::endl;
  if (_thread->joinable())
    _thread->join();
  std::cout << "MONO Here 2" << std::endl;
  RAY_LOG(INFO) << fmt::format("Stop return {} {}", _channel_id, _app_id);
}

VtplVideoStream::~VtplVideoStream() {}