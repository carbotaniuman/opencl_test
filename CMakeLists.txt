cmake_minimum_required(VERSION 3.22)

project(opencl_test VERSION 1.0 LANGUAGES CXX)
find_package(OpenCL REQUIRED)

# Add a library
add_executable(opencl_test src/main.cpp)
target_include_directories(opencl_test PUBLIC include)
target_compile_features(opencl_test PRIVATE cxx_std_17)
set_target_properties(opencl_test PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
target_link_libraries(opencl_test PRIVATE OpenCL::OpenCL)
