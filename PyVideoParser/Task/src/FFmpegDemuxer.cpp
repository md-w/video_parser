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

void FFmpegDemuxer::Flush() {
  avio_flush(fmtc->pb);
  avformat_flush(fmtc);
}

bool FFmpegDemuxer::Demux(uint8_t *&pVideo, size_t &rVideoBytes, PacketData &pktData, uint8_t **ppSEI,
                          size_t *pSEIBytes) {
  if (!fmtc) {
    return false;
  }

  if (pktSrc.data) {
    av_packet_unref(&pktSrc);
  }

  if (!annexbBytes.empty()) {
    annexbBytes.clear();
  }

  if (!seiBytes.empty()) {
    seiBytes.clear();
  }

  auto appendBytes = [](std::vector<uint8_t> &elementaryBytes, AVPacket &avPacket, AVPacket &avPacketOut,
                        AVBSFContext *pAvbsfContext, int streamId, bool isFilteringNeeded) {
    if (avPacket.stream_index != streamId) {
      return;
    }

    if (isFilteringNeeded) {
      if (avPacketOut.data) {
        av_packet_unref(&avPacketOut);
      }

      av_bsf_send_packet(pAvbsfContext, &avPacket);
      av_bsf_receive_packet(pAvbsfContext, &avPacketOut);

      if (avPacketOut.data && avPacketOut.size) {
        elementaryBytes.insert(elementaryBytes.end(), avPacketOut.data, avPacketOut.data + avPacketOut.size);
      }
    } else if (avPacket.data && avPacket.size) {
      elementaryBytes.insert(elementaryBytes.end(), avPacket.data, avPacket.data + avPacket.size);
    }
  };
  int ret = 0;
  bool isDone = false, gotVideo = false;
  while (!isDone) {
    ret = av_read_frame(fmtc, &pktSrc);
    gotVideo = (pktSrc.stream_index == videoStream);
    isDone = (ret < 0) || gotVideo;

    if (pSEIBytes && ppSEI) {
      // Bitstream filter lazy init;
      // We don't do this in constructor as user may not be needing SEI
      // extraction at all;
      if (!bsfc_sei) {
        std::cout << "Initializing SEI filter;" << std::endl;

        // SEI has NAL type 6 for H.264 and NAL type 39 & 40 for H.265;
        const std::string sei_filter = is_mp4H264   ? "filter_units=pass_types=6"
                                       : is_mp4HEVC ? "filter_units=pass_types=39-40"
                                                    : "unknown";
        ret = av_bsf_list_parse_str(sei_filter.c_str(), &bsfc_sei);
        if (0 > ret) {
          throw std::runtime_error("Error initializing " + sei_filter + " bitstream filter: " + av_err2str(ret));
        }

        ret = avcodec_parameters_copy(bsfc_sei->par_in, fmtc->streams[videoStream]->codecpar);
        if (0 != ret) {
          throw std::runtime_error("Error copying codec parameters: " + sei_filter +
                                   " bitstream filter: " + av_err2str(ret));
        }

        ret = av_bsf_init(bsfc_sei);
        if (0 != ret) {
          throw std::runtime_error("Error initializing " + sei_filter + " bitstream filter: " + av_err2str(ret));
        }
      }

      // Extract SEI NAL units from packet;
      auto pCopyPacket = av_packet_clone(&pktSrc);
      appendBytes(seiBytes, *pCopyPacket, pktSei, bsfc_sei, videoStream, true);
      av_packet_free(&pCopyPacket);
    }

    /* Unref non-desired packets as we don't support them yet;
     */
    if (pktSrc.stream_index != videoStream) {
      av_packet_unref(&pktSrc);
      continue;
    }
  }

  if (ret < 0) {
    std::cerr << "Failed to read frame: " << av_err2str(ret) << std::endl;
    return false;
  }
  const bool bsf_needed = is_mp4H264 || is_mp4HEVC;
  appendBytes(annexbBytes, pktSrc, pktDst, bsfc_annexb, videoStream, bsf_needed);

  pVideo = annexbBytes.data();
  rVideoBytes = annexbBytes.size();

  /* Save packet props to PacketData, decoder will use it later.
   * If no BSF filters were applied, copy input packet props.
   */
  if (!bsf_needed) {
    av_packet_copy_props(&pktDst, &pktSrc);
  }

  last_packet_data.pts = pktDst.pts;
  last_packet_data.dts = pktDst.dts;
  last_packet_data.pos = pktDst.pos;
  last_packet_data.duration = pktDst.duration;

  pktData = last_packet_data;

  if (pSEIBytes && ppSEI && !seiBytes.empty()) {
    *ppSEI = seiBytes.data();
    *pSEIBytes = seiBytes.size();
  }

  return true;
}
bool FFmpegDemuxer::Seek(SeekContext &seekCtx, uint8_t *&pVideo, size_t &rVideoBytes, PacketData &pktData,
                         uint8_t **ppSEI, size_t *pSEIBytes) {
  if (!is_seekable) {
    std::cerr << "Seek isn't supported for this input." << std::endl;
    return false;
  }

  // Convert timestamp in time units to timestamp in stream base units;
  auto ts_from_time = [&](int64_t ts_sec) {
    auto const ts_tbu = (int64_t)(ts_sec * AV_TIME_BASE);
    AVRational factor;
    factor.num = 1;
    factor.den = AV_TIME_BASE;
    return av_rescale_q(ts_tbu, factor, fmtc->streams[videoStream]->time_base);
  };

  // Convert frame number to timestamp;
  auto ts_from_num = [&](int64_t frame_num) {
    auto const ts_sec = (double)seekCtx.seek_frame / GetFramerate();
    return ts_from_time(ts_sec);
  };

  // Seek for single frame;
  auto seek_frame = [&](SeekContext const &seek_ctx, int flags) {
    bool seek_backward = false;
    int ret = 0;

    switch (seek_ctx.crit) {
      case BY_NUMBER:
        seek_backward = last_packet_data.dts > seek_ctx.seek_frame * pktDst.duration;
        ret = av_seek_frame(fmtc, GetVideoStreamIndex(), ts_from_num(seek_ctx.seek_frame),
                            seek_backward ? AVSEEK_FLAG_BACKWARD | flags : flags);
        if (ret < 0) throw std::runtime_error("Error seeking for frame: " + std::string(av_err2str(ret)));
        break;
      case BY_TIMESTAMP:
        seek_backward = last_packet_data.dts > seek_ctx.seek_frame;
        ret = av_seek_frame(fmtc, GetVideoStreamIndex(), ts_from_time(seek_ctx.seek_frame),
                            seek_backward ? AVSEEK_FLAG_BACKWARD | flags : flags);
        break;
      default:
        throw std::runtime_error("Invalid seek mode");
    }
    return;
  };

  // Check if frame satisfies seek conditions;
  auto is_seek_done = [&](PacketData &pkt_data, SeekContext const &seek_ctx) {
    int64_t target_ts = 0;

    switch (seek_ctx.crit) {
      case BY_NUMBER:
        target_ts = ts_from_num(seek_ctx.seek_frame);
        break;
      case BY_TIMESTAMP:
        target_ts = ts_from_time(seek_ctx.seek_frame);
        break;
      default:
        throw std::runtime_error("Invalid seek criteria");
        break;
    }

    if (pkt_data.dts == target_ts) {
      return 0;
    } else if (pkt_data.dts > target_ts) {
      return 1;
    } else {
      return -1;
    };
  };

  // This will seek for exact frame number;
  // Note that decoder may not be able to decode such frame;
  auto seek_for_exact_frame = [&](PacketData &pkt_data, SeekContext &seek_ctx) {
    // Repetititive seek until seek condition is satisfied;
    SeekContext tmp_ctx(seek_ctx.seek_frame);
    seek_frame(tmp_ctx, AVSEEK_FLAG_ANY);

    int condition = 0;
    do {
      Demux(pVideo, rVideoBytes, pkt_data, ppSEI, pSEIBytes);
      condition = is_seek_done(pkt_data, seek_ctx);

      // We've gone too far and need to seek backwards;
      if (condition > 0) {
        tmp_ctx.seek_frame--;
        seek_frame(tmp_ctx, AVSEEK_FLAG_ANY);
      }
      // Need to read more frames until we reach requested number;
      else if (condition < 0) {
        continue;
      }
    } while (0 != condition);

    seek_ctx.out_frame_pts = pkt_data.pts;
    seek_ctx.out_frame_duration = pkt_data.duration;
  };

  // Seek for closest key frame in the past;
  auto seek_for_prev_key_frame = [&](PacketData &pkt_data, SeekContext &seek_ctx) {
    // Repetititive seek until seek condition is satisfied;
    auto tmp_ctx = seek_ctx;
    seek_frame(tmp_ctx, AVSEEK_FLAG_BACKWARD);

    Demux(pVideo, rVideoBytes, pkt_data, ppSEI, pSEIBytes);
    seek_ctx.out_frame_pts = pkt_data.pts;
    seek_ctx.out_frame_duration = pkt_data.duration;
  };

  switch (seekCtx.mode) {
    case EXACT_FRAME:
      seek_for_exact_frame(pktData, seekCtx);
      break;
    case PREV_KEY_FRAME:
      seek_for_prev_key_frame(pktData, seekCtx);
      break;
    default:
      throw std::runtime_error("Unsupported seek mode");
      break;
  }

  return true;
}