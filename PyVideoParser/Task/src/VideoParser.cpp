#include "VideoParser.hpp"

#include "FFmpegDemuxer.hpp"
#include "MemoryInterfaces.hpp"
#include "SimpleBinStream.hpp"
namespace vp {

class MemoryDataProvider : public DataProvider {
 private:
  simple::mem_ostream<simple::LittleEndian> out;
  simple::mem_istream<simple::LittleEndian> in;

 public:
  MemoryDataProvider() : out(simple::mem_ostream<simple::LittleEndian>()), in(out.get_internal_vec()) {}
  ~MemoryDataProvider() {}
  int GetData(uint8_t *pBuf, int nBuf) {
    in.read((char *)pBuf, nBuf);
    return nBuf;
  }
  void WriteData(uint8_t *pBuf, int nBuf) { out.write((const char *)pBuf, nBuf); }
};

struct VideoParser_Impl {
  FFmpegDemuxer *pDemuxer;
  MemoryDataProvider *pMemoryDataProvider;
  size_t videoBytes = 0U;

  Buffer *pElementaryVideo;
  Buffer *pMuxingParams;
  Buffer *pSei;
  Buffer *pPktData;

  VideoParser_Impl() = delete;
  VideoParser_Impl(const VideoParser_Impl &other) = delete;
  VideoParser_Impl &operator=(const VideoParser_Impl &other) = delete;

  explicit VideoParser_Impl(const std::string &url, const std::map<std::string, std::string> &ffmpeg_options) {
    if (url.length() > 0) {
      pDemuxer = new FFmpegDemuxer(url, ffmpeg_options);
    } else {
      pMemoryDataProvider = new MemoryDataProvider();
      pDemuxer = new FFmpegDemuxer(pMemoryDataProvider, ffmpeg_options);
    }

    pElementaryVideo = Buffer::MakeOwnMem(0U);
    pMuxingParams = Buffer::MakeOwnMem(sizeof(MuxingParams));
    pSei = Buffer::MakeOwnMem(0U);
    pPktData = Buffer::MakeOwnMem(0U);
  }

  ~VideoParser_Impl() {
    delete pDemuxer;
    delete pMemoryDataProvider;
    delete pElementaryVideo;
    delete pMuxingParams;
    delete pSei;
    delete pPktData;
  }
};

void VideoParser::GetParams(struct MuxingParams &params) {
  auto &demuxer = *pImpl->pDemuxer;
  params.videoContext.width = demuxer.GetWidth();
  params.videoContext.height = demuxer.GetHeight();
  params.videoContext.avgFrameRate = demuxer.GetAvgFramerate();
  params.videoContext.gop_size = demuxer.GetGopSize();
  params.videoContext.num_frames = demuxer.GetNumFrames();
  params.videoContext.frameRate = demuxer.GetFramerate();
  params.videoContext.avgFrameRate = demuxer.GetAvgFramerate();
  params.videoContext.timeBase = demuxer.GetTimebase();
  params.videoContext.is_vfr = demuxer.IsVFR();
  params.videoContext.streamIndex = demuxer.GetVideoStreamIndex();
  params.videoContext.codec = demuxer.GetVideoCodec();
}

TaskExecStatus VideoParser::Execute() {
  ClearOutputs();
  uint8_t *pVideo = nullptr;
  MuxingParams params = {0};
  PacketData pkt_data = {0};
  auto &videoBytes = pImpl->videoBytes;
  auto &demuxer = *pImpl->pDemuxer;
  auto &dataProvider = *pImpl->pMemoryDataProvider;
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
  } else {
    auto pFrame = (Buffer *)GetInput(2U);
    pFrame->GetRawMemSize();
    dataProvider.WriteData((uint8_t *)pFrame->GetRawMemPtr(), pFrame->GetRawMemSize());
    if (!demuxer.Demux(pVideo, videoBytes, pkt_data, needSEI ? &pSEI : nullptr, &seiBytes)) {
      return TaskExecStatus::TASK_EXEC_FAIL;
    }
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