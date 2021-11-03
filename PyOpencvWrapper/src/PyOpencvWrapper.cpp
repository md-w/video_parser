#include "PyOpencvWrapper.h"
#include <iostream>
using namespace vp;
PyOpencvWrapper::PyOpencvWrapper()
{
  std::cout << "PyOpencvWrapper constructor" << std::endl;
}
PyOpencvWrapper::~PyOpencvWrapper()
{
  std::cout << "PyOpencvWrapper destructor" << std::endl;
}

#ifdef GENERATE_PYTHON_BINDINGS
PYBIND11_MODULE(PyOpencvWrapper, m)
{
  m.doc() = "Python bindings for opencv wrapper";
  py::register_exception<OpencvWrapperException>(m, "OpencvWrapperException");
  py::class_<PyOpencvWrapper>(m, "PyOpencvWrapper").def(py::init());
}
#endif