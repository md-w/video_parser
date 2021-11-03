#include "PyOpencvWrapper.h"
#include <chrono>
#include <iostream>
#include <thread>

using namespace vp;
PyOpencvWrapper::PyOpencvWrapper()
{
  ultra_face.reset(new UltraFace());
  std::cout << "PyOpencvWrapper constructor" << std::endl;
}
PyOpencvWrapper::~PyOpencvWrapper()
{
  std::cout << "PyOpencvWrapper destructor" << std::endl;
}
void PyOpencvWrapper::AnalyticsEventHandler(
    AnalyticsEventHandlerCallback callback)
{
  std::cout << "PyOpencvWrapper AnalyticsEventHandler" << std::endl;

  analytics_event_handlers.push_back(callback);
}
void PyOpencvWrapper::Call(AnalyticsEventHandlerInput& image)
{
  std::vector<uint8_t> temp;
  for (size_t i = 0; i < 10; i++)
  {
    temp.push_back(i);
  }

  for (auto&& analytics_event_handler : analytics_event_handlers) {
    image.resize({10}, false);
#ifdef GENERATE_PYTHON_BINDINGS
      //memcpy(image.mutable_data(), elementaryVideo->GetDataAs<void>(), elementaryVideo->GetRawMemSize());
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
}
#endif