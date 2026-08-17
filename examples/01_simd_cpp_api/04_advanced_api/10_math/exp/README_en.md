# Exp Example

## Overview

This example uses the Exp high-level API to implement natural exponential computation, supporting element-wise computation of $e^x$. The Exp high-level API allows setting the number of Taylor expansion terms, with a value range of [0, 255]. When the number of Taylor expansion terms is 0, the Taylor formula is not used for computation. This example sets the number of Taylor expansion terms to 10 by default.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```text
├── exp
│   ├── scripts
│   │   └── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── exp.asc                 // Ascend C operator implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  Compute the natural exponential element-wise. Users can choose whether to use the Taylor expansion formula for computation. The computation formulas are as follows:

  $$
  dstLocal_i = Exp(srcLocal_i)
  $$

  When the number of Taylor expansion terms is set to 0, the Taylor expansion formula is not used:

  $$Exp(x) = e^x$$

  When the number of Taylor expansion terms is not 0, the Taylor expansion formula is used:

  $$Exp(x) = e^{x A_{i}} * e^{x B_{i}}$$

  $xA_i$ represents the integer part of the source operand, obtained through floor(x). $xB_i$ represents the fractional part of the source operand.

  The Taylor expansion formula is:

  $$e^{x B_i} = 1 + xB_i +... + \frac{xB_i^n}{n!}$$

- Example specifications:  
  <table>
  <caption>Table 1: Example Input/Output Specifications</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> exp </td></tr>

  <tr><td rowspan="3" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src</td><td align="center">[1, 8192]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">dst</td><td align="center">[1, 8192]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">exp_custom</td></tr>
  </table>

- Example implementation:  
  This example implements an exp_custom example with fixed shapes of input src[1, 8192] and output dst[1, 8192].

  - Kernel implementation

    Use the Exp high-level API to compute the natural exponential, with optional use of the Taylor expansion formula and temporary buffer to improve precision.

  - Tiling implementation

    The host side obtains the maximum and minimum temporary space required by the Exp API through GetExpMaxMinTmpSize.

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
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, NPU mode by default
  python3 ../scripts/gen_data.py   # Generate test input data
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

- Execution result

  The following execution result indicates that the precision comparison is successful.

  ```bash
  test pass!
  ```
