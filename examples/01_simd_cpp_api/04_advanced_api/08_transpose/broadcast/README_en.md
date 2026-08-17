# Broadcast Example

## Overview

This example uses the Broadcast high-level API to implement data broadcasting, which extends the input tensor along specified axes to a target shape. It is applicable to scenarios such as data alignment and dimension expansion.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```text
├── broadcast
│   ├── scripts
│   │   └── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── broadcast.asc           // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  Perform broadcast computation on the input tensor.

- Example specifications:

<table>
  <caption>Table 1: Example Specifications - Scenario 0</caption>
  <tr>
    <td align="center">Example Type (OpType)</td>
    <td colspan="4" align="center">broadcast</td>
  </tr>
  <tr>
    <td rowspan="2" align="center">Example Input</td>
    <td align="center">name</td>
    <td align="center">shape</td>
    <td align="center">data type</td>
    <td align="center">format</td>
  </tr>
  <tr>
    <td align="center">x</td>
    <td align="center">[1, 48]</td>
    <td align="center">float</td>
    <td align="center">ND</td>
  </tr>
  <tr>
    <td align="center">Example Output</td>
    <td align="center">y</td>
    <td align="center">[96, 48]</td>
    <td align="center">float</td>
    <td align="center">ND</td>
  </tr>
  <tr>
    <td align="center">Kernel Function Name</td>
    <td colspan="4" align="center">broadcast_custom</td>
  </tr>
</table>

<table>
  <caption>Table 2: Example Specifications - Scenario 1</caption>
  <tr>
    <td align="center">Example Type (OpType)</td>
    <td colspan="4" align="center">broadcast</td>
  </tr>
  <tr>
    <td rowspan="2" align="center">Example Input</td>
    <td align="center">name</td>
    <td align="center">shape</td>
    <td align="center">data type</td>
    <td align="center">format</td>
  </tr>
  <tr>
    <td align="center">x</td>
    <td align="center">[96, 1]</td>
    <td align="center">float</td>
    <td align="center">ND</td>
  </tr>
  <tr>
    <td align="center">Example Output</td>
    <td align="center">y</td>
    <td align="center">[96, 96]</td>
    <td align="center">float</td>
    <td align="center">ND</td>
  </tr>
  <tr>
    <td align="center">Kernel Function Name</td>
    <td colspan="4" align="center">broadcast_custom</td>
  </tr>
</table>

- Scenario description:

  <table>
  <caption>Table 3: TESTCASE Parameter Description</caption>
  <tr><td align="center">TESTCASE</td><td align="center">Input Shape</td><td align="center">Output Shape</td><td align="center">Broadcast Axis</td><td align="center">Description</td></tr>
  <tr><td align="center">0</td><td align="center">[1, 48]</td><td align="center">[96, 48]</td><td align="center">0</td><td align="center">Broadcast along the first dimension, expanding 1 to 96</td></tr>
  <tr><td align="center">1</td><td align="center">[96, 1]</td><td align="center">[96, 96]</td><td align="center">1</td><td align="center">Broadcast along the second dimension, expanding 1 to 96</td></tr>
  </table>

- Example implementation:  
  This example implements two broadcast scenarios: broadcasting from [1, 48] to [96, 48] and from [96, 1] to [96, 96].

  - Kernel implementation  
    Use the Broadcast high-level API to complete broadcasting, extending the input tensor along the specified axis to the target shape.

  - Tiling implementation  
    The tiling implementation process for the broadcast example is as follows: first obtain the 2D shapes of input and output, then populate TilingData with the broadcast axis and the dimensions of the input/output tensors.

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
  TESTCASE=1                    # 0: shape[1, 48]->[96,48]  1: shape[96,1]->[96,96]
  mkdir -p build && cd build;   # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Build the project, NPU mode by default
  python3 ../scripts/gen_data.py --testcase $TESTCASE  # Generate test input data
  ./demo $TESTCASE              # Run the compiled executable to execute the example
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
