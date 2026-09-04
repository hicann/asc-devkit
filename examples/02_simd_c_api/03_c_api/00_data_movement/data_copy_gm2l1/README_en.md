# data_copy_gm2l1 Example

## Overview

This example shows how to use the Ascend C C API to move input data from GM (Global Memory) to L1 Buffer. It supports Nz, ND, and DN input formats and vector-quantization parameter movement. After the input is moved, the example moves data from L1 Buffer to L0A Buffer and L0B Buffer, performs matrix multiplication, and uses Fixpipe to move the L0C Buffer result to GM (Global Memory).

This example applies to Ascend 950PR/Ascend 950DT (`dav-3510`) and can run in NPU execution or NPU simulation mode. CPU debug mode is not provided.

## Supported Products and CANN Versions

| Product | CANN version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```
├── data_copy_gm2l1
│   ├── figures                        // Illustrations
│   ├── scripts
│   │   ├── gen_data.py                // Generates input and golden data
│   │   └── verify_result.py           // Verifies that output matches the golden data
│   ├── data_utils.h                   // Data input/output helpers
│   ├── CMakeLists.txt                 // CMake build file
│   ├── data_copy_gm2l1.asc            // C API implementation and launcher
│   ├── README.md                      // Chinese example documentation
│   └── README_en.md                   // English example documentation
```

## Detailed Scenario Description

Use the `SCENARIO_NUM` build parameter to select an input scenario. The meaning of each value is shown below. All scenarios use the same matrix multiplication shape: [M, K, N] = [128, 128, 256]. The kernel is named `data_copy_gm2l1`.

**Table 1: SCENARIO_NUM Values**

| SCENARIO_NUM | Input format | Input type | Output type | Vector quantization enabled |
|---|---|---|---|---|
| 1 | Nz | half | float | No |
| 2 | ND | half | float | No |
| 3 | DN | half | float | No |
| 4 | ND | half | int8_t | Yes |

`SCENARIO_NUM` is passed by CMake as a compile-time macro. The kernel selects the corresponding scenario with `if constexpr`. After changing the scenario, recompile the project.

All device-side movement and compute use asynchronous Ascend C C API calls. `asc_sync_notify` and `asc_sync_wait` establish the required `MTE2→MTE1→M→FIX` dependencies; scenario 4 also uses `MTE2→FIX` to make the quantization parameters ready, then uses `asc_sync_pipe(PIPE_FIX)` after the L1 Buffer-to-Fixpipe Buffer transfer before Fixpipe moves the L0C Buffer result to GM. `asc_sync_pipe(PIPE_ALL)` is called at the end of the kernel to ensure that all pipelines complete.

**Scenario 1: Nz input, half input type**

- Input: A [128, 128] in `half` Nz format; B [128, 256] in `half` Nz format
- Output: C [128, 256] in `float` ND format
- Implementation: Use `asc_copy_gm2l1` to move the Nz input data from GM to L1
- Description: The inputs are already in Nz format, so no format conversion is required
<p align="center">
  <img src="figures/data_copy_gm2l1_nz2nz.png" width="800">
</p>

**Scenario 2: ND input, half input type**

- Input: A [128, 128] in `half` ND format; B [128, 256] in `half` ND format
- Output: C [128, 256] in `float` ND format
- Implementation: Configure ND2NZ parameters with `asc_set_gm2l1_nz_para`, then use `asc_copy_gm2l1_nd2nz` to move ND input data from GM to L1 and convert it to Nz
- Description: The ND inputs are converted to Nz during the movement
<p align="center">
  <img src="figures/data_copy_gm2l1_nd2nz.png" width="800">
</p>

**Scenario 3: DN input, half input type (Ascend 950PR/Ascend 950DT only)**

- Input: A [128, 128] in `half` DN format; B [128, 256] in `half` DN format
- Output: C [128, 256] in `float` ND format
- Implementation: Configure DN2NZ parameters with `asc_set_gm2l1_nz_para`, then use `asc_copy_gm2l1_dn2nz` to move DN input data from GM to L1 and convert it to Nz
- Description: The DN inputs are converted to Nz during the movement
<p align="center">
  <img src="figures/data_copy_gm2l1_dn2nz.png" width="800">
</p>

**Scenario 4: ND input, half input type, vector quantization enabled**

- Input: A [128, 128] in `half` ND format; B [128, 256] in `half` ND format; quantization parameters [256] in `uint64_t`
- Output: C [128, 256] in `int8_t` ND format
- Implementation: Configure ND2NZ parameters with `asc_set_gm2l1_nz_para`, then use `asc_copy_gm2l1_nd2nz` to move input data from GM to L1 and perform matrix multiplications; use `asc_copy_gm2l1` and `asc_copy_l12fb` to move quantization parameters from GM through L1 to Fixpipe Buffer; configure their address with `asc_set_l0c2gm_config`, then use `asc_copy_l0c2gm` with `VQF322B8_PRE` for vector-quantized output
- Description: The `float` matrix multiplication result is vector-quantized to `int8_t`; each C matrix column uses one quantization parameter. `asc_set_l0c2gm_config` configures the Fixpipe vector-quantization parameter address through `quant_pre`, which uses 128B address units. The parameter file must therefore be rounded up to 128B, with any padding zero-filled. In this example, the `[256] uint64_t` parameters occupy 2048B and already meet this requirement. Moving these parameters from L1 to Fixpipe Buffer transfers 2048B of data in 64B data blocks, so `len_burst` in `asc_copy_l12fb` is set to 32.

## Build and Run

Run the following steps in the example root directory to build and execute the example.

- Configure environment variables

  Configure environment variables according to the CANN development kit [installation instructions](../../../../../docs/en/quick_start.md#prepare&install).

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. The default is `/usr/local/Ascend` for the root user and `${HOME}/Ascend` for non-root users.

- Run the example

  Run the following commands in the example directory.

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # Build the project; NPU mode by default
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the example executable
  python3 ../scripts/verify_result.py output/output.bin ./output/golden.bin  # Verify the result
  ```

  Set both `SCENARIO_NUM` and `-scenarioNum` to a value from 1 through 4 to run the corresponding scenario.

  To use NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Notice:** Clear the CMake cache before switching run modes. Run `rm CMakeCache.txt` in the build directory and re-run CMake.

- Build options

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture for Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | 1-4 | Scenario number |

- Expected result

  For scenarios 1-3, the floating-point result must satisfy the accepted tolerance. For scenario 4, the int8_t result must exactly match the golden data. In either case, successful verification prints:

  ```bash
  test pass!
  ```
