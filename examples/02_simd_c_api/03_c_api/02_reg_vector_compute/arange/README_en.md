# arange Example

## Overview
This example implements the Arange operation based on the C API programming interface, primarily calling the [asc_arange](../../../../../docs/zh/api/SIMD-API/C-API/reg/index_operate/asc_arange.md) API.
- The asc_arange API generates an increasing/decreasing index sequence starting from the provided scalar value
- This example uses the increasing mode with a starting value of 0

## Supported Products and CANN Versions

| Product | CANN Version |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure
```
├── arange
│   ├── scripts
│   │   │   ├── gen_data.py            // Ground truth data generation script
│   ├── CMakeLists.txt                 // Build configuration file
│   ├── data_utils.h                   // Data read/write functions
│   ├── arange.asc                     // Ascend C example implementation & invocation example
│   ├── README.md                      // Example description
│   └── README_en.md                   // Example description (English)
```

## Example Description
- Example functionality:
  Generates an increasing index sequence starting from 0. The vector shape is [1, 256] and the data type is float.
  Arange generates increasing indices starting from 0: {0, 1, 2, 3, ..., 255}.
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="3" align="center">arange_custom</td></tr>
  </table>
- Example implementation:
  The arange_vf function calls the asc_arange API to generate indices:
  - Uses the asc_arange API to generate increasing indices, producing one_rep_size indices per repeat
  - Updates the starting value after each repeat
  - Invocation implementation
    Uses the kernel invocation syntax <<<>>> to call the kernel function.

## Notes
- The asc_arange API does not require input data; only the starting scalar value needs to be specified
- Each repeat generates one_rep_size increasing indices. When multiple repeats are needed, manually update the starting value

## Build and Run
Run the following steps in the root directory of this example to build and run it.
- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;                                               # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # Build the project (default npu mode)
  python3 ../scripts/gen_data.py                                            # Generate test ground truth data
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
