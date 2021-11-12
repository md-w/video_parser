#include "VtplVmsStreamGrabberFrameSrc.h"
#include "VReturnStatus.h"
#include <Poco/ByteOrder.h>
#include <Poco/Net/NetException.h>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <thread>

bool read_data_n(Poco::Net::StreamSocket& s, std::vector<uint8_t>& data_array, int bytes_to_read)
{
  int length = 0;
  if (bytes_to_read > data_array.size()) {
    return false;
  }

  while (true) {
    int x = bytes_to_read - length;
    if (x > 0) {
      try {
        int length_received = s.receiveBytes(&(data_array[length]), x);
        if (length_received <= 0) {
          // std::string host = s.peerAddress().toString();
          // std::cout << "host: " << host << " s.getReceiveBufferSize() " << s.getReceiveBufferSize() << std::endl;
          break;
        }
        length += length_received;
      } catch (const Poco::TimeoutException& e) {
        break;
      } catch (const Poco::Net::NetException& e) {
        std::cerr << __FILE__ << " : " << __LINE__ << " " << e.what() << '\n';
        break;
      } catch (const Poco::IOException& e) {
        std::cerr << __FILE__ << " : " << __LINE__ << " " << e.what() << '\n';
        break;
      } catch (const std::exception& e) {
        std::cerr << __FILE__ << " : " << __LINE__ << " " << e.what() << '\n';
        break;
      }
    } else {
      break;
    }
  }
  return (length == bytes_to_read);
}
VDeviceId::VDeviceId() : VDeviceId(0, 0, 0, 1, 2) {}
VDeviceId::VDeviceId(int32_t _device_id, int16_t _channel_id, uint8_t _major_or_minor, uint8_t _real_time_mode_or_not,
                     uint8_t _decoder_initialized_or_not)
    : device_id(_device_id), channel_id(_channel_id), major_or_minor(_major_or_minor),
      real_time_mode_or_not(_real_time_mode_or_not), decoder_initialized_or_not(_decoder_initialized_or_not)

{
  data_out.resize(sizeof(VDeviceId));
}
VDeviceId::~VDeviceId() {}
void VDeviceId::fromNetwork(std::vector<uint8_t>& data_in)
{
  int offset = 0;
  device_id = Poco::ByteOrder::fromNetwork(decltype(device_id)(*(decltype(device_id)*)(data_in.data() + offset)));
  offset += sizeof(decltype(device_id));
  channel_id = Poco::ByteOrder::fromNetwork(decltype(channel_id)(*(decltype(channel_id)*)(data_in.data() + offset)));
  offset += sizeof(decltype(channel_id));
  major_or_minor = decltype(major_or_minor)(*(decltype(major_or_minor)*)(data_in.data() + offset));
  offset += sizeof(decltype(major_or_minor));
  real_time_mode_or_not = decltype(real_time_mode_or_not)(*(decltype(real_time_mode_or_not)*)(data_in.data() + offset));
  offset += sizeof(decltype(real_time_mode_or_not));
  decoder_initialized_or_not =
      decltype(decoder_initialized_or_not)(*(decltype(decoder_initialized_or_not)*)(data_in.data() + offset));
  offset += sizeof(decltype(decoder_initialized_or_not));
}
std::vector<uint8_t>& VDeviceId::toNetwork()
{

  int offset = 0;
  *(decltype(device_id)*)(data_out.data() + offset) = Poco::ByteOrder::toNetwork(device_id);
  offset += sizeof(decltype(device_id));
  *(decltype(channel_id)*)(data_out.data() + offset) = Poco::ByteOrder::toNetwork(channel_id);
  offset += sizeof(decltype(channel_id));
  *(decltype(major_or_minor)*)(data_out.data() + offset) = major_or_minor;
  offset += sizeof(decltype(major_or_minor));
  *(decltype(real_time_mode_or_not)*)(data_out.data() + offset) = real_time_mode_or_not;
  offset += sizeof(decltype(real_time_mode_or_not));
  *(decltype(decoder_initialized_or_not)*)(data_out.data() + offset) = decoder_initialized_or_not;
  offset += sizeof(decltype(decoder_initialized_or_not));
  return data_out;
}
VMediaFrame::VMediaFrame() { data_out.resize(sizeof(VMediaFrame)); }
VMediaFrame::~VMediaFrame() {}
void VMediaFrame::fromNetwork(std::vector<uint8_t>& data_in)
{
  int offset = 0;
  media_type = Poco::ByteOrder::fromNetwork(decltype(media_type)(*(decltype(media_type)*)(data_in.data() + offset)));
  offset += sizeof(decltype(media_type));
  frame_type = Poco::ByteOrder::fromNetwork(decltype(frame_type)(*(decltype(frame_type)*)(data_in.data() + offset)));
  offset += sizeof(decltype(frame_type));
  bit_rate = Poco::ByteOrder::fromNetwork(decltype(bit_rate)(*(decltype(bit_rate)*)(data_in.data() + offset)));
  offset += sizeof(decltype(bit_rate));
  fps = Poco::ByteOrder::fromNetwork(decltype(fps)(*(decltype(fps)*)(data_in.data() + offset)));
  offset += sizeof(decltype(fps));
  time_stamp = Poco::ByteOrder::fromNetwork(decltype(time_stamp)(*(decltype(time_stamp)*)(data_in.data() + offset)));
  offset += sizeof(decltype(time_stamp));

  motion_available = decltype(motion_available)(*(decltype(motion_available)*)(data_in.data() + offset));
  offset += sizeof(decltype(motion_available));
  stream_type = decltype(stream_type)(*(decltype(stream_type)*)(data_in.data() + offset));
  offset += sizeof(decltype(stream_type));
  channel_id = Poco::ByteOrder::fromNetwork(decltype(channel_id)(*(decltype(channel_id)*)(data_in.data() + offset)));
  offset += sizeof(decltype(channel_id));
}
std::vector<uint8_t>& VMediaFrame::toNetwork()
{

  int offset = 0;
  *(decltype(media_type)*)(data_out.data() + offset) = Poco::ByteOrder::toNetwork(media_type);
  offset += sizeof(decltype(media_type));
  *(decltype(frame_type)*)(data_out.data() + offset) = Poco::ByteOrder::toNetwork(frame_type);
  offset += sizeof(decltype(bit_rate));
  *(decltype(bit_rate)*)(data_out.data() + offset) = Poco::ByteOrder::toNetwork(bit_rate);
  offset += sizeof(decltype(fps));
  *(decltype(fps)*)(data_out.data() + offset) = Poco::ByteOrder::toNetwork(fps);
  offset += sizeof(decltype(fps));
  *(decltype(time_stamp)*)(data_out.data() + offset) = Poco::ByteOrder::toNetwork(time_stamp);
  offset += sizeof(decltype(time_stamp));

  *(decltype(motion_available)*)(data_out.data() + offset) = motion_available;
  offset += sizeof(decltype(motion_available));
  *(decltype(stream_type)*)(data_out.data() + offset) = stream_type;
  offset += sizeof(decltype(stream_type));
  *(decltype(channel_id)*)(data_out.data() + offset) = channel_id;
  offset += sizeof(decltype(channel_id));
  return data_out;
}
VtplVmsStreamGrabberFrameSrc::VtplVmsStreamGrabberFrameSrc(std::string source_url, int device_id)
    : _force_major_vms_stream(false), _force_minor_vms_stream(false), _is_shutdown(false), _last_decoded_timestamp(0.0),
      _last_fps(0.0)
{
  _get_remote_ip_port_channel(source_url);
  if (_force_major_vms_stream) {
    std::cout << "Forcing major url for channel " << _channel_id << std::endl;
    _major_minor = 0;
  }
  if (_force_major_vms_stream) {
    std::cout << "Forcing minorr url for channel " << _channel_id << std::endl;
    _major_minor = 1;
  }
  _v_device_id.device_id = device_id;
  _v_device_id.channel_id = _channel_id;
  _v_device_id.major_or_minor = _major_minor;
  std::cout << " _remote_ip: " << _remote_ip << " _remote_port: " << _remote_port << " _channel_id: " << _channel_id
            << " _major_minor: " << (uint16_t)_major_minor << std::endl;
  int buff_size = 600 * 1024;
  _buff.resize(buff_size);
  _buff2.resize(28);
  _connect_to_vms();

  bin_file.open("34.264", std::ios::out | std::ios::binary);
}

VtplVmsStreamGrabberFrameSrc::~VtplVmsStreamGrabberFrameSrc() {}

std::vector<std::string> split(std::string s, std::string delimiter)
{
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  std::string token;
  std::vector<std::string> tokens;
  while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    tokens.push_back(token);
  }
  tokens.push_back(s.substr(pos_start));
  return tokens;
}

/**
 * @brief Parse source_url
 *
 * @param source_url
 * @throws std::out_of_range out of range exception
 */

void VtplVmsStreamGrabberFrameSrc::_get_remote_ip_port_channel(std::string& source_url)
{
  std::vector<std::string> l = split(source_url, "://");
  std::string stream_type = l.at(0);
  std::vector<std::string> l1 = split(l.at(1), "/");
  std::vector<std::string> l2 = split(l1.at(0), ":");
  _remote_ip = l2.at(0);
  _remote_port = std::stoi(l2.at(1));
  _channel_id = (uint16_t)std::stoi(l1.at(1));
  _major_minor = (uint8_t)std::stoi(l1.at(2));
}

double VtplVmsStreamGrabberFrameSrc::get(int a)
{
  switch (a) {
  case 1:
    return _last_decoded_timestamp;
    break;
  case 2:
    return _last_fps;
    break;
  default:
    break;
  }
  return 0;
}

void VtplVmsStreamGrabberFrameSrc::_connect_to_vms()
{
  try {
    Poco::Net::SocketAddress address(_remote_ip, _remote_port);
    _s.reset(new Poco::Net::StreamSocket());
    _s->connect(address, Poco::Timespan(SOCKET_DEFAULT_TIMEOUT_SEC, 0));
    _s->setReceiveTimeout(Poco::Timespan(SOCKET_DEFAULT_TIMEOUT_SEC, 0));
    std::string host = _s->peerAddress().toString();

    int send_size = sizeof(VDeviceId);
    int count = _s->sendBytes(_v_device_id.toNetwork().data(), send_size);
    if (count != send_size)
      throw std::runtime_error("Send size does not match");
    std::cout << "Success to connect ip " << host << " port " << _remote_port << " channel id " << _channel_id
              << std::endl;
  } catch (const Poco::TimeoutException& e) {
    std::cerr << __FILE__ << " : " << __LINE__ << " " << e.what() << '\n';
    _close();
  } catch (const Poco::Net::NetException& e) {
    std::cerr << __FILE__ << " : " << __LINE__ << " " << e.what() << '\n';
    _close();
  }
}

bool VtplVmsStreamGrabberFrameSrc::read(std::vector<uint8_t>& data)
{
  bool ret = false;
  // mat = None
  // _time_out_in_sec;
  const std::chrono::time_point<std::chrono::system_clock> entry_time = std::chrono::system_clock::now();

  while (!_is_shutdown) {
    std::chrono::time_point<std::chrono::system_clock> current_time = std::chrono::system_clock::now();
    if (std::chrono::duration_cast<std::chrono::seconds>(current_time - entry_time).count() > _time_out_in_sec)
      break;
    if (read_data_n(*_s, _buff, 4) == false) {
      std::this_thread::sleep_for(std::chrono::seconds(1));
      continue;
    }
    int32_t buff_len = Poco::ByteOrder::fromNetwork(*reinterpret_cast<int32_t*>(&_buff[0]));
    if (buff_len <= 0) {
      // std::cerr << __FILE__ << " : " << __LINE__ << " Zero length buffer received" << '\n';
      continue;
    } else if (buff_len > 10 * 1024 * 1024) {
      std::cerr << __FILE__ << " : " << __LINE__ << "Too long encoded buffer received " << _remote_ip << " "
                << _remote_port << " channel " << _channel_id << '\n';
      std::this_thread::sleep_for(std::chrono::seconds(1));
      continue;
    } else {
      if (buff_len > _buff.size()) { // low buffer
        std::cout << "Reallocating memory" << std::endl;

        int buff_size = ((buff_len / 32 + 1) * 32);
        _buff.resize(buff_size);
      }
      if (read_data_n(*_s, _buff, buff_len) == false) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        continue;
      }
      if (read_data_n(*_s, _buff2, 28) == false) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        continue;
      }
      VMediaFrame frame_info;
      frame_info.fromNetwork(_buff2);
      _last_decoded_timestamp = frame_info.time_stamp;
      // std::cout << " size: " << _buff.size() << " buff_len: " << buff_len << std::endl;

      // std::cout << "frame_info.channel_id " << (int)frame_info.channel_id << "frame_info.media_type "
      //           << (int)frame_info.media_type << "frame_info.frame_type " << (int)frame_info.frame_type
      //           << "frame_info.fps " << (int)frame_info.fps << "frame_info.bit_rate " << (int)frame_info.bit_rate
      //           << "frame_info.motion_available " << (int)frame_info.motion_available << " frame_info.time_stamp "
      //           << frame_info.time_stamp << " frame_info.stream_type " << (int)frame_info.stream_type << std::endl;

      if (!(((Codec_Type)frame_info.media_type == Codec_Type::H264) ||
            ((Codec_Type)frame_info.media_type == Codec_Type::H265))) {
        continue;
      }
      // std::cout << "buff_len: " << buff_len << std::endl;
      bin_file.write(reinterpret_cast<const char*>(&_buff[0]), buff_len);
#if 1
      if (_decoder_data_space == nullptr) {
        if (!(initDecoder(&_decoder_data_space, (Codec_Type)frame_info.media_type, ARGB) == 0)) {
          continue;
        } else {
          if (_rgb_buffer == nullptr) {
            _rgb_buffer = (unsigned char*)malloc(_rgb_width * _rgb_height * 4);
          }
        }
      }
      if (_decoder_data_space == nullptr) {
        continue;
      }

      int tempRgbWidth = _rgb_width;
      int tempRgbHeight = _rgb_height;
      while (true) {
        // int rs = decodeFrameGetGPUPoiner(decoderDataSpace, buffer,
        // readLen,
        //                                  &rgbBuffGPUPtr, &tempRgbWidth,
        //                                  &tempRgbHeight, frame.timeStamp,
        //                                  &outTimeStamp, frame.frameType);

        int rs = decodeFrame(_decoder_data_space, (unsigned char*)_buff.data(), buff_len, _rgb_buffer, &tempRgbWidth,
                             &tempRgbHeight);
        if (rs == VA_LOWBUFFERSIZE) {
          if (_rgb_buffer)
            free(_rgb_buffer);
          _rgb_width = tempRgbWidth;
          _rgb_height = tempRgbHeight;
          _rgb_buffer = (unsigned char*)malloc(_rgb_width * _rgb_height * 4);
        } else if (rs == VA_SUCCESS) {
          // printf("thread=====%d\n", std::this_thread::get_id());
          // std::this_thread::sleep_for(std::chrono::milliseconds(1));
          // char op[250] = {
          //     0,
          // };
          // sprintf(op, "./dump_1/%05d_%05llu.ppm", frame_count,
          //         outTimeStamp);
          // writePPMFromBgra(rgbBuffer, rgbHeight, rgbWidth, op);
          // printf("\n[%d] Success", frame_count);
          cv::Mat inMat = cv::Mat(_rgb_height, _rgb_width, CV_8UC4, (unsigned char*)_rgb_buffer);
          cv::Mat frame;
          cv::cvtColor(inMat, frame, cv::ColorConversionCodes::COLOR_BGRA2BGR);
          // ultraface.getResult(frame);
          cv::namedWindow("vtpl_opencv", cv::WINDOW_NORMAL);
          cv::imshow("vtpl_opencv", frame);
          cv::waitKey(1);
          break;
        } else {
          // printf("\n[%d] Error\n", i);
          break;
        }
      }
#endif
      long long outTimeStamp = 0;
      ret = true;
      break;
    }
  }
  return ret;
}
void VtplVmsStreamGrabberFrameSrc::_close()
{
  if (_s == nullptr)
    return;
  try {
    std::cerr << __FILE__ << " : " << __LINE__ << " "
              << "socket shutdown called \n";
    _s->shutdown();
  } catch (const Poco::Net::NetException& e) {
    std::cerr << __FILE__ << " : " << __LINE__ << " " << e.what() << '\n';
  }
  try {
    std::cerr << __FILE__ << " : " << __LINE__ << " "
              << "socket close called \n";
    _s->close();
  } catch (const Poco::Net::NetException& e) {
    std::cerr << __FILE__ << " : " << __LINE__ << " " << e.what() << '\n';
  }
  _s = nullptr;
}
void VtplVmsStreamGrabberFrameSrc::stop() { release(); }
void VtplVmsStreamGrabberFrameSrc::release()
{
  if (_is_already_shutting_down)
    return;
  _is_already_shutting_down = true;
  _is_shutdown = true;
  std::cout << "Closing file" << std::endl;
  bin_file.close();
  std::cout << "File closed" << std::endl;
  _close();
  std::cout << "Here 2" << std::endl;
  if (_rgb_buffer) {
    free(_rgb_buffer);
    _rgb_buffer = nullptr;
  }
  std::cout << "Here 3" << std::endl;
  if (_decoder_data_space) {
    freeDecoder(_decoder_data_space);
    _decoder_data_space = nullptr;
  }
  std::cout << "Here 4" << std::endl;
}