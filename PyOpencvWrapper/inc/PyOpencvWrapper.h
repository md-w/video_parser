#pragma once
#ifndef PyOpencvWrapper_h
#define PyOpencvWrapper_h

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;
template <class T> using vector_t = py::array_t<T>;
#else
template <class T> using vector_t = std::vector<T>;
#endif

#ifdef GENERATE_PYTHON_BINDINGS
#define VP_API
#else
#if (defined(_WIN32) || defined(_WIN32_WCE))
#if defined(VP_EXPORTS)
#define VP_API __declspec(dllexport)
#else
#define VP_API __declspec(dllimport)
#endif
#endif

#if !defined(VP_API)
#if defined(__GNUC__) && (__GNUC__ >= 4)
#define VP_API __attribute__((visibility("default")))
#else
#define VP_API
#endif
#endif
#endif

namespace vp
{
class VP_API OpencvWrapperException : public std::runtime_error
{
public:
  OpencvWrapperException(const std::string& str) : std::runtime_error(str) {}
  OpencvWrapperException() : OpencvWrapperException("Opencv wrapper Exception")
  {
  }
  ~OpencvWrapperException() {}
};
class VP_API PyOpencvWrapper
{

public:
  PyOpencvWrapper();
  ~PyOpencvWrapper();
};
} // namespace vp

#endif