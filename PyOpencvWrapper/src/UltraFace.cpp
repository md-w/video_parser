#include <iostream>
#include <numeric>
#include <vector>

#include "UltraFace.h"

#ifdef _WIN32
#define strcasecmp _wcsicmp
#define strdup _strdup
#endif

template <typename T> T vectorProduct(const std::vector<T>& v)
{
  return accumulate(v.begin(), v.end(), (T)1, std::multiplies<T>());
}

/**
 * @brief Operator overloading for printing vectors
 * @tparam T
 * @param os
 * @param v
 * @return std::ostream&
 */
template <typename T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& v)
{
  os << "[";
  for (int i = 0; i < v.size(); ++i) {
    os << v[i];
    if (i != v.size() - 1) {
      os << ", ";
    }
  }
  os << "]";
  return os;
}

/**
 * @brief Print ONNX tensor data type
 * https://github.com/microsoft/onnxruntime/blob/rel-1.6.0/include/onnxruntime/core/session/onnxruntime_c_api.h#L93
 * @param os
 * @param type
 * @return std::ostream&
 */
std::ostream& operator<<(std::ostream& os,
                         const ONNXTensorElementDataType& type)
{
  switch (type) {
  case ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_UNDEFINED:
    os << "undefined";
    break;
  case ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT:
    os << "float";
    break;
  case ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT8:
    os << "uint8_t";
    break;
  case ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_INT8:
    os << "int8_t";
    break;
  case ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT16:
    os << "uint16_t";
    break;
  case ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_INT16:
    os << "int16_t";
    break;
  case ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_INT32:
    os << "int32_t";
    break;
  case ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_INT64:
    os << "int64_t";
    break;
  case ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_STRING:
    os << "std::string";
    break;
  case ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_BOOL:
    os << "bool";
    break;
  case ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_FLOAT16:
    os << "float16";
    break;
  case ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_DOUBLE:
    os << "double";
    break;
  case ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT32:
    os << "uint32_t";
    break;
  case ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_UINT64:
    os << "uint64_t";
    break;
  case ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_COMPLEX64:
    os << "float real + float imaginary";
    break;
  case ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_COMPLEX128:
    os << "double real + float imaginary";
    break;
  case ONNXTensorElementDataType::ONNX_TENSOR_ELEMENT_DATA_TYPE_BFLOAT16:
    os << "bfloat16";
    break;
  default:
    break;
  }

  return os;
}

UltraFace::UltraFace()
    : _env(ORT_LOGGING_LEVEL_ERROR, "UltraFace"), _memory_info{nullptr}
{
  cv::setNumThreads(1);
  _model_path = L"version-RFB-320.onnx";
  std::transform(_model_path.begin(), _model_path.end(),
                 std::back_inserter(_model_path_str),
                 [](wchar_t c) { return (char)c; });

  _session_options.SetIntraOpNumThreads(1);
  _session_options.SetGraphOptimizationLevel(
      GraphOptimizationLevel::ORT_ENABLE_EXTENDED);
  std::cout << "Trying to load " << _model_path_str << std::endl;
  try {
#ifdef WIN32
    _session = Ort::Session(_env, _model_path.c_str(), _session_options);
#else
  _session = Ort::Session(_env, _model_path_str.c_str(), _session_options);
#endif
    Ort::AllocatorWithDefaultOptions allocator;

    size_t numInputNodes = _session.GetInputCount();
    size_t numOutputNodes = _session.GetOutputCount();

    std::cout << "Number of Input Nodes: " << numInputNodes << std::endl;
    std::cout << "Number of Output Nodes: " << numOutputNodes << std::endl;
    assert(("Number of Input Nodes should equal to one.", numInputNodes == 1));
    assert(
        ("Number of Output Nodes should equal to two.", numOutputNodes == 2));
    for (size_t i = 0; i < numInputNodes; i++) {
      _input_names.push_back(strdup(_session.GetInputName(i, allocator)));
      std::cout << "Input Name: " << _input_names.at(i) << std::endl;

      Ort::TypeInfo inputTypeInfo = _session.GetInputTypeInfo(i);
      auto inputTensorInfo = inputTypeInfo.GetTensorTypeAndShapeInfo();

      ONNXTensorElementDataType inputType = inputTensorInfo.GetElementType();
      std::cout << "Input Type: " << inputType << std::endl;
      std::vector<int64_t> input_dims = inputTensorInfo.GetShape();
      _input_dims_list.push_back(input_dims);
      int64_t input_tensor_size = vectorProduct<int64_t>(input_dims);
      _input_image_tensor_values.resize(input_tensor_size);
      std::cout << "Input Dimensions: " << input_dims << std::endl;
    }
    for (size_t i = 0; i < numOutputNodes; i++) {
      _output_names.push_back(strdup(_session.GetOutputName(i, allocator)));
      std::cout << "Output Name: " << _output_names.at(i) << std::endl;

      Ort::TypeInfo outputTypeInfo = _session.GetOutputTypeInfo(i);
      auto outputTensorInfo = outputTypeInfo.GetTensorTypeAndShapeInfo();

      ONNXTensorElementDataType outputType = outputTensorInfo.GetElementType();
      std::cout << "Output Type: " << outputType << std::endl;

      std::vector<int64_t> output_dims = outputTensorInfo.GetShape();
      int64_t output_tensor_size = vectorProduct<int64_t>(output_dims);
      if (i == 0) { // score
        _output_confidence_tensor_values.resize(output_tensor_size);
      } else if (i == 1) { // boxes
        _output_boxes_tensor_values.resize(output_tensor_size);
      } else {
        throw std::runtime_error("Unexpected output dimension");
      }

      _output_dims_list.push_back(output_dims);
      std::cout << "Output Dimensions: " << output_dims << std::endl;
    }
    std::cout << "Output names: " << _output_names << std::endl;
    _memory_info = Ort::MemoryInfo::CreateCpu(
        OrtAllocatorType::OrtArenaAllocator, OrtMemType::OrtMemTypeDefault);
    _pTracker = new vtpl::Sort_tracker(10, 1, 0.5, false);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    throw std::runtime_error(e.what());
  }

  std::cout << "Success to load " << _model_path_str << std::endl;
}

void UltraFace::faceDetector(cv::Mat& orig_image_bgr, float threshold,
                             float iou_threshold)
{
  try {
    bool show_img = false;
    std::vector<int64_t> input_dims = _input_dims_list.at(0);
    int image_height = orig_image_bgr.rows;
    int image_width = orig_image_bgr.cols;

    int network_height = input_dims.at(2);
    int network_width = input_dims.at(3);

    cv::resize(orig_image_bgr, _resized_image_bgr,
               cv::Size(network_height, network_width), cv::INTER_CUBIC);

    // std::cout << _resized_image_bgr.rowRange(0, 3).colRange(0, 1) <<
    // std::endl;

    cv::cvtColor(_resized_image_bgr, _resized_image_rgb,
                 cv::ColorConversionCodes::COLOR_BGR2RGB);

    cv::Scalar image_std = cv::Scalar(128.0);
    // HWC to CHW
    _preprocessed_image = cv::dnn::blobFromImage(
        _resized_image_rgb, 1 / 127.5, cv::Size(network_width, network_height),
        cv::Scalar::all(127.5));

    _input_image_tensor_values.assign(_preprocessed_image.begin<float>(),
                                      _preprocessed_image.end<float>());

    std::vector<Ort::Value> input_tensors;
    {

      int64_t input_tensor_size = vectorProduct<int64_t>(input_dims);

      input_tensors.push_back(Ort::Value::CreateTensor<float>(
          _memory_info, _input_image_tensor_values.data(), input_tensor_size,
          input_dims.data(), input_dims.size()));
    }

    std::vector<Ort::Value> output_tensors;
    {
      std::vector<int64_t> output_dims = _output_dims_list.at(0);

      int64_t output_tensor_size = vectorProduct<int64_t>(output_dims);

      output_tensors.push_back(Ort::Value::CreateTensor<float>(
          _memory_info, _output_confidence_tensor_values.data(),
          output_tensor_size, output_dims.data(), output_dims.size()));
    }
    {
      std::vector<int64_t> output_dims = _output_dims_list.at(1);

      int64_t output_tensor_size = vectorProduct<int64_t>(output_dims);

      output_tensors.push_back(Ort::Value::CreateTensor<float>(
          _memory_info, _output_boxes_tensor_values.data(), output_tensor_size,
          output_dims.data(), output_dims.size()));
    }
    _session.Run(Ort::RunOptions{nullptr}, _input_names.data(),
                 input_tensors.data(), _input_names.size(),
                 _output_names.data(), output_tensors.data(),
                 _output_names.size());

    int count = _output_dims_list[0].at(1);

    std::vector<cv::Rect> local_boxes;
    std::vector<float> local_confidences;
    std::vector<int> nms_indices;
    for (size_t i = 0; i < count; i++) {
      float prob = _output_confidence_tensor_values.at(i * 2 + 1);
      if (prob < threshold) {
        continue;
      }
      int left = _output_boxes_tensor_values.at(i * 4 + 0) * image_width;
      int top = _output_boxes_tensor_values.at(i * 4 + 1) * image_height;
      int right = _output_boxes_tensor_values.at(i * 4 + 2) * image_width;
      int bottom = _output_boxes_tensor_values.at(i * 4 + 3) * image_height;
      local_confidences.push_back(prob);
      local_boxes.push_back(
          cv::Rect(left, top, (right - left + 1), (bottom - top + 1)));
    }
    std::vector<cv::Rect> nms_boxes;
    std::vector<float> nms_confidences;
    std::vector<int> nms_class_ids;
    cv::dnn::NMSBoxes(local_boxes, local_confidences, threshold, iou_threshold,
                      nms_indices);

    std::vector<FaceInfo> face_list;
    for (size_t i = 0; i < nms_indices.size(); i++) {
      size_t idx = nms_indices[i];
      nms_boxes.push_back(local_boxes[idx]);
      nms_confidences.push_back(local_confidences[idx]);
      nms_class_ids.push_back(0);
      FaceInfo temp_face;
      temp_face.id = 0;
      temp_face.prob = local_confidences[idx];
      temp_face.rect = local_boxes[idx];
      face_list.push_back(temp_face);
    }

    // Tracking - SORT
    vtpl::TrackingBox detected_box;
    for (size_t idx = 0; idx < nms_boxes.size(); ++idx) {
      cv::Rect box = nms_boxes[idx];
      detected_box.frame_id = 0;
      detected_box.id = 0;
      detected_box.box = box;
      _pTracker_result.push_back(detected_box);
    }
    _pTracker_result1 = _pTracker->getResult(_pTracker_result, 1.0,
                                             image_height, image_width, true);
    _pTracker_result.clear();
    for (auto& face : face_list) {
      for (auto& track : _pTracker_result1) {
        cv::Rect temp_item_box;
        temp_item_box.x = int(track.box.x);
        temp_item_box.y = int(track.box.y);
        temp_item_box.width = int(track.box.width);
        temp_item_box.height = int(track.box.height);

        bool is_lp_overlapped =
            (temp_item_box & face.rect).area() > (face.rect.area() * .5);
        if (!is_lp_overlapped)
          continue;
        face.id = track.id;
      }
    }

    if (show_img) {
      for (auto& item : _pTracker_result1) {
        std::string s = std::to_string(item.id);
        cv::putText(orig_image_bgr, s, cv::Point(item.box.x, item.box.y), 1,
                    2.0, CV_RGB(255, 255, 0), 3);
      }
    }
    _pTracker_result1.clear();

    if (show_img) {
      for (auto& face : face_list) {
        cv::Rect box = face.rect;
        int left = cv::max(0, box.x);
        int top = cv::max(0, box.y);
        int right = cv::min(box.width + box.x, image_width - 1);
        int bottom = cv::min(box.height + box.y, image_height - 1);
        cv::rectangle(orig_image_bgr, cv::Point(left, top),
                      cv::Point(right, bottom), CV_RGB(255, 0, 0), 4);
      }
    }

    face_list.clear();

    // cv::imwrite("output.jpg", orig_image_bgr);
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }
}
UltraFace::~UltraFace()
{
  for (auto&& i : _input_names) {
    free(i);
  }
  for (auto&& i : _output_names) {
    free(i);
  }
  if (_pTracker)
    delete _pTracker;
}
int UltraFace::load() { return -1; }
void UltraFace::stop() {}
void UltraFace::getResult(cv::Mat& inMat) { faceDetector(inMat); }
