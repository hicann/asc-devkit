# fill Example

## Overview

This example uses the experimental `asc::te::experimental::fill` interface for scalar filling and broadcasting the highest element of a register.

## Supported Products and CANN Versions

| Product | CANN Version |
| --- | --- |
| Ascend 950PR&950DT products | >= CANN 9.3.0 |

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
  The `fill_vf` function fills a source register with 1 and sets its lowest element to 0 to distinguish the two broadcast positions.
  - Use `update_mask` to generate the write mask for the current block.
  - Call `fill<high_low_part::high>` to broadcast the highest source element, then use `store` to write the result back to UB.
  - Invoke the kernel function using the `<<<>>>` kernel launch syntax.

## Build and Run

Perform the following steps in the example root directory to build and run the example.

- Configure environment variables  
  Configure the environment variables according to the CANN development kit [installation instructions](../../../../../../docs/en/quick_start.md#prepare&install).

  > **Note:** `${install_path}` is the CANN package installation directory. If no installation directory is specified, the default directory is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory:

  ```bash
  mkdir -p build && cd build;                                               # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # Build the project (NPU mode by default)
  python3 ../scripts/gen_data.py                                            # Generate test ground truth data
  ./demo                                                                    # Run the generated executable
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` option.

  Example:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Clear the CMake cache before switching build modes. Run `rm CMakeCache.txt` in the `build` directory, and then run CMake again.

- Build options

| Option | Values | Description |
| --- | --- | --- |
| `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution or NPU simulation |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: `dav-3510` corresponds to Ascend 950PR&950DT products |

- Execution result

  The following output indicates that the accuracy comparison is successful:

  ```bash
  test pass!
  ```
