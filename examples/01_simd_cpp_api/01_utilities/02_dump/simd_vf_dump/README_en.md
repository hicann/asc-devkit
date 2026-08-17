# SimdVF Dump API Description

## Overview

This example introduces the usage of the asc_dump series APIs on the simd_vf side. In vector functions, APIs such as `asc_dump_ubuf`, `asc_dump_reg`, and `asc_dump` enable visual printing of UB and register data. On the host side, `ReadFile` and `WriteFile` read and write binary data. The kernel reads input data from GM, performs Adds computation, and writes the result back to GM. The result is then verified by comparing the output against the ground truth.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```
├── simd_vf_dump
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying output data against ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── simd_vf_dump.asc        // Ascend C example implementation & invocation example
│   └── README.md               // Example description document
```

## Example Description

- Example features:

  Using the vector programming mode, this example demonstrates the basic usage of the asc_dump series APIs on the simd_vf side. The kernel function reads binary input data through `ReadFile`, uses `asc_vf_call` to invoke simd_vf functions for dump printing, performs Adds computation, writes binary output data through `WriteFile`, and finally compares the output against ground truth through a verification script.

  1. `asc_dump_ubuf<T>` - Print UB data
  2. `asc_dump_reg<T>` - Print register data
  3. `asc_dump<T>` - Print UB/Reg data (generic API)

- simd_vf dump API usage:

  | API | Address Type | Example |
  |-----|-------------|---------|
  | `asc_dump_ubuf<T>` | `__ubuf__` | `asc_dump_ubuf<float>(input, desc, dump_size);` |
  | `asc_dump_reg<T>` | Reg | `asc_dump_reg<float>(input, desc, dump_size);` |
  | `asc_dump<T>` | `__ubuf__` / Reg | `asc_dump<float>(input, desc, dump_size);` |

  Parameter description:
  - Template parameter `T`: Specifies the data type (float/half, and so on)
  - `input`: UB data address or Reg register object
  - `desc`: Print control descriptor
  - `dump_size`: Number of data elements per single dump

- Invocation implementation

  Use `asc_vf_call<FuncName>()` inside the `__global__ __vector__` kernel function to call simd_vf functions for dump printing, then perform computation through `AscendC::Adds` and write results back to GM. The host side reads and writes binary data through `ReadFile`/`WriteFile`.

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
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify the output result to confirm algorithm correctness
  ```

- Build options

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (default) | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result
  The final execution result is as follows, indicating successful accuracy comparison.
  ```bash
  test pass!
  ```
