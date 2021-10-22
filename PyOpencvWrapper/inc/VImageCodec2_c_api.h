#ifndef VIMAGECODEC2_C_API_H
#define VIMAGECODEC2_C_API_H

/**
 * @file
 *
 * */

#if (defined(_WIN32) || defined(_WIN32_WCE))
#if defined(VIMAGECODEC2_EXPORTS)
#define VIMAGECODEC2_API __declspec(dllexport)
#else
#define VIMAGECODEC2_API __declspec(dllimport)
#endif
#endif

#if !defined(VIMAGECODEC2_API)
#if !defined(VIMAGECODEC_NO_GCC_API_ATTRIBUTE) && defined(__GNUC__) && \
    (__GNUC__ >= 4)
#define VIMAGECODEC2_API __attribute__((visibility("default")))
#else
#define VIMAGECODEC2_API
#endif
#endif

#include "VCodecMediaFrameType.h"
#include "Vtype.h"

#ifdef __cplusplus
extern "C" {
#endif
/**
 * @brief Image codec initialization function. This should be called olny once
 * to load the library correctly
 *
 */
void VIMAGECODEC2_API initCodecLibrary();

/**
 * @brief Get the Single Image Snap object
 *
 * @param codecType
 * @param frameBuff
 * @param frameSize
 * @param rgbBuff
 * @param width
 * @param height
 * @param outFormatType
 * @return int
 */
int VIMAGECODEC2_API getSingleImageSnap(Codec_Type codecType,
                                        unsigned char *frameBuff, int frameSize,
                                        unsigned char *rgbBuff, int *width,
                                        int *height, Format_Type outFormatType);
/**
 * @brief
 *
 * @param ppDataSpace
 * @param codecType
 * @param width
 * @param height
 * @param bitRate
 * @param inFormatType
 * @return int
 */
int VIMAGECODEC2_API initEncoder(void **ppDataSpace, Codec_Type codecType,
                                 int width, int height, int bitRate,
                                 Format_Type inFormatType);

/**
 * @brief
 *
 * @param dataSpace
 * @return int
 */
int VIMAGECODEC2_API freeEncoder(void *dataSpace);

/**
 * @brief
 *
 * @param dataSpace
 * @param frameBuff
 * @param frameSize
 * @param rgbBuff
 * @return int
 */
int VIMAGECODEC2_API encodeFrame(void *dataSpace, unsigned char *frameBuff,
                                 int *frameSize, unsigned char *rgbBuff);

/**
 * @brief
 *
 * @param ppDataSpace
 * @param codecType
 * @param outFormatType
 * @return int
 */
int VIMAGECODEC2_API initDecoder(void **ppDataSpace, Codec_Type codecType,
                                 Format_Type outFormatType);
/**
 * @brief
 *
 * @param ppDataSpace
 * @param codecType
 * @param outFormatType
 * @param gpuIndex
 * @return int
 */
int VIMAGECODEC2_API initDecoderGPU(void **ppDataSpace, Codec_Type codecType,
                                    Format_Type outFormatType, int gpuIndex);

/**
 * @brief
 *
 * @param ppDataSpace
 * @param codecType
 * @param outFormatType
 * @param hWnd
 * @param bCreateWindow
 * @param desiredFPS
 * @return int
 */
int VIMAGECODEC2_API initDecoderAndRender(void **ppDataSpace,
                                          Codec_Type codecType,
                                          Format_Type outFormatType,
                                          long long hWnd, bool bCreateWindow,
                                          int desiredFPS);

/**
 * @brief
 *
 * @param ppDataSpace
 * @param codecType
 * @param outFormatType
 * @param hWnd
 * @param bCreateWindow
 * @param desiredFPS
 * @param gpuIndex
 * @return int
 */
int VIMAGECODEC2_API initDecoderAndRenderGPU(void **ppDataSpace,
                                             Codec_Type codecType,
                                             Format_Type outFormatType,
                                             long long hWnd, bool bCreateWindow,
                                             int desiredFPS, int gpuIndex);
/**
 * @brief
 *
 * @param dataSpace
 * @return int
 */
int VIMAGECODEC2_API freeDecoder(void *dataSpace);
/**
 * @brief
 *
 * @param dataSpace
 * @param frameBuff
 * @param frameSize
 * @param rgbBuff
 * @param width
 * @param height
 * @return int
 */
int VIMAGECODEC2_API decodeFrame(void *dataSpace, unsigned char *frameBuff,
                                 int frameSize, unsigned char *rgbBuff,
                                 int *width, int *height);

/**
 * @brief
 *
 * @param dataSpace
 * @param frameBuff
 * @param frameSize
 * @param rgbBuff
 * @param width
 * @param height
 * @return int
 */
int VIMAGECODEC2_API decodeFrameGetGPUPoiner2(void *dataSpace,
                                             unsigned char *frameBuff,
                                             int frameSize,
                                             unsigned char **rgbBuff,
                                             int *width, int *height);
/**
 * @brief
 *
 * @param dataSpace
 * @param frameBuff
 * @param frameSize
 * @param rgbBuff
 * @param width
 * @param height
 * @param pOutTimeStamp
 * @param frameType
 * @return int
 */
int VIMAGECODEC2_API decodeFrameGetGPUPoiner(void *dataSpace,
                                             unsigned char *frameBuff,
                                             int frameSize,
                                             unsigned char **rgbBuff,
                                             int *width, int *height, 
                                             long long frameTimeStamp,
                                             long long *pOutTimeStamp,
                                             unsigned int frameType);
/**
 * @brief Get the Last Decoded Frame object
 *
 * @param dataSpace
 * @param rgbBuff
 * @return int
 */
int VIMAGECODEC2_API getLastDecodedFrame(void *dataSpace,
                                         unsigned char *rgbBuff);

/**
 * @brief
 *
 * @param dataSpace
 * @param frameBuff
 * @param frameSize
 * @param rgbBuff
 * @param rgbBuffLen
 * @param width
 * @param height
 * @param decodeAndResize
 * @param timeStamp
 * @param desiredTimeDiff
 * @return int
 */
int VIMAGECODEC2_API decodeFrameNew(void *dataSpace, unsigned char *frameBuff,
                                    int frameSize, unsigned char *rgbBuff,
                                    int rgbBuffLen, int *width, int *height,
                                    bool decodeAndResize, long long timeStamp,
                                    long long desiredTimeDiff);
/**
 * @brief
 *
 * @param dataSpace
 * @param frameBuff
 * @param frameSize
 * @param rgbBuff
 * @param rgbBuffLen
 * @param width
 * @param height
 * @param decodeAndResize
 * @param timeStamp
 * @param desiredTimeDiff
 * @param pOutTimeStamp
 * @param frameType
 * @return int
 */
int VIMAGECODEC2_API decodeFrameNewWithOutputTS(
    void *dataSpace, unsigned char *frameBuff, int frameSize,
    unsigned char *rgbBuff, int rgbBuffLen, int *width, int *height,
    bool decodeAndResize, long long timeStamp, long long desiredTimeDiff,
    long long *pOutTimeStamp, unsigned int frameType);
/**
 * @brief
 *
 * @param dataSpace
 * @param frameBuff
 * @param frameSize
 * @param rgbBuff
 * @param width
 * @param height
 * @return int
 */
int VIMAGECODEC2_API decodeScaleFrame(void *dataSpace, unsigned char *frameBuff,
                                      int frameSize, unsigned char *rgbBuff,
                                      int *width, int *height);
/**
 * @brief
 *
 * @param dataSpace
 * @param frameBuff
 * @param frameSize
 * @param outBuff
 * @param outSize
 * @return int
 */
int VIMAGECODEC2_API decodeAudioFrame(void *dataSpace, unsigned char *frameBuff,
                                      int frameSize, unsigned char *outBuff,
                                      int *outSize);
/**
 * @brief
 *
 * @param dataSpace
 * @param frameBuff
 * @param frameSize
 * @return int
 */
int VIMAGECODEC2_API addBufferToDecoder(void *dataSpace,
                                        unsigned char *frameBuff,
                                        int frameSize);
void VIMAGECODEC2_API writePPM(unsigned char *bgraImg, int width, int height, const char *opf);

#ifdef __cplusplus
}
#endif
#endif
