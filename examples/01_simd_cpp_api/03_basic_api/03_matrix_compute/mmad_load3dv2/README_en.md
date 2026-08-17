# LoadData (convolution data transfer) v2 Data Transfer Example in Matrix Multiplication

## Overview

This example introduces the usage scenarios and methods of the LoadData (convolution data transfer) v2 instruction in matrix multiplication. LoadData (convolution data transfer) v2 can transfer 2D A and B matrices from L1 to L0A/L0B, where A and B represent the left and right input matrices of matrix multiplication respectively.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── mmad_load3dv2
│   ├── scripts
│   │   ├── gen_data.py                      // Script for generating input data and ground truth data
│   │   └── verify_result.py                 // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt                       // Build project file
│   ├── data_utils.h                         // Data read/write functions
│   ├── load3d.py                            // LoadData3D helper script
│   ├── mmad_load3dv2.asc                    // Ascend C operator implementation & invocation example
│   └── README.md                            // Example documentation
```
## Example Description

The LoadData (convolution data transfer) v2 instruction (referred to as load3dv2 in this example) supports the following data types:
- Ascend 950PR/Ascend 950DT: int8_t, uint8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, int32_t, uint32_t, float.

- Atlas A3 Training Series Products/Atlas A3 Inference Series Products and Atlas A2 Training Series Products/Atlas A2 Inference Series Products:
    - When the destination address is on A2: int4b_t, int8_t, uint8_t, half, bfloat16_t, int32_t, uint32_t, float.
    - When the destination address is on B2: half, bfloat16_t, int32_t, uint32_t, float.

Transpose capabilities of the load3dv2 instruction for 2D matrices:
- Ascend 950PR/Ascend 950DT: the enTranspose parameter takes effect only when the destination address is on A2 and the data type is B8/B16/B32.
- Atlas A3 Training Series Products/Atlas A3 Inference Series Products and Atlas A2 Training Series Products/Atlas A2 Inference Series Products: the enTranspose parameter takes effect only when the destination address is on A2 and the data type is B16/B32.

Since this example does not currently support int4b_t input data type, it demonstrates the following five load3dv2 usage scenarios in matrix multiplication:

### LoadData (convolution data transfer) v2 Interface Scenario Table

| scenarioNum | Input Data Type | A Matrix Transpose | B Matrix Transpose |
| --- | --- | --- | --- |
| 1 | half | No transpose | No transpose |
| 2 | half | Transpose | No transpose |
| 3 | float | No transpose | No transpose |
| 4 | float | Transpose | No transpose |
| 5 | int8_t | No transpose | Transpose |

Note: For Atlas A3 Training Series Products/Atlas A3 Inference Series Products and Atlas A2 Training Series Products/Atlas A2 Inference Series Products, when the input data type is B8 and the destination address is on L0B, the load3dv2 instruction is not supported. For compatibility, when scenarioNum=5, SplitB calls the load2d instruction.

  In this example, the A matrix in scenarioNum=3 and 4 is consistent with scenarioNum=12 and 13 in the [load_data_l12l0](../load_data_l12l0/README_en.md) example respectively, and the B matrix is consistent with scenarioNum=13 in that example. Therefore, the specific parameter configuration and diagrams of the load3dv2 instruction can be found in the "6. L1 to L0 (LoadData)" section of that example's readme.

  Since the input data type differences have minimal impact on load3dv2 instruction parameter configuration, the remaining scenarios in this example can reference scenarioNum=3 and 4.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.
- Configure environment variables  
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.
- Run the example

  Run the following commands in the example directory.
  ```bash
  SCENARIO_NUM=4
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;    # Build the project in NPU mode by default
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` option.

  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the CMake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then rerun CMake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim`, `cpu` | Run mode: NPU execution, NPU simulation, CPU debug |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2/A3 training and inference products; dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`, `2`, `3`, `4` (default), `5` | Scenario number |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
