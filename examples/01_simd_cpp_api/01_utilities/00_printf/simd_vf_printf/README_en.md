# SimdVF Printf API Description

## Overview

This example introduces the usage of the printf API on the simd_vf side. The kernel function uses fixed inputs (without relying on external data) and prints debug information and computation results through the `printf` API in vector functions.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── simd_vf_printf
│   ├── CMakeLists.txt          // Build project file
│   ├── simd_vf_printf.asc      // Ascend C example implementation & invocation example
│   └── README.md               // Example description document
```

## Example Description

- Example features:

  Using the vector programming mode, the kernel function uses fixed inputs to demonstrate the basic usage of the simd_vf printf API. The kernel function sequentially calls the simd_vf printf API to print data of different types and computation results.

  This example demonstrates the following printf features:
  1. `printf` prints int/uint/float/string formatted data
  2. `printf` prints hex/ptr formatted data
  3. simd_vf `printf` prints plain strings
  4. Write fixed data into a static `LocalTensor` in the `.asc` file, perform tensor addition via AICore `AscendC::Add`, and print the computation result via simd_vf `printf`
  5. AICore `AscendC::printf` prints kernel function start, blockIdx, and end messages

  | Format Specifier | Description | Example |
  |------------------|-------------|---------|
  | `%d` | Integer print | `printf(fmt, 10);` |
  | `%u` | Unsigned integer print | `printf(fmt, 20U);` |
  | `%x` | Hexadecimal print | `printf(fmt, 255);` |
  | `%f` | Floating-point print (float) | `printf(fmt, 3.14f);` |
  | `%s` | String print | `printf(fmt, "test");` |
  | `%p` | Pointer print | `printf(fmt, ptr);` |

- Invocation implementation

  Use `asc_vf_call<FuncName>()` inside the `__global__ __vector__` kernel function to call simd_vf functions. The kernel function uses fixed input values and does not rely on external data or host-side data transfer. In the Add demonstration, the AICore side writes fixed data into a static UB `LocalTensor`, calls `AscendC::Add` to produce the result, and then passes the UB address to the simd_vf function for printing.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.

  ```bash
  mkdir -p build && cd build;                                               # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # Build the project
  ./demo                                                                     # Run the example
  ```

- Build options

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (default) | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result
  The execution log shows printf output from both the AICore side and the simd_vf side.

  ```bash
  [AIV Block 0/1] [aicore] simd_vf printf demo start ...
  [AIV Block 0/1] [aicore] blockIdx=0
  [simd_vf] int=10, uint=20, float=3.140000, string=hello
  [simd_vf] hex=ff, ptr=0x12ff
  [simd_vf] This is a simd_vf printf demo string.
  [simd_vf] add[0]: 1.250000 + 10.000000 = 11.250000
  [AIV Block 0/1] [aicore] simd_vf printf demo end ...
  ```