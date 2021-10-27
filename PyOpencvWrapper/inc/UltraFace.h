#pragma once

#ifndef UltraFace_H
#define UltraFace_H
#include "Sort_tracker.h"
#include <onnxruntime_cxx_api.h>
#include <opencv2/opencv.hpp>
typedef struct _FaceInfo {
  cv::Rect rect;
  int id;
  float prob;
} FaceInfo;

class UltraFace
{
private:
  Ort::Env _env;
  Ort::SessionOptions _session_options;
  std::wstring _model_path;
  std::string _model_path_str;
  Ort::Session _session{nullptr};
  std::vector<char*> _input_names;
  std::vector<char*> _output_names;
  std::vector<std::vector<int64_t>> _input_dims_list;
  std::vector<std::vector<int64_t>> _output_dims_list;
  // int64_t _input_tensor_size;
  // int64_t _output_tensor_size;
  cv::Mat _resized_image_bgr;
  cv::Mat _resized_image_rgb;
  cv::Mat _resized_image;
  cv::Mat _preprocessed_image;
  std::vector<float> _input_image_tensor_values;
  std::vector<float> _output_confidence_tensor_values;
  std::vector<float> _output_boxes_tensor_values;
  Ort::MemoryInfo _memory_info;

  vtpl::Sort_tracker* _pTracker{nullptr};
  std::vector<vtpl::TrackingBox> _pTracker_result;
  std::vector<vtpl::TrackingBox> _pTracker_result1;

  void faceDetector(cv::Mat& origImage, float threshold = 0.7,
                    float iou_threshold = 0.5);
public:
  UltraFace();
  ~UltraFace();
  int load();
  void stop();
  void getResult(cv::Mat& inMat);
};
#endif