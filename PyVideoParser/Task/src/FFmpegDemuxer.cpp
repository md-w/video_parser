#include "FFmpegDemuxer.hpp"

#include <iostream>
#include <sstream>
using namespace vp;
#if _MSC_VER
#undef av_err2str
#define av_err2str(errnum) \
  av_make_error_string((char *)_malloca(AV_ERROR_MAX_STRING_SIZE), AV_ERROR_MAX_STRING_SIZE, errnum)
#else
#undef av_err2str
#define av_err2str(errnum) \
  av_make_error_string((char *)__builtin_alloca(AV_ERROR_MAX_STRING_SIZE), AV_ERROR_MAX_STRING_SIZE, errnum)
#endif  // _MSC_VER

inline std::string AVCodecIDToStr(AVCodecID id) {
  switch (id) {
    case AV_CODEC_ID_MPEG1VIDEO:
      return "MPEG1";
    case AV_CODEC_ID_MPEG2VIDEO:
      return "MPEG2";
    case AV_CODEC_ID_MPEG4:
      return "MPEG4";
    case AV_CODEC_ID_VC1:
      return "VC1";
    case AV_CODEC_ID_H264:
      return "H264";
    case AV_CODEC_ID_HEVC:
      return "HEVC";
    case AV_CODEC_ID_VP8:
      return "VP8";
    case AV_CODEC_ID_VP9:
      return "VP9";
    case AV_CODEC_ID_MJPEG:
      return "JPEG";
    default:
      return "UNKNOWN";
  }
}

int FFmpegDemuxer::ReadPacket(void *opaque, uint8_t *pBuf, int nBuf) {
  return ((DataProvider *)opaque)->GetData(pBuf, nBuf);
}

AVFormatContext *FFmpegDemuxer::CreateFormatContext(DataProvider *pDataProvider,
                                                    const std::map<std::string, std::string> &ffmpeg_options) {
  AVFormatContext *ctx = avformat_alloc_context();
  if (!ctx) {
    std::cerr << "Can't allocate AVFormatContext at " << __FILE__ << " " << __LINE__;
    return nullptr;
  }
  uint8_t *avioc_buffer = nullptr;
  int avioc_buffer_size = 8 * 1024 * 1024;
  avioc_buffer = (uint8_t *)av_malloc(avioc_buffer_size);
  if (!avioc_buffer) {
    std::cerr << "Can't allocate avioc_buffer at " << __FILE__ << " " << __LINE__;
    return nullptr;
  }
  avioc = avio_alloc_context(avioc_buffer, avioc_buffer_size, 0, pDataProvider, &ReadPacket, nullptr, nullptr);
  ctx->pb = avioc;

  // Set up format context options;
  AVDictionary *options = NULL;
  for (auto &pair : ffmpeg_options) {
    auto err = av_dict_set(&options, pair.first.c_str(), pair.second.c_str(), 0);
    if (err < 0) {
      std::cerr << "Can't set up dictionary option: " << pair.first << " " << pair.second << ": " << av_err2str(err)
                << "\n";
      return nullptr;
    }
  }

  auto err = avformat_open_input(&ctx, nullptr, nullptr, &options);
  if (0 != err) {
    std::cerr << "Can't open input. Error message: " << av_err2str(err);
    return nullptr;
  }

  return ctx;
}
AVFormatContext *FFmpegDemuxer::CreateFormatContext(const std::string &url,
                                                    const std::map<std::string, std::string> &ffmpeg_options) {
  avformat_network_init();

  // Set up format context options;
  AVDictionary *options = NULL;
  for (auto &pair : ffmpeg_options) {
    std::cout << pair.first << ": " << pair.second << std::endl;
    auto err = av_dict_set(&options, pair.first.c_str(), pair.second.c_str(), 0);
    if (err < 0) {
      std::cerr << "Can't set up dictionary option: " << pair.first << " " << pair.second << ": " << av_err2str(err)
                << "\n";
      return nullptr;
    }
  }

  AVFormatContext *ctx = nullptr;
  auto err = avformat_open_input(&ctx, url.c_str(), nullptr, &options);
  if (err < 0) {
    std::cerr << "Can't open " << url << ": " << av_err2str(err) << "\n";
    return nullptr;
  }

  return ctx;
}

FFmpegDemuxer::FFmpegDemuxer(DataProvider *pDataProvider, const std::map<std::string, std::string> &ffmpeg_options)
    : FFmpegDemuxer(CreateFormatContext(pDataProvider, ffmpeg_options)) {}
FFmpegDemuxer::FFmpegDemuxer(const std::string &url, const std::map<std::string, std::string> &ffmpeg_options)
    : FFmpegDemuxer(CreateFormatContext(url, ffmpeg_options)) {}

FFmpegDemuxer::FFmpegDemuxer(AVFormatContext *fmtcx) : fmtc(fmtcx) {
  if (!fmtc) {
    std::stringstream ss;
    ss << __FUNCTION__ << ": no AVFormatContext provided." << std::endl;
    throw std::invalid_argument(ss.str());
  }
  auto ret = avformat_find_stream_info(fmtc, nullptr);
  if (0 != ret) {
    std::stringstream ss;
    ss << __FUNCTION__ << ": can't find stream info;" << av_err2str(ret) << std::endl;
    throw std::runtime_error(ss.str());
  }
  videoStream = av_find_best_stream(fmtc, AVMEDIA_TYPE_VIDEO, -1, -1, nullptr, 0);
  if (videoStream < 0) {
    std::stringstream ss;
    ss << __FUNCTION__ << ": can't find video stream in input file." << std::endl;
    throw std::runtime_error(ss.str());
  }
  gop_size = fmtc->streams[videoStream]->codec->gop_size;
  eVideoCodec = fmtc->streams[videoStream]->codecpar->codec_id;
  width = fmtc->streams[videoStream]->codecpar->width;
  height = fmtc->streams[videoStream]->codecpar->height;
  framerate =
      (double)fmtc->streams[videoStream]->r_frame_rate.num / (double)fmtc->streams[videoStream]->r_frame_rate.den;
  avg_framerate =
      (double)fmtc->streams[videoStream]->avg_frame_rate.num / (double)fmtc->streams[videoStream]->avg_frame_rate.den;
  timebase = (double)fmtc->streams[videoStream]->time_base.num / (double)fmtc->streams[videoStream]->time_base.den;
  eChromaFormat = (AVPixelFormat)fmtc->streams[videoStream]->codecpar->format;
  nb_frames = fmtc->streams[videoStream]->nb_frames;
  color_space = fmtc->streams[videoStream]->codec->colorspace;
  color_range = fmtc->streams[videoStream]->codec->color_range;

  is_mp4H264 = (eVideoCodec == AV_CODEC_ID_H264);
  is_mp4HEVC = (eVideoCodec == AV_CODEC_ID_HEVC);
  is_VP9 = (eVideoCodec == AV_CODEC_ID_VP9);
  av_init_packet(&pktSrc);
  pktSrc.data = nullptr;
  pktSrc.size = 0;
  av_init_packet(&pktDst);
  pktDst.data = nullptr;
  pktDst.size = 0;
  av_init_packet(&pktSei);
  pktSei.data = nullptr;
  pktSei.size = 0;

  // Initialize Annex.B BSF;
  const std::string bfs_name = is_mp4H264   ? "h264_mp4toannexb"
                               : is_mp4HEVC ? "hevc_mp4toannexb"
                               : is_VP9     ? std::string()
                                            : "unknown";
  if (!bfs_name.empty()) {
    const AVBitStreamFilter *toAnnexB = av_bsf_get_by_name(bfs_name.c_str());
    if (!toAnnexB) {
      throw std::runtime_error("can't get " + bfs_name + " filter by name");
    }
    ret = av_bsf_alloc(toAnnexB, &bsfc_annexb);
    if (0 != ret) {
      throw std::runtime_error("Error allocating " + bfs_name + " filter: " + av_err2str(ret));
    }
    ret = avcodec_parameters_copy(bsfc_annexb->par_in, fmtc->streams[videoStream]->codecpar);
    if (0 != ret) {
      throw std::runtime_error("Error copying codec parameters " + bfs_name + " error: " + av_err2str(ret));
    }

    ret = av_bsf_init(bsfc_annexb);
    if (0 != ret) {
      throw std::runtime_error("Error initializing " + bfs_name + " bitstream filter: " + av_err2str(ret));
    }
  }
  // SEI extraction filter has lazy init as this feature is optional;
  bsfc_sei = nullptr;

  /* Some inputs doesn't allow seek functionality.
   * Check this ahead of time. */
  is_seekable = fmtc->iformat->read_seek || fmtc->iformat->read_seek2;
}

FFmpegDemuxer::~FFmpegDemuxer() {
  if (pktSrc.data) {
    av_packet_unref(&pktSrc);
  }
  if (pktDst.data) {
    av_packet_unref(&pktDst);
  }
  if (bsfc_annexb) {
    av_bsf_free(&bsfc_annexb);
  }
  if (bsfc_sei) {
    av_bsf_free(&bsfc_sei);
  }
  avformat_close_input(&fmtc);
  if (avioc) {
    av_freep(&avioc->buffer);
    av_freep(&avioc);
  }
}
uint32_t FFmpegDemuxer::GetWidth() const { return width; }

uint32_t FFmpegDemuxer::GetHeight() const { return height; }

uint32_t FFmpegDemuxer::GetGopSize() const { return gop_size; }

uint32_t FFmpegDemuxer::GetNumFrames() const { return nb_frames; }

double FFmpegDemuxer::GetFramerate() const { return framerate; }

double FFmpegDemuxer::GetAvgFramerate() const { return avg_framerate; }

double FFmpegDemuxer::GetTimebase() const { return timebase; }

bool FFmpegDemuxer::IsVFR() const { return framerate != avg_framerate; }

uint32_t FFmpegDemuxer::GetVideoStreamIndex() const { return videoStream; }

std::string FFmpegDemuxer::GetVideoCodec() const { return AVCodecIDToStr(eVideoCodec); }