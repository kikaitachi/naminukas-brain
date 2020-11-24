# naminukas

![Last build result](https://github.com/kikaitachi/naminukas-robot/workflows/CI/badge.svg)

Software running on naminukas robot.

For remote control software see [kikaitachi-ui project](https://github.com/kikaitachi/kikaitachi-ui).

## Installation

Install required libraries:
```
sudo apt-get install libwebp-dev libgtest-dev
```

On BeagleBone Blue librealsense2 must be [compliled from source](https://github.com/IntelRealSense/librealsense/blob/master/doc/installation.md). So far I got best results by forcing usage of `libuvc`:
```
cmake ../ -DCMAKE_BUILD_TYPE=Release -DBUILD_GRAPHICAL_EXAMPLES=false -DFORCE_RSUSB_BACKEND=true
```

## Coding style

See: https://google.github.io/styleguide/cppguide.html
