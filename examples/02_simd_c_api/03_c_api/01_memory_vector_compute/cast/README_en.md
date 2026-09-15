# cast Example

## Overview

This example implements Cast based on C API programming interfaces. It primarily calls [asc_half2int4_ru](../../../../../docs/zh/api/SIMD-API/c_api/vector_compute/vector_convert/asc_half2int4.md) and [asc_half2int32_ru](../../../../../docs/zh/api/SIMD-API/c_api/vector_compute/vector_convert/asc_half2int32.md) for data type conversion.

This example supports two conversion scenarios, selected with the CMake build parameter `SCENARIO_NUM`.

  | SCENARIO_NUM | Data Type Conversion |
  | --- | --- |
  | 1 | Narrowing conversion, using half to int4b_t as an example |
  | 2 | Widening conversion, using half to int32_t as an example |

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.2.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.2.0 |

## Directory Structure

```text
├── cast
│   ├── scripts
│   │   └── gen_data.py                // Input data and ground truth data generation script
│   ├── CMakeLists.txt                 // Build project file
│   ├── data_utils.h                   // Data read/write functions
│   ├── cast.asc                       // Ascend C example implementation & invocation example
│   ├── README.md                      // Example documentation
│   └── README_en.md                   // Example documentation (English)
```

## Example Description

This example performs data type conversion on the input vector. The details are as follows:

**Scenario 1: Wide to Narrow Data Type Conversion**

- Example functionality: Converts half data to int4b_t data.
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 512]</td><td align="center">half</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 512]</td><td align="center">int4b_t</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="3" align="center">cast_kernel</td></tr>
  </table>

- Example implementation:
  The `cast_kernel` function calls `asc_half2int4_ru` to convert 512 half values to int4b_t values. The process is as follows:
  - Load: Calls `asc_copy_gm2ub_align` to load 512 half values from GM to UB.
  - Compute: Calls `asc_half2int4_ru` for data type conversion.
  - Store: Calls `asc_copy_ub2gm_align` to store the result from UB to GM.
  - Invocation implementation: Uses the kernel invocation syntax `<<<>>>` to call the kernel function.

**Scenario 2: Narrow to Wide Data Type Conversion**

- Example functionality: Converts half data to int32_t data.
- Example specifications:
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="3" align="center">AIV Example</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[1, 512]</td><td align="center">half</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td></tr>
  <tr><td align="center">y</td><td align="center">[1, 512]</td><td align="center">int32_t</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="3" align="center">cast_kernel</td></tr>
  </table>

- Example implementation:
  The `cast_kernel` function calls `asc_half2int32_ru` to convert 512 half values to int32_t values. The process is as follows:
  - Load: Calls `asc_copy_gm2ub_align` to load 512 half values from GM to UB.
  - Compute: Calls `asc_half2int32_ru` for data type conversion.
  - Store: Calls `asc_copy_ub2gm_align` to store the result from UB to GM.
  - Invocation implementation: Uses the kernel invocation syntax `<<<>>>` to call the kernel function.

## Build and Run

Run the following steps in the root directory of this example to build and run it.

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit on the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. The default is `/usr/local/Ascend` for the root user and `${HOME}/Ascend` for non-root users.

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
  | `SCENARIO_NUM` | `1` (default), `2` | Example execution scenario: Scenario 1: wide to narrow data type, Scenario 2: narrow to wide data type |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default) | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products |

- Execution result

  The following execution result indicates that the precision comparison is successful.

  ```bash
  test pass!
  ```
