#include "VideoParser.hpp"

#include "FFmpegDemuxer.hpp"
#include "MemoryInterfaces.hpp"

namespace vp {
struct VideoParser_Impl {
  FFmpegDemuxer demuxer;
  size_t videoBytes = 0U;

  Buffer *pElementaryVideo;
  Buffer *pMuxingParams;
  Buffer *pSei;
  Buffer *pPktData;

  VideoParser_Impl() = delete;
  VideoParser_Impl(const VideoParser_Impl &other) = delete;
  VideoParser_Impl &operator=(const VideoParser_Impl &other) = delete;

  explicit VideoParser_Impl(const std::string &url, const std::map<std::string, std::string> &ffmpeg_options)
      : demuxer(url, ffmpeg_options) {
    pElementaryVideo = Buffer::MakeOwnMem(0U);
    pMuxingParams = Buffer::MakeOwnMem(sizeof(MuxingParams));
    pSei = Buffer::MakeOwnMem(0U);
    pPktData = Buffer::MakeOwnMem(0U);
  }

  ~VideoParser_Impl() {
    delete pElementaryVideo;
    delete pMuxingParams;
    delete pSei;
    delete pPktData;
  }
};

void VideoParser::GetParams(struct MuxingParams &params) {
  params.videoContext.width = pImpl->demuxer.GetWidth();
  params.videoContext.height = pImpl->demuxer.GetHeight();
  params.videoContext.avgFrameRate = pImpl->demuxer.GetAvgFramerate();
  params.videoContext.gop_size = pImpl->demuxer.GetGopSize();
  params.videoContext.num_frames = pImpl->demuxer.GetNumFrames();
  params.videoContext.frameRate = pImpl->demuxer.GetFramerate();
  params.videoContext.avgFrameRate = pImpl->demuxer.GetAvgFramerate();
  params.videoContext.timeBase = pImpl->demuxer.GetTimebase();
  params.videoContext.is_vfr = pImpl->demuxer.IsVFR();
  params.videoContext.streamIndex = pImpl->demuxer.GetVideoStreamIndex();
  params.videoContext.codec = pImpl->demuxer.GetVideoCodec();
}

TaskExecStatus VideoParser::Execute() {
  ClearOutputs();
  uint8_t *pVideo = nullptr;
  MuxingParams params = {0};
  PacketData pkt_data = {0};
  auto &videoBytes = pImpl->videoBytes;
  auto &demuxer = pImpl->demuxer;
  uint8_t *pSEI = nullptr;
  size_t seiBytes = 0U;
  bool needSEI = (nullptr != GetInput(0U));
  auto pSeekCtxBuf = (Buffer *)GetInput(1U);
  if (pSeekCtxBuf) {
    SeekContext seek_ctx = *pSeekCtxBuf->GetDataAs<SeekContext>();
    auto ret = demuxer.Seek(seek_ctx, pVideo, videoBytes, pkt_data, needSEI ? &pSEI : nullptr, &seiBytes);
    if (!ret) {
      return TaskExecStatus::TASK_EXEC_FAIL;
    }
  } else if (!demuxer.Demux(pVideo, videoBytes, pkt_data, needSEI ? &pSEI : nullptr, &seiBytes)) {
    return TaskExecStatus::TASK_EXEC_FAIL;
  }

  if (videoBytes) {
    pImpl->pElementaryVideo->Update(videoBytes, pVideo);
    SetOutput(pImpl->pElementaryVideo, 0U);

    GetParams(params);
    pImpl->pMuxingParams->Update(sizeof(MuxingParams), &params);
    SetOutput(pImpl->pMuxingParams, 1U);
  }

  if (pSEI) {
    pImpl->pSei->Update(seiBytes, pSEI);
    SetOutput(pImpl->pSei, 2U);
  }

  pImpl->pPktData->Update(sizeof(pkt_data), &pkt_data);
  SetOutput((Token *)pImpl->pPktData, 3U);

  return TaskExecStatus::TASK_EXEC_SUCCESS;
}
VideoParser::~VideoParser() { delete pImpl; }
VideoParser *VideoParser::Make(const std::string &url, const std::map<std::string, std::string> &ffmpeg_options) {
  return new VideoParser(url, ffmpeg_options);
}

VideoParser::VideoParser(const std::string &url, const std::map<std::string, std::string> &ffmpeg_options)
    : Task("VideoParser", VideoParser::numInputs, VideoParser::numOutputs),
      pImpl(new VideoParser_Impl(url, ffmpeg_options)) {}
}  // namespace vp