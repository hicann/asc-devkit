# Mmad unitFlag Feature Example

## Overview

This example introduces how to use the unitFlag feature when calling the Mmad instruction.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.2.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.2.0 |

## Directory Structure

```
├── mmad_unitflag
│   ├── scripts
│   │   ├── gen_data.py             // Script for generating input data and ground truth data
│   │   └── verify_result.py        // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt              // Build project file
│   ├── data_utils.h                // Data read/write functions
│   ├── mmad_unitflag.asc           // Ascend C example implementation & invocation example
│   └── README.md                   // Example documentation
```

## Example Description

unitFlag is used to control fine-grained parallelism between Mmad and Fixpipe instructions. When this feature is enabled, after the hardware completes computing each fractal, the computation result is transferred out. This feature is not applicable to scenarios that accumulate in L0C Buffer.

In this example, the A matrix shape is [128, 512], the B matrix shape is [512, 256]. When executing the Mmad instruction, 8 iteration loops are performed along the K axis, with each iteration having a K length of 64. The input file `x1_gm.bin` is stored in a continuous K-block layout of `[kRound, M, K/kRound]`, facilitating direct reading of the current K block in each iteration. The first 7 Mmad operations still need to continue accumulating to L0C, so unitFlag is set to 2; the last Mmad operation needs to allow Fixpipe to read L0C after accumulation is complete, so the unitFlag for both Mmad and Fixpipe is set to 3. The unitFlag values during Mmad computation in each iteration are described in Table 1:

<a name="table1"></a>
<table border="2" align="center">
<caption style="font-weight: normal;">
    <span style="font-weight: bold; font-size: 1.2em;">Table 1: unitFlag Value Description</span></caption>
  <tr>
    <td>unitFlag Value</td>
    <td>Description</td>
    <td>Example Implementation</td>
  </tr>
  <tr>
    <td>0</td>
    <td>Do not enable unitFlag</td>
    <td>-</td>
  </tr>
  <tr>
    <td>1</td>
    <td>Reserved value</td>
    <td>-</td>
  </tr>
  <tr>
    <td>2</td>
    <td>Enable unitFlag; after the hardware completes the instruction, the unitFlag feature is not disabled</td>
    <td>The first 7 Mmad operations set unitFlag to 2 to ensure subsequent Mmad operations can write to L0C</td>
  </tr>
  <tr>
    <td>3</td>
    <td>Enable unitFlag; after the hardware completes the instruction, the unitFlag feature is disabled</td>
    <td>The last Mmad and Fixpipe must be set to 3 to ensure Fixpipe can read L0C</td>
  </tr>
</table>

## Build and Run

Run the following steps in the root directory of this example to build and run the operator.
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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DENABLE_UNITFLAG=1 ..;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic is correct
  ```
  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DENABLE_UNITFLAG=1 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DENABLE_UNITFLAG=1 ..;make -j; # NPU simulation mode
  ```
  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products/Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `ENABLE_UNITFLAG` | `0`, `1` (default) | Whether to enable unitFlag |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
