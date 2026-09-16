# compare Example

## Overview

This example implements comparison based on C API programming interfaces. It primarily calls [asc_lt](../../../../../docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_lt.md), [asc_gt_scalar](../../../../../docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_gt_scalar.md), and [asc_get_cmp_mask](../../../../../docs/zh/api/SIMD-API/c_api/vector_compute/vector_compare/asc_get_cmp_mask.md) for data comparison.

This example supports three comparison scenarios, selected with the CMake build parameter `SCENARIO_NUM`.

  | SCENARIO_NUM | Comparison Scenario |
  | --- | --- |
  | 1 | Element-wise comparison between two vectors |
  | 2 | Element-wise comparison between two vectors (with the result read through cmp_mask) |
  | 3 | Element-wise comparison between a vector and a scalar |

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.2.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.2.0 |

## Directory Structure

```text
├── compare
│   ├── scripts
│   │   └── gen_data.py                // Input data and ground truth data generation script
│   ├── CMakeLists.txt                 // Build project file
│   ├── data_utils.h                   // Data read/write functions
│   ├── compare.asc                    // Ascend C example implementation & invocation example
│   ├── README.md                      // Example documentation
│   └── README_en.md                   // Example documentation (English)
```

## Example Description

This example performs comparison on the input vectors. The details are as follows:

**Scenario 1: Vector Comparison**

- Example functionality: Performs an element-wise comparison of `x[i] < y[i]`.
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 64]</td><td align="center">float</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 64]</td><td align="center">float</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">output</td><td align="center">[1, 8]</td><td align="center">uint8_t</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="3" align="center">compare_kernel</td></tr>
  </table>

- Example implementation:
  The `compare_kernel` function calls `asc_lt` to compare two float vectors element by element. The process is as follows:
  - Load: Calls `asc_copy_gm2ub_align` to load `x` and `y` from GM to UB.
  - Compute: Calls the output-parameter overload of `asc_lt` to compare the vectors.
  - Store: Calls `asc_copy_ub2gm_align` to store the result from UB to GM.
  - Invocation implementation: Uses the kernel invocation syntax `<<<>>>` to call the kernel function.

**Scenario 2: Vector Comparison (cmp_mask Read)**

- Example functionality: Performs an element-wise comparison of `x[i] < y[i]` and obtains the comparison result through cmp_mask.
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 64]</td><td align="center">float</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 64]</td><td align="center">float</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">output</td><td align="center">[1, 8]</td><td align="center">uint8_t</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="3" align="center">compare_kernel</td></tr>
  </table>

- Example implementation:
  The `compare_kernel` function calls `asc_lt` and `asc_get_cmp_mask` to compare two float vectors element by element. The process is as follows:
  - Load: Calls `asc_copy_gm2ub_align` to load `x` and `y` from GM to UB.
  - Compute: Calls the output-less `asc_lt` to generate cmp_mask, then calls `asc_get_cmp_mask` to read cmp_mask.
  - Store: Calls `asc_copy_ub2gm_align` to store the result from UB to GM.
  - Invocation implementation: Uses the kernel invocation syntax `<<<>>>` to call the kernel function.

**Scenario 3: Vector-Scalar Comparison**

- Example functionality: Performs an element-wise comparison of `x[i] > y[0]`, with `y[0]` used as a scalar.
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 64]</td><td align="center">float</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 64]</td><td align="center">float</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">output</td><td align="center">[1, 8]</td><td align="center">uint8_t</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="3" align="center">compare_kernel</td></tr>
  </table>

- Example implementation:
  The `compare_kernel` function calls `asc_gt_scalar` to compare a float vector with a scalar element by element. The process is as follows:
  - Load: Calls `asc_copy_gm2ub_align` to load `x` and `y` from GM to UB.
  - Compute: Calls `asc_gt_scalar` to compare `x[i]` with scalar `y[0]`.
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
  SCENARIO_NUM=1                                                                    # Execute scenario 1
  mkdir -p build && cd build;                                                       # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # Build the project (default npu mode)
  python3 ../scripts/gen_data.py -scenario_num=$SCENARIO_NUM                        # Generate test input and golden data
  ./demo                                                                            # Run the compiled executable to execute the example
  ```

- Build option description

  | Option | Values | Description |
  | --- | --- | --- |
  | `SCENARIO_NUM` | `1` (default), `2`, `3` | Example execution scenario: Scenario 1: vector comparison, Scenario 2: vector comparison (cmp_mask read), Scenario 3: vector-scalar comparison |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default) | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products |

- Execution result

  The following execution result indicates that the precision comparison is successful.

  ```bash
  test pass!
  ```
