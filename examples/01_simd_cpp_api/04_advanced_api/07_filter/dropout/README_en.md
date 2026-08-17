# DropOut Example

## Overview

This example uses the DropOut high-level API in a neural network training scenario to implement dropout functionality, which filters the source operand based on a mask to produce the destination operand. DropOut is a commonly used regularization technique that prevents overfitting by randomly dropping some neurons. This example uses the byte mode of DropOut, which supports selective retention or discarding of input data based on a mask.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── dropout
│   ├── scripts
│   │   └── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── dropout.asc             // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  This example filters the input source operand based on the input mask in the byte mode of DropOut to produce the output destination operand.
- Example specifications:  
  <table>
  <caption>Table 1: Example Input/Output Specifications</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> dropout </td></tr>

  <tr><td rowspan="4" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src</td><td align="center">[4, 256]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">mask</td><td align="center">[4, 32]</td><td align="center">uint8_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">dst</td><td align="center">[4, 256]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">dropout_custom</td></tr>
  </table>

- Example implementation:  
    This example implements dropout functionality with fixed shapes of input src[4, 256], mask[4, 32], and output dst[4, 256].

  - Kernel implementation:  
    The computation logic is: Input data must first be transferred to on-chip storage, then the DropOut high-level API is used to complete the dropout computation, and the result is transferred out.

  - Tiling implementation:  
    The tiling implementation process is as follows: Use the GetDropOutMaxMinTmpSize API to calculate the required maximum/minimum temporary space size, use the minimum temporary space, and then determine the required tiling parameters based on the input length.

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
    
- Run the example (NPU mode)
  ```bash
  mkdir -p build && cd build;   # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j;             # Build the project
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                        # Run the compiled executable to execute the example
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
