# L1 to GM Transfer Compatibility Example

## Overview

This example demonstrates the end-to-end flow of L1 data transfer to GM, isolating different hardware implementations through compile-time macros.

- Atlas A2/A3 Training/Inference Series Products use the DataCopy interface directly for the transfer.
- The new Ascend 950PR/950DT architecture does not support direct transfer. Data is output to the L0C Buffer through Mmad matrix multiplication, and then transferred from the L0C Buffer to GM through Fixpipe.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── data_copy_l1togm
│   ├── scripts
│   │   ├── gen_data.py         // Input data and ground truth generation script
│   │   └── verify_result.py    // Verification script for comparing output data with ground truth
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── data_copy_l1togm.asc    // AscendC example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Specifications

This example has different implementation logic depending on the architecture:

### Atlas A2/A3 Training/Inference Series Products

- Example specifications:
  <table>
  <caption>Atlas A2/A3 Training/Inference Series Products Example Specification Table</caption>
  <tr><td rowspan="1" align="center">Category</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td rowspan="1" align="center">Example Input</td>
  <td align="center">x</td><td align="center">[64, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[64, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">data_copy_l1togm</td></tr>
  </table>
- Example implementation: Call the DataCopy instruction to implement data transfer from GM to L1 and then to GM.

### Ascend 950PR/950DT

- Example specifications:
  <table>
  <caption>Ascend 950PR/950DT Product Example Specification Table</caption>
  <tr><td rowspan="1" align="center">Category</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td></tr>
  <tr><td align="center">x</td><td align="center">[64, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[128, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[64, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">data_copy_l1togm</td></tr>
  </table>
- Example implementation: The new Ascend 950PR/950DT architecture does not support direct L1 to GM transfer. In the cube-only scenario, matrix multiplication can be used to achieve the transfer effect. An identity matrix is allocated in GM (original matrix × identity matrix = original matrix). The data is output to the L0C Buffer through Mmad matrix multiplication, and then transferred from the L0C Buffer to GM through Fixpipe. The data flow is: GM -> A1/B1 -> L0A/L0B -> L0C -> GM.

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
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct
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
