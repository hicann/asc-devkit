# TopK Example

## Overview

This example uses the TopK high-level API in a sorting scenario to find the top K maximum or minimum values from the input tensor by numerical value while retaining the corresponding index information. It supports joint sorting of float type values and int32_t type indices.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── topk
│   ├── scripts
│   │   └── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── topk.asc                // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  This example performs TopK computation on the input tensor in Normal mode to obtain the top k maximum or minimum values and their corresponding indices along the last dimension.
- Example specifications:  
  <table>
  <caption>Table 1: Example Input/Output Specifications</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> topk </td></tr>

  <tr><td rowspan="5" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">srcLocalValue</td><td align="center">[2, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">srcLocalIndex</td><td align="center">[1, 32]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td align="center">srcLocalFinish</td><td align="center">[1, 32]</td><td align="center">int32_t</td><td align="center">ND</td></tr>


  <tr><td rowspan="3" align="center">Example Output</td></tr>
  <tr><td align="center">dstLocalValue</td><td align="center">[2, 8]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">dstLocalIndex</td><td align="center">[2, 8]</td><td align="center">int32_t</td><td align="center">ND</td></tr>


  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">topk_custom</td></tr>
  </table>

- Example implementation:  
    This example implements a topk example with fixed shapes of input value tensor srcLocalValue[2, 32], index tensor srcLocalIndex[1, 32], finish flag srcLocalFinish[1, 32], and output result values dstLocalValue[2, 8], result indices dstLocalIndex[2, 8].

  - Kernel implementation:  
    The computation logic is: Input data must first be transferred to on-chip storage, the TopK high-level API is used to complete the topk computation, and the result is transferred out.

  - Tiling implementation:  
    The tiling implementation process is as follows: Use the GetTopKMaxMinTmpSize API to calculate the required maximum/minimum temporary space size, use the minimum temporary space, and then determine the required tiling parameters based on the input length.

  - Invocation implementation  
    Use the kernel invocation operator <<<>>> to call the kernel function.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.
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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU mode by default
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

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result
  The following execution result indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
