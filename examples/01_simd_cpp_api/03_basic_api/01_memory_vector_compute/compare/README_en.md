# Compare Class Example

## Overview

This example implements multi-scenario data comparison functionality based on Compare and Compares interfaces, performing element-wise size comparison. If the comparison result is true, the corresponding bit in the output is 1; otherwise it is 0. Comparison results are stored in 8-bit compressed format, with every 8 comparison results packed into one byte (uint8_t/int8_t).

The example supports switching between different scenarios through compilation parameters to help developers understand the usage and implementation differences of Compare class interfaces.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── compare
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── compare.asc             // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Scenario Description
This example switches between different scenarios through the compilation parameter `SCENARIO_NUM`:

**Scenario 1: Compare**
- Description: Element-wise comparison of the values in `src0Local` and `src1Local`
- Input: src0Local=[1, 256], src1Local=[1, 256]
- Input data type: float
- Output: dstLocal=[1, 32]
- Output data type: uint8_t
- Implementation:
  ```cpp
  AscendC::Compare(dstLocal, src0Local, src1Local, cmpMode, srcDataSize);
  ```
- Parameters: cmpMode=AscendC::CMPMODE::LT, srcDataSize=256

**Scenario 2: Compare (Result Stored in Register)**
- Description: Element-wise comparison of the values in `src0Local` and `src1Local`, storing the results in the cmpMask register
- Input: src0Local=[1, 64], src1Local=[1, 64] 
- Input data type: float
- Output: [1, 32] 
- Output data type: uint8_t
- Implementation:
  ```cpp
    AscendC::Compare(src0Local, src1Local, cmpMode, mask, repeatParams);  // Compare interface has no repeat input, repeat defaults to 1, supporting 256 bytes of data per instruction
    AscendC::PipeBarrier<PIPE_V>();
    AscendC::GetCmpMask(dstLocal);  // Obtain saved data from register via GetCmpMask, dstLocal requires at least 128 bytes, but actual result data only occupies 8 bytes
  ```
- Parameters: cmpMode=AscendC::CMPMODE::LT; repeatParams is default value, controlling operand address stride

**Scenario 3: Compares**
- Description: Element-wise comparison of elements in `src0Local` (tensor) with `src1Scalar` (scalar)
- Input: src0Local=[1, 256], src1Local=[1, 16]  where `src1Scalar` is obtained from GetValue(idx) method to extract one element as scalar for comparison
- Input data type: float
- Output: dstLocal=[1, 32]
- Output data type: uint8_t
- Data type: float
- Implementation:
    ```cpp
    AscendC::Compares(dstLocal, src0Local, src1Scalar, cmpMode, srcDataSize);
    ```
- Parameters: src1Scalar=src1Local.GetValue(0), cmpMode=AscendC::CMPMODE::LT, srcDataSize=256

**Scenario 4: Compares (Flexible Scalar Position) ---- This scenario is only supported on Ascend 950PR/Ascend 950DT products**
- Description: Element-wise comparison of elements in `src0Local` (tensor) with `src1Scalar` (scalar), where the scalar supports both front and back positions
- Input: src0Local=[1, 256], src1Local=[1, 16]  where `src1Scalar` is obtained from src1Local[idx] method to extract one element as scalar for comparison
- Input data type: float
- Output: dstLocal=[1, 32]
- Output data type: uint8_t
- Implementation:
    ```cpp
    AscendC::Compares(dstLocal, src0Local, src1Scalar, cmpMode, srcDataSize);  // Scalar at back
    AscendC::Compares(dstLocal, src1Scalar, src0Local, cmpMode, srcDataSize);  // Scalar at front
    ```
- Parameters: src1Scalar=src1Local[0], cmpMode=AscendC::CMPMODE::LT, srcDataSize=256

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
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin -scenario_num=$SCENARIO_NUM  # Verify whether the output result is correct
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
  | `SCENARIO_NUM` | `1` (default), `2`, `3`, `4` | Scenario number: 1: Compare, 2: Compare (result in register), 3: Compares, 4: Compares (flexible scalar) |

- Execution result  
  The following execution result indicates that the accuracy comparison is successful.
  ```bash
  test pass!
  ```
