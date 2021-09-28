#pragma once
#ifndef PyVideoParser_H
#define PyVideoParser_H

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
template <class T>
using vector_t = py::array_t<T>;
#else
template <class T>
using vector_t = std::vector<T>;
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

#include "VideoParser.hpp"

namespace vp {
class VP_API VideoParserException : public std::runtime_error {
 public:
  VideoParserException(const std::string &str) : std::runtime_error(str) {}
  VideoParserException() : VideoParserException("Video Parser Exception") {}
  ~VideoParserException() {}
};

class VP_API PyVideoParser {
  std::unique_ptr<VideoParser> upVideoParser;

 public:
  PyVideoParser();
  PyVideoParser(const std::string &pathToFile);
  PyVideoParser(const std::string &pathToFile, const std::map<std::string, std::string> &ffmpeg_options);
  bool DemuxSinglePacket(vector_t<uint8_t> &packet);
  bool DemuxSinglePacket(vector_t<uint8_t> &frame, vector_t<uint8_t> &packet);
  uint32_t Width() const;
  uint32_t Height() const;
  std::string Codec() const;
  double Framerate() const;
  ~PyVideoParser();
};

}  // namespace vp

#endif