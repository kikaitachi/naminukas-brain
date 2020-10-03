cmake_minimum_required(VERSION 3.14)

project(naminukas VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(ExternalProject)

IF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
set(DYNAMIXEL_BUILD_DIR c++/build/linux_sbc)
ELSE(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")
set(DYNAMIXEL_BUILD_DIR c++/build/linux64)
ENDIF(${CMAKE_SYSTEM_PROCESSOR} MATCHES "arm")

ExternalProject_Add(DynamixelSDK
  GIT_REPOSITORY    https://github.com/ROBOTIS-GIT/DynamixelSDK.git
  GIT_TAG           3.7.31
  SOURCE_SUBDIR     ${DYNAMIXEL_BUILD_DIR}
  CONFIGURE_COMMAND ""
  BUILD_IN_SOURCE   1
  INSTALL_COMMAND   ""
)

add_executable(naminukas src/main.cpp)
