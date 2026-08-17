# Select Example

## Overview

This example uses the Select high-level API to implement conditional selection. Based on the mask, it selects elements from the vector source operand src0 and the scalar source operand src1 at corresponding positions for output. When the mask value is 1, the value of the scalar source operand src1 is selected. When the mask value is 0, the element at the corresponding position of the vector source operand src0 is selected.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── select
│   ├── scripts
│   │   └── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── select.asc              // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  This example selects elements from source operand src0 and source operand src1Scalar based on the mask value at the corresponding position to produce dstTensor. When the mask value is 1, src1Scalar is selected. When the mask value is 0, the element at the corresponding position of src0 is selected.
  
- Example specifications:
  <table>
  <caption>Table 1: Example Input/Output Specifications</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> select </td></tr>

  <tr><td rowspan="4" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src</td><td align="center">[2, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">mask</td><td align="center">[2, 32]</td><td align="center">uint8_t</td><td align="center">ND</td></tr>
  <tr></tr>


  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">dst</td><td align="center">[2, 32]</td><td align="center">float</td><td align="center">ND</td></tr>


  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">select_custom</td></tr>
  </table>

- Example implementation:  
    This example implements a SelectCustom example with fixed shapes of input src0[2, 32], mask[2, 32], and output dst[2, 32]. src1Scalar is a fixed value of 1.234 with float data type.

  - Kernel implementation:  
    The computation logic is: Input data must first be transferred to on-chip storage, then the Select high-level API is used to complete the computation, and the result is transferred out.

  - Tiling implementation:  
    The tiling implementation process is as follows: Use the GetSelectMaxMinTmpSize API to calculate the required maximum/minimum temporary space size, use the minimum temporary space, then determine the required tiling parameters based on the input length, and include the scalar-type source operand in the tiling to pass to the kernel side.

  - Invocation implementation:  
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
