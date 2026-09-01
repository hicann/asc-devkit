# fill Example

## Overview

This example implements the `fill` operation in scalar fill mode based on the Tensor API. It primarily calls the experimental `asc::te::experimental::fill` interface.

## Supported Products and CANN Versions

| Product | CANN Version |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure

```text
fill
├── scripts
│   └── gen_data.py            // Script for generating ground truth data
├── CMakeLists.txt             // Build project file
├── data_utils.h               // Data read/write functions
├── fill.asc                   // Ascend C example implementation and invocation
└── README_en.md               // Example documentation
```

## Example Description

- Function:  
  Fills every position of the output vector with the scalar value 1. The vector contains 256 elements of the float data type.
- Specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="2" align="center">Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function</td><td colspan="4" align="center">fill</td></tr>
  </table>
- Implementation:  
  The `fill_vf` function calls the `fill` interface to perform scalar filling:
  - Use `fill` to write the scalar value to every element of the destination `reg_tensor`.
  - Use `store` to write the result back to UB.
  - Invoke the kernel function using the `<<<>>>` kernel launch syntax.

## Build and Run

Perform the following steps in the example root directory to build and run the example.

- Configure environment variables  
  Configure the environment variables according to the CANN development kit [installation instructions](../../../../../../docs/en/quick_start.md#prepare&install). **Currently, only [CANN master](../../../../../../docs/en/quick_start.md#cann-install) is supported.**

  > **Note:** `${install_path}` is the CANN package installation directory. If no installation directory is specified, the default directory is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory:

  ```bash
  mkdir -p build && cd build;                                               # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..;make -j; # Build the project (NPU mode by default)
  python3 ../scripts/gen_data.py                                            # Generate test ground truth data
  ./demo                                                                    # Run the generated executable
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` option.

  Example:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCANN_ASC_USE_EXPERIMENTAL=ON ..;make -j; # NPU simulation mode
  ```

  > **Note:** Clear the CMake cache before switching build modes. Run `rm CMakeCache.txt` in the `build` directory, and then run CMake again.

- Build options

| Option | Values | Description |
| --- | --- | --- |
| `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution or NPU simulation |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: `dav-3510` corresponds to Ascend 950PR/Ascend 950DT |
| `CANN_ASC_USE_EXPERIMENTAL` | `ON` (required for this example), `OFF` (default) | Enables experimental ASC interfaces |

- Execution result

  The following output indicates that the accuracy comparison is successful:

  ```bash
  test pass!
  ```
