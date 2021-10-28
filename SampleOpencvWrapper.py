import sys
import os
import numpy as np

if os.name == 'nt':
    sys.path.append(
        os.path.join(os.path.dirname(os.path.realpath(__file__)),
                     "build/PyOpencvWrapper/Debug"))
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
                     "build/PyOpencvWrapper"))

import PyOpencvWrapper as pw

print(help(pw))
# py_opencv_wrapper = pw.PyOpencvWrapper()

# print(help(pvp))