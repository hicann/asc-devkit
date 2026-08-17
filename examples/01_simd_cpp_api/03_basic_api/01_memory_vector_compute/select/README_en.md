# Select Class Example

## Overview

This example implements data selection functionality across multiple scenarios based on the Select interface, selecting elements from two vectors or between a vector and a scalar based on the `selMask` mask to write into the destination vector. The selection rule is: when a selMask bit is 1, select from src0; when the bit is 0, select from src1.

The example supports switching between different scenarios through compilation parameters to help developers understand the usage and implementation differences of the Select interface.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── select
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── select.asc              // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Scenario description

  **Scenario 1: Select**

  - Description: Select elements from two tensors based on selMask. In each iteration, selection is performed based on the valid bits of selMask (limited to 256/sizeof(T) valid bits, where T is the input data type, float in this example).
  - Implementation:
    ```cpp
    // cmpMode = AscendC::SELMODE::VSEL_CMPMASK_SPR
    // Both src0 and src1 are tensors. When selMask bit is 1, select from src0; when bit is 0, select from src1
    AscendC::Select(dstl, selMask, src0, src1, cmpMode, count);
    ```
  
  **Scenario 2: Select (Scalar)**

  - Description: Select elements from 1 tensor and 1 scalar based on selMask, with no valid data limit on selMask. In multi-iteration scenarios, each iteration continuously uses different portions of selMask.
  - Implementation:
    ```cpp
    // cmpMode = AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE
    // src0 is tensor, src1 is scalar. When selMask bit is 1, select from src0; when bit is 0, equals src1
    AscendC::Select(dst, selMask, src0, src1, cmpMode, count);
    ```

  **Scenario 3: Select (selMask without valid bit limit)**

  - Description: Select elements from two tensors based on selMask, with no valid data limit on selMask. In multi-iteration scenarios, each iteration continuously uses different portions of selMask.
  - Implementation:
    ```cpp
    // cmpMode = AscendC::SELMODE::VSEL_TENSOR_TENSOR_MODE
    // Both src0 and src1 are tensors. When selMask bit is 1, select from src0; when bit is 0, select from src1
    AscendC::Select(dst, selMask, src0, src1, cmpMode, count);
    ```

**Scenario 4: Select (Flexible Scalar Position) ---- This scenario is only supported on Ascend 950PR/Ascend 950DT products**

  - Description: Similar to Scenario 2, but with more flexible scalar position.
  - Implementation:
    ```cpp
    // cmpMode = AscendC::SELMODE::VSEL_TENSOR_SCALAR_MODE
    // Scalar at back. src0 is tensor, src1 is scalar. When selMask bit is 1, select from src0; when bit is 0, equals src1
    static constexpr AscendC::BinaryConfig config = { 1 };
    AscendC::Select<AscendC::BinaryDefaultType, uint8_t, config>(dst, selMask, src0, src1, cmpMode, count);

    // Scalar at front. src0 is scalar, src1 is tensor. When selMask bit is 1, equals src0; when bit is 0, select from src1
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Select<AscendC::BinaryDefaultType, uint8_t, config>(dst, selMask, src0, src1, cmpMode, count); 
    ```

- Scenario specifications

  The example can switch between different scenarios through the compilation parameter `SCENARIO_NUM`. See the table below for parameter details:

  | Scenario Number | Scenario Name | src0 shape | src0 data type | src1 shape | src1 data type | selMask shape | selMask data type | dst shape | dst data type |
  |------|------|------|------|------|------|------|------|------|------|
  | 1 | Select | [1, 256] | float | [1, 256] | float | [1, 8] valid bit limit | uint8_t |[1, 256] | float |
  | 2 | Select (Scalar) | [1, 256] | float | scalar | float | [1, 32] | uint8_t |[1, 256] | float |
  | 3 | Select (no valid bit limit) | [1, 256] | float | [1, 256] | float | [1, 32] | uint8_t |[1, 256] | float |
  | 4 | Select (flexible scalar) | [1, 256] | float | scalar | float | [1, 32] | uint8_t |[1, 256] | float |

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
  SCENARIO_NUM=1  # Set the scenario number
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # Build the project
  python3 ../scripts/gen_data.py -scenario_num=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin  # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  Examples:
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # CPU debug mode
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU simulation mode
  ```
  
  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1` (default), `2`, `3`, `4` | Scenario number: 1 for Select, 2 for Select (Scalar), 3 for Select (no valid bit limit), 4 for Select (flexible scalar) |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.
  ```bash
  test pass!
  ```
