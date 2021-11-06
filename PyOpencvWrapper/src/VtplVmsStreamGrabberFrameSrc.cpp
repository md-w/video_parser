#include "VtplVmsStreamGrabberFrameSrc.h"
#include <Poco/ByteOrder.h>
#include <Poco/Net/NetException.h>
#include <Poco/Net/StreamSocket.h>

bool read_data_n(Poco::Net::StreamSocket& s, std::vector<char>& data_array, int bytes_to_read)
{
  int length = 0;
  if (bytes_to_read > data_array.size()) {
    return false;
  }
  while (true) {
    int x = bytes_to_read - length;
    if (x > 0) {
      try {
        int length_received = s.receiveBytes(data_array.data(), bytes_to_read);
        if (length_received <= 0) {
          break;
        }
      } catch (const Poco::TimeoutException& e) {
        break;
      } catch (const Poco::Net::NetException& e) {
        std::cerr << e.what() << '\n';
        break;
      }
    }
  }
  return (length == bytes_to_read);
}
VDeviceId::VDeviceId() : VDeviceId(0, 0, 0, 1, 2) {}
VDeviceId::VDeviceId(int32_t _device_id, int16_t _ch_id, uint8_t _major_or_minor, uint8_t _real_time_mode_or_not,
                     uint8_t _decoder_initialized_or_not)
    : device_id(_device_id), ch_id(_ch_id), major_or_minor(_major_or_minor),
      real_time_mode_or_not(_real_time_mode_or_not), decoder_initialized_or_not(_decoder_initialized_or_not)

{
  data_out.resize(sizeof(VDeviceId));
}
VDeviceId::~VDeviceId() {}
void VDeviceId::fromNetwork(std::vector<char>& data_in)
{
  int offset = 0;
  device_id = Poco::ByteOrder::fromNetwork(decltype(device_id)(*(decltype(device_id)*)(data_in.data() + offset)));
  offset += sizeof(decltype(device_id));
  ch_id = Poco::ByteOrder::fromNetwork(decltype(ch_id)(*(decltype(ch_id)*)(data_in.data() + offset)));
  offset += sizeof(decltype(ch_id));
  major_or_minor = decltype(major_or_minor)(*(decltype(major_or_minor)*)(data_in.data() + offset));
  offset += sizeof(decltype(major_or_minor));
  real_time_mode_or_not = decltype(real_time_mode_or_not)(*(decltype(real_time_mode_or_not)*)(data_in.data() + offset));
  offset += sizeof(decltype(real_time_mode_or_not));
  decoder_initialized_or_not =
      decltype(decoder_initialized_or_not)(*(decltype(decoder_initialized_or_not)*)(data_in.data() + offset));
  offset += sizeof(decltype(decoder_initialized_or_not));
}
std::vector<char>& VDeviceId::toNetwork()
{

  int offset = 0;
  decltype(device_id)(*(decltype(device_id)*)(data_out.data() + offset)) = Poco::ByteOrder::toNetwork(device_id);
  offset += sizeof(decltype(device_id));
  decltype(ch_id)(*(decltype(ch_id)*)(data_out.data() + offset)) = Poco::ByteOrder::toNetwork(ch_id);
  offset += sizeof(decltype(ch_id));
  decltype(major_or_minor)(*(decltype(major_or_minor)*)(data_out.data() + offset)) = major_or_minor;
  offset += sizeof(decltype(major_or_minor));
  decltype(real_time_mode_or_not)(*(decltype(real_time_mode_or_not)*)(data_out.data() + offset)) =
      real_time_mode_or_not;
  offset += sizeof(decltype(real_time_mode_or_not));
  decltype(decoder_initialized_or_not)(*(decltype(decoder_initialized_or_not)*)(data_out.data() + offset)) =
      decoder_initialized_or_not;
  offset += sizeof(decltype(decoder_initialized_or_not));
  return data_out;
}
VMediaFrame::VMediaFrame() { data_out.resize(sizeof(VMediaFrame)); }
VMediaFrame::~VMediaFrame() {}
void VMediaFrame::fromNetwork(std::vector<char>& data_in)
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
  ch_id = decltype(ch_id)(*(decltype(ch_id)*)(data_in.data() + offset));
  offset += sizeof(decltype(ch_id));
}
std::vector<char>& VMediaFrame::toNetwork()
{

  int offset = 0;
  decltype(media_type)(*(decltype(media_type)*)(data_out.data() + offset)) = Poco::ByteOrder::toNetwork(media_type);
  offset += sizeof(decltype(media_type));
  decltype(frame_type)(*(decltype(frame_type)*)(data_out.data() + offset)) = Poco::ByteOrder::toNetwork(frame_type);
  offset += sizeof(decltype(bit_rate));
  decltype(bit_rate)(*(decltype(bit_rate)*)(data_out.data() + offset)) = Poco::ByteOrder::toNetwork(bit_rate);
  offset += sizeof(decltype(fps));
  decltype(fps)(*(decltype(fps)*)(data_out.data() + offset)) = Poco::ByteOrder::toNetwork(fps);
  offset += sizeof(decltype(fps));
  decltype(time_stamp)(*(decltype(time_stamp)*)(data_out.data() + offset)) = Poco::ByteOrder::toNetwork(time_stamp);
  offset += sizeof(decltype(time_stamp));

  decltype(motion_available)(*(decltype(motion_available)*)(data_out.data() + offset)) = motion_available;
  offset += sizeof(decltype(motion_available));
  decltype(stream_type)(*(decltype(stream_type)*)(data_out.data() + offset)) = stream_type;
  offset += sizeof(decltype(stream_type));
  decltype(ch_id)(*(decltype(ch_id)*)(data_out.data() + offset)) = ch_id;
  offset += sizeof(decltype(ch_id));
  return data_out;
}
VtplVmsStreamGrabberFrameSrc::VtplVmsStreamGrabberFrameSrc(std::string source_url, int device_id)
  : _force_major_vms_stream(false), _force_minor_vms_stream(false)
{
  _get_remote_ip_port_channel(source_url);
  if (_force_major_vms_stream) {
    std::cout << "Forcing major url for channel " << _ch_id << std::endl;
    _major_minor = 0;
  }
  if (_force_major_vms_stream) {
    std::cout << "Forcing minorr url for channel " << _ch_id << std::endl;
    _major_minor = 1;
  }
  std::cout << " _remote_ip: " << _remote_ip << " _remote_port: " << _remote_port << " _ch_id: " << _ch_id
            << " _major_minor: " << (uint16_t)_major_minor << std::endl;
  _buff.resize(600*1024);
  _buff2.resize(28);

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
  _ch_id = (uint16_t)std::stoi(l1.at(1));
  _major_minor = (uint8_t)std::stoi(l1.at(2));
}
