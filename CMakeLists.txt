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

message("${WebP_INCLUDE_DIRS}")
message("${WebP_LIBRARIES}")

find_package(realsense2 REQUIRED)

enable_testing()
find_package(GTest REQUIRED)
include(GoogleTest)

set(COMMON_SOURCES
  src/action/Action.cpp
  src/action/ActionRotate.cpp
  src/action/ActionSequential.cpp
  src/geom/Point3D.cpp
  src/geom/Orientation.cpp
  src/Logger.cpp
  src/IOServer.cpp
  src/WebSocket.cpp
  src/Message.cpp
  src/Telemetry.cpp
  src/DynamixelUtil.cpp
  src/Hardware.cpp
  src/DynamixelKinematics.cpp
  src/Robot.cpp
  src/SystemTelemetry.cpp
  src/Client.cpp
  src/HardwareBeagleBoneBlue.cpp
  src/Model.cpp
  src/Transform.cpp
  src/Camera.cpp
  src/locomotion/Locomotion.cpp
  src/locomotion/LocomotionCaterpillar.cpp
  src/locomotion/LocomotionHang.cpp
  src/locomotion/LocomotionIdle.cpp
  src/locomotion/LocomotionPole.cpp
  src/locomotion/LocomotionPoleGestures.cpp
  src/locomotion/LocomotionSegway.cpp
  src/locomotion/LocomotionSki.cpp
  src/locomotion/LocomotionTail.cpp
  src/locomotion/LocomotionUnicycle.cpp
  src/locomotion/LocomotionWaddle.cpp
  src/locomotion/TraitTilting.cpp
  src/impl/RCRadio_PRU.cpp
  ${DynamixelSDK_SRC})

add_executable(${PROJECT_NAME}
  ${COMMON_SOURCES}
  src/main.cpp
)

add_executable(tests
  ${COMMON_SOURCES}
  test/main.cpp
)

set(COMMON_LIBRARIES
  robotics_cape
  webp
  Threads::Threads
  OpenSSL::Crypto
  ${WebP_LIBRARIES}
  ${realsense2_LIBRARY}
)

target_link_libraries(${PROJECT_NAME}
  ${COMMON_LIBRARIES}
)

target_link_libraries(tests
  ${COMMON_LIBRARIES}
  GTest::GTest
)
