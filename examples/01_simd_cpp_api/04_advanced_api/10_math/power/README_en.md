# Power Example

## Overview

This example uses the Power high-level API to implement element-wise power operations, supporting three modes: tensor-to-tensor, tensor-to-scalar, and scalar-to-tensor power operations.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```text
├── power
│   ├── scripts
│   │   └── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── power.asc               // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  Implement element-wise power operations, supporting three modes: tensor-to-tensor, tensor-to-scalar, and scalar-to-tensor power operations.

  The computation formula is as follows:
  $$Power(x, y) = x^y$$

  Tensor-to-tensor, mode = 0: Two tensors of the same length, performing element-wise power operations.
  $$dstTensor_i = Power(srcbaseTensor_i, srcexpTensor_i)$$

  Tensor-to-scalar, mode = 1: Using a scalar as the exponent, all tensor elements use the same exponent for power operations.
  $$dstTensor_i = Power(srcbaseTensor_i, srcexpScalar)$$

  Scalar-to-tensor, mode = 2: Using a scalar as the fixed base, all tensor elements use the same base for power operations.
  $$dstTensor_i = Power(srcbaseScalar, srcexpTensor_i)$$

- Example specifications:  
  <table>
  <caption>Table 1: Example Input/Output Specifications</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> power </td></tr>
  <tr><td rowspan="4" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">srcbase</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">srcexp</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">dst</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">power_custom</td></tr>
  </table>

- Scenario description:
  <table>
  <caption>Table 2: SCENARIO Parameter Description</caption>
  <tr><td align="center">SCENARIO</td><td align="center">Base</td><td align="center">Exponent</td><td align="center">Description</td></tr>
  <tr><td align="center">0</td><td align="center">Tensor</td><td align="center">Tensor</td><td align="center">Both base and exponent are tensors</td></tr>
  <tr><td align="center">1</td><td align="center">Tensor</td><td align="center">Scalar</td><td align="center">Base is a tensor, exponent is a scalar</td></tr>
  <tr><td align="center">2</td><td align="center">Scalar</td><td align="center">Tensor</td><td align="center">Base is a scalar, exponent is a tensor</td></tr>
  </table>

- Example implementation:  
  This example implements a power_custom example with fixed shapes of input srcbase[1, 16], srcexp[1, 16], and output dst[1, 16]. The example function mode parameter defaults to 0, meaning both exponent and base are tensors.

  - Kernel implementation

    Use the Power high-level API to perform power operations, supporting tensor-to-tensor, tensor-to-scalar, and scalar-to-tensor modes.

  - Tiling implementation

    The host side obtains the maximum and minimum temporary space required by the Power API through GetPowerMaxMinTmpSize.

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
  SCENARIO=0
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO=$SCENARIO ..;make -j;    # Build the project, NPU mode by default
  python3 ../scripts/gen_data.py --scenario $SCENARIO  # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
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
  | `SCENARIO` | `0` (default), `1`, `2` | Scenario: 0-tensor to tensor, 1-tensor to scalar, 2-scalar to tensor |

- Execution result

  The following execution result indicates that the precision comparison is successful.

  ```bash
  test pass!
  ```
