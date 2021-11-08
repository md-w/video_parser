
#include "VtplVmsStreamGrabberFrameSrc.h"
#include <iostream>

int main(int argc, char const* argv[])
{
  try
  {
    VtplVmsStreamGrabberFrameSrc grabber("vms://192.168.1.178:3005/1/1/1/1");
  }
  catch(const std::exception& e)
  {
    std::cerr << "Here: " << typeid(e).name()  << '\n';
  }


  std::cout << "Hello world" << std::endl;
  return 0;
}