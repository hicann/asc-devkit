# Load and Store Example

## Overview

This example demonstrates how the `Load` and `Store` convenience interfaces support `RegTraitNumOne` and `RegTraitNumTwo`. Select a scenario using the CMake build parameter `SCENARIO_NUM`. The scenarios cover both `Store` overloads, with and without `count`.

| SCENARIO_NUM | RegTensor Type | Load Interface | Store Interface | Transfer Size per Call | Number of Calls | Total Transfer Size |
| --- | --- | --- | --- | --- | --- | --- |
| 1 | `RegTensor<uint64_t, RegTraitNumOne>` | `Load(srcReg, srcAddr)` | `Store(dstAddr, srcReg)` | 32 `uint64_t` elements (256 bytes) | 2 | 64 `uint64_t` elements (512 bytes) |
| 2 | `RegTensor<uint64_t, RegTraitNumOne>` | `Load(srcReg, srcAddr)` | `Store(dstAddr, srcReg, count)`, with `count` set to 32 | 32 `uint64_t` elements (256 bytes) | 2 | 64 `uint64_t` elements (512 bytes) |
| 3 | `RegTensor<uint64_t, RegTraitNumTwo>` | `Load(srcReg, srcAddr)` | `Store(dstAddr, srcReg)` | 64 `uint64_t` elements (512 bytes) | 1 | 64 `uint64_t` elements (512 bytes) |
| 4 | `RegTensor<uint64_t, RegTraitNumTwo>` | `Load(srcReg, srcAddr)` | `Store(dstAddr, srcReg, count)`, with `count` set to 64 | 64 `uint64_t` elements (512 bytes) | 1 | 64 `uint64_t` elements (512 bytes) |

## Supported Products and CANN Versions

| Product | CANN Version |
| --- | --- |
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```text
reg_load_store/
├── scripts/
│   └── gen_data.py        // Generates input and golden data
├── CMakeLists.txt         // Build configuration
├── data_utils.h           // File input and output helpers
├── load_store.asc         // Ascend C implementation and entry point
├── README.md              // Chinese documentation
└── README_en.md           // English documentation
```

## Example Description

The input and output buffers each contain 68 `uint64_t` elements: 64 elements to be transferred, one leading element used to create an unaligned address, and three trailing padding elements that keep the total buffer size 32-byte aligned. The example transfers and verifies the data as follows:

1. Copies the input data from GM to UB and initializes the output UB to 0.
2. Offsets the input and output UB addresses by one `uint64_t` element (8 bytes) to make the start addresses not 32-byte aligned.
3. Calls `Load` to transfer data from UB to a RegTensor.
4. Calls the corresponding SIMD VF based on `SCENARIO_NUM` and transfers the data from the RegTensor back to UB.
5. Copies the result from UB to GM and compares it with the golden data.

All four scenarios transfer 512 bytes in total. The `RegTraitNumOne` scenarios transfer one VL (256 bytes) per call and iterate twice. The `RegTraitNumTwo` scenarios transfer two VLs (512 bytes) in one call. `Load` and `Store` perform the preprocessing and postprocessing required for the unaligned transfers.

## Build and Run

- Configure environment variables.

  Configure the environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. If no installation directory is specified, the default installation directory is `/usr/local/Ascend`.

- Run the example.

  Run the following commands in the example directory. NPU mode is used by default:

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # Build the project in the default NPU mode
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input data
  ./demo                           # Run the compiled executable
  ```

  To use CPU debugging or NPU simulation mode, add `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim`, respectively.

  Examples:
  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # CPU debugging mode
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Before switching the run mode or scenario, delete `CMakeCache.txt` from the `build` directory, and then run CMake and rebuild the example.

- Build options.

  | Option | Values | Description |
  | --- | --- | --- |
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | NPU execution, CPU debugging, and NPU simulation modes, respectively. |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | `dav-3510` corresponds to Ascend 950PR/Ascend 950DT. |
  | `SCENARIO_NUM` | `1`, `2`, `3`, `4` | Scenario number. See the scenario table in the overview. |

- Execution result.

  The following output indicates that the precision comparison is successful.
  ```bash
  test pass!
  ```
