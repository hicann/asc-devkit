# SetLoadDataBoundary Compatibility Example

## Overview

This example implements the setting of L1 Buffer boundary values, isolating different hardware implementations through compile-time macros.

Atlas A2/A3 Series Products set the L1 Buffer boundary value through the `SetLoadDataBoundary` interface, while the new Ascend 950PR/950DT architecture does not support this interface and requires manually splitting LoadData (convolution data transfer) instructions to implement the data wrap-around function for circular data reading.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.2.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.2.0 |

## Directory Structure

```
├── set_loaddata_boundary
│   ├── scripts
│   │   ├── gen_data.py              // Input data and ground truth generation script
│   │   └── verify_result.py         // Verification script for comparing output data with ground truth
│   ├── CMakeLists.txt               // Build project file
│   ├── data_utils.h                 // Data read/write functions
│   ├── set_loaddata_boundary.asc    // AscendC example implementation & invocation example
│   └── README.md                    // Example documentation
```

## Example Specifications

<table>
<caption>Example Specification Table</caption>
<tr><td rowspan="1" align="center">Category</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td rowspan="2" align="center">Example Input</td><td align="center">x</td><td align="center">[32, 32]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[32, 32]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[32, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">set_loaddata_boundary</td></tr>
</table>

## Example Implementation
    
- **Atlas A2/A3 Training/Inference Series Products**: Set the L1 Buffer boundary value to 1024 bytes through the SetLoadDataBoundary interface. When the LoadData (convolution data transfer) instruction processes the source operand, if the source operand address on the L1 Buffer exceeds the set boundary, data is automatically read from the start address of the L1 Buffer, implementing the circular data reading function.

- **Ascend 950PR/950DT**: The new architecture hardware has removed the L1 Buffer boundary value setting registers and no longer supports the SetLoadDataBoundary interface. To implement the same circular data reading function, the LoadData (convolution data transfer) interface must be manually split into multiple instructions, implementing manual wrap-around by adjusting the destination operand address offset.

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
  mkdir -p build && cd build;                                               # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # Build the project
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic correctness
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;  # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;  # NPU simulation mode
  ```

  > **Notice**: Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution results

  The following execution result indicates that the accuracy comparison succeeded.

  ```bash
  test pass!
  ```
