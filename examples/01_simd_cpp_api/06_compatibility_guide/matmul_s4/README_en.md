# Matmul Compatibility Example with int4 Data Type

## Overview

This example demonstrates the end-to-end implementation of int4 matrix multiplication, isolating different hardware implementations through compile-time macros.
- Atlas A2/A3 Training/Inference Series Products support the int4 data type and can perform Matmul computation directly.
- The Cube computation unit in Ascend 950PR/950DT does not support the int4 data type. int4x2 must be unzipped to int8 before performing Matmul computation.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── matmul_s4
│   ├── scripts                   // Data generation script directory
│   │   ├── gen_data.py           // Input data and ground truth generation script
│   │   └── verify_result.py      // Verification script for comparing output data with ground truth
│   ├── CMakeLists.txt            // Build project file
│   ├── data_utils.h              // Data read/write functions
│   ├── matmul_s4.asc             // AscendC example implementation & invocation example
│   └── README.md                 // Example documentation
```

## Example Specifications:

  <table>
  <caption>Table 1 Example Specification Description</caption>
  <tr><td rowspan="1" align="center">Category</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td rowspan="4" align="center">Example Input</td></tr>
  <tr><td align="center">x4</td><td align="center">[256, 256]</td><td align="center">int4b_t</td><td align="center">ND</td></tr>
  <tr><td align="center">y4</td><td align="center">[256, 256]</td><td align="center">int4b_t</td><td align="center">ND</td></tr>
  <tr><td align="center">tiling</td><td align="center">[1, 32]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[256, 256]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">matmul_s4</td></tr>
  </table>

## Example Implementation:

  - **Atlas A2/A3 Training/Inference Series Products**: The Cube computation unit supports the int4 data type and can execute Matmul computation directly.
  - **Ascend 950PR/950DT**: The Cube computation unit does not support the int4 data type. The Unzip operation must be performed on the Vector Core through mix mode to unzip int4x2 to int8 before performing Matmul computation.

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables

  Configure environment variables based on the [installation method](../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in this example directory.
  ```bash
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # Default npu mode
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Run the compiled executable
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify output results
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution results

  The following execution result indicates that the accuracy comparison succeeded:

  ```bash
  test pass!
  ```
