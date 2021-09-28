#pragma once
#ifndef FFmpegDemuxer_hpp
#define FFmpegDemuxer_hpp
#include <cstdint>
#include <map>
#include <string>
#include <vector>

#include "CodecSupport.hpp"
extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
}

namespace vp {
class DataProvider {
 public:
  virtual ~DataProvider() = default;
  virtual int GetData(uint8_t *pBuf, int nBuf) = 0;
};

class FFmpegDemuxer {
 private:
  AVIOContext *avioc = nullptr;
  AVBSFContext *bsfc_annexb = nullptr, *bsfc_sei = nullptr;
  AVFormatContext *fmtc = nullptr;

  AVPacket pktSrc, pktDst, pktSei;
  AVCodecID eVideoCodec = AV_CODEC_ID_NONE;
  AVPixelFormat eChromaFormat;
  AVColorSpace color_space;
  AVColorRange color_range;

  uint32_t width;
  uint32_t height;
  uint32_t gop_size;
  uint64_t nb_frames;
  double framerate;
  double avg_framerate;
  double timebase;

  int videoStream = -1;

  bool is_seekable;
  bool is_mp4H264;
  bool is_mp4HEVC;
  bool is_VP9;
  bool is_EOF = false;

  PacketData last_packet_data;

  std::vector<uint8_t> annexbBytes;
  std::vector<uint8_t> seiBytes;

  explicit FFmpegDemuxer(AVFormatContext *fmtcx);
  AVFormatContext *CreateFormatContext(DataProvider *pDataProvider,
                                       const std::map<std::string, std::string> &ffmpeg_options);
  AVFormatContext *CreateFormatContext(const std::string &url,
                                       const std::map<std::string, std::string> &ffmpeg_options);
  static int ReadPacket(void *opaque, uint8_t *pBuf, int nBuf);

 public:
  FFmpegDemuxer(DataProvider *pDataProvider, const std::map<std::string, std::string> &ffmpeg_options);
  FFmpegDemuxer(const std::string &url, const std::map<std::string, std::string> &ffmpeg_options);
  ~FFmpegDemuxer();

  std::string GetVideoCodec() const;

  uint32_t GetWidth() const;

  uint32_t GetHeight() const;

  uint32_t GetGopSize() const;

  uint32_t GetNumFrames() const;

  double GetFramerate() const;

  double GetAvgFramerate() const;

  bool IsVFR() const;

  double GetTimebase() const;

  uint32_t GetVideoStreamIndex() const;

  AVPixelFormat GetPixelFormat() const;

  AVColorSpace GetColorSpace() const;

  AVColorRange GetColorRange() const;

  bool Demux(uint8_t *&pVideo, size_t &rVideoBytes, PacketData &pktData, uint8_t **ppSEI = nullptr,
             size_t *pSEIBytes = nullptr);
};

}  // namespace vp
#endif