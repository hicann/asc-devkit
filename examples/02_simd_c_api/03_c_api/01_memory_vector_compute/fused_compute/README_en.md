# fused_compute Example

## Overview

This example implements Fused Compute based on C API programming interfaces. It primarily calls [asc_leakyrelu](../../../../../docs/zh/api/SIMD-API/c_api/vector_compute/vector_fused/asc_leakyrelu.md) to compute Leaky ReLU.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.2.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.2.0 |

## Directory Structure

```text
├── fused_compute
│   ├── scripts
│   │   └── gen_data.py                // Input data and ground truth data generation script
│   ├── CMakeLists.txt                 // Build project file
│   ├── data_utils.h                   // Data read/write functions
│   ├── fused_compute.asc              // Ascend C example implementation & invocation example
│   ├── README.md                      // Example documentation
│   └── README_en.md                   // Example documentation (English)
```

## Example Description

This example performs fused computation on the input vector. The details are as follows:

**Scenario: Leaky ReLU**

- Example functionality: Computes `y = x` when `x > 0`, or `y = 0.01 * x` when `x <= 0`.
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 512]</td><td align="center">half</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 512]</td><td align="center">half</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="3" align="center">fused_compute_kernel</td></tr>
  </table>

- Example implementation:
  The `fused_compute_kernel` function calls `asc_leakyrelu` to perform Leaky ReLU computation on 512 half values. The process is as follows:
  - Load: Calls `asc_copy_gm2ub_align` to load 512 half values from GM to UB.
  - Compute: Calls `asc_leakyrelu` for Leaky ReLU computation.
  - Store: Calls `asc_copy_ub2gm_align` to store the result from UB to GM.
  - Invocation implementation: Uses the kernel invocation syntax `<<<>>>` to call the kernel function.

## Build and Run

Run the following steps in the root directory of this example to build and run it.

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. The default is `/usr/local/Ascend` for the root user and `${HOME}/Ascend` for a non-root user.

- Run the example

  Run the following commands in the example directory.

  ```bash
  mkdir -p build && cd build;                                     # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;            # Build the project (default npu mode)
  python3 ../scripts/gen_data.py                                  # Generate test input and golden data
  ./demo                                                          # Run the compiled executable to execute the example
  ```

- Build option description

  | Option | Values | Description |
  | --- | --- | --- |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default) | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products |

- Execution result

  The following execution result indicates that the precision comparison is successful.

  ```bash
  test pass!
  ```
