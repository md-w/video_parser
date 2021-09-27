#include "PyVideoParser.hpp"
using namespace vp;

PyVideoParser::PyVideoParser(const std::string& pathToFile) {
  throw VideoParserException();
}

PyVideoParser::~PyVideoParser() {}

#ifdef GENERATE_PYTHON_BINDINGS
PYBIND11_MODULE(PyVideoParser, m) {
  m.doc() = "Python bindings for video parsing";
  py::register_exception<VideoParserException>(m, "VideoParserException");

  py::class_<PyVideoParser>(m, "PyVideoParser")
      .def(py::init<const std::string&>());
}
#endif