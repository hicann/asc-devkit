# Large Tensor Printing

## Overview

This sample demonstrates large tensor printing. Large tensor printing refers to a single tensor print whose data exceeds the FIFO capacity of the current core. The framework automatically transfers the data in segments and reuses FIFO space.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
| --- | --- |
| Ascend 950PR&950DT series | >= CANN 9.3.0 |
| Atlas A3 series | >= CANN 9.3.0 |
| Atlas A2 series | >= CANN 9.3.0 |

## Directory Structure

```text
large_tensor_dump
├── scripts
│   ├── gen_data.py           # Generate input and reference data
│   └── verify_result.py      # Compare output with reference data
├── CMakeLists.txt            # Build configuration
├── data_utils.h              # Binary data read/write functions
├── large_tensor_dump.asc     # Ascend C implementation and kernel launch
├── README.md                # Example documentation
└── README_en.md
```

## Example Description

- Example functionality:

  One Vector core performs vector-scalar addition and prints the intermediate UB tensor and the final GM tensor to demonstrate output larger than the per-core FIFO.

  The computation formula is:

  ```text
  y = x + 1
  ```

- Example specifications:

  Input and output contain 65,536 float32 elements. The input data is `x[i] = i % 256`. The per-core FIFO uses the default size of 32 KiB. The printed tensors are listed below:

  | Tensor | desc | Elements | Payload |
  | --- | --- | --- | --- |
  | First output tile in UB | 100 | 16,384 | 64 KiB |
  | Complete output in GM | 200 | 65,536 | 256 KiB |

- Printing API usage:

  This example uses `asc_dump` to output UB and GM data.

  | Memory | Address Type | Example Call |
  | --- | --- | --- |
  | UB | `__ubuf__ float *` | `asc_dump<float>(ubPtr, 100, 16384);` |
  | GM | `__gm__ float *` | `asc_dump<float>(gmPtr, 200, 65536);` |

  For large tensor printing, at most the first 32 KiB of raw tensor data is displayed on screen. The remaining data is replaced with `...` and a warning. To view the complete data, set `ASCEND_DUMP_PATH` to enable dumping to disk, then parse the files with `show_kernel_debug_data`. See the [tool guide (Chinese)](../../../../../docs/zh/guide/programming_guide/appendix/show_kernel_debug_data_tool.md) and [tool example](https://gitcode.com/cann/asc-tools/tree/master/examples/01_show_kernel_debug_data).

- Invocation implementation:

  Launch the kernel using `<<<>>>`. The kernel processes 16,384 elements at a time and writes the results back to GM. The host uses `ReadFile` and `WriteFile` to read and write binary data.

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
  mkdir -p build && cd build                                                # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 .. && make -j                    # Build the project
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify the output result to confirm algorithm correctness
  ```

- Build options

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 products and Atlas A3 products; dav-3510 corresponds to Ascend 950PR&950DT products |

- Execution result
  A sample of the terminal output is shown below:

  ```text
  [AIV Block 0] DumpTensor: desc=100, addr=0x10000, data_type=float32, position=UB, dump_size=16384
  1.000000,2.000000,3.000000,4.000000,...,253.000000,254.000000,255.000000,256.000000,...
  Warning: the tensor data is too large. Only the first 32768 bytes are printed and the rest is omitted. To view the full data, set the environment variable ASCEND_DUMP_PATH or ASCEND_WORK_PATH to enable dumping it to disk, and then use the show_kernel_debug_data tool to view it.
  [AIV Block 0] DumpTensor: desc=200, addr=0x57000, data_type=float32, position=GM, dump_size=65536
  1.000000,2.000000,3.000000,4.000000,...,253.000000,254.000000,255.000000,256.000000,...
  Warning: the tensor data is too large. Only the first 32768 bytes are printed and the rest is omitted. To view the full data, set the environment variable ASCEND_DUMP_PATH or ASCEND_WORK_PATH to enable dumping it to disk, and then use the show_kernel_debug_data tool to view it.
  ```

  When the computation result passes verification, the script prints:

  ```bash
  test pass!
  ```
