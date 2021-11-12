/*
 * CodecManager.c
 *
 *  Created on: 07-Jul-2010
 *      Author: root
 */

#include "crtdbgHelper.h"
#include <stdio.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define GET_FUNCTION_ADDRESS GetProcAddress
#else
#include <dlfcn.h>
#define GET_FUNCTION_ADDRESS dlsym
#endif

#include "CodecManager.h"
#include "VReturnStatus.h"
#include "Vtype.h"

#define CODEC_LIB_DEBUG 1

typedef Int32 (*FnPtrCodecLib_NP)();
typedef Int32 (*FnPtrCodecLib_5I1VP)(void* dataSpace, Codec_Type codecType, Int32 width, Int32 height, Int32 bitRate,
                                     Format_Type inFormatType);
typedef Int32 (*FnPtrCodecLib_1VP)(void* dataSpace);
typedef Int32 (*FnPtrCodecLib_1VP2UCP1IP)(void* dataSpace, unsigned char* frameBuff, Int32* frameSize,
                                          unsigned char* rgbBuff);
typedef Int32 (*FnPtrCodecLib_1VP2I)(void* dataSpace, Codec_Type codecType, Format_Type outFormatType);
typedef Int32 (*FnPtrCodecLib_1VP2UC2IP1I)(void* dataSpace, unsigned char* frameBuff, Int32 frameSize,
                                           unsigned char* rgbBuff, Int32* width, Int32* height);

typedef Int32 (*FnPtrCodecLib_decodeframeNew)(void* dataSpace, unsigned char* frameBuff, int frameSize,
                                              unsigned char* rgbBuff, int rgbBuffLen, int* width, int* height,
                                              bool decodeAndResize, long long timeStamp);

typedef Int32 (*FnPtrCodecLib_2UC2IP3I)(Codec_Type codecType, unsigned char* frameBuff, Int32 frameSize,
                                        unsigned char* rgbBuff, Int32* width, Int32* height, Format_Type outFormatType);

typedef Int32 (*FnPtrCodecLib_decodeFrameGetGPUPoiner)(void* dataSpace, unsigned char* frameBuff, int frameSize,
                                                       unsigned char** rgbBuff, int* width, int* height,
                                                       long long frameTimeStamp, long long* pOutTimeStamp,
                                                       unsigned int frameType);
typedef Int32 (*FnPtrCodecLib_getLastDecodedFrame)(void* dataSpace, unsigned char* rgbBuff);
typedef Int32 (*FnPtrCodecLib_initDecoderGPU)(void* dataSpace, Codec_Type codecType, Format_Type outFormatType,
                                              int gpuIndex);

typedef FnPtrCodecLib_NP InitCodecLib;
typedef FnPtrCodecLib_5I1VP InitEncoder;
typedef FnPtrCodecLib_1VP FreeEncoder;
typedef FnPtrCodecLib_1VP2UCP1IP EncodeFrame;
typedef FnPtrCodecLib_1VP2I InitDecoder;
typedef FnPtrCodecLib_1VP FreeDecoder;
typedef FnPtrCodecLib_1VP2UC2IP1I Decodeframe;
typedef FnPtrCodecLib_decodeframeNew DecodeframeNew;
typedef FnPtrCodecLib_2UC2IP3I DecodeSingleImage;
typedef FnPtrCodecLib_decodeFrameGetGPUPoiner DecodeFrameGetGPUPoiner;
typedef FnPtrCodecLib_getLastDecodedFrame GetLastDecodedFrame;
typedef FnPtrCodecLib_initDecoderGPU InitDecoderGPU;

InitCodecLib _initCodecLib;
InitEncoder _initEncoder;
FreeEncoder _freeEncoder;
EncodeFrame _encodeFrame;
InitDecoder _initDecoder;
FreeDecoder _freeDecoder;
Decodeframe _decodeFrame;
DecodeframeNew _decodeframeNew;
DecodeSingleImage _decodeSingleFrame;
DecodeFrameGetGPUPoiner _decodeFrameGetGPUPoiner;
GetLastDecodedFrame _getLastDecodedFrame;
InitDecoderGPU _initDecoderGPU;

int _LoadCodecLibrary()
{
  const char* libName =
#ifdef _WIN32
      "./lib/VImageCodec.dll";
#else
      "./lib/libVImageCodec3.so";
#endif
#ifdef _WIN32
  HINSTANCE dllHandle = LoadLibraryA(libName);
#else
  void* dllHandle = dlopen(libName, RTLD_LAZY);
#endif

  if (!dllHandle) {
#if CODEC_LIB_DEBUG
    printf("Failed to load %s library\n", libName);
    fflush(stdout);
    exit(0);
#endif
    return VA_LIBRARY_LOAD_FAIL;
  }

  _initCodecLib = (InitCodecLib)GET_FUNCTION_ADDRESS(dllHandle, "initCodecLibrary");
  if (!_initCodecLib) {
#if CODEC_LIB_DEBUG
    printf("initCodecLibrary loading failed\n"
           "codecManager.c:LoadCodecLibrary\n");
    fflush(stdout);
    exit(0);
#endif
    return VA_LIBRARY_LOAD_FAIL;
  }

  _initEncoder = (InitEncoder)GET_FUNCTION_ADDRESS(dllHandle, "initEncoder");
  if (!_initEncoder) {
#if CODEC_LIB_DEBUG
    printf("initEncoder loading failed\n"
           "codecManager.c:LoadCodecLibrary\n");
    fflush(stdout);
    exit(0);
#endif
    return VA_LIBRARY_LOAD_FAIL;
  }

  _freeEncoder = (FreeEncoder)GET_FUNCTION_ADDRESS(dllHandle, "freeEncoder");
  if (!_freeEncoder) {
#if CODEC_LIB_DEBUG
    printf("freeEncoder loading failed\n"
           "codecManager.c:LoadCodecLibrary\n");
    fflush(stdout);
    exit(0);
#endif
    return VA_LIBRARY_LOAD_FAIL;
  }

  _encodeFrame = (EncodeFrame)GET_FUNCTION_ADDRESS(dllHandle, "encodeFrame");
  if (!_encodeFrame) {
#if CODEC_LIB_DEBUG
    printf("encodeFrame loading failed\n"
           "codecManager.c:LoadCodecLibrary\n");
    fflush(stdout);
    exit(0);
#endif
    return VA_LIBRARY_LOAD_FAIL;
  }

  _initDecoder = (InitDecoder)GET_FUNCTION_ADDRESS(dllHandle, "initDecoder");
  if (!_initDecoder) {
#if CODEC_LIB_DEBUG
    printf("initDecoder loading failed\n"
           "codecManager.c:LoadCodecLibrary\n");
    fflush(stdout);
    exit(0);
#endif
    return VA_LIBRARY_LOAD_FAIL;
  }

  _freeDecoder = (FreeDecoder)GET_FUNCTION_ADDRESS(dllHandle, "freeDecoder");
  if (!_freeDecoder) {
#if CODEC_LIB_DEBUG
    printf("freeDecoder loading failed\n"
           "codecManager.c:LoadCodecLibrary\n");
    fflush(stdout);
    exit(0);
#endif
    return VA_LIBRARY_LOAD_FAIL;
  }

  //	_decodeframeNew = (DecodeframeNew)GET_FUNCTION_ADDRESS(dllHandle,
  //"decodeFrameNew"); 	if (!_decodeframeNew) { #if CODEC_LIB_DEBUG
  //		printf("decodeframeNew loading failed\n"
  //			"codecManager.c:LoadCodecLibrary\n");
  //		fflush(stdout);
  //		exit(0);
  //#endif
  //		return VA_LIBRARY_LOAD_FAIL;
  //	}

  _decodeFrame = (Decodeframe)GET_FUNCTION_ADDRESS(dllHandle, "decodeFrame");
  if (!_decodeFrame) {
#if CODEC_LIB_DEBUG
    printf("decodeFrame loading failed\n"
           "codecManager.c:LoadCodecLibrary\n");
    fflush(stdout);
    exit(0);
#endif
    return VA_LIBRARY_LOAD_FAIL;
  }

  _decodeSingleFrame = (DecodeSingleImage)GET_FUNCTION_ADDRESS(dllHandle, "getSingleImageSnap");
  if (!_decodeSingleFrame) {
#if CODEC_LIB_DEBUG
    printf("getSingleImageSnap loading failed\n"
           "codecManager.c:LoadCodecLibrary\n");
    fflush(stdout);
    exit(0);
#endif
    return VA_LIBRARY_LOAD_FAIL;
  }

  _decodeFrameGetGPUPoiner = (DecodeFrameGetGPUPoiner)GET_FUNCTION_ADDRESS(dllHandle, "decodeFrameGetGPUPoiner");
  if (!_decodeFrameGetGPUPoiner) {
#if CODEC_LIB_DEBUG
    printf("decodeFrameGetGPUPoiner loading failed\n"
           "codecManager.c:LoadCodecLibrary\n");
    fflush(stdout);
#endif
    // return VA_LIBRARY_LOAD_FAIL;
  }

  _getLastDecodedFrame = (GetLastDecodedFrame)GET_FUNCTION_ADDRESS(dllHandle, "getLastDecodedFrame");
  if (!_getLastDecodedFrame) {
#if CODEC_LIB_DEBUG
    printf("getLastDecodedFrame loading failed\n"
           "codecManager.c:LoadCodecLibrary\n");
    fflush(stdout);
#endif
    // return VA_LIBRARY_LOAD_FAIL;
  }

  _initDecoderGPU = (InitDecoderGPU)GET_FUNCTION_ADDRESS(dllHandle, "initDecoderGPU");
  if (!_initDecoderGPU) {
#if CODEC_LIB_DEBUG
    printf("initDecoderGPU loading failed\n"
           "codecManager.c:LoadCodecLibrary\n");
    fflush(stdout);
#endif
    // return VA_LIBRARY_LOAD_FAIL;
  }
  return SUCCESS;
}

int initCodecLibrary()
{
  int rs = _LoadCodecLibrary();
  if (rs == VA_SUCCESS) {
    _initCodecLib();
  }
  return rs;
}

int getSingleImageSnap(Codec_Type codecType, unsigned char* frameBuff, Int32 frameSize, unsigned char* rgbBuff,
                       Int32* width, Int32* height, Format_Type outFormatType)
{
  return _decodeSingleFrame(codecType, frameBuff, frameSize, rgbBuff, width, height, outFormatType);
}

Int32 initEncoder(void* dataSpace, Codec_Type codecType, Int32 width, Int32 height, Int32 bitRate,
                  Format_Type inFormatType)
{
  return _initEncoder(dataSpace, codecType, width, height, bitRate, inFormatType);
}
Int32 freeEncoder(void* dataSpace) { return _freeEncoder(dataSpace); }
Int32 encodeFrame(void* dataSpace, unsigned char* frameBuff, Int32* frameSize, unsigned char* rgbBuff)
{
  return _encodeFrame(dataSpace, frameBuff, frameSize, rgbBuff);
}

Int32 initDecoder(void* dataSpace, Codec_Type codecType, Format_Type outFormatType)
{
  return _initDecoder(dataSpace, codecType, outFormatType);
}

Int32 freeDecoder(void* dataSpace) { return _freeDecoder(dataSpace); }

Int32 decodeFrame(void* dataSpace, unsigned char* frameBuff, Int32 frameSize, unsigned char* rgbBuff, Int32* width,
                  Int32* height)
{
  return _decodeFrame(dataSpace, frameBuff, frameSize, rgbBuff, width, height);
}

Int32 decodeFrameNew(void* dataSpace, unsigned char* frameBuff, int frameSize, unsigned char* rgbBuff, int rgbBuffLen,
                     int* width, int* height, bool decodeAndResize, long long timeStamp)
{
  return _decodeframeNew(dataSpace, frameBuff, frameSize, rgbBuff, rgbBuffLen, width, height, decodeAndResize,
                         timeStamp);
}

int decodeFrameGetGPUPoiner(void* dataSpace, unsigned char* frameBuff, int frameSize, unsigned char** rgbBuff,
                            int* width, int* height, long long frameTimeStamp, long long* pOutTimeStamp,
                            unsigned int frameType)
{
  return _decodeFrameGetGPUPoiner(dataSpace, frameBuff, frameSize, rgbBuff, width, height, frameTimeStamp,
                                  pOutTimeStamp, frameType);
}

int getLastDecodedFrame(void* dataSpace, unsigned char* rgbBuff) { return _getLastDecodedFrame(dataSpace, rgbBuff); }

int initDecoderGPU(void* dataSpace, Codec_Type codecType, Format_Type outFormatType, int gpuIndex)
{
  return _initDecoderGPU(dataSpace, codecType, outFormatType, gpuIndex);
}
