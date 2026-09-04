# data_copy_ub2l1 Example

## Overview

This example uses the Ascend C C API to move matrix data from UB (Unified Buffer) to L1 (L1 Buffer). It keeps the original Nz-input and ND-input UB-to-L1 matrix multiplication scenarios, changes the matrix shape to a non-aligned shape, and adds a GM->UB->L1->UB->GM bidirectional data path scenario. It applies to Ascend 950PR/Ascend 950DT (`dav-3510`) and can run in NPU execution or NPU simulation mode.

## Supported Products and CANN Versions

| Product | CANN version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```
├── data_copy_ub2l1
│   ├── scripts
│   │   ├── gen_data.py                // Generates input and golden data
│   │   └── verify_result.py           // Compares output with golden data
│   ├── CMakeLists.txt                 // CMake build file
│   ├── data_utils.h                   // Data read/write helper functions
│   ├── data_copy_ub2l1.asc            // C API implementation and launcher
│   ├── README.md                      // Chinese example documentation
│   └── README_en.md                   // English example documentation
```

## Scenario Description

Use the `SCENARIO_NUM` build parameter to select an input scenario. Scenarios 1 and 2 are the original UB-to-L1 matrix multiplication scenarios with the matrix shape changed to the non-aligned `[M, K, N] = [127, 129, 130]`. Scenario 3 is the newly added GM->UB->L1->UB->GM bidirectional path. The L1 storage used by Nz-format copies and matrix multiplication is padded by C0 or Cube blocks: A is stored as `[128, 144]`, and B is stored as `[144, 144]`. The kernel name is `data_copy_ub2l1`.

**Table 1: SCENARIO_NUM Values**

| SCENARIO_NUM | Input format | Input type | Output type | Data path |
|---|---|---|---|---|
| 1 | Padded Nz | half | float | Original Nz-input scenario: GM->UB->L1->L0->GM, contiguous UB-to-L1 copy |
| 2 | Compact ND | half | float | Original ND-input scenario: GM->UB with zero padding, ND-to-Nz rearrangement in UB, then L1 copy and matrix multiplication |
| 3 | Padded Nz | half | half | New bidirectional-path scenario: GM->UB->L1->UB->GM |

`SCENARIO_NUM` is passed by CMake as a compile-time macro. The kernel selects the scenario with `if constexpr`. After changing the scenario, recompile the project.

**Scenario 1: Original Nz-input scenario, half input type**

- Input: A has the logical shape `[127, 129]` and is padded to `[128, 144]` in `half` Nz format. B has the logical shape `[129, 130]` and is padded to `[144, 144]` in `half` Nz format
- Output: C `[127, 130]` in `float` ND format
- Implementation: The AIV side first moves A and B from GM to UB with `asc_copy_gm2ub`. After `asc_sync_notify` and `asc_sync_wait` establish the MTE2-to-MTE3 dependency, `asc_copy_ub2l1` copies the padded Nz-format data contiguously to L1

**Scenario 2: Original ND-input scenario, half input type**

- Input: A `[127, 129]` in compact `half` ND format. B `[129, 130]` in compact `half` ND format
- Output: C `[127, 130]` in `float` ND format
- Implementation: The AIV side uses `asc_ndim_copy_gm2ub` to move non-aligned ND input from GM to UB and zero-pad the right columns and bottom rows to the aligned shape. After MTE2-to-V synchronization, `copy_ub_nd_to_nz` calls `asc_copy_ub2ub` for each C0 column block to rearrange padded ND-format data into Nz format. A and B use separate UB temporary buffers, avoiding read/write overlap between the two rearrangements and the following UB-to-L1 transfers. After V-to-MTE3 synchronization, `asc_copy_ub2l1` copies the Nz-format data contiguously to L1

**Scenario 3: New GM->UB->L1->UB->GM bidirectional path**

- Input: A has the logical shape `[127, 129]` and is padded to `[128, 144]` in `half` Nz format. B input is not used in this scenario
- Output: The `[128, 144]` Nz-format A data after the L1 round trip, in `half` type
- Implementation: Scenario 3 uses `asc_get_phy_buf_addr` to manage UB and L1 addresses with fixed offsets, avoiding a mismatch between the UB address written by AIC and the UB address read by AIV. The AIV side uses `asc_copy_gm2ub` and `asc_copy_ub2l1` for GM->UB->L1. After the L1 data is ready, the AIC side uses `asc_copy_l12ub_sync` for synchronous L1->UB movement. The AIV side then waits for the AIC notification and writes the UB data back to GM with `asc_copy_ub2gm`.

### Data Flow and Synchronization

1. In scenario 1, the AIV side calls `asc_copy_gm2ub` to move padded Nz-format data from GM to UB with MTE2. In scenario 2, the AIV side calls `asc_ndim_copy_gm2ub` to move compact ND input into the padded UB region, using `asc_set_ndim_pad_value` and `asc_set_ndim_pad_count` for zero padding.
2. In scenario 1, the AIV side establishes MTE2-to-MTE3 synchronization, then calls `asc_copy_ub2l1` to copy Nz-format data contiguously from UB to L1. In scenario 2, it first establishes MTE2-to-V synchronization, then calls `copy_ub_nd_to_nz`, which calls `asc_copy_ub2ub` for each C0 column block to rearrange ND-format data in UB into Nz format. It then establishes V-to-MTE3 synchronization and calls `asc_copy_ub2l1` to copy the data contiguously from UB to L1.
3. In scenarios 1 and 2, `__mix__(1, 2)` starts two AIVs. AIV0 performs the actual data movement and calls `asc_sync_intra_arrive` to notify the AIC side. Both AIVs call `asc_sync_block_arrive` to participate in the group-level synchronization. The AIC side first waits for the real data producer with `asc_sync_intra_wait`, and then waits for the group-level synchronization with `asc_sync_block_wait` before reading L1 data.
4. In scenarios 1 and 2, the AIC side calls `asc_copy_l12l0a` and `asc_copy_l12l0b_transpose` to move Nz-format data from L1 Buffer to L0A Buffer and L0B Buffer, then calls `asc_sync_notify` and `asc_sync_wait` to establish MTE1-to-M synchronization before calling `asc_mmad` for the non-aligned logical matrix multiplication.
5. In scenarios 1 and 2, the AIC side calls `asc_sync_notify` and `asc_sync_wait` to establish M-to-FIX synchronization, then calls `asc_set_l0c2gm_nz2nd` to configure the conversion from Nz format to ND format and uses the Fixpipe API `asc_copy_l0c2gm` to move the `[127, 130]` result to GM.
6. In scenario 3, AIV0 uses a fixed UB offset to complete GM->UB->L1 and notifies the AIC side with `asc_sync_intra_arrive`. Both AIVs call `asc_sync_block_arrive` to participate in the group-level synchronization. The AIC side synchronously moves L1 data to the fixed UB offset for AIV0 and notifies AIV0 with `asc_sync_intra_arrive`. AIV0 writes the round-trip data back to GM with `asc_copy_ub2gm`.

The required pipeline dependencies are established during processing. `asc_sync_pipe(PIPE_ALL)` is called at the end of the kernel to ensure that all pipelines complete.

## Build and Run

Run the following steps in the example root directory.

- Configure environment variables

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:**`${install_path}` is the CANN package installation directory. The default is `/usr/local/Ascend` for the root user and `${HOME}/Ascend` for non-root users.

- Run the example

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # Build the project; NPU mode by default
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate input and golden data
  ./demo                           # Run the example
  python3 ../scripts/verify_result.py output/output.bin ./output/golden.bin -scenarioNum=$SCENARIO_NUM  # Verify the result
  ```

  Set `SCENARIO_NUM` and `-scenarioNum` to the same value: 1, 2, or 3. Scenario 3 outputs `half` Nz-format round-trip data, and the verification script compares it element by element as `half`.

  To use NPU simulation mode, add `-DCMAKE_ASC_RUN_MODE=sim` to the CMake command:

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;
  ```

  > **Notice:**Clear the CMake cache before switching run modes or scenarios. Run `rm CMakeCache.txt` in the build directory and rerun CMake.

- Build options

  | Option | Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution or NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture for Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`, `2`, `3` | Scenario number |

- Expected result

  A successful accuracy comparison prints:

  ```bash
  test pass!
  ```
