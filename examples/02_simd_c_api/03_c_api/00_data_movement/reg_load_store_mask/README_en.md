# reg_load_store_mask Example

## Overview

This example demonstrates loading, storing, and using mask registers with the Ascend C C API. It includes a basic data-transfer scenario and a mask-register-based data-selection scenario.

## Supported Products and CANN Version

| Product | CANN Version |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```text
reg_load_store_mask/
├── figures/
│   └── reg_load_store_mask.png    // Scenario diagram
├── scripts/
│   ├── gen_data.py                // Generates input and golden data
│   └── verify_result.py           // Verifies output data
├── CMakeLists.txt                 // Build configuration
├── data_utils.h                   // Input and output file utilities
├── reg_load_store_mask.asc        // Ascend C C API implementation and entry
├── README.md                      // Chinese documentation
└── README_en.md                   // English documentation
```

## Detailed Scenario Description

MTE2 first copies input data from GM (Global Memory) to UB (Unified Buffer). The entry function invokes SIMD VF functions through `asc_vf_call` to load or generate a mask register and complete the register operation. Finally, MTE3 copies the result from UB to GM, and `asc_sync_pipe(PIPE_ALL)` at the end of the kernel completes all pipeline operations.

### Scenario 1: Basic Transfer Scenario

| Data | Name | Length | Data Type |
| --- | --- | --- | --- |
| Input | `src` | 1024 | `uint8_t` |
| Output | `dst` | 1024 | `uint8_t` |

The implementation proceeds as follows:

![Scenario 1 mask-register operation diagram](./figures/reg_load_store_mask.png)

1. `asc_loadalign` loads the first 32B of input into a mask register. In the generated input, the 1st and 32nd bytes are 1 and all other bytes are 0, so the 1st and 249th mask positions are active.
2. `asc_duplicate_scalar` fills the vector-register positions selected by the mask register with the value 2, and `asc_storealign` writes them back to UB. Only the 1st and 249th output elements are 2; all other elements remain 0.
3. `asc_create_mask_b8(PAT_ALL)` generates an all-one mask register, and `asc_storealign` writes the corresponding 32B of 0xFF data to elements 257 through 288 in UB.

### Scenario 2: Composite Computation Scenario

| Data | Name | Length | Data Type |
| --- | --- | --- | --- |
| Input | `src0` | 256 | `float` |
| Input | `src1` | 256 | `float` |
| Input | `mask` | 128 | `uint8_t` |
| Output | `dst` | 256 | `float` |

The implementation proceeds as follows:

1. `src0`, `src1`, and the mask data are copied from GM (Global Memory) to UB (Unified Buffer).
2. The SIMD VF function iterates four times. Each iteration uses `asc_loadalign_postupdate` to load 64 float elements and the corresponding 32B mask-data segment into vector registers and a mask register.
3. `asc_select` selects data according to mask-register bits: a value of 1 selects the corresponding element from `src0`, and a value of 0 selects it from `src1`.
4. `asc_storealign_postupdate` stores 64 float results from each iteration contiguously in UB, after which MTE3 copies them to GM.

## Build and Run

### Configure Environment Variables

Configure environment variables according to the CANN toolkit installation in the current environment:

```bash
source ${install_path}/cann/set_env.sh
```

> **Note:** `${install_path}` is the CANN package installation directory. The default is `/usr/local/Ascend` for the root user and `${HOME}/Ascend` for non-root users.

### Run the Example

Run the following commands in the example root directory. `SCENARIO_NUM` can be 1 or 2 and defaults to 1.

```bash
SCENARIO_NUM=1                                                                    # Select scenario 1
mkdir -p build && cd build;                                                        # Create and enter the build directory
cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # Build the project; NPU mode by default
python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM                         # Generate test input and golden data
./demo                                                                             # Run the example executable
python3 ../scripts/verify_result.py output/output.bin output/golden.bin           # Verify the result
```

The default mode is NPU execution. To use NPU simulation, add `-DCMAKE_ASC_RUN_MODE=sim`:

```bash
cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
```

Before changing the scenario or run mode, delete `CMakeCache.txt` in the `build` directory and rerun CMake to avoid using cached configuration.

### Build Options

| Option | Valid Values | Description |
| --- | --- | --- |
| `SCENARIO_NUM` | `1`, `2` | Selects the mask-register load, store, or selection scenario. |
| `CMAKE_ASC_RUN_MODE` | `npu`, `sim` | Selects NPU execution or NPU simulation mode. |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | Specifies the NPU architecture for Ascend 950PR/Ascend 950DT. |

## Expected Result

When the output matches the golden data, the verification script prints:

```bash
test pass!
```
