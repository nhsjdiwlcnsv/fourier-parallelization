cmake_minimum_required(VERSION 3.26)

project(fourier-parallelization)

find_package(OpenMP REQUIRED)
find_package(OpenCV REQUIRED)

set(CMAKE_CXX_STANDARD 23)

add_executable(${PROJECT_NAME}
        main.cpp
        nn/AlexNet.cpp
        nn/AlexNet.h
        src/fft.cpp
        src/fft.hpp
        src/tools.cpp
        src/tools.hpp
        src/types.hpp
        src/ShaderExecutor.cpp
        src/ShaderExecutor.hpp
)

target_include_directories(${PROJECT_NAME} PUBLIC ${OpenCV_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PUBLIC metal-cpp/)
target_include_directories(${PROJECT_NAME} PUBLIC metal-cpp/Metal)
target_include_directories(${PROJECT_NAME} PUBLIC metal-cpp/Foundation)
target_include_directories(${PROJECT_NAME} PUBLIC metal-cpp/QuartzCore)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")

target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_C)
target_link_libraries(${PROJECT_NAME} OpenMP::OpenMP_CXX)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
target_link_libraries(${PROJECT_NAME}
        "-framework Metal"
        "-framework Foundation"
        "-framework QuartzCore"
)

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
        COMMAND xcrun -sdk macosx metal -c ${CMAKE_CURRENT_SOURCE_DIR}/metal-library/library.metal -o ${CMAKE_CURRENT_SOURCE_DIR}/metal-library/library.air
        COMMAND xcrun -sdk macosx metallib ${CMAKE_CURRENT_SOURCE_DIR}/metal-library/library.air -o ${CMAKE_CURRENT_SOURCE_DIR}/metal-library/library.metallib
)