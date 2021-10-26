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
    _session = Ort::Session(_env, _model_path.c_str(), _session_options);
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
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
  }

  std::cout << "Success to load " << _model_path_str << std::endl;
}
void UltraFace::faceDetector(cv::Mat& orig_image_bgr, float threshold)
{
  try {
    std::vector<int64_t> input_dims = _input_dims_list.at(0);
    cv::resize(orig_image_bgr, _resized_image_bgr,
               cv::Size(input_dims.at(2), input_dims.at(3)),
               cv::InterpolationFlags::INTER_CUBIC);

    cv::cvtColor(_resized_image_bgr, _resized_image_rgb,
                 cv::ColorConversionCodes::COLOR_BGR2RGB);

    _resized_image_rgb.convertTo(_resized_image, CV_32F, 1.0 / 128);

    cv::Mat channels[3];
    cv::split(_resized_image, channels);

    channels[0] = (channels[0] - (127.0 / 128));
    channels[1] = (channels[1] - (127.0 / 128));
    channels[2] = (channels[2] - (127.0 / 128));

    cv::merge(channels, 3, _resized_image);
    // HWC to CHW
    cv::dnn::blobFromImage(_resized_image, _preprocessed_image);

    _input_image_tensor_values.assign(_preprocessed_image.begin<float>(),
                                      _preprocessed_image.end<float>());

    // assert(("Output tensor size should equal to the label set size.",
    //         labels.size() == outputTensorSize));

    std::vector<Ort::Value> input_tensors;
    std::vector<Ort::Value> output_tensors;
    {

      int64_t input_tensor_size = vectorProduct<int64_t>(input_dims);

      input_tensors.push_back(Ort::Value::CreateTensor<float>(
          _memory_info, _input_image_tensor_values.data(), input_tensor_size,
          input_dims.data(), input_dims.size()));
    }

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
    // cv::dnn::NMSBoxes();
    // std::cout << "Output confidence tensor values: " << _output_confidence_tensor_values << std::endl;
    // std::cout << "Output boxes tensor values: " << _output_boxes_tensor_values << std::endl;
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
}
int UltraFace::load() { return -1; }
void UltraFace::stop() {}
void UltraFace::getResult(cv::Mat& inMat) { faceDetector(inMat); }
