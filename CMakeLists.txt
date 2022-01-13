cmake_minimum_required (VERSION 3.16)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_BUILD_TYPE Debug)
set (CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(opengl-ui)

find_package(glfw3 REQUIRED)
find_package(OpenGL REQUIRED)

if(gtest_SOURCE_DIR)
    message(STATUS "gtest variables already defined. Skipping.")
else()
    add_subdirectory(googletest)
endif()

include(CTest)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fstandalone-debug") # clang debugger flag

set(glad_src src/glad.c)

add_library(${PROJECT_NAME} STATIC src/glad.c)

target_include_directories(${PROJECT_NAME} PUBLIC inc
                                                  src)

target_link_libraries(${PROJECT_NAME} PUBLIC ${CMAKE_DL_LIBS}
                                             gtest_main
                                             OpenGL::GL
                                             glfw)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_LIST_DIR}/inc/shaders/* ${CMAKE_BINARY_DIR})

#####################################################
## Unit tests definitions
#####################################################

# Per default, BUILD_TESTING is enabled by ctest
if(NOT BUILD_TESTING)
	return()
endif()


set(testTarget ${PROJECT_NAME}_test)
set(testSource tst/test.cpp)

add_executable(${testTarget} ${testSource}
                             ${glad_src})


target_include_directories(${testTarget} PUBLIC inc)

target_link_libraries(${testTarget} PUBLIC ${CMAKE_DL_LIBS}
                                           gtest_main
                                           OpenGL::GL
                                           glfw)


add_test (NAME ${testTarget}
          COMMAND ${testTarget})
