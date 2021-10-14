cmake_minimum_required(VERSION 3.5)
project(state_machine)

if(NOT CMAKE_C_STANDARD)
  set(CMAKE_C_STANDARD 99)
endif()

if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

add_executable(motor)
target_sources(motor PRIVATE src/motor_main.cpp src/motor.cpp
                             src/state_machine.cpp)
target_include_directories(motor PRIVATE include)

add_executable(centrifuge_test)
target_sources(
  centrifuge_test PRIVATE src/centrifuge_test_main.cpp src/centrifuge_test.cpp
                          src/self_test.cpp src/state_machine.cpp)
target_include_directories(centrifuge_test PRIVATE include)
