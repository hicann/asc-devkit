# Add Sample Based on TPipe and TQue

## Overview

This sample implements the Add vector addition operation based on the memory and synchronization management mechanisms of TPipe and TQue.

## Supported Products and CANN Software Versions for This Sample

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure Introduction

```
├── add_tpipe_tque
│   ├── scripts
│   │   ├── gen_data.py             // Input data and ground truth data generation script
│   │   └── verify_result.py        // Verification script for checking consistency between output data and ground truth data
│   ├── CMakeLists.txt              // Build project file
│   ├── data_utils.h                // Data read and write functions
│   ├── add_tpipe_tque.asc          // Ascend C sample implementation, TPipe and TQue manage memory and synchronization & sample call
│   └── README.md                   // Sample documentation
```

## Sample Description

- Sample Function:  
  Calculation formula:
  ```
  z = x + y
  ```
  This sample performs element-wise addition of two tensors with the same shape and writes the result to the output tensor. The sample inputs are two `float` type tensors `x` and `y`, both with shape `[8, 2048]`, and the output `z` has the same shape as the inputs. The sample launches the kernel with 8 cores for parallel computation, with each core responsible for processing a contiguous segment of data.

- Processing Flow:
  1. `add_custom` serves as the kernel entry point and receives `totalLength`.
  2. In `add_custom`, calculate the data length for the current block using `GetBlockNum()`, and calculate the data start point in GM for the current core using `GetBlockIdx()`.
  3. In the kernel function, use `DataCopy` to move input data from GM to UB, and place the input `LocalTensor` into the input queue through `EnQue`.
  4. Retrieve the input tensor from the input queue through `DeQue`, execute `Add` in UB, and then place the result `LocalTensor` into the output queue through `EnQue`.
  5. Retrieve the result from the output queue through `DeQue`, and use `DataCopy` to write back to the GM slice that the current core is responsible for.
- Queue Description:
  This sample uses `TPipe` and `TQue` to demonstrate the basic queue-style programming approach. `EnQue` is used to enqueue the `LocalTensor` that has been moved to UB, and `DeQue` is used to retrieve the tensor from the queue in subsequent stages for continued processing.
- Sample Specifications:
  <table>
  <tr><td rowspan="1" align="center">Sample Type(OpType)</td><td colspan="4" align="center">Add</td></tr>
  <tr><td rowspan="3" align="center">Sample Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Sample Output</td><td align="center">z</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">add_custom</td></tr>
  </table>

- Sample Implementation:
  - Kernel Implementation  
    The kernel function entry `add_custom` in `add_tpipe_tque.asc` receives `totalLength`, and inside the kernel function calculates the `blockLength` that each core needs to process based on `GetBlockNum()`, then calculates the data start point in GM for the current core using `GetBlockIdx()`. After that, the kernel function directly completes the entire flow of moving input data to UB and enqueuing, executing `Add` after dequeuing, and writing the result back to GM.

  - Kernel Entry Implementation
    `add_custom` is responsible for creating `TPipe`, `TQue`, and `GlobalTensor` objects, and executes the input move, compute, and output move processing chain in sequence.

  - Call Implementation
    Use the kernel call operator `<<<>>>` to invoke the kernel function. During the call, runtime parameters pass the Device-side x, y, z tensor addresses and the total data length `totalLength`.

## Compilation and Execution

Execute the following steps in the root directory of this sample to compile and run the sample.
- Configure Environment Variables  
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development toolkit package on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, it defaults to `/usr/local/Ascend`.
- Sample Execution

  Execute the following commands in this sample directory.
  ```bash
  mkdir -p build && cd build;                                               # Create and enter build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # Build project (default npu mode)
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                     # Execute the compiled executable program to run the sample
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify if output result is correct and confirm algorithm logic is correct
  ```

  When using CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Before switching compilation modes, you need to clean the cmake cache. You can execute `rm CMakeCache.txt` in the build directory and then run cmake again.

- Compilation Options Description

  | Option | Available Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution Result  
  The execution result is as follows, indicating that the precision comparison succeeded.
  ```bash
  test pass!
  ```