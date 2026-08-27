# select Example

## Overview
This example implements the Select operation based on the C API programming interface, primarily calling the [asc_select](../../../../../docs/zh/api/SIMD-API/c_api/reg_compute/reg_permute_sel/asc_select.md) API.

## Supported Products and CANN Versions

| Product | CANN Version |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure
```
├── select
│   ├── scripts
│   │   ├── gen_data.py                // Input data and ground truth generation script
│   ├── CMakeLists.txt                 // Build configuration file
│   ├── data_utils.h                   // Data read/write functions
│   ├── select.asc                     // Ascend C example implementation & invocation example
│   ├── README.md                      // Example description
│   └── README_en.md                   // Example description (English)
```

## Example Description
- Example functionality:
  Selects values from the x vector data register or y vector data register vector at corresponding positions based on the bits in mask. When a mask bit is 1, the corresponding element from src0 is selected; when a mask bit is 0, the corresponding element from src1 is selected.
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="4" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td align="center">mask</td><td align="center">[1, 128]</td><td align="center">uint8_t</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">z</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="3" align="center">select_custom</td></tr>
  </table>
- Example implementation:
   The select_vf function calls the asc_select API for computation and writes the result back to UB
  - Invocation implementation
    Uses the kernel invocation syntax <<<>>> to call the kernel function.

## Build and Run
Run the following steps in the root directory of this example to build and run it.
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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                                     # Build the project (default npu mode)
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Run the compiled executable to execute the example
  ```

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and re-run cmake.

- Build option description

  | Option | Values | Description |
  | --- | --- | --- |
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
