#include "PyOpencvWrapper.h"
#include "VtplVideoFrame.h"
#include <chrono>
#include <iostream>
#include <thread>

using namespace vp;
PyOpencvWrapper::PyOpencvWrapper()
{
  ultra_face.reset(new UltraFace());
  std::cout << "PyOpencvWrapper constructor" << std::endl;
}
PyOpencvWrapper::~PyOpencvWrapper() { std::cout << "PyOpencvWrapper destructor" << std::endl; }
void PyOpencvWrapper::AnalyticsEventHandler(AnalyticsEventHandlerCallback callback)
{
  std::cout << "PyOpencvWrapper AnalyticsEventHandler" << std::endl;

  analytics_event_handlers.push_back(callback);
}
void PyOpencvWrapper::Call(AnalyticsEventHandlerInput& image)
{
  std::vector<uint8_t> temp;
  for (size_t i = 0; i < 10; i++) {
    temp.push_back(i);
  }

  for (auto&& analytics_event_handler : analytics_event_handlers) {
    image.resize({10}, false);
#ifdef GENERATE_PYTHON_BINDINGS
    // memcpy(image.mutable_data(), elementaryVideo->GetDataAs<void>(), elementaryVideo->GetRawMemSize());
    memcpy(image.mutable_data(), temp.data(), 10);
#else
    memcpy(image.data(), temp.data(), 10);
#endif
    analytics_event_handler(image);
  }
}

#ifdef GENERATE_PYTHON_BINDINGS
PYBIND11_MODULE(PyOpencvWrapper, m)
{
  m.doc() = "Python bindings for opencv wrapper";
  py::register_exception<OpencvWrapperException>(m, "OpencvWrapperException");
  py::class_<PyOpencvWrapper>(m, "PyOpencvWrapper")
      .def(py::init())
      .def("analytics_event_handler", &PyOpencvWrapper::AnalyticsEventHandler)
      .def("call", &PyOpencvWrapper::Call);

  py::class_<VtplVideoFrame>(m, "VtplVideoFrame", py::buffer_protocol())
      .def(py::init<>())
      .def_readonly("channel_id", &VtplVideoFrame::channel_id)
      .def_readonly("app_id",&VtplVideoFrame::app_id)
      .def_readonly("frame_id", &VtplVideoFrame::frame_id)
      .def_readonly("time_stamp",&VtplVideoFrame::time_stamp)
      .def_readonly("fps", &VtplVideoFrame::fps)
      .def_readonly("width", &VtplVideoFrame::width)
      .def_readonly("height", &VtplVideoFrame::height)
      .def_readonly("is_first_frame", &VtplVideoFrame::is_first_frame)
      .def_readonly("is_end_of_stream", &VtplVideoFrame::is_end_of_stream)
      .def_buffer([](VtplVideoFrame& f) -> py::buffer_info {
        return py::buffer_info(f.data(),                                 /* Pointer to buffer */
                               sizeof(uint8_t),                          /* Size of one scalar */
                               py::format_descriptor<uint8_t>::format(), /* Python struct-style format descriptor */
                               3,                                        /* Number of dimensions */
                               {f.rows(), f.cols(), 4},                     /* Buffer dimensions */
                               {sizeof(uint8_t) * f.cols() * 4,          /* Strides (in bytes) for each index */
                                sizeof(uint8_t) * 4,
                                sizeof(uint8_t) * 1});
      });
}
#endif