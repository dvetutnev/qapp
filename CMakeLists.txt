cmake_minimum_required(VERSION 3.7)
project(qapp C CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

include(conan.cmake)

conan_cmake_autodetect(settings)
conan_cmake_install(
    PATH_OR_REFERENCE ${CMAKE_SOURCE_DIR}/conanfile.txt
    BUILD missing
    REMOTE conan-center
    SETTINGS ${settings})

find_package(GTest REQUIRED)

enable_testing()

add_library(common OBJECT logger.c)

add_executable(logger $<TARGET_OBJECTS:common> main.c)

add_executable(test_logger $<TARGET_OBJECTS:common> test_logger.cpp)
target_link_libraries(test_logger GTest::gtest_main)
add_test(NAME test_logger COMMAND test_logger)


