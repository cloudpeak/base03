# Support systems
  + Windows XP or later, Linux, MacOS X

# How to build
  + git clone https://github.com/cloudpeak/base03.git
  + mkdir build
  + cd build
  + Visual Studio 2015 Win64
    + cmake -G "Visual Studio 14 2015 Win64" ../base03 -DCMAKE_BUILD_TYPE=RELEASE
  + Visual Studio 2015 Win32
    + cmake -G "Visual Studio 14 2015" ../base03 -DCMAKE_BUILD_TYPE=RELEASE
  + GCC or Clang
    + cmake .. -DCMAKE_BUILD_TYPE=RELEASE && make
