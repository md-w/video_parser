#pragma once
#ifndef IVtplStreamFrameSrc_h
#define IVtplStreamFrameSrc_h

#include <atomic>
#include <memory>
#include <vector>
class VDeviceId
{
private:
  std::vector<uint8_t> data_out;

public:
  VDeviceId();
  VDeviceId(int32_t _device_id, int16_t _channel_id, uint8_t _major_or_minor, uint8_t _real_time_mode_or_not,
            uint8_t _decoder_initialized_or_not);
  ~VDeviceId();
  void fromNetwork(std::vector<uint8_t>& data_in);
  std::vector<uint8_t>& toNetwork();

  int32_t device_id;
  int16_t channel_id;
  uint8_t major_or_minor;
  uint8_t real_time_mode_or_not;
  uint8_t decoder_initialized_or_not;
};
class VMediaFrame
{
private:
  std::vector<uint8_t> data_out;

public:
  VMediaFrame();
  ~VMediaFrame();
  void fromNetwork(std::vector<uint8_t>& data_in);
  std::vector<uint8_t>& toNetwork();

  int32_t media_type;
  int32_t frame_type;
  int32_t bit_rate;
  int32_t fps;
  int64_t time_stamp;
  int8_t motion_available;
  int8_t stream_type;
  int16_t channel_id;
};
constexpr int SOCKET_DEFAULT_TIMEOUT_SEC = 5;
class IVtplStreamFrameSrc
{
public:
  ~IVtplStreamFrameSrc();
  virtual void release() = 0;
  virtual bool read(std::vector<uint8_t>& data) = 0;
  virtual double get(int a) = 0;
};

#endif