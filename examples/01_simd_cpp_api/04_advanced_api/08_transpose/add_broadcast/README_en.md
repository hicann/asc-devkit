# AddBroadcast Operator Direct Invocation Example

## Overview

This example describes the direct invocation method for the Add operator kernel function, with Broadcast added to the input in multi-core and tiling scenarios.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── add_broadcast
│   ├── scripts
│   │   ├── gen_data.py          // Script for generating input data and ground truth data
│   │   └── verify_result.py     // Script for verifying whether the output data matches the ground truth data
│   ├── CMakeLists.txt           // Build project file
│   ├── data_utils.h             // Data read/write functions
│   ├── add_broadcast.asc        // Ascend C operator implementation & invocation example
│   └── README.md                // Example documentation
```

## Operator Description

- Operator function:  
  The Add operator adds two data inputs and returns the sum. The corresponding mathematical expression is:
  ```
  z = x + y
  ```
- Operator specifications:
  <table>
  <tr><td rowspan="1" align="center">Operator Type (OpType)</td><td colspan="4" align="center">Add</td></tr>
  <tr><td rowspan="3" align="center">Operator Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">axis = 0: (8, 1024) / (8, 1022) / (17, 1024) / (17, 1022) axis = 1: (16, 1) / (16, 1) / (20, 1) / (20, 1)
  </td><td align="center">bfloat16_t/int8_t/float/half/int16_t/int32_t</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">axis = 0: (1, 1024) / (1, 1022) / (1, 1024) / (1, 1022) axis = 1: (16, 256) / (16, 255) / (20, 256) / (20, 255)</td><td align="center">bfloat16_t/int8_t/float/half/int16_t/int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Operator Output</td><td align="center">z</td><td align="center">axis = 0: (8, 1024) / (8, 1022) / (17, 1024) / (17, 1022) axis = 1: (16, 256) / (16, 255) / (20, 256) / (20, 255)</td><td align="center">bfloat16_t/int8_t/float/half/int16_t/int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">add_broadcast_custom</td></tr>
  </table>

- Operator implementation:  
  
  The computation logic is: The vector computation APIs provided by Ascend C operate on LocalTensor elements. Input data must first be transferred to on-chip storage, then the computation APIs are used to add the two input parameters, obtain the final result, and transfer it to external storage.

    The implementation process of the Add operator consists of three basic tasks: CopyIn, Compute, and CopyOut.
    The CopyIn task transfers the input tensors xGm and yGm from Global Memory to Local Memory, stored in xLocal and yLocal respectively.
    The Compute task performs Broadcast on xLocal and yLocal, stores the result in temporary memory, then executes the addition operation, with the result stored in zLocal.
    The CopyOut task transfers the output data from zLocal to the output tensor zGm in Global Memory.

  - Tiling implementation  
    TilingData parameter design: TilingData parameters are essentially parameters related to parallel data splitting.

    This example attempts to evenly distribute the input data across a given number of cores. If even distribution is not possible, it calculates the number of regular cores and tail cores, which process different amounts of data. UB_BLOCK_NUM is defined as the maximum available UB data blocks, corresponding to a UB size of UB_BLOCK_NUM * 32 Bytes. Tiling splitting is performed with UB_BLOCK_NUM as the granularity.

    This example operator uses 20 tiling parameters: xLen, yLen, and dataType are the total length and data type of x and y input data; coef is the expansion factor for the input that requires Broadcast; axis is the Broadcast axis of the input data; blockLength, tileNum, tileLength, and lastTileLength are the computation amount per core, number of tiling splits, and regular/tail block lengths in the even core distribution scenario; former(tail)Num, former(tail)Length, former(tail)TileNum, former(tail)TileLength, and former(tail)LastTileLength are the splitting data related to regular and tail cores in the uneven core distribution scenario, corresponding to the even core distribution scenario; isEvenCore is used to determine whether the current scenario is an even core distribution.

  - Invocation implementation  
    Use the kernel invocation operator <<<>>> to call the kernel function.

## Build and Run

Run the following steps in the root directory of this example to build and run the operator.

- Configure environment variables
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # NPU mode by default
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm the algorithm logic is correct
  ```

  To use CPU debugging or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  For example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debugging mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Before switching the build mode, clear the cmake cache by running `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
