# Axpy Example

## Overview

This example uses the Axpy high-level API to multiply each element in the source operand src by a scalar and add the result to the corresponding element in the destination operand dst. The Axpy API supports three data type combinations for source and destination operands: (half, half), (float, float), and (half, float). In this example, the input tensor and scalar data type is half, and the output tensor data type is float.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```text
├── axpy_half_float
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying whether the output data matches the ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── axpy_half_float.asc     // Ascend C operator implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

- Example function:  
  The Axpy example multiplies each element in the source operand src by a scalar, adds the result to the corresponding element in the destination operand dst, and returns the computation result.

  The corresponding mathematical expression is:  

  $$
  out = x * scalar + out
  $$

- Example specifications:
  <table>
  <caption>Table 1: Example Specifications</caption>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="5" align="center"> Axpy </td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">default</td></tr>
  <tr><td align="center">x</td><td align="center">[4, 128]</td><td align="center">half</td><td align="center">ND</td><td align="center">\</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">out</td><td align="center">[4, 128]</td><td align="center">float32</td><td align="center">ND</td><td align="center">\</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">kernel_vec_ternary_scalar_Axpy_half_2_float</td></tr>
  </table>

- Example implementation:  
  This example implements an Axpy example with fixed shapes of input x[4, 128] and output out[4, 128].
  - Kernel implementation

    First use the Duplicate API to initialize the output tensor to 0, then use the Axpy API to compute x multiplied by scalar plus the original value in out, obtain the final result, and transfer it to external storage.

  - Tiling implementation

    The host side obtains the maximum and minimum temporary space required by the Axpy API through GetAxpyMaxMinTmpSize.

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
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm the algorithm logic is correct
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
