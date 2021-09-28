import sys
import os

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

try:

    x = pvp.PyVideoParser(
        os.path.join(os.path.dirname(os.path.realpath(__file__)),
                     "videos/2min_1080p.mp4"))
    print(f"Width: {x.Width()}, Height: {x.Height()}, Codec: {x.Codec()}")
except (pvp.VideoParserException, ValueError) as e:
    print(f"Here {e}")

# print(help(pvp))