/*
 * CodecManager.h
 *
 *  Created on: 07-Jul-2010
 *      Author: root
 */

#ifndef CODECMANAGER_H_
#define CODECMANAGER_H_

#include "VCodecMediaFrameType.h"
/*

typedef enum _Codec_Name_
{
	MJPEG = 0,
			MPEG,
			H264
}Codec_Type;

typedef enum _Format_Type_
{
	RGB = 0,
			ARGB,
			RGB32
}Format_Type;
*/

int initCodecLibrary();

int getSingleImageSnap(Codec_Type codecType, unsigned char * frameBuff, int frameSize,
		unsigned char *rgbBuff, int *width, int *height, Format_Type outFormatType);

int initEncoder(void *dataSpace, Codec_Type codecType, int width, int height, int bitRate, Format_Type inFormatType);
int freeEncoder(void *dataSpace);
int encodeFrame(void * dataSpace, unsigned char *frameBuff, int *frameSize, unsigned char *rgbBuff);

int initDecoder(void *dataSpace, Codec_Type codecType, Format_Type outFormatType);

int freeDecoder(void *dataSpace);

int decodeFrame(void * dataSpace, unsigned char * frameBuff, int frameSize,
		unsigned char *rgbBuff, int *width, int * height);

int decodeFrameNew(void * dataSpace, unsigned char * frameBuff, int frameSize,
	unsigned char *rgbBuff, int rgbBuffLen, int *width, int * height, bool decodeAndResize, long long timeStamp);

int decodeFrameGetGPUPoiner(void *dataSpace, unsigned char *frameBuff, int frameSize, unsigned char **rgbBuff, int *width, int *height, 
                                            long long frameTimeStamp,  long long *pOutTimeStamp, unsigned int frameType);

int getLastDecodedFrame(void * dataSpace, unsigned char *rgbBuff);

int initDecoderGPU(void *dataSpace, Codec_Type codecType, Format_Type outFormatType, int gpuIndex);
#endif /* CODECMANAGER_H_ */
