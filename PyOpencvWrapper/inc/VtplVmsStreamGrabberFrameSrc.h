#pragma once
#ifndef VtplVmsStreamGrabberFrameSrc_h
#define VtplVmsStreamGrabberFrameSrc_h
#include <vector>
#include <Poco/Net/StreamSocket.h>

bool read_data_n(Poco::Net::StreamSocket& s, Poco::FIFOBuffer&  data_array, int bytes_to_read);

#endif