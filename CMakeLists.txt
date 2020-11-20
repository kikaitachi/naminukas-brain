cmake_minimum_required(VERSION 3.14)

project(naminukas VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)

FetchContent_Declare(DynamixelSDK
  GIT_REPOSITORY    https://github.com/ROBOTIS-GIT/DynamixelSDK.git
  GIT_TAG           3.7.31
)
FetchContent_MakeAvailable(DynamixelSDK)
include_directories("${FETCHCONTENT_BASE_DIR}/dynamixelsdk-src/c++/include/dynamixel_sdk")
file(GLOB DynamixelSDK_SRC
    "${FETCHCONTENT_BASE_DIR}/dynamixelsdk-src/c++/src/dynamixel_sdk/*.cpp"
)

FetchContent_Declare(librobotcontrol
  GIT_REPOSITORY    https://github.com/beagleboard/librobotcontrol.git
  GIT_TAG           V1.0.5
)
FetchContent_MakeAvailable(librobotcontrol)
include_directories("${librobotcontrol_SOURCE_DIR}/library/include")

find_package(Threads)

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

find_package(realsense2 REQUIRED)

add_executable(naminukas
  src/main.cpp
  src/Logger.cpp
  src/IOServer.cpp
  src/WebSocket.cpp
  src/Message.cpp
  src/Telemetry.cpp
  src/DynamixelUtil.cpp
  src/DynamixelKinematics.cpp
  src/Robot.cpp
  src/SystemTelemetry.cpp
  src/Client.cpp
  src/Locomotion.cpp
  src/BeagleBoneBlueIMU.cpp
  src/Model.cpp
  src/Transform.cpp
  ${DynamixelSDK_SRC}
)

target_link_libraries(${PROJECT_NAME}
  robotics_cape
  Threads::Threads
  OpenSSL::Crypto
  ${realsense2_LIBRARY}
)
