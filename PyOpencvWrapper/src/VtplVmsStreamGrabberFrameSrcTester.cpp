
#include "VtplVideoStream.h"
#include "VtplVmsStreamGrabberFrameSrc.h"
#include <Poco/ErrorHandler.h>
#include <Poco/Net/HTTPServer.h>
#include <Poco/Util/HelpFormatter.h>
#include <Poco/Util/ServerApplication.h>
#include <iostream>
#include <logging.h>
#include <memory>
#include <vector>

class ServerErrorHandler : public Poco::ErrorHandler
{
public:
  void exception(const Poco::Exception& exc) { RAY_LOG(FATAL) << "Poco::Exception " << exc.what(); }
  void exception(const std::exception& exc) { RAY_LOG(FATAL) << "std::exception " << exc.what(); }
  void exception() { RAY_LOG(FATAL) << "unknown exception "; }
};

class VtplVmsStreamGrabberFrameSrcTester
{
private:
  std::unique_ptr<VtplVmsStreamGrabberFrameSrc> grabber;
  std::atomic_bool _is_shutdown;
  std::unique_ptr<std::thread> _thread;

public:
  VtplVmsStreamGrabberFrameSrcTester() : _is_shutdown(false)
  {
    // grabber.reset(new VtplVideoStream("rtsp://admin:admin123@192.168.0.58/h264/ch1/main/"));
    // grabber.reset(new VtplVideoStream("rtsp://192.168.0.8/1/h264major"));
    // grabber.reset(new VtplVideoStream("rtsp://192.168.0.33/axis-media/media.amp", 0, 207));
    // grabber.reset(new VtplVideoStream("vms://192.168.1.199:3005/4/0/192_168_1_198", 4, 201));
    // grabber.reset(new VtplVideoStream("file:///home/vadmin/gva/data/video/11.mp4", 0, 0, 20.0));
    // grabber.reset(new VtplVideoStream("file:///home/vadmin/gva/data/video/11.mp4", 0, 0, 20.0, true));
    // grabber.reset(new VtplVideoStream("vms://192.168.1.178:3005/1/1/1/1"));
    grabber.reset(new VtplVmsStreamGrabberFrameSrc("vms://192.168.0.171:3005/9/0/1/1"));
    _thread.reset(new std::thread(&VtplVmsStreamGrabberFrameSrcTester::run, this));
  }
  void run()
  {
    RAY_LOG(INFO) << "VtplVmsStreamGrabberFrameSrcTester start";
    // grabber->start();
    std::vector<uint8_t> vtpl_frame;
    int count = 0;
    int consecutive_error_count = 0;
    while (!_is_shutdown) {
      std::chrono::time_point<std::chrono::system_clock> entry_time = std::chrono::system_clock::now();

      std::cout << "Read start" << std::endl;
      bool ret = grabber->read(vtpl_frame);
      std::chrono::time_point<std::chrono::system_clock> current_time = std::chrono::system_clock::now();
      int diff = std::chrono::duration_cast<std::chrono::seconds>(current_time - entry_time).count();

      std::cout << "Read end " << diff << std::endl;
      if (ret == false) {
        consecutive_error_count++;
      }
      if (consecutive_error_count > 100) {
        std::cout << "--------- break false";
        break;
      }
      // if (vtpl_frame.empty()) {
      //   RAY_LOG(INFO) << "--------- break empty";
      //   break;
      // }
      count += 1;
      RAY_LOG(INFO) << fmt::format("count {}", count);
    }
    RAY_LOG(INFO) << "VtplVmsStreamGrabberFrameSrcTester end";
  }
  void stop()
  {
    _is_shutdown = true;
    grabber->stop();
    _thread->join();
  }
};

class VtplVideoStreamTester
{
private:
  std::unique_ptr<VtplVideoStream> grabber;
  std::atomic_bool _is_shutdown;
  std::unique_ptr<std::thread> _thread;

public:
  VtplVideoStreamTester() : _is_shutdown(false)
  {
    // grabber.reset(new VtplVideoStream("rtsp://admin:admin123@192.168.0.58/h264/ch1/main/"));
    // grabber.reset(new VtplVideoStream("rtsp://192.168.0.8/1/h264major"));
    // grabber.reset(new VtplVideoStream("rtsp://192.168.0.33/axis-media/media.amp", 0, 207));
    // grabber.reset(new VtplVideoStream("vms://192.168.1.199:3005/4/0/192_168_1_198", 4, 201));
    // grabber.reset(new VtplVideoStream("file:///home/vadmin/gva/data/video/11.mp4", 0, 0, 20.0));
    // grabber.reset(new VtplVideoStream("file:///home/vadmin/gva/data/video/11.mp4", 0, 0, 20.0, true));
    // grabber.reset(new VtplVideoStream("vms://192.168.1.178:3005/1/1/1/1"));
    grabber.reset(new VtplVideoStream("vms://192.168.0.171:3005/9/0/1/1"));
    _thread.reset(new std::thread(&VtplVideoStreamTester::run, this));
  }
  void run()
  {
    RAY_LOG(INFO) << "VtplVideoStreamTester start";
    grabber->start();
    std::vector<uint8_t> vtpl_frame;
    int count = 0;
    while (!_is_shutdown) {
      bool ret = grabber->read(vtpl_frame);
      if (ret == false) {
        RAY_LOG(INFO) << "--------- break false";
        break;
      }
      if (vtpl_frame.empty()) {
        RAY_LOG(INFO) << "--------- break empty";
        break;
      }
      count += 1;
      RAY_LOG(INFO) << fmt::format("count {}", count);
    }
    RAY_LOG(INFO) << "VtplVideoStreamTester end";
  }
  void stop()
  {
    _is_shutdown = true;
    grabber->stop();
    _thread->join();
  }
};

class EntryPoint : public Poco::Util::ServerApplication
{
private:
  std::string _session_dir;
  ServerErrorHandler _serverErrorHandler;

public:
  EntryPoint() : _session_dir("") {}
  ~EntryPoint() {}
  void initialize(Application& self)
  {
    loadConfiguration(); // load default configuration files, if present
    Poco::ErrorHandler::set(&_serverErrorHandler);
    ServerApplication::initialize(self);
  }

  void uninitialize() { ServerApplication::uninitialize(); }

  void defineOptions(Poco::Util::OptionSet& options)
  {
    ServerApplication::defineOptions(options);
    options.addOption(Poco::Util::Option("help", "h", "display help information on command line arguments")
                          .required(false)
                          .repeatable(false));
  }

  void handleOption(const std::string& name, const std::string& value) { ServerApplication::handleOption(name, value); }

  void displayHelp()
  {
    Poco::Util::HelpFormatter helpFormatter(options());
    helpFormatter.setCommand(commandName());
    helpFormatter.setUsage("OPTIONS");
    helpFormatter.setHeader("A web server that shows how to work with HTML forms.");
    helpFormatter.format(std::cout);
  }
  const std::string& get_session_folder()
  {
    if (_session_dir.empty()) {
      Poco::Path base_path;
      if (config().getBool("application.runAsService", false) || config().getBool("application.runAsDaemon", false)) {
        base_path.assign(config().getString("application.dataDir", config().getString("application.dir", "./")));
        base_path.pushDirectory(config().getString("application.baseName", "ojana"));
      } else {
        base_path.assign(config().getString("application.dir", "./"));
      }
      base_path.pushDirectory("session");
      std::cout << "Session folder is at: " << base_path.toString() << std::endl;
      _session_dir = base_path.toString();
    }
    return _session_dir;
  }
  int main(const ArgVec& args)
  {
    std::string name_of_app = config().getString("application.baseName");
    //::ray::RayLog::StartRayLog(name_of_app, ::ray::RayLogLevel::DEBUG, get_session_folder());
    std::string a;
    ::ray::RayLog::StartRayLog(name_of_app, ::ray::RayLogLevel::DEBUG, a);
    RAY_LOG(INFO) << "main Started: " << name_of_app;
    std::string data_dir("");
    if (data_dir.empty()) {
      data_dir = get_session_folder();
    }
    Poco::Net::initializeNetwork();
    VtplVideoStreamTester vtpl_video_stream_tester;
    waitForTerminationRequest();
    vtpl_video_stream_tester.stop();
    RAY_LOG(INFO) << "main Stopped: " << name_of_app;
    Poco::Net::uninitializeNetwork();
    return Application::EXIT_OK;
  }
};

POCO_SERVER_MAIN(EntryPoint);
