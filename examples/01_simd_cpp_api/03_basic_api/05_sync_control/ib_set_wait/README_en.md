# IBSet and IBWait Inter-Core Synchronization Example

## Overview

This example implements inter-core synchronization based on IBSet and IBWait, applicable to the following scenario: when two cores collaboratively operate on the same block of global memory with data dependencies, IBSet and IBWait are used to achieve inter-core synchronization and avoid data read/write errors.

> **Notice:** This example is only applicable to the TPipe and TQue programming model.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── ib_set_wait
│   ├── scripts
│   │   ├── gen_data.py         // Script for generating input data and ground truth data
│   │   └── verify_result.py    // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── ib_set_wait.asc         // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Description

This example uses two cores working collaboratively, with each core processing 256 half data elements, totaling 512 elements. Core 0 reads input x[0:256] and y[0:256], computes the result and writes it to z[0:256]; Core 1 reads the output z[0:256] from Core 0 and input y[256:512], computes the result and writes it to z[256:512]. It is necessary to ensure that Core 1 reads z[0:256] only after Core 0 completes the write operation, avoiding write-after-read data dependency issues.

This example achieves inter-core synchronization through IBSet and IBWait:

- After Core 0 completes the write operation of z[0:256], it sets the synchronization flag through IBSet
- Before Core 1 reads z[0:256], it waits for Core 0 to complete the write operation through IBWait

### Example Specifications

<table>
<caption>Table 1: Example Input/Output Specifications</caption>
<tr><td rowspan="1" align="center">Example Type</td><td colspan="5" align="center">Inter-Core Synchronization</td></tr>
<tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center"></td></tr>
<tr><td align="center">x</td><td align="center">[512]</td><td align="center">half</td><td align="center">ND</td><td align="center"></td></tr>
<tr><td align="center">y</td><td align="center">[512]</td><td align="center">half</td><td align="center">ND</td><td align="center"></td></tr>
<tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[512]</td><td align="center">half</td><td align="center">ND</td><td align="center"></td></tr>
<tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="5" align="center">kernel_ib_set_wait</td></tr>
</table>

## Build and Run

Run the following steps in the root directory of this example to build and run the example.

- Configure environment variables
  Configure environment variables according to the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the example

  Run the following commands in the example directory.
  ```bash
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j;    # Build the project
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin  # Verify whether the output result is correct
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # CPU debug mode
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
