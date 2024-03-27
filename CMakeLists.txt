cmake_minimum_required(VERSION 3.1)
project(cvtest VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)

add_executable(cvtest main.cpp)

target_compile_features(cvtest PRIVATE cxx_std_11)
target_link_libraries(cvtest PRIVATE ${OpenCV_LIBS} Threads::Threads)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
