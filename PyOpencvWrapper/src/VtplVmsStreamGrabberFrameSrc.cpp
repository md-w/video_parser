#include "VtplVmsStreamGrabberFrameSrc.h"


bool read_data_n(Poco::Net::StreamSocket& s, Poco::FIFOBuffer&  data_array, int bytes_to_read)
{
    int length = 0;
    while (true)
    {
        int x = bytes_to_read - length;
        if (x > 0){
            int length_received = s.receiveBytes(data_array.begin());

        }
    }

    return false;
}