

#include "AVFReader.h"
#include "CodecManager.h"
#include "UltraFace.h"
#include "VReturnStatus.h"
#include "opencv2/opencv.hpp"
#include "utils.hpp"
#include <atomic>
#include <condition_variable>
#include <csignal>
#include <cstdlib>
#include <iomanip> // std::setprecision
#include <iostream>
#include <map>
#include <sstream>
#include <thread>
#include <vector>
// namespace

class FpsValue
{
public:
  FpsValue() : FpsValue(0.0, 0) {}
  FpsValue(float fps, int frame_id) : fps(fps), frame_id(frame_id) {}
  float fps = 0.0;
  int frame_id = -1;
};
class FpsStats
{
public:
  FpsStats() : FpsStats(0) {}
  FpsStats(int gpu_id) : gpu_id(gpu_id) {}
  FpsValue instant_fps;
  float total_fps = 0.0;
  int total_frame_count = 0;
  float total_time_diff = 0.0;
  FpsValue min_fps;
  FpsValue max_fps;
  int fps_counter = 0;
  int gpu_id = 0;
};

class StatusMonitor
{
private:
  std::mutex m;
  std::map<size_t, FpsStats> thread_to_fps;

public:
  StatusMonitor(/* args */){};
  ~StatusMonitor(){};
  void put_fps(size_t thread_id, int frame_count, float time_diff, float fps,
               int gpu_id)
  {
    std::lock_guard<std::mutex> lk(m);
    if (thread_to_fps.find(thread_id) == thread_to_fps.end()) {
      thread_to_fps[thread_id] = FpsStats(gpu_id = gpu_id);
      thread_to_fps[thread_id].min_fps.fps = 10000;
    }
    auto& fps_stats = thread_to_fps[thread_id];
    auto& frame_id = fps_stats.fps_counter;
    auto fps_value = FpsValue(fps, frame_id);

    fps_stats.instant_fps = fps_value;
    fps_stats.total_fps += fps_value.fps;
    fps_stats.fps_counter += 1;
    fps_stats.total_frame_count += frame_count;
    fps_stats.total_time_diff += time_diff;
    if (fps_value.fps < fps_stats.min_fps.fps) {
      fps_stats.min_fps.fps = fps_value.fps;
      fps_stats.min_fps.frame_id = frame_id;
    }

    if (fps_value.fps > fps_stats.max_fps.fps) {
      fps_stats.max_fps.fps = fps_value.fps;
      fps_stats.max_fps.frame_id = frame_id;
    }
  }
  void print_fps()
  {
    std::stringstream ss;
    std::lock_guard<std::mutex> lk(m);
    for (auto& fps_stats : thread_to_fps) {
      ss << " [";
      ss << std::setprecision(0) << std::fixed
         << (fps_stats.second.total_fps / fps_stats.second.fps_counter);
      ss << " " << fps_stats.second.gpu_id << "] ";
    }
    std::cout << ss.str() << std::endl;
  }
};
namespace
{
std::mutex m;
std::condition_variable cond_var;
volatile std::atomic<bool> shutdown_requested{false};
StatusMonitor status_monitor;
} // namespace
void signal_handler(int signal)
{
  {
    std::lock_guard<std::mutex> lk(m);
    shutdown_requested = true;
  }
  cond_var.notify_all();
}

class Worker
{
private:
  int gpu_id;
  int channel_id;
  const std::string enc_file;
  int to_width;
  int to_height;
  float fps;
  int frame_count = 0;
  std::atomic<bool> shutdown_requested{false};
  std::atomic<bool> done{false}; // Use an atomic flag.
  std::thread th;
  int64_t last_frame_time = -1;
  std::unique_ptr<AVFReader> reader;
  unsigned char* rgbBuffGPUPtr;

  void* decoderDataSpace = NULL;
  int rgbWidth = 1920;
  int rgbHeight = 1080;
  int bufferLen = 2 * 1024 * 1024;
  unsigned char* buffer = NULL;
  unsigned char* rgbBuffer = NULL;
  UltraFace ultraface;

public:
  Worker(int gpu_id, int channel_id, const std::string enc_file, int to_width,
         int to_height, int fps = 25)
      : gpu_id(gpu_id), channel_id(channel_id), enc_file(enc_file),
        to_width(to_width), fps(fps)
  {
    reader.reset(new AVFReader(enc_file));
    buffer = (unsigned char*)malloc(bufferLen);
    rgbBuffer = (unsigned char*)malloc(rgbWidth * rgbHeight * 4);
  };
  void start() { th = std::thread(&Worker::run, this); }
  bool is_alive() { return !done; }
  bool decode_and_detect(size_t this_id)
  {
    auto t = std::chrono::duration_cast<std::chrono::seconds>(
                 std::chrono::system_clock::now().time_since_epoch())
                 .count();
    if (last_frame_time < 1)
      last_frame_time = t;
    size_t diff = t - last_frame_time;
    if (diff > 10) {
      fps = (float)frame_count / diff;
      last_frame_time = t;
      status_monitor.put_fps(this_id, frame_count, diff, fps, gpu_id);
      frame_count = 0;
    }

    FrameInfo frame;
    frame.totalSize = bufferLen;
    int rs = reader->getFrame(frame, buffer);
    if (rs == 0) {
      int readLen = frame.frameSize;
      if (decoderDataSpace == NULL) {
        if (initDecoder(&decoderDataSpace, H264, ARGB) == 0) {
        }
      }
      if (decoderDataSpace != NULL) {
        int tempRgbWidth = rgbWidth;
        int tempRgbHeight = rgbHeight;
        long long outTimeStamp = 0;

        rs = 0;
        if (frame.mediaType == 2) {
          while (true) {
            // int rs = decodeFrameGetGPUPoiner(decoderDataSpace, buffer,
            // readLen,
            //                                  &rgbBuffGPUPtr, &tempRgbWidth,
            //                                  &tempRgbHeight, frame.timeStamp,
            //                                  &outTimeStamp, frame.frameType);

            int rs = decodeFrame(decoderDataSpace, buffer, readLen, rgbBuffer,
                                 &tempRgbWidth, &tempRgbHeight);
            if (rs == VA_LOWBUFFERSIZE) {
              if (rgbBuffer)
                free(rgbBuffer);
              rgbWidth = tempRgbWidth;
              rgbHeight = tempRgbHeight;
              rgbBuffer = (unsigned char*)malloc(rgbWidth * rgbHeight * 4);
            } else if (rs == VA_SUCCESS) {
              // printf("thread=====%d\n", std::this_thread::get_id());
              // std::this_thread::sleep_for(std::chrono::milliseconds(1));
              // char op[250] = {
              //     0,
              // };
              // sprintf(op, "./dump_1/%05d_%05llu.ppm", frame_count,
              //         outTimeStamp);
              // writePPMFromBgra(rgbBuffer, rgbHeight, rgbWidth, op);
              // printf("\n[%d] Success", frame_count);
              // cv::Mat inMat = cv::Mat(rgbHeight, rgbWidth, CV_8UC4,
              //                         (unsigned char*)rgbBuffer);
              // cv::Mat frame;
              // cv::cvtColor(inMat, frame,
              //              cv::ColorConversionCodes::COLOR_BGRA2BGR);
              // ultraface.getResult(frame);
              break;
            } else {
              // printf("\n[%d] Error\n", i);
              break;
            }
          }
        }
      }
    } else if (rs == 9) {
      free(buffer);
      bufferLen = frame.totalSize;
      buffer = (unsigned char*)malloc(bufferLen);
      return true;
    } else {
      return false;
    }

    frame_count++;
    return true;
  }
  void run(void)
  {
    size_t this_id = std::hash<std::thread::id>{}(std::this_thread::get_id());

    std::cout << "Started " << this_id << " from file: " << enc_file
              << " GPU ID: " << gpu_id << " CHANNEL ID: " << channel_id
              << std::endl;
    while (shutdown_requested == false) {
      if (!decode_and_detect(this_id))
        break;
    }
    done = true;
    std::cout << "End " << this_id << std::endl;
  }
  void stop(void)
  {
    shutdown_requested = true;
    th.join();
  }
  ~Worker(){};
};

int main(int argc, char const* argv[])
{
  // Install a signal handler
  std::signal(SIGINT, signal_handler);
  std::signal(SIGTERM, signal_handler);

  std::cout << "VPF Becnchmark started" << std::endl;
  initCodecLibrary();
  int nGpu = 1;
  int number_of_channels = 1;
  int gpu_id = -1;

  ///*
  if (argc < 2) {
    std::cout << " number_of_channels "
              << " gpu_id " << std::endl;
    return -1;
  }
  std::string arg_number_of_channels = argv[1];
  try {
    std::size_t pos;
    number_of_channels = std::stoi(arg_number_of_channels, &pos);
    if (pos < arg_number_of_channels.size()) {
      std::cerr << "Trailing characters after number: "
                << arg_number_of_channels << '\n';
    }
  } catch (std::invalid_argument const& ex) {
    std::cerr << "Invalid number: " << arg_number_of_channels << '\n';
  } catch (std::out_of_range const& ex) {
    std::cerr << "Number out of range: " << arg_number_of_channels << '\n';
  }

  std::string arg_gpu_id = argv[2];
  try {
    std::size_t pos;
    gpu_id = std::stoi(arg_gpu_id, &pos);
    if (pos < arg_gpu_id.size()) {
      std::cerr << "Trailing characters after number: " << arg_gpu_id << '\n';
    }
  } catch (std::invalid_argument const& ex) {
    std::cerr << "Invalid number: " << arg_gpu_id << '\n';
  } catch (std::out_of_range const& ex) {
    std::cerr << "Number out of range: " << arg_gpu_id << '\n';
  }
  //*/
  int num_of_gpus = nGpu;
  if (gpu_id > nGpu)
    gpu_id = -1;
  if (gpu_id >= 0)
    num_of_gpus = 1; // gpu specified

  std::vector<std::unique_ptr<Worker>> thread_list;
  std::cout << "Number of GPUS: " << nGpu
            << " number_of_channels: " << number_of_channels << " gpu_id "
            << gpu_id << " num_of_gpus: " << num_of_gpus << std::endl;
  std::stringstream ss;
  ss << ".";
  // ss << "/2min_1080p.mp4";
  ss << "/1.AVF";
  // ss << "/Merged_20200918_90003.mp4";
  // ss << "/workspaces/VideoProcessingFramework/Merged_20200918_90003.mp4";
  for (size_t indx = 0; indx < num_of_gpus; indx++) {
    int i = indx;
    if ((num_of_gpus == 1) && (gpu_id >= 0))
      i = gpu_id;
    for (int j = 0; j < number_of_channels; j++) {
      thread_list.push_back(std::make_unique<Worker>(i, j, ss.str(), 800, 800));
    }
  }
  for (auto& x : thread_list) {
    x->start();
  }
  while (true) {
    std::unique_lock<std::mutex> lk(m);
    cond_var.wait_for(lk, std::chrono::seconds(10));
    if (shutdown_requested)
      break;
    status_monitor.print_fps();
    int live_threads = 0;
    for (auto& x : thread_list) {
      if (x->is_alive())
        live_threads += 1;
    }
    if (live_threads == 0)
      break;
  }
  for (auto& x : thread_list) {
    x->stop();
  }

  std::cout << "VPF Becnchmark end" << std::endl;

  return 0;
}
