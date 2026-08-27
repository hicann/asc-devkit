# reduce_pair Example

## Overview
This example implements the PairReduceElem operation based on the C API programming interface, primarily calling the [asc_pair_reduce_sum](../../../../../docs/zh/api/SIMD-API/c_api/reg_compute/reg_reduce/asc_pair_reduce_sum.md) API (SUM mode).
- The asc_pair_reduce_sum API performs reduction sum on adjacent odd-even element pairs. Currently, only the SUM reduction mode is supported.


## Supported Products and CANN Versions

| Product | CANN Version |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure
```
├── reduce_pair
│   ├── scripts
│   │   ├── gen_data.py                // Input data and ground truth generation script
│   ├── CMakeLists.txt                 // Build configuration file
│   ├── data_utils.h                   // Data read/write functions
│   ├── reduce_pair.asc                // Ascend C example implementation & invocation example
│   ├── README.md                      // Example description
│   └── README_en.md                   // Example description (English)
```

## Example Description
- Example functionality:
  Performs PairReduceElem reduction sum on the input vector. The vector shape is [1, 256] and the data type is float.
  PairReduceElem sums adjacent odd-even element pairs: dst[i] = src[2i] + src[2i+1]
  256 float inputs produce 128 sum results.

- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">float</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 128]</td><td align="center">float</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="3" align="center">reduce_pair_custom</td></tr>
  </table>
- Example implementation:
   The reduce_pair_vf function calls the asc_pair_reduce_sum API for reduction computation:
   - Uses asc_loadalign to load data into registers
   - Uses asc_pair_reduce_sum to sum adjacent odd-even element pairs
   - Uses asc_storealign to write the result back to UB
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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # Build the project (default npu mode)
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
