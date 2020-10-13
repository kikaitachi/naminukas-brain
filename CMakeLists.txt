cmake_minimum_required(VERSION 3.14)

project(naminukas VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(ExternalProject)
include(FetchContent)

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "^arm")
set(DYNAMIXEL_BUILD_DIR c++/build/linux_sbc)
set(DYNAMIXEL_OBJECT_FILE_NAME libdxl_sbc_cpp.so)
else()
set(DYNAMIXEL_BUILD_DIR c++/build/linux64)
set(DYNAMIXEL_OBJECT_FILE_NAME libdxl_x64_cpp.so)
endif()

ExternalProject_Add(DynamixelSDK
  GIT_REPOSITORY    https://github.com/ROBOTIS-GIT/DynamixelSDK.git
  GIT_TAG           3.7.31
  SOURCE_SUBDIR     ${DYNAMIXEL_BUILD_DIR}
  CONFIGURE_COMMAND ""
  BUILD_IN_SOURCE   1
  INSTALL_COMMAND   ""
)

ExternalProject_Get_property(DynamixelSDK SOURCE_DIR)
set(DYNAMIXEL_DIR "${SOURCE_DIR}")
include_directories("${DYNAMIXEL_DIR}/c++/include")

FetchContent_Declare(librobotcontrol
  GIT_REPOSITORY    https://github.com/beagleboard/librobotcontrol.git
  GIT_TAG           V1.0.5
)
FetchContent_MakeAvailable(librobotcontrol)
include_directories("${librobotcontrol_SOURCE_DIR}/library/include")

find_package(Threads)

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

add_executable(naminukas src/main.cpp
  src/Logger.cpp
  src/IOServer.cpp
  src/WebSocket.cpp
  src/Message.cpp
  src/Telemetry.cpp)

add_dependencies(naminukas DynamixelSDK)

target_link_libraries(naminukas
  "${DYNAMIXEL_DIR}/${DYNAMIXEL_BUILD_DIR}/${DYNAMIXEL_OBJECT_FILE_NAME}"
  robotics_cape
  Threads::Threads
  OpenSSL::Crypto)
