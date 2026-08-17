# Assert API Description

## Overview

This example introduces the usage of the ascendc_assert assertion API. If the assertion condition evaluates to false, an error message is printed and program execution is terminated.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|---------|----------------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── 01_assert
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying output data against ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── assert.asc              // Ascend C example implementation & invocation example
│   └── README.md               // Example description document
```

## Example Description

- Example features:

  Implement matrix multiplication using the static Tensor programming mode to demonstrate the basic usage of the ascendc_assert API.

  The matrix multiplication formula is:

  ```
  C = A * B
  ```

- Example specifications:

  The example parameters are: M = 256, N = 256, K = 64. The shape information is shown in the following table:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">Matmul</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">a</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td align="center">b</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">c</td><td align="center">[M, N]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">mmad_custom</td></tr>
  </table>

- ascendc_assert API usage:

  | Usage | Example |
  |-------|---------|
  | Basic assertion | `ascendc_assert(condition, "message.\n");` |
  | Assertion with parameters | `ascendc_assert(value > 0, "Value %d must be positive.\n", value);` |
  | Alignment check | `ascendc_assert(size % 16 == 0, "Size %u must align to 16.\n", size);` |
  | Range check | `ascendc_assert(idx < max, "Index %u exceeds max %u.\n", idx, max);` |

  Notice: The ascendc_assert API affects runtime performance. It is recommended to use it only during the debugging phase.

- Invocation implementation

  Use the kernel launch syntax <<<>>> to call the kernel function.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;                                               # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # Build the project
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify the output result to confirm algorithm correctness
  ```

- Build options

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result
  The final execution result is as follows, indicating successful accuracy comparison.
  ```bash
  test pass!
  ```