# Matmul Performance Optimization with hifloat8_t

## Overview

This sample implements `C = A * B` with the Matmul high-level API. It provides three cases that demonstrate half inputs, hifloat8_t inputs, and Fixpipe on-the-fly quantization. All cases use the same matrix shapes and multi-core tiling flow.

## Supported Products and CANN Versions

| Product | CANN Version |
| --- | --- |
| Ascend 950PR&950DT products | >= CANN 9.2.0 |

## Directory Structure

```text
├── matmul_hif8_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // Generates inputs and golden data
│   │   ├── hif8.py             // Converts hifloat8_t data types
│   │   └── verify_result.py    // Verifies the output
│   ├── CMakeLists.txt          // Build configuration
│   ├── data_utils.h            // Binary file utilities
│   ├── matmul_hif8.asc         // Kernel and host implementation
│   ├── README.md               // Chinese documentation
│   └── README_en.md            // English documentation
```

## Cases

The matrix dimensions are `M = N = K = 1024`. A, B, and C use the ND format without transposition.

| `SCENARIO_NUM` | A/B Input Type | C Output Type | Description |
| --- | --- | --- | --- |
| 0 | half | float | Non-quantized baseline |
| 1 | hifloat8_t | float | HIF8 inputs and float output |
| 2 | hifloat8_t | hifloat8_t | HIF8 inputs and Fixpipe on-the-fly quantization |

Case 2 configures `DequantType::SCALAR` during host tiling and calls `SetQuantScalar` with a scale of 1.0 in the kernel. Quantization is performed while the L0C result is moved out.

## Build and Run

Run the following steps from the sample root directory.

- Configure the environment.

  Configure the environment variables according to the [CANN installation guide](../../../../../docs/en/quick_start.md#prepare&install).

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Build and run the sample.

  Pass the same `SCENARIO_NUM` to the build, data generation, and verification commands.

  ```bash
  SCENARIO_NUM=2                                                                    # Select a scenario
  mkdir -p build && cd build;                                                       # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # Build the project in npu mode by default
  python3 ../scripts/gen_data.py --scenario $SCENARIO_NUM                           # Generate test input and reference data
  ./demo                                                                            # Run the compiled executable to execute the sample
  python3 ../scripts/verify_result.py --scenario $SCENARIO_NUM                      # Verify the output and confirm the algorithm logic
  ```

  Add `-DCMAKE_ASC_RUN_MODE=sim` to use NPU simulation mode.

  Example:

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes or Scenarios. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build options

  | Option | Values | Description |
  | --- | --- | --- |
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | NPU execution or NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture |
  | `SCENARIO_NUM` | `0`, `1`, `2` | Data type and quantization case |

- Execution results

  The following execution result indicates that the accuracy comparison succeeded.

  ```bash
  test pass!
  ```

## Accuracy

The data generation script generates the inputs and reference data for each case. Case 0 converts the half inputs to float before computing the Matmul reference. Case 1 converts the hifloat8_t inputs to float before computing the Matmul reference. Case 2 further converts the float reference to hifloat8_t using Half to Away Round.

For Cases 0 and 1, the verification script uses relative and absolute tolerances of `1e-3`. For Case 2, it compares the device output with the hifloat8_t reference element by element.
