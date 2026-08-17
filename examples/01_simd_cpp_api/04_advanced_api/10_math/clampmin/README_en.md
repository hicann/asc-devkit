# Single-Side Clamp Example

## Overview

This example uses the ClampMin/ClampMax high-level APIs to implement single-side clamping of a tensor to a scalar value.  
ClampMin replaces elements in the input tensor that are less than the scalar with the scalar. ClampMax replaces elements in the input tensor that are greater than the scalar with the scalar. Use parameter configuration to select ClampMin or ClampMax functionality.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```text
├── clampmin
│   ├── scripts
│   │   └── gen_data.py   // Script for generating input data and ground truth data
│   ├── CMakeLists.txt    // Build project file
│   ├── data_utils.h      // Data read/write functions
│   ├── clampmin.asc      // Ascend C example implementation & invocation example
│   └── README.md         // Example documentation
```

## Example Description

- Example function:  
  Select ClampMin or ClampMax functionality through parameter configuration:
  - ClampMin: Replace values in srcTensor that are less than the scalar with the scalar; values greater than or equal to the scalar remain unchanged and are output as dstTensor.
  - ClampMax: Replace values in srcTensor that are greater than the scalar with the scalar; values less than or equal to the scalar remain unchanged and are output as dstTensor.

  The computation formulas are as follows:  

  $$
  ClampMin(srcTensor_i, scalar) =
  \begin{cases}
  srcTensor_i, & srcTensor_i \geq scalar \\
  scalar, & srcTensor_i < scalar
  \end{cases}
  $$

  $$
  ClampMax(srcTensor_i, scalar) =
  \begin{cases}
  srcTensor_i, & srcTensor_i \leq scalar \\
  scalar, & srcTensor_i > scalar
  \end{cases}
  $$

- Example specifications:  
  <table>
  <caption>Table 1: Example Specifications</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> clampmin/clampmax </td></tr>

  <tr><td rowspan="3" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src</td><td align="center">[1, 256]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">dst</td><td align="center">[1, 256]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">clampmin_clampmax_custom</td></tr>
  </table>

- Example implementation:  

  - Kernel implementation

    Use the ClampMin/ClampMax high-level APIs to complete single-side clamping computation. Use parameter configuration to select ClampMin or ClampMax functionality.

  - Tiling implementation

    The host side obtains the maximum and minimum temporary space required by the ClampMin/ClampMax APIs through the GetClampMaxMinTmpSize API.

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
  SCENARIO=0  # 0: ClampMin, 1: ClampMax
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DSCENARIO=$SCENARIO -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, NPU mode by default
  python3 ../scripts/gen_data.py --scenario $SCENARIO  # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  ```

  To use CPU debugging or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.
  
  For example:

  ```bash
  cmake -DSCENARIO=$SCENARIO -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debugging mode
  cmake -DSCENARIO=$SCENARIO -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Before switching the build mode, clear the cmake cache by running `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU run, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products; dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `SCENARIO` | `0` (default), `1` | Scenario: 0 corresponds to ClampMin, 1 corresponds to ClampMax |

- Execution result

  The following execution result indicates that the precision comparison is successful.

  ```bash
  test pass!
  ```
