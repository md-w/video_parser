#pragma once
#ifndef CodecSupport_hpp
#define CodecSupport_hpp
#include <cstdint>
#include <string>

namespace vp {
enum SeekMode {
  /* Seek for exact frame number.
   * Suited for standalone demuxer seek. */
  EXACT_FRAME = 0,
  /* Seek for previous key frame in past.
   * Suitable for seek & decode.  */
  PREV_KEY_FRAME = 1,

  SEEK_MODE_NUM_ELEMS
};

enum SeekCriteria {
  /* Seek frame by number.
   */
  BY_NUMBER = 0,
  /* Seek frame by timestamp.
   */
  BY_TIMESTAMP = 1,

  SEEK_CRITERIA_NUM_ELEMS
};

struct SeekContext {
  /* Will be set to false for default ctor, true otherwise;
   */
  bool use_seek;

  /* Frame we want to get. Set by user.
   * Shall be set to frame timestamp in case seek is done by time.
   * Shall be set to frame number in case seek is done by number.
   */
  uint64_t seek_frame;

  /* Mode in which we seek. */
  SeekMode mode;

  /* Criteria by which we seek. */
  SeekCriteria crit;

  /* PTS of frame found after seek. */
  int64_t out_frame_pts;

  /* Duration of frame found after seek. */
  int64_t out_frame_duration;

  /* Number of frames that were decoded during seek. */
  uint64_t num_frames_decoded;

  SeekContext()
      : use_seek(false),
        seek_frame(0),
        mode(PREV_KEY_FRAME),
        crit(BY_NUMBER),
        out_frame_pts(0),
        out_frame_duration(0),
        num_frames_decoded(0U) {}

  SeekContext(uint64_t frame_id)
      : use_seek(true),
        seek_frame(frame_id),
        mode(PREV_KEY_FRAME),
        crit(BY_NUMBER),
        out_frame_pts(0),
        out_frame_duration(0),
        num_frames_decoded(0U) {}

  SeekContext(uint64_t frame_id, SeekCriteria criteria)
      : use_seek(true),
        seek_frame(frame_id),
        mode(PREV_KEY_FRAME),
        crit(criteria),
        out_frame_pts(0),
        out_frame_duration(0),
        num_frames_decoded(0U) {}

  SeekContext(uint64_t frame_num, SeekMode seek_mode)
      : use_seek(true),
        seek_frame(frame_num),
        mode(seek_mode),
        crit(BY_NUMBER),
        out_frame_pts(0),
        out_frame_duration(0),
        num_frames_decoded(0U) {}

  SeekContext(uint64_t frame_num, SeekMode seek_mode, SeekCriteria criteria)
      : use_seek(true),
        seek_frame(frame_num),
        mode(seek_mode),
        crit(criteria),
        out_frame_pts(0),
        out_frame_duration(0),
        num_frames_decoded(0U) {}

  SeekContext(const SeekContext &other)
      : use_seek(other.use_seek),
        seek_frame(other.seek_frame),
        mode(other.mode),
        crit(other.crit),
        out_frame_pts(other.out_frame_pts),
        out_frame_duration(other.out_frame_duration),
        num_frames_decoded(other.num_frames_decoded) {}

  SeekContext &operator=(const SeekContext &other) {
    use_seek = other.use_seek;
    seek_frame = other.seek_frame;
    mode = other.mode;
    crit = other.crit;
    out_frame_pts = other.out_frame_pts;
    out_frame_duration = other.out_frame_duration;
    num_frames_decoded = other.num_frames_decoded;
    return *this;
  }
};
struct PacketData {
  int64_t pts;
  int64_t dts;
  uint64_t poc;
  uint64_t pos;
  uint64_t duration;
};

struct VideoContext {
  uint32_t width;
  uint32_t height;
  uint32_t gop_size;
  uint32_t num_frames;
  uint32_t is_vfr;
  double frameRate;
  double avgFrameRate;
  double timeBase;
  uint32_t streamIndex;
  std::string codec;
  // cudaVideoCodec codec;
  // Pixel_Format format;
  // ColorSpace color_space;
  // ColorRange color_range;
};

struct AudioContext {
  // Reserved for future use;
};

struct MuxingParams {
  VideoContext videoContext;
  AudioContext audioContext;
};
}  // namespace vp
#endif