import sys
import os
import numpy as np

if os.name == 'nt':
    sys.path.append(
        os.path.join(os.path.dirname(os.path.realpath(__file__)),
                     "build/PyVideoParser/Debug"))
    try:
        ffmpeg_path = os.environ["FFMPEG_PATH"]
    except KeyError:
        ffmpeg_path = os.path.join(os.path.dirname(os.path.realpath(__file__)),
                                   "thirdparty/ffmpeg/bin")

    if ffmpeg_path:
        os.add_dll_directory(ffmpeg_path)
    else:
        print("FFMPEG_PATH environment variable is not set.", file=sys.stderr)
        print("Can't set CUDA DLLs search path.", file=sys.stderr)

        exit(1)
else:
    sys.path.append(
        os.path.join(os.path.dirname(os.path.realpath(__file__)),
                     "build/PyVideoParser"))
import PyVideoParser as pvp

packet = np.ndarray(shape=(0), dtype=np.uint8)
try:
    # nvDmx = pvp.PyVideoParser(
    #     os.path.join(os.path.dirname(os.path.realpath(__file__)),
    #                  "videos/2min_1080p.mp4"))

    # nvDmx = pvp.PyVideoParser("rtsp://admin:AdmiN1234@192.168.1.72/live1s1.sdp")
    nvDmx = pvp.PyVideoParser()
    print(f"Width: {nvDmx.Width()}, Height: {nvDmx.Height()}, Codec: {nvDmx.Codec()}")
    packet_data = pvp.PacketData()
    while True:
        # Demuxer has sync design, it returns packet every time it's called.
        # If demuxer can't return packet it usually means EOF.
        if not nvDmx.DemuxSinglePacket(packet):
            break
        nvDmx.LastPacketData(packet_data)
        print(f"pts: {packet_data.pts} dts: {packet_data.dts} poc: {packet_data.poc} pos: {packet_data.pos} duration: {packet_data.duration} packet size: {packet.size}")
    print("DemuxSinglePacket returned false")

except (pvp.VideoParserException, ValueError) as e:
    print(f"Here {e}")

# print(help(pvp))