#pragma once
#ifndef VtplVmsStreamGrabberFrameSrc_h
#define VtplVmsStreamGrabberFrameSrc_h
#include <stdexcept>
#include <string>
#include <vector>
class VDeviceId
{
private:
  int32_t device_id;
  int16_t ch_id;
  uint8_t major_or_minor;
  uint8_t real_time_mode_or_not;
  uint8_t decoder_initialized_or_not;
  std::vector<char> data_out;

public:
  VDeviceId();
  VDeviceId(int32_t _device_id, int16_t _ch_id, uint8_t _major_or_minor, uint8_t _real_time_mode_or_not,
            uint8_t _decoder_initialized_or_not);
  ~VDeviceId();
  void fromNetwork(std::vector<char>& data_in);
  std::vector<char>& toNetwork();
};

class VMediaFrame
{
private:
  int32_t media_type;
  int32_t frame_type;
  int32_t bit_rate;
  int32_t fps;
  int64_t time_stamp;
  uint8_t motion_available;
  uint8_t stream_type;
  uint8_t ch_id;
  std::vector<char> data_out;

public:
  VMediaFrame();
  ~VMediaFrame();
  void fromNetwork(std::vector<char>& data_in);
  std::vector<char>& toNetwork();
};

class VtplVmsStreamGrabberFrameSrc
{
private:
  bool _force_major_vms_stream;
  bool _force_minor_vms_stream;
  VDeviceId _v_device_id;
  std::string _remote_ip;
  uint32_t _remote_port;
  uint16_t _ch_id;
  uint8_t _major_minor;
  std::vector<uint8_t> _buff;
  std::vector<uint8_t> _buff2;


  void _get_remote_ip_port_channel(std::string& source_url);

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
  ~VtplVmsStreamGrabberFrameSrc();
};

#endif