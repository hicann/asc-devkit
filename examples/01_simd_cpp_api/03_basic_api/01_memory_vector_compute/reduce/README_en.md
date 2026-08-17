# Reduce Example

## Overview

This example describes how to call the `ReduceRepeat<SUM>` and `ReduceDataBlock<SUM>` basic APIs to implement the reduce operator, which accumulates data elements in contiguous memory and returns the accumulation result.

Note: `ReduceRepeat` and `ReduceDataBlock` are APIs renamed in CANN 9.1.0. For CANN 9.0.0 and earlier versions, use `WholeReduceSum` and `BlockReduceSum` instead.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

``` text
├── reduce
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── reduce.asc              // Ascend C operator implementation & invocation example
│   └── README.md               // Example documentation
```

## Operator Description

- Operator function:  
  The ReduceCustom operator accumulates data elements in contiguous memory and returns the accumulation result. The corresponding mathematical expression is:

  ```cpp
  z = sum(x)
  ```

- Operator specifications:  
  <table>
  <tr><td rowspan="1" align="center">Operator Type (OpType)</td><td colspan="4" align="center"> reduce </td></tr>

  <tr><td rowspan="3" align="center">Operator Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">4096</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Operator Output</td></tr>
  <tr><td align="center">y</td><td align="center">32</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">reduce_custom</td></tr>
  </table>

- Operator implementation:  

  When the input data type is `float`, the valid range for the input data length is `(0, 4096]`. When the input data type is `half`, the valid range is `(0, 16384]`. This example uses `float` type input as an example, with a fixed input `x` shape of `[4096]` and output `y` shape of `[32]`.

    The example performs accumulation on a contiguous input segment to obtain the sum of elements within that contiguous buffer.

      - 1. When the size is less than 256B, ComputeKey = 1, and a single ReduceRepeat<SUM> instruction produces the result.

      - 2. When the length is in float input (256B, 2KB] or half input (256B, 4KB], ComputeKey = 2. Since ReduceDataBlock<SUM> executes faster than ReduceRepeat<SUM> for the same input length, one ReduceDataBlock<SUM> followed by one ReduceRepeat<SUM> is used for higher computation efficiency.

      - 3. When the length is in float input (2KB, 16KB] or half input (4KB, 32KB], ComputeKey = 3. Since the accumulation efficiency of one ReduceRepeat<SUM> is higher than that of two ReduceDataBlock<SUM> instructions, two ReduceRepeat<SUM> instructions (rather than two ReduceDataBlock<SUM> + one ReduceRepeat<SUM>) are used to obtain the accumulation sum of this buffer.

      - 4. When the float input length is 10000, ComputeKey = 4, corresponding to the processing method in ReduceRepeatSumImpl. In Counter mode, ReduceRepeat<SUM> instructions are used in a loop to process each row of the 2D data, obtaining the reduction result for each row.

      - 5. When the float input length is 20000, ComputeKey = 5, corresponding to the processing method in BinaryReduceSumImpl. In Counter mode, the operation data is first split in half, the Add instruction is used to add the two parts together, and this process repeats. Finally, one ReduceRepeat<SUM> instruction obtains the reduction result. This approach delivers better performance compared to single ReduceRepeat<SUM> instruction operations in scenarios with large data volumes and many loop iterations.  
      Note that the code uses Counter mode.

  - Kernel implementation  
    The computation logic is: The vector computation APIs provided by Ascend C operate on LocalTensor elements. Input data must first be transferred to on-chip storage, then the Reduce basic API is used to complete the reduce computation, obtain the final result, and transfer it to external storage.

    The implementation process of the ReduceCustom operator consists of three basic tasks: CopyIn, Compute, and CopyOut. The CopyIn task stores the input tensor xGm from Global Memory into xLocal. The Compute task performs reduce computation on xLocal, with the computation method determined by the ComputeKey parameter, which is determined by the input length. The computation result is stored in zLocal. The CopyOut task transfers the output data from zLocal to the output tensor zGm in Global Memory.

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
  ```

  To use CPU debugging or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  For example:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debugging mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Before switching the build mode, clear the cmake cache by running `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description
- 
  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

  The following execution result indicates that the precision comparison is successful.
  
  ```bash
  test pass!
  ```
