# Subnormal Compatibility Example

## Overview

This example uses the Ln interface to demonstrate the Subnormal computation mode compatibility adaptation of vector compute interfaces, isolating different hardware implementations through compile-time macros.

SubNormal floating-point numbers are those with all exponent bits set to 0 and a non-zero mantissa, used to represent values smaller than the minimum normal number. The 3510 architecture version does not support Subnormal by default; Subnormal floating-point numbers are treated as 0 in computation (FTZ, Flush To Zero).

- Atlas A2/A3 Training/Inference Series Products: The hardware supports Subnormal by default. No extra configuration is required when calling the `Ln` interface.
- Ascend 950PR/950DT: Subnormal is not supported by default. The `algo` parameter of `LnConfig` must be set to `LnAlgo::PRECISION_1ULP_FTZ_FALSE` so that Subnormal computation results are preserved through software simulation.

## Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── subnormal
│   ├── scripts
│   │   ├── gen_data.py         // Input data and ground truth generation script
│   │   └── verify_result.py    // Verify whether output data matches ground truth
│   ├── CMakeLists.txt          // Build project file
│   ├── data_utils.h            // Data read/write functions
│   ├── subnormal_custom.asc    // Ascend C example implementation & invocation example
│   └── README.md               // Example documentation
```

## Example Specifications

| Category | name | shape | data type | format |
|----------|------|-------|-----------|--------|
| Example Input | x | [1, 1024] | half | ND |
| Example Output | z | [1, 1024] | half | ND |
| Kernel Function Name | subnormal_custom | | | |

> **Note:** The input data `x` contains Subnormal half values (all exponent bits 0, non-zero mantissa, in the range `(0, 2^-14)` i.e. `(0, 6.10e-5)`). The ground truth `z` is the natural logarithm of the input computed at high precision, preserving the true logarithm of Subnormal inputs, instead of the `-inf` obtained when Subnormals are approximated as 0.

## Example Implementation

The data flow is: GM -> UB (Unified Buffer) -> GM.

1. Copy the input data from GM to UB.
2. Call the `Ln` interface to compute the natural logarithm, using different configurations depending on the architecture to preserve Subnormal computation results:
   - Atlas A2/A3 Training/Inference Series Products: Call the `Ln` interface directly; the hardware supports Subnormal by default.
   - Ascend 950PR/950DT: Configure `algo` of `LnConfig` to `LnAlgo::PRECISION_1ULP_FTZ_FALSE` and then call the `Ln` interface; Subnormal computation results are preserved through software simulation.
3. Copy the computation result from UB back to GM.

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin
  ```

  To use CPU debug or NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU simulation mode
  ```

  To build for Ascend 950PR/950DT:

  ```bash
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # Build project (Ascend 950PR/950DT)
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
