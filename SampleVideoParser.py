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
    file_name = os.path.join(os.path.dirname(os.path.realpath(__file__)), "videos/1.264")
    # nvDmx = pvp.PyVideoParser(
    #     os.path.join(os.path.dirname(os.path.realpath(__file__)),
    #                  "videos/2min_1080p.mp4"))
    # nvDmx = pvp.PyVideoParser(
    #     os.path.join(os.path.dirname(os.path.realpath(__file__)),
    #                  "videos/1.264"))
    # nvDmx = pvp.PyVideoParser(
    #     "rtsp://admin:AdmiN1234@192.168.0.72/live1s1.sdp")
    with open(file_name, "rb") as f:
        nvDmx = pvp.PyVideoParser()
        print(
            f"Width: {nvDmx.Width()}, Height: {nvDmx.Height()}, Codec: {nvDmx.Codec()}"
        )
        packet_data = pvp.PacketData()
        frame_count = 0
        while True:
            try:
                data = f.read(1024)
                # Demuxer has sync design, it returns packet every time it's called.
                # If demuxer can't return packet it usually means EOF.
                if not nvDmx.DemuxSinglePacket(data, packet):
                    break
                nvDmx.LastPacketData(packet_data)
                frame_count += 1
                # print(
                #     f"pts: {packet_data.pts} dts: {packet_data.dts} poc: {packet_data.poc} pos: {packet_data.pos} duration: {packet_data.duration} packet size: {packet.size}"
                # )
            except KeyboardInterrupt:
                break
    print(f"DemuxSinglePacket returned false with frame count {frame_count}")

except (pvp.VideoParserException, ValueError) as e:
    print(f"Here {e}")

# print(help(pvp))