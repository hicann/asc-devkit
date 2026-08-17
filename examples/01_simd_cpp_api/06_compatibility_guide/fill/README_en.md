# Fill Compatibility Example

## Overview

This example demonstrates how to use the Fill interface to initialize the L0A Buffer and L0B Buffer, isolating different hardware implementations through compile-time macros.

- In Atlas A2/A3 Series Products, the Fill interface can be used directly to initialize the L0A/L0B Buffer.
- In the Ascend 950PR/Ascend 950DT platform, since the hardware instructions related to L0A Buffer/L0B Buffer initialization have been removed, the Fill interface cannot be used directly to initialize the L0A/L0B Buffer. Instead, the L1 Buffer must be initialized first, and then the initialized data must be transferred into L0A/L0B to indirectly complete the L0A/L0B Buffer initialization.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── fill
│   ├── scripts
│   │   ├── gen_data.py         // Input data and ground truth generation script
│   │   └── verify_result.py    // Verify whether output data matches ground truth
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── fill.asc                // AscendC example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Specifications

<table>
<caption>Example Specification Table</caption>
<tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[128, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[128, 64]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[128, 64]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">fill</td></tr>
</table>

> **Note:** In the specification table, x and y are placeholders used only in the end-to-end process (to generate the input files and golden data). The kernel does not actually read these two GM inputs. Instead, it uses the `Fill` interface to construct all-ones matrices for the `Mmad` computation. Therefore, the values of x and y do not affect the computation result.

### Example Implementation

   1. First, initialize the L0A Buffer and L0B Buffer. Different hardware architectures use different methods:
      - Atlas A2/A3 Training/Inference Series Products: Call the `Fill` interface to directly initialize the L0A Buffer and L0B Buffer to a specified value (initialized to 1 in this example).
      - Ascend 950PR/950DT: Use the `Fill` interface to initialize the L1 Buffer to a specified value (initialized to 1 in this example), then transfer the data to the L0A Buffer and L0B Buffer through the `LoadData` interface.
   2. Call the `Mmad` interface to perform matrix multiplication.
   3. Transfer the result to Global Memory through the `Fixpipe` interface.

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;                      # Build the project (Atlas A2/A3 Series Products)
  python3 ../scripts/gen_data.py                                            # Generate test input data
  ./demo                                                                    # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic correctness
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  To build for Ascend 950PR/950DT:

  ```bash
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # Build the project (Ascend 950PR/950DT)
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
