# reg_load_store_align Example

## Overview

This example demonstrates the Reg vector computation interfaces in Ascend C C API for aligned data loads and stores between Unified Buffer (UB) and vector registers. Multiple scenarios demonstrate different addressing and data-distribution modes.

## Supported Products and CANN Version

| Product | CANN Version |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```text
reg_load_store_align/
├── scripts/
│   ├── gen_data.py                // Generates input and golden data
│   └── verify_result.py           // Verifies output data
├── CMakeLists.txt                 // Build configuration
├── data_utils.h                   // Input and output file utilities
├── reg_load_store_align.asc       // Ascend C C API implementation and entry
├── README.md                      // Chinese documentation
└── README_en.md                   // English documentation
```

## Detailed Scenario Description

MTE2 first copies the input from GM (Global Memory) to UB (Unified Buffer). The entry function invokes SIMD VF functions through `asc_vf_call` to process register-width chunks for loading, addition, and storage. Finally, MTE3 copies the result from UB to GM, and `asc_sync_pipe(PIPE_ALL)` at the end of the kernel completes all pipeline operations.

| Scenario | Input Length (`src0`, `src1`) | Input Data Type | Data Load/Store Mode | Output Length | Description |
| --- | --- | --- | --- | --- | --- |
| 1 | 1024 | half | Developer-defined iteration offset | 1024 | Contiguously loads and stores 1021 elements; the last three elements remain zero. |
| 2 | 1024 | half | postupdate mode for iteration offsets | 1024 | Automatically updates the address after each load and store for contiguous access. |
| 3 | 1024 | half | Address register (add_reg) for iteration offsets | 1024 | Uses an address register to configure the offset of each load and store for contiguous access. |
| 4 | 1024 | half | DataBlock non-contiguous load | 512 | Uses a DataBlock stride of 2 for input: each complete 32B DataBlock is loaded and the next DataBlock is skipped; results are stored contiguously. |
| 5 | 1024 | half | Broadcast load | 1024 | Broadcasts the first half element in each 256B input-data segment to a vector register for subsequent computation. |
| 6 | 1024 | half | Upsample load | 2048 | Each iteration loads 64 half elements from each input, duplicates every element into two adjacent vector-register elements, performs addition, and then stores 128 half results contiguously. |

## Build and Run

### Configure Environment Variables

Configure environment variables according to the CANN toolkit installation in the current environment:

```bash
source ${install_path}/cann/set_env.sh
```

> **Note:** `${install_path}` is the CANN package installation directory. The default is `/usr/local/Ascend` for the root user and `${HOME}/Ascend` for non-root users.

### Run the Example

Run the following commands in the example root directory. `SCENARIO_NUM` can be 1 through 6 and defaults to 1.

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
| `SCENARIO_NUM` | `1`-`6` | Selects the aligned register load/store scenario. |
| `CMAKE_ASC_RUN_MODE` | `npu`, `sim` | Selects NPU execution or NPU simulation mode. |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | Specifies the NPU architecture for Ascend 950PR/Ascend 950DT. |

## Expected Result

When the output matches the golden data, the verification script prints:

```bash
test pass!
```
