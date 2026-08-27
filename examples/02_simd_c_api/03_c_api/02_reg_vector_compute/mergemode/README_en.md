# mergemode Example

## Overview
This example verifies the behavior characteristics of the MaskMergeMode::MERGING mode based on the C API programming interface, demonstrating the mechanism where inactive mask bits preserve the original value of dst vector data register. The [asc_copy](../../../../../docs/zh/api/SIMD-API/c_api/reg_compute/reg_copy/asc_copy.md) API is used for verification.

## Supported Products and CANN Versions

| Product | CANN Version |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure
```
├── mergemode
│   ├── scripts
│   │   ├── gen_data.py                // Input data and ground truth generation script
│   ├── CMakeLists.txt                 // Build configuration file
│   ├── data_utils.h                   // Data read/write functions
│   ├── mergemode.asc                  // Ascend C example implementation & invocation example
│   ├── README.md                      // Example description
│   └── README_en.md                   // Example description (English)
```

## Example Description
- Example functionality:
  Demonstrates using the asc_copy API to perform data movement between vector data registers in MERGING mode. Positions where mask is 1 are filled with src data, and positions where mask is 0 retain the original dst value. This example uses the PAT_H mask (upper half active) to move the upper half of input x into dst_reg, while the lower half retains the original dst_reg value (i.e., input y).

  - Example specifications:
    <table>
    <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
    <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">x</td><td align="center">[1, 128]</td><td align="center">float</td></tr>
    <tr><td align="center">y</td><td align="center">[1, 128]</td><td align="center">float</td></tr>
    <tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
    <tr><td align="center">z</td><td align="center">[1, 128]</td><td align="center">float</td></tr>
    <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="3" align="center">mergemode_custom</td></tr>
    </table>

  - Example implementation:
    - The copy_merge_vf function uses PAT_H mask to create vmask, activating the upper half elements
    - Calls asc_loadalign to load input x (src) and input y (dst) into vector data registers
    - Calls asc_copy to move src_reg into dst_reg; in MERGING mode, active bits are overwritten by src, inactive bits retain the original dst value
    - Calls asc_storealign to write the result back to UB
    - Invocation implementation  
      Uses the kernel invocation syntax `<<<>>>` to call the kernel function.

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
