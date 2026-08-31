# data_copy_l0c2ub Example

## Overview

This example uses the Ascend C C API to move matrix-multiplication results from L0C Buffer to UB (Unified Buffer) through Fixpipe. It performs scalar or vector quantization, ReLU activation, and NZ2ND conversion during the movement. The ND-format input matrices A and B are converted to Nz while moved from GM to L1 Buffer, then processed through L0A Buffer and L0B Buffer by two K-axis matrix-multiplication chunks. For host-side verification, the example then copies the UB output back to GM.

This example applies to Ascend 950PR/Ascend 950DT (`dav-3510`) and can run in NPU execution or NPU simulation mode. CPU debug mode is not provided.

## Supported Products and CANN Versions

| Product | CANN version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```
data_copy_l0c2ub
├── scripts
│   ├── gen_data.py                // Generates input and golden data
│   └── verify_result.py           // Verifies output against golden data
├── data_utils.h                   // Data input/output helpers
├── CMakeLists.txt                 // CMake build file
├── data_copy_l0c2ub.asc           // C API implementation and launcher
├── README.md                      // Chinese example documentation
└── README_en.md                   // English example documentation
```

## Detailed Scenario Description

Use the `SCENARIO_NUM` build parameter to select a scenario. Every scenario uses the matrix-multiplication shape `[M, K, N] = [128, 128, 256]`; the K axis is split into two `[128, 64]` chunks. The kernel is named `data_copy_l0c2ub`.

| SCENARIO_NUM | A/B type | L0C type | Output type and format | Quantization mode | ReLU | NZ2ND |
|---|---|---|---|---|---|---|
| 1 | int8_t | int32_t | half, ND | Scalar `DEQF16` | Yes | Yes |
| 2 | int8_t | int32_t | half, Nz | Vector `VDEQF16` | No | No |
| 3 | half | float | int8_t, Nz | Scalar `QF322B8_PRE` | No | No |
| 4 | half | float | int8_t, ND | Vector `VQF322B8_PRE` | Yes | Yes |
| 5 | int8_t | int32_t | int8_t, ND | Scalar `REQ8` | No | Yes |
| 6 | int8_t | int32_t | int8_t, Nz | Vector `VREQ8` | Yes | No |
| 7 | half | float | float, ND | `NoQuant` | No | Yes, M-axis dual-destination split |

`SCENARIO_NUM` is passed by CMake as a compile-time macro. The kernel selects the corresponding scenario with `if constexpr`. After changing the scenario, recompile the project.

The device-side flow is as follows. Each K chunk follows `GM→L1 Buffer (MTE2)→L0A Buffer/L0B Buffer (MTE1)→Mmad (M)`. `MTE1→MTE2` and `M→MTE1` protect L1 Buffer, L0A Buffer, and L0B Buffer reuse for the next chunk. The final `M→FIX` dependency ensures that Fixpipe uses `asc_copy_l0c2ub` to move the L0C Buffer result to UB after Mmad completes. Scenarios 1 through 6 use single-destination mode, write the result to subblock0 UB, and let subblock0 write it back to GM. Scenario 7 uses `DUAL_DST_SPLIT_M` to split the L0C result along M and write each half to one AIV UB; both AIVs wait for the result and write their own GM slice. `asc_sync_pipe(PIPE_ALL)` is called at the end of the kernel to ensure that all pipelines complete.

**Scenario 1: int8 input, scalar dequantization, and ND half output**

- Input: A `[128, 128]` and B `[128, 256]`, both ND `int8_t`
- Output: C `[128, 256]` in ND `half`
- Implementation: Configure scalar dequantization with `asc_set_l0c_copy_prequant`, use `DEQF16`, and enable ReLU and NZ2ND

**Scenario 2: int8 input, vector dequantization, and Nz half output**

- Input: A `[128, 128]` and B `[128, 256]`, both ND `int8_t`; `[256] uint64_t` quantization parameters
- Output: C `[128, 256]` in Nz `half`
- Implementation: Use `VDEQF16`; each output column uses one vector quantization parameter

**Scenario 3: half input, scalar quantization, and Nz int8_t output**

- Input: A `[128, 128]` and B `[128, 256]`, both ND `half`
- Output: C `[128, 256]` in Nz `int8_t`
- Implementation: Configure scalar quantization with `asc_set_l0c_copy_prequant` and use `QF322B8_PRE`

**Scenario 4: half input, vector quantization, and ND int8_t output**

- Input: A `[128, 128]` and B `[128, 256]`, both ND `half`; `[256] uint64_t` quantization parameters
- Output: C `[128, 256]` in ND `int8_t`
- Implementation: Use `VQF322B8_PRE`, and enable ReLU and NZ2ND

**Scenario 5: int8 input, scalar quantization, and ND int8_t output**

- Input: A `[128, 128]` and B `[128, 256]`, both ND `int8_t`
- Output: C `[128, 256]` in ND `int8_t`
- Implementation: Configure scalar quantization with `asc_set_l0c_copy_prequant`, use `REQ8`, and enable NZ2ND

**Scenario 6: int8 input, vector quantization, and Nz int8_t output**

- Input: A `[128, 128]` and B `[128, 256]`, both ND `int8_t`; `[256] uint64_t` quantization parameters
- Output: C `[128, 256]` in Nz `int8_t`
- Implementation: Use `VREQ8` and enable ReLU

**Scenario 7: half input, L0C-to-UB dual-destination M split, and ND float output**

- Input: A `[128, 128]` and B `[128, 256]`, both ND `half`
- Output: C `[128, 256]` in ND `float`
- Implementation: Use `asc_copy_l0c2ub` with `DUAL_DST_SPLIT_M` and `NoQuant` to split the result across the two AIV UBs, then let each AIV write back its GM slice

For scenarios 2, 4, and 6, the parameters are moved from GM to L1 Buffer with `asc_copy_gm2l1`, then from L1 Buffer to Fixpipe Buffer with `asc_copy_l12fb`. `asc_set_l0c_copy_config` configures the vector-quantization parameter address. Its parameter address is expressed in 128B units, so the parameter file must be rounded up to 128B and any padding must be zero-filled. The `[256] uint64_t` parameters in this example occupy 2048B and already meet this requirement. Moving these parameters from L1 Buffer to Fixpipe Buffer transfers 2048B of data in 64B data blocks, so `len_burst` in `asc_copy_l12fb` is set to 32. `asc_sync_pipe(PIPE_FIX)` completes the L1 Buffer-to-Fixpipe Buffer movement and parameter configuration before Fixpipe moves the L0C Buffer result to UB.

## Build and Run

Run the following steps in the example root directory.

- Configure environment variables

  Configure the environment according to the CANN development kit [installation instructions](../../../../../docs/en/quick_start.md#prepare&install).

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. The default is `/usr/local/Ascend` for the root user and `${HOME}/Ascend` for non-root users.

- NPU execution

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # Build the project; NPU mode by default
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input and golden data
  ./demo                           # Run the example executable
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin ./output/golden.bin  # Verify the result
  ```

  > **Note:** `gen_data.py` and `verify_result.py` must use the same `SCENARIO_NUM`. If `-scenarioNum` is omitted from the verification command, the script uses scenario 1 by default. Scenario 7 writes `float32` output, which would be read as `float16` and cause verification to fail.

- NPU simulation

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  Clear the CMake cache before switching run modes. For example, run `rm CMakeCache.txt` in the `build` directory before reconfiguring.

- Build options

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution or NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture for Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | 1-7 | Scenario number |

- Expected result

  Successful output-to-golden verification prints:

  ```bash
  test pass!
  ```
