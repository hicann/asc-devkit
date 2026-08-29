# Aclrtc Runtime Compilation + printf Output Example

## Overview

This example demonstrates how to use the **Aclrtc (runtime compilation)** mode of AscendC to dynamically compile kernel function source code on the host side and execute it. The kernel function is a simple `hello_world` function that outputs results via printf. The kernel function source code is embedded as a string in the host code and compiled and executed at runtime through the aclrtc API.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.1.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.1.0 |

## Directory Structure

```
├── rtc_hello_world
│   ├── CMakeLists.txt           // Build project file
│   ├── rtc_hello_world.cpp      // Host-side code (containing RTC kernel function source string)
│   └── README.md                // Example documentation
```

## Example Description

### Kernel Implementation

The kernel function source code is embedded as a string in the host code and outputs results via printf.

### Invocation Implementation

The core process of this example is divided into two phases: the **compilation phase** (aclrtc compiles the kernel function source code into deviceELF at runtime) and the **execution phase** (aclrt loads the deviceELF onto the device and launches the kernel function).

**1. Compilation Phase: aclrtc Interfaces**

- `aclrtcCreateProg` -- Create a compilation program object, passing in the kernel function source string
- `aclrtcCompileProg` -- Execute runtime compilation, passing `--npu-arch` through options to specify the NPU architecture
- `aclrtcGetCompileLogSize` / `aclrtcGetCompileLog` -- (Optional) Retrieve error logs when compilation fails
- `aclrtcGetBinDataSize` / `aclrtcGetBinData` -- Obtain the binary size and data of the compilation output (deviceELF)
- `aclrtcDestroyProg` -- Destroy the compilation program object

**2. Execution Phase: Key aclrt Interfaces**

- `aclrtBinaryLoadFromData` -- Load the compilation output onto the device
- `aclrtBinaryGetFunction` -- Obtain the kernel function handle using the kernel function name `hello_world`
- `aclrtLaunchKernelWithArgsArray` -- The kernel function in this example has no parameters; pass an empty parameter array and launch the kernel function
- `aclrtSynchronizeStream` -- Wait for kernel function execution on the stream to complete

## Build and Run

Run the following steps in the root directory of this example to build and execute the example.

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in this example directory.
  ```bash
  mkdir -p build && cd build;                                               # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # Build the project
  ./demo                                                                    # Run the compiled executable to execute the example
  ```

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

  > [!WARNING] Notice
  Aclrtc only supports NPU runtime mode. It does not support CPU debugging or simulation mode.

- Execution result
  The following output indicates successful execution.
  
  ```bash
  [AIV Block 0/8] Hello World!!!
  [AIV Block 1/8] Hello World!!!
  [AIV Block 2/8] Hello World!!!
  [AIV Block 3/8] Hello World!!!
  [AIV Block 4/8] Hello World!!!
  [AIV Block 5/8] Hello World!!!
  [AIV Block 6/8] Hello World!!!
  [AIV Block 7/8] Hello World!!!
  ```
