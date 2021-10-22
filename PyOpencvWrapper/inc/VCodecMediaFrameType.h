#ifndef VCODEC_MEDIA_FRAME_TYPE_H_
#define VCODEC_MEDIA_FRAME_TYPE_H_
/**
 * @file
 * @brief
 *
 */
// typedef enum _Codec_Type_ {
//	MJPEG = 0,
//	MPEG,
//	H264,
//	MLAW,
//	ALAW,
//	L16
//}Codec_Type;
//
// typedef enum _Format_Type_ {
//	RGB = 0,
//	ARGB,
//	RGB32,
//	AUDIO
//}Format_Type;

/**
 * @brief 
 * 
 */
typedef enum {
  H_FRAME = 0, /*<*/
  I_FRAME = 1,
  P_FRAME = 2,
  CONNECT_HEADER = 3,
  AUDIO_FRAME = 16,
  UNKNOWN_FRAME = 99

} Frame_Type;

// typedef enum {
//	MJPG = 0,
//	MPEG = 1,
//	H264 = 2,
//	PCMU = 3,
//	PCMA = 4,
//	L16 = 5
//
//}Media_Type;

typedef enum _Codec_Type_ {
  MJPEG = 0,
  MJPG = 0,
  MJPEG_TYPE = 0,
  MPEG = 1,
  MPEG_TYPE = 1,
  H264 = 2,
  H264_TYPE = 2,
  MLAW = 3,  // Audio
  PCMU = 3,  // Audio
  ALAW = 4,  // Audio
  PCMA = 4,  // Audio
  L16 = 5,   // Audio
  // H265 = 6
  ACC_CODEC = 6,
  ACC = 6,
  UNKNOWN_CODEC = 7,
  UNKNOWN = 7,
  H265 = 8,
  H265_CODEC = 8
} Codec_Type,
    Media_Type, StreamCodecType;

typedef enum _Format_Type_ {
  RGB = 0,
  RGB_OP = 0,
  ARGB = 1,
  ARGB_OP = 1,
  RGB32 = 2,  // RGB4(RGB32) color planes
  RGB32_OP = 2,
  AUDIO,
  YV12,  // YV12 color planes
  NV12,  // NV12 color planes
  NV16,  // 4:2:2 color format with similar to NV12 layout.
  UNKNOWN_FORMAT = 255
} Format_Type,
    DecodingOPType;

#pragma pack(push, 1)

typedef struct _FrameHeader_ {
  _FrameHeader_()
      : totalSize(0),
#ifdef _WIN32
        frameType(Frame_Type::UNKNOWN_FRAME),
        mediaType(Codec_Type::UNKNOWN_CODEC),
#else
        frameType(UNKNOWN_FRAME),
        mediaType(UNKNOWN_CODEC),
#endif
        frameSize(0),
        timeStamp(0) {
  }
  _FrameHeader_(const _FrameHeader_ &o)
      : totalSize(o.totalSize),
        frameType(o.frameType),
        mediaType(o.mediaType),
        frameSize(o.frameSize),
        timeStamp(o.timeStamp) {}
  int totalSize;
  int frameType;
  int mediaType;
  int frameSize;
  long long timeStamp;
} FrameHeader;

#pragma pack(pop)

#pragma pack(push, 1)
typedef struct _FrameHeaderCompact_ {
  _FrameHeaderCompact_()
      : frameType(255),
        mediaType(255),
        frameSize(0),
        frameID(0),
        presentationTimeStamp(0),
        systemTimeStamp(0),
        normalPresentationTimeStamp(0) {}
  _FrameHeaderCompact_(const _FrameHeaderCompact_ &o)
      : frameType(o.frameType),
        mediaType(o.mediaType),
        frameSize(o.frameSize),
        frameID(o.frameID),
        presentationTimeStamp(o.presentationTimeStamp),
        systemTimeStamp(o.systemTimeStamp),
        normalPresentationTimeStamp(o.normalPresentationTimeStamp) {}
  unsigned char frameType;
  unsigned char mediaType;
  int frameSize;
  long long frameID;
  long long presentationTimeStamp;
  long long systemTimeStamp;
  long long normalPresentationTimeStamp;
} FrameHeaderCompact;
#pragma pack(pop)

#define IS_AUDIO(x)                                                \
  (x == MLAW || x == PCMU || x == ALAW || x == PCMA || x == L16 || \
   x == ACC_CODEC || x == UNKNOWN_CODEC)
#endif
