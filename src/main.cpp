#define CL_HPP_TARGET_OPENCL_VERSION 300
#define CL_HPP_ENABLE_EXCEPTIONS
#include <CL/opencl.hpp>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

int main(void) {
  try {
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    std::cout << "Found " << platforms.size() << " platforms." << std::endl;

    cl::Platform plat;
    for (auto &p : platforms) {
      std::string name = p.getInfo<CL_PLATFORM_NAME>();
      std::string platver = p.getInfo<CL_PLATFORM_VERSION>();
      std::cout << "A " << name << " B " << platver << std::endl;
      if (platver.find("OpenCL 3.") != std::string::npos) {
        plat = p;
      }
    }

    if (plat() == 0) {
      std::cout << "No OpenCL 3.0 or newer platform found." << std::endl;
      return EXIT_FAILURE;
    }

    std::vector<cl::Device> devices;
    plat.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    if (devices.size() == 0) {
      std::cout << "No Rusticl devices found." << std::endl;
      exit(1);
    }

    std::cout << "Found " << devices.size() << " devices." << std::endl;
    std::cout << "Using device: " << devices[0].getInfo<CL_DEVICE_NAME>()
              << std::endl;

    cl::Context context({devices[0]});

    cl_int err;

    std::ifstream test_file("../test.cl");
    std::string src(std::istreambuf_iterator<char>(test_file),
                    (std::istreambuf_iterator<char>()));

    cl::Program::Sources sources{src};
    cl::Program program(context, sources);

    try {
      err = program.build("-cl-std=CL3.0");
    } catch (...) {
      std::cout << "err was " << err << std::endl;
      auto buildInfo = program.getBuildInfo<CL_PROGRAM_BUILD_LOG>();
      for (auto &pair : buildInfo) {
        std::cerr << pair.second << std::endl << std::endl;
      }
      return EXIT_FAILURE;
    }

    cl::CommandQueue queue(context, devices[0]);

    size_t n = 2000;

    cl::Buffer buffer_A(context, CL_MEM_READ_WRITE, sizeof(int) * n);
    cl::Buffer buffer_B(context, CL_MEM_READ_WRITE, sizeof(int) * n * 2);

    int tofill = 0;

    queue.enqueueFillBuffer(buffer_A, tofill, 0, sizeof(int) * n);
    queue.enqueueFillBuffer(buffer_B, tofill, 0, sizeof(int) * n * 2);

    cl::KernelFunctor<cl::Buffer, cl::Buffer> decimate(
        cl::Kernel(program, "test"));
    decimate(
        cl::EnqueueArgs(queue, cl::NullRange, cl::NDRange(n), cl::NullRange),
        buffer_A, buffer_B);

    int *data = new int[n * 2];

    queue.enqueueReadBuffer(buffer_B, true, 0, sizeof(int) * n * 2, data);

    queue.finish();

    for (int i = 0; i < 200; i++) {
      std::cout << data[i] << std::endl;
    }

    return EXIT_SUCCESS;
  } catch (const cl::Error &ex) {
    std::cout << ex.what() << " " << ex.err() << std::endl;
    return EXIT_FAILURE;
  } catch (...) {
    return EXIT_FAILURE;
  }
}
