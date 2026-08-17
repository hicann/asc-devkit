# Gather Class Example

## Overview

This example implements data selection functionality across multiple scenario modes based on GatherMask, Gather, Gatherb, and other interfaces, including built-in fixed mode, user-defined mode, tensor offset mode, and DataBlock offset mode, selecting elements from source operands to write into destination operands. The example supports switching between different scenarios through compilation parameters to help developers understand the usage and implementation differences of Gather class interfaces.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── gather
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── gather.asc              // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Scenario Description
This example switches between different mask generation scenarios through the compilation parameter `SCENARIO_NUM`:

**Scenario 1: Built-in Fixed Mode**
- Description: Select the corresponding binary as a mask through `src1Pattern` to obtain data
- Input: src0Local=[1, 256]
- Output: [1, 256]
- Data type: uint32_t
- Implementation:
    ```cpp
    AscendC::GatherMask(dstLocal, src0Local, src1Pattern, reduceMode, mask, gatherMaskParams, rsvdCnt);
    ```
- Parameters: dstLocal and src0Local use address reuse, with built-in fixed mode src1Pattern=2 for element selection, reduceMode=false (Normal mode), mask=0, gatherMaskParams={1, 4, 8, 0}

**Scenario 2: User-Defined Mode**
- Description: Use the binary corresponding to user-input `src1Local` as a mask to obtain data
- Input: src0Local=[1, 256], src1Local=[1, 32]
- Output: [1, 256]
- Data type: uint32_t
- Implementation:
    ```cpp
    AscendC::GatherMask (dstLocal, src0Local, src1Local, reduceMode, mask, gatherMaskParams, rsvdCnt);
    ```
- Parameters: Use user-provided Tensor for element selection, reduceMode=true (Counter mode), mask=70, gatherMaskParams={1, 2, 4, 0}

**Scenario 3: Tensor Offset Mode**
- Description: Perform address offset based on user-input address offset tensor `srcOffset` to obtain data
- Input: src0Local=[1, 128], srcOffset=[1, 128]
- Output: [1, 128]
- Data type: input/output uint16_t, srcOffset type uint32_t
- Implementation:
    ```cpp
    AscendC::Gather(dstLocal, src0Local, srcOffset, srcBaseOffset, count);
    ```
- Parameters: Use user-provided srcOffset for per-element address offset, srcBaseOffset=0 indicates the start address of the source operand, count=128 indicates the number of data elements to process

**Scenario 4: DataBlock Offset Mode**
- Description: Perform address offset based on user-input address offset tensor `srcOffset` (at DataBlock granularity) to obtain data
- Input: src0Local=[1, 128], srcOffset=[1, 8]
- Output: [1, 128]
- Data type: input/output uint16_t, srcOffset type uint32_t
- Implementation:
    ```cpp
    AscendC::Gatherb<T>(dstLocal, src0Local, srcOffset, repeatTime, params);
    ```
- Parameters: User-input srcOffset is the address offset of each datablock in the source operand, repeatTime=1 indicates the repeat iteration count, params={1,8}

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
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin $SCENARIO_NUM  # Verify whether the output result is correct
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
  | `SCENARIO_NUM` | `1` (default), `2`, `3`, `4` | Scenario number: 1 (Built-in fixed mode), 2 (User-defined mode), 3 (Tensor offset mode), 4 (DataBlock offset mode) |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.
  ```bash
  test pass!
  ```
