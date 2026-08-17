# Aclrtc Runtime Compilation + Static Tensor Programming Add Example

## Overview

This example demonstrates how to use the **Aclrtc (runtime compilation)** mode of AscendC to dynamically compile kernel function source code on the host side and execute it. The kernel function uses the **static tensor programming paradigm** (LocalMemAllocator + SetFlag/WaitFlag event synchronization). The kernel function source code is embedded as a string in the host code and compiled and executed at runtime through the aclrtc API.

Unlike traditional custom operator projects, Aclrtc does not require pre-compiling operator packages (.run), making it suitable for rapid verification and prototype development.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── rtc_template_add
│   ├── CMakeLists.txt                   // Build project file
│   ├── data_utils.h                     // Data generation, precision verification utility functions + ASCENDC_CHECK macro
│   ├── rtc_template_add.cpp             // Host-side code (containing RTC kernel function source string)
│   └── README.md                        // Example documentation
```

## Example Description

- Example functionality:
  Add computation formula:

  ```
  z = x + y
  ```

- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">Add</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">Kernel::add_custom&lt;float&gt;</td></tr>
  </table>

### Example Implementation

### Kernel Implementation

The kernel function source code is embedded as a string in the host code. Memory management is handled through the LocalMemAllocator interface, and event synchronization is achieved through the SetFlag/WaitFlag interfaces, implementing the complete workflow of data transfer, computation, and result transfer out.

### Invocation Implementation

The core process of this example is divided into two phases: the **compilation phase** (aclrtc compiles the template kernel function source code into deviceELF at runtime) and the **execution phase** (aclrt loads the deviceELF onto the device and launches the kernel function, while the host side completes data preparation and precision verification).

**1. Compilation Phase: aclrtc Interfaces**

- `aclrtcCreateProg` -- Create a compilation program object, passing in the kernel function source string
- `aclrtcAddNameExpr` -- Register the template kernel function names to be exported (for example, `Kernel::add_custom<float>`)
- `aclrtcCompileProg` -- Execute runtime compilation, passing `--npu-arch` through options to specify the NPU architecture
- `aclrtcGetBinDataSize` / `aclrtcGetBinData` -- Obtain the binary size and data of the compilation output (deviceELF)
- `aclrtcGetLoweredName` -- Obtain the mangled name of the compiled template kernel function for subsequent `aclrtBinaryGetFunction` lookup
- `aclrtcDestroyProg` -- Destroy the compilation program object

**2. Execution Phase: Key aclrt Interfaces**

- `aclrtMallocHost` / `aclrtMalloc` -- Allocate host/device memory for input/output data
- `aclrtMemcpy` -- Data copy between host and device
- `aclrtBinaryLoadFromData` -- Load the compilation output onto the device
- `aclrtBinaryGetFunction` -- Look up the kernel function handle using the mangled name obtained during the compilation phase
- `aclrtLaunchKernelWithArgsArray` -- Pass three device memory pointer parameters (`x`, `y`, `z`) through a host-side parameter array and launch the kernel function on the specified stream
- `aclrtSynchronizeStream` -- Wait for kernel function execution on the stream to complete

Data generation and precision verification are completed in host-side C++ without relying on external scripts.

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # Build the project, supports dav-3510, dav-2201
  ./demo                                                                    # Run the compiled executable to execute the example
  ```

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

  > [!WARNING] Notice
  Aclrtc only supports NPU runtime mode. It does not support CPU debugging or simulation mode.

- Execution result
  The following output indicates successful precision comparison.
  ```bash
  test pass!
  ```
