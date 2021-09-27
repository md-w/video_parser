import sys
import os

if os.name == 'nt':
    sys.path.append(
        os.path.join(os.path.dirname(os.path.realpath(__file__)),
                     "build/PyVideoParser/Debug"))
else:
    sys.path.append(
        os.path.join(os.path.dirname(os.path.realpath(__file__)),
                     "build/PyVideoParser"))
import PyVideoParser as pvp

try:
    x = pvp.PyVideoParser("Hello")
except pvp.VideoParserException as e:
    print(f"Here {e}")

# print(help(pvp))