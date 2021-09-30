#include "PyVideoParser.hpp"

#include <string.h>

#include "MemoryInterfaces.hpp"
using namespace vp;
PyVideoParser::PyVideoParser() : PyVideoParser("") {}
PyVideoParser::PyVideoParser(const std::string &pathToFile)
    : PyVideoParser(pathToFile, std::map<std::string, std::string>()) {}
PyVideoParser::PyVideoParser(const std::string &pathToFile, const std::map<std::string, std::string> &ffmpeg_options) {
  upVideoParser.reset(VideoParser::Make(pathToFile, ffmpeg_options));
}

bool PyVideoParser::DemuxSinglePacket(vector_t<uint8_t> &frame, vector_t<uint8_t> &packet) {
  Buffer *elementaryVideo = nullptr;
  if (frame.size() > 0) {
    auto pRawFrame = Buffer::Make(frame.size(),
#ifdef GENERATE_PYTHON_BINDINGS
                                  frame.mutable_data());
#else
                                  frame.data());
#endif

    upVideoParser->SetInput(pRawFrame, 0U);
    if (TaskExecStatus::TASK_EXEC_FAIL == upVideoParser->Execute()) {
      upVideoParser->ClearInputs();
      delete pRawFrame;
      return false;
    }
    delete pRawFrame;
    elementaryVideo = (Buffer *)upVideoParser->GetOutput(0U);
    if (elementaryVideo) {
      packet.resize({elementaryVideo->GetRawMemSize()}, false);
#ifdef GENERATE_PYTHON_BINDINGS
      memcpy(packet.mutable_data(), elementaryVideo->GetDataAs<void>(), elementaryVideo->GetRawMemSize());
#else
      memcpy(packet.data(), elementaryVideo->GetDataAs<void>(), elementaryVideo->GetRawMemSize());
#endif
      upVideoParser->ClearInputs();
      return true;

    } else {
      upVideoParser->ClearInputs();
      return false;
    }
  } else {
    do {
      if (TaskExecStatus::TASK_EXEC_FAIL == upVideoParser->Execute()) {
        upVideoParser->ClearInputs();
        return false;
      }
      elementaryVideo = (Buffer *)upVideoParser->GetOutput(0U);
    } while (!elementaryVideo);

    packet.resize({elementaryVideo->GetRawMemSize()}, false);
#ifdef GENERATE_PYTHON_BINDINGS
    memcpy(packet.mutable_data(), elementaryVideo->GetDataAs<void>(), elementaryVideo->GetRawMemSize());
#else
    memcpy(packet.data(), elementaryVideo->GetDataAs<void>(), elementaryVideo->GetRawMemSize());
#endif

    upVideoParser->ClearInputs();
    return true;
  }
};
bool PyVideoParser::DemuxSinglePacket(vector_t<uint8_t> &packet) {
  vector_t<uint8_t> t;
  return DemuxSinglePacket(t, packet);
}
uint32_t PyVideoParser::Width() const {
  MuxingParams params;
  upVideoParser->GetParams(params);
  return params.videoContext.width;
}
uint32_t PyVideoParser::Height() const {
  MuxingParams params;
  upVideoParser->GetParams(params);
  return params.videoContext.height;
}
std::string PyVideoParser::Codec() const {
  MuxingParams params;
  upVideoParser->GetParams(params);
  return params.videoContext.codec;
}
double PyVideoParser::Framerate() const {
  MuxingParams params;
  upVideoParser->GetParams(params);
  return params.videoContext.frameRate;
}
bool PyVideoParser::IsVFR() const {
  MuxingParams params;
  upVideoParser->GetParams(params);
  return params.videoContext.is_vfr;
}
double PyVideoParser::Timebase() const {
  MuxingParams params;
  upVideoParser->GetParams(params);
  return params.videoContext.timeBase;
}
uint32_t PyVideoParser::Numframes() const {
  MuxingParams params;
  upVideoParser->GetParams(params);
  return params.videoContext.num_frames;
}
void PyVideoParser::GetLastPacketData(PacketData &pkt_data) {
  auto pkt_data_buf = (Buffer *)upVideoParser->GetOutput(3U);
  if (pkt_data_buf) {
    auto pkt_data_ptr = pkt_data_buf->GetDataAs<PacketData>();
    pkt_data = *pkt_data_ptr;
  }
}
double PyVideoParser::AvgFramerate() const {
  MuxingParams params;
  upVideoParser->GetParams(params);
  return params.videoContext.avgFrameRate;
}
bool PyVideoParser::Seek(SeekContext &ctx, vector_t<uint8_t> &packet) {
  Buffer *elementaryVideo = nullptr;
  auto pSeekCtxBuf = std::shared_ptr<Buffer>(Buffer::MakeOwnMem(sizeof(ctx), &ctx));
  do {
    upVideoParser->SetInput((Token *)pSeekCtxBuf.get(), 1U);
    if (TaskExecStatus::TASK_EXEC_FAIL == upVideoParser->Execute()) {
      upVideoParser->ClearInputs();
      return false;
    }
    elementaryVideo = (Buffer *)upVideoParser->GetOutput(0U);
  } while (!elementaryVideo);

  packet.resize({elementaryVideo->GetRawMemSize()}, false);
#ifdef GENERATE_PYTHON_BINDINGS
  memcpy(packet.mutable_data(), elementaryVideo->GetDataAs<void>(), elementaryVideo->GetRawMemSize());
#else
  memcpy(packet.data(), elementaryVideo->GetDataAs<void>(), elementaryVideo->GetRawMemSize());
#endif

  auto pktDataBuf = (Buffer *)upVideoParser->GetOutput(3U);
  if (pktDataBuf) {
    auto pPktData = pktDataBuf->GetDataAs<PacketData>();
    ctx.out_frame_pts = pPktData->pts;
    ctx.out_frame_duration = pPktData->duration;
  }

  upVideoParser->ClearInputs();
  return true;
}

PyVideoParser::~PyVideoParser() {}

#ifdef GENERATE_PYTHON_BINDINGS
PYBIND11_MODULE(PyVideoParser, m) {
  m.doc() = "Python bindings for video parsing";
  py::register_exception<VideoParserException>(m, "VideoParserException");

  py::enum_<SeekMode>(m, "SeekMode")
      .value("EXACT_FRAME", SeekMode::EXACT_FRAME)
      .value("PREV_KEY_FRAME", SeekMode::PREV_KEY_FRAME)
      .export_values();

  py::enum_<SeekCriteria>(m, "SeekCriteria")
      .value("BY_NUMBER", SeekCriteria::BY_NUMBER)
      .value("BY_TIMESTAMP", SeekCriteria::BY_TIMESTAMP)
      .export_values();

  py::class_<SeekContext, std::shared_ptr<SeekContext>>(m, "SeekContext")
      .def(py::init<int64_t>(), py::arg("seek_frame"))
      .def(py::init<int64_t, SeekCriteria>(), py::arg("seek_frame"), py::arg("seek_criteria"))
      .def(py::init<int64_t, SeekMode>(), py::arg("seek_frame"), py::arg("mode"))
      .def(py::init<int64_t, SeekMode, SeekCriteria>(), py::arg("seek_frame"), py::arg("mode"),
           py::arg("seek_criteria"))
      .def_readwrite("seek_frame", &SeekContext::seek_frame)
      .def_readwrite("mode", &SeekContext::mode)
      .def_readwrite("out_frame_pts", &SeekContext::out_frame_pts)
      .def_readonly("num_frames_decoded", &SeekContext::num_frames_decoded);

  py::class_<PacketData, std::shared_ptr<PacketData>>(m, "PacketData")
      .def(py::init<>())
      .def_readwrite("pts", &PacketData::pts)
      .def_readwrite("dts", &PacketData::dts)
      .def_readwrite("pos", &PacketData::pos)
      .def_readwrite("poc", &PacketData::poc)
      .def_readwrite("duration", &PacketData::duration);

  py::class_<PyVideoParser>(m, "PyVideoParser")
      .def(py::init())
      .def(py::init<const std::string &>())
      .def(py::init<const std::string &, const std::map<std::string, std::string> &>())
      .def("DemuxSinglePacket", py::overload_cast<py::array_t<uint8_t> &>(&PyVideoParser::DemuxSinglePacket),
           py::call_guard<py::gil_scoped_release>())
      .def("DemuxSinglePacket",
           py::overload_cast<py::array_t<uint8_t> &, py::array_t<uint8_t> &>(&PyVideoParser::DemuxSinglePacket),
           py::call_guard<py::gil_scoped_release>())
      .def("Width", &PyVideoParser::Width)
      .def("Height", &PyVideoParser::Height)
      .def("Codec", &PyVideoParser::Codec)
      .def("Framerate", &PyVideoParser::Framerate)
      .def("AvgFramerate", &PyVideoParser::AvgFramerate)
      .def("IsVFR", &PyVideoParser::IsVFR)
      .def("Timebase", &PyVideoParser::Timebase)
      .def("Numframes", &PyVideoParser::Numframes)
      .def("LastPacketData", &PyVideoParser::GetLastPacketData)
      .def("Seek", &PyVideoParser::Seek);
  // .def("ColorSpace", &PyVideoParser::GetColorSpace)
  // .def("ColorRange", &PyVideoParser::GetColorRange);
}

#endif