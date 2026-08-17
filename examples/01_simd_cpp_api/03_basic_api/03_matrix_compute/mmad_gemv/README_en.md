# Mmad GEMV Example

## Overview

This example introduces matrix multiplication in GEMV (M=1) mode.

## Supported Products and CANN Versions

| Product | CANN Version |
|---------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── mmad_gemv
│   ├── figures                     // Illustration files in this document
│   ├── scripts
│   │   ├── gen_data.py             // Script for generating input data and ground truth data
│   │   └── verify_result.py        // Script for verifying whether output data matches ground truth data
│   ├── CMakeLists.txt              // Build project file
│   ├── data_utils.h                // Data read/write functions
│   ├── mmad_gemv.asc               // Ascend C example implementation & invocation example
│   └── README.md                   // Example documentation
```

## Example Description

The GEMV mode refers to the scenario in Mmad computation where M=1, and a left matrix A with shape (1, K) performs matrix multiplication with a right matrix B with shape (K, N). When M=1, GEMV mode is automatically enabled, and it can only be disabled by setting `mmadParams.disableGemv = true` on Ascend 950PR/Ascend 950DT. In this example, the compilation parameter `DISABLE_GEMV` selects whether to disable GEMV mode: 0 means GEMV is enabled, 1 means disabled.

Using M=1, K=256, N=32, with left and right matrix data type half as a specific example, the Mmad computation process in GEMV mode and non-GEMV mode is described below.

- GEMV mode

  When transferring the A matrix from A1 to A2, the 1 * 256 vector is treated as a 16 * 16 matrix, and the LoadData interface is called once to complete the matrix transfer with a 16 * 16 fractal size. The transfer of the B matrix and the matrix multiplication computation are the same as in the basic scenario, as shown in Figure 1 below.
  <p align="center">
  <img src="figures/开启gemv.png" width="600">
  </p>
  <p align="center">
  Figure 1: GEMV mode, Mmad computation diagram
  </p>

- Non-GEMV mode

  When transferring the A matrix from A1 to A2, the 1 * 256 vector is treated as unaligned matrix data, with the M direction aligned to 16 before transfer. The LoadData interface is called to transfer 16 * 16 fractal-sized matrices each time, for a total of CeilDiv(K, 16)=16 transfers, resulting in increased data transfer volume and worse performance compared to GEMV mode, as shown in Figure 2 below.
  <p align="center">
  <img src="figures/关闭gemv.png" width="600">
  </p>
  <p align="center">
  Figure 2: Non-GEMV mode, Mmad computation diagram
  </p>

## Constraints

- In Mmad computation, to enable GEMV mode, the parameter `mmadParams.m` must equal 1.
- In GEMV scenarios, the left matrix A does not support transpose when transferring from L1 to L0A.

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DDISABLE_GEMV=0 ..;make -j;    # Build the project, default npu mode
  python3 ../scripts/gen_data.py   # Generate test input data
  ./demo                           # Run the compiled executable to execute the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # Verify whether the output result is correct and confirm algorithm logic is correct
  ```
  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DDISABLE_GEMV=0 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DDISABLE_GEMV=0 ..;make -j; # NPU simulation mode
  ```
  > **Notice:** Clear the cmake cache before switching build modes. Run `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build option description

  | Option | Values | Description |
  |--------|--------|-------------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products/Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |
  | `DISABLE_GEMV` | `0` (default), `1` | Whether to disable GEMV mode, `only supported when CMAKE_ASC_ARCHITECTURES==dav-3510 to set to 1` |

- Execution result

  The following execution result indicates that the accuracy comparison is successful.

  ```bash
  test pass!
  ```
