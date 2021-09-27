#ifndef PyVideoParser_H
#define PyVideoParser_H

#pragma once

#ifdef GENERATE_PYTHON_BINDINGS
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
namespace py = pybind11;
#endif

#include <stdexcept>
#include <string>
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
namespace vp {
class VP_API VideoParserException : public std::runtime_error {
 public:
  VideoParserException(const std::string &str) : std::runtime_error(str) {}
  VideoParserException() : VideoParserException("Video Parser Exception") {}
  ~VideoParserException() {}
};

class VP_API PyVideoParser {
 public:
  PyVideoParser(const std::string &pathToFile);
  ~PyVideoParser();
};

}  // namespace vp

#endif