cmake_minimum_required(VERSION 3.13)
project(gpsdo_status)

file(GLOB_RECURSE SRC "src/*.cpp" "vendor/*.cpp")

add_executable(${PROJECT_NAME} ${SRC})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)
target_include_directories(${PROJECT_NAME} PRIVATE "src/" "vendor/")