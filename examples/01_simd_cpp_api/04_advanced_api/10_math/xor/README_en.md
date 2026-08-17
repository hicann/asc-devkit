# Xor Example

## Overview

This example uses the Xor high-level API to implement element-wise XOR operations.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```text
├── xor
│   ├── scripts
│   │   └── gen_data.py         // Script for generating input data and ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── xor.asc                 // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  Perform element-wise XOR operations. The concept and rules of XOR are as follows:  
  Concept: Two data operands participate in a bitwise "exclusive OR" operation.  
  Operation rules: 0^0=0; 0^1=1; 1^0=1; 1^1=0. That is, if the two corresponding bits of the operands are different, the result bit is 1; otherwise it is 0 (same=0, different=1).

  The computation formulas are as follows:
  $$
  dstTensor_i = Xor(src0Tensor_i, src1Tensor_i)
  $$
  $$
  Xor(x, y) = (x \mid y) \& (\sim(x \& y))
  $$

- Example specifications:  
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center"> xor </td></tr>

  <tr><td rowspan="4" align="center">Example Input</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src0</td><td align="center">[1, 1024]</td><td align="center">int16_t</td><td align="center">ND</td></tr>
  <tr><td align="center">src1</td><td align="center">[1, 1024]</td><td align="center">int16_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td></tr>
  <tr><td align="center">dst</td><td align="center">[1, 1024]</td><td align="center">int16_t</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">xor_custom</td></tr>
  </table>

- Example implementation:  
  This example implements a xor_custom example with fixed shapes of input src0[1, 1024], src1[1, 1024], and output dst[1, 1024].

  - Kernel implementation

    Use the Xor high-level API to perform element-wise XOR operations, with optional use of a temporary buffer and specification of the number of computation elements.

  - Tiling implementation

    The host side obtains the maximum and minimum temporary space required by the Xor API through GetXorMaxMinTmpSize.

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
