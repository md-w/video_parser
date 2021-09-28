#include "PyVideoParser.hpp"
using namespace vp;
PyVideoParser::PyVideoParser() : PyVideoParser("") {}
PyVideoParser::PyVideoParser(const std::string &pathToFile)
    : PyVideoParser(pathToFile, std::map<std::string, std::string>()) {}
PyVideoParser::PyVideoParser(const std::string &pathToFile, const std::map<std::string, std::string> &ffmpeg_options) {
  upVideoParser.reset(VideoParser::Make(pathToFile, ffmpeg_options));
}
bool PyVideoParser::DemuxSinglePacket(vector_t<uint8_t> &frame, vector_t<uint8_t> &packet) { return false; };
bool PyVideoParser::DemuxSinglePacket(vector_t<uint8_t> &packet) {
  return DemuxSinglePacket(vector_t<uint8_t>(), packet);
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

PyVideoParser::~PyVideoParser() {}

#ifdef GENERATE_PYTHON_BINDINGS
PYBIND11_MODULE(PyVideoParser, m) {
  m.doc() = "Python bindings for video parsing";
  py::register_exception<VideoParserException>(m, "VideoParserException");

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
      .def("Codec", &PyVideoParser::Codec);
}

#endif