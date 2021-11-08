#include "VtplVideoStream.h"
#include <Poco/URI.h>

VtplVideoStream::VtplVideoStream(std::string path, uint16_t channel_id, uint32_t app_id, float suggested_fps,
                                 bool is_blocking, int gpu_id, int seek_time)
{
}

void VtplVideoStream::_get_stream_url(std::string& path) {

    if (path.size() > 0) {
        Poco::URI uri(path);
        uri.getScheme();
    }

}
VtplVideoStream::~VtplVideoStream() {}