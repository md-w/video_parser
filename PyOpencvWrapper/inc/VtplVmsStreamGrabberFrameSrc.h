#pragma once
#ifndef VtplVmsStreamGrabberFrameSrc_h
#define VtplVmsStreamGrabberFrameSrc_h
#include "CodecManager.h"
#include "IVtplStreamFrameSrc.h"
#include <Poco/Net/StreamSocket.h>
#include <atomic>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

class VtplVmsStreamGrabberFrameSrc : public IVtplStreamFrameSrc
{
private:
  bool _force_major_vms_stream;
  bool _force_minor_vms_stream;
  VDeviceId _v_device_id;
  std::string _remote_ip;
  uint32_t _remote_port;
  uint16_t _channel_id;
  uint8_t _major_minor;
  std::vector<char> _buff;
  std::vector<char> _buff2;
  double _last_decoded_timestamp;
  double _last_fps;
  int _time_out_in_sec = 20;
  std::unique_ptr<Poco::Net::StreamSocket> _s;
  bool _is_already_shutting_down = false;
  std::atomic_bool _is_shutdown;



  void* _decoder_data_space = nullptr;
  int _rgb_width = 1920;
  int _rgb_height = 1080;
  unsigned char* _rgb_buffer = nullptr;

  void _get_remote_ip_port_channel(std::string& source_url);
  void _connect_to_vms();
  void _close();

public:
  /**
   * @brief Construct a new Vtpl Vms Stream Grabber Frame Src object
   *
   * @param source_url
   * @param device_id
   * @throws std::out_of_range Out of range
   * @throws std::invalid_argument Invalid argument
   */
  VtplVmsStreamGrabberFrameSrc(std::string source_url, int device_id = 0);
  VtplVmsStreamGrabberFrameSrc(const VtplVmsStreamGrabberFrameSrc&) = delete;
  ~VtplVmsStreamGrabberFrameSrc();
  void stop();
  void release();
  bool read(std::vector<uint8_t>& data);

  double get(int a);
};

#endif