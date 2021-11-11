import sys
import os
import numpy as np
import threading
import time

if os.name == 'nt':
    sys.path.append(
        os.path.join(os.path.dirname(os.path.realpath(__file__)),
                     "build/PyOpencvWrapper/Debug"))
    try:
        ffmpeg_path = os.environ["FFMPEG_PATH"]
    except KeyError:
        ffmpeg_path = os.path.join(os.path.dirname(os.path.realpath(__file__)),
                                   "thirdparty/ffmpeg/bin")

    third_party_path = os.path.join(
        os.path.dirname(os.path.realpath(__file__)), "thirdparty/install/bin")
    if os.path.isdir(third_party_path):
        os.add_dll_directory(third_party_path)
    else:
        print("Third party path does not exists")
        exit(1)

    if ffmpeg_path:
        if os.path.isdir(ffmpeg_path):
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


# print(help(pw))
class TestCallback:
    def __init__(self) -> None:
        pass

    def on_event(self) -> int:
        return 0


def on_analytics_event(in_val: int) -> int:
    print(f"On event called {threading.get_ident()} {in_val}")
    return in_val + 12


# py_opencv_wrapper = pw.PyOpencvWrapper()
# py_opencv_wrapper.analytics_event_handler(on_analytics_event)

# image = np.ndarray(shape=(2), dtype=np.uint8)
# py_opencv_wrapper.call(image)
# test_async_callbacks()
# print(help(pvp))
# time.sleep(10)

py_vtpl_video_frame = pw.VtplVideoFrame()
mat2 = np.array(py_vtpl_video_frame, dtype=np.ubyte, copy=False)
print(mat2)
array = np.ones((100,100,4))
print(array.ndim)
print(array.strides)