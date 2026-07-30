# histogram Example

## Overview
This example implements histogram statistics functionality based on the C API programming interface, using the [asc_frequency_histogram_bin0](../../../../../docs/zh/api/SIMD-API/C-API/reg/histogram_compute/asc_frequency_histogram.md)/asc_frequency_histogram_bin1 API.

## Supported Products and CANN Versions

| Product | CANN Version |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Directory Structure
```
├── histogram
│   ├── scripts
│   │   ├── gen_data.py                // Input data and ground truth generation script
│   ├── CMakeLists.txt                 // Build configuration file
│   ├── data_utils.h                   // Data read/write functions
│   ├── histogram.asc                  // Ascend C example implementation & invocation example
│   ├── README.md                      // Example description
│   └── README_en.md                   // Example description (English)
```

## Example Description
- Example functionality:
  Performs histogram statistics on uint8_t input data. The input vector shape is [1, 256] with data type uint8_t, and the output vector shape is [1, 256], representing the histogram statistics for 256 values, with data type uint16_t.

  **Scenario 1: Single-core Mode**
  - One core processes all 256 input data elements, and the statistics result is the frequency count for 256 bins
  - Example specifications:
    <table>
    <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
    <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">uint8_t</td></tr>
    <tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">y</td><td align="center">[1, 256]</td><td align="center">uint16_t</td></tr>
    <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="3" align="center">histogram_custom</td></tr>
    </table>
  - Example implementation:
    Inside the histogram_vf function:
    1. Uses asc_duplicate_scalar to initialize dst vector data register to 0
    2. Uses asc_loadalign to load input data
    3. Calls the asc_frequency_histogram_bin0/asc_frequency_histogram_bin1 API for statistics computation: dst0 counts the [0-127] range, dst1 counts the [128-255] range
    4. Accumulates statistics results from all input data blocks in a loop
    5. Uses asc_storealign to output the statistics results to UB
    - Invocation implementation
      Uses the kernel invocation syntax `<<<>>>` to call the kernel function.

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
  mkdir -p build && cd build;                                                    # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                           # Build the project (default npu mode)
  python3 ../scripts/gen_data.py                                                 # Generate test input data
  ./demo                                                                         # Run the compiled executable to execute the example
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
