# L2 Cache Splitting Sample

## Overview

This sample uses a fixed in-place addition workload to show how L2 Cache splitting affects data reuse across compute rounds.

The sample uses one 384MB GM data buffer and runs two rounds of `data = data + 1`. The two scenarios issue the same logical GM-to-UB copies, UB-to-GM copies, and Adds operations. The difference is the access order:

- Scenario 1 `no-split`: each round processes the whole 384MB range.
- Scenario 2 `l2-split`: the 384MB range is split into four 96MB L2 batches, and each batch finishes both rounds before the next batch starts.

## Supported Products And CANN Versions

| Product | CANN version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 training series products/Atlas A3 inference series products | >= CANN 9.0.0 |
| Atlas A2 training series products/Atlas A2 inference series products | >= CANN 9.0.0 |

## Directory Structure

```text
├── 10_add_l2_cache_split
│   ├── scripts
│   │   ├── gen_data.py               // Input and golden data generation script
│   │   └── verify_result.py          // Result verification script
│   ├── add_l2cache_nosplit.asc       // Scenario 1 kernel entry and host main
│   ├── add_l2cache_split.asc         // Scenario 2 kernel entry and host main
│   ├── CMakeLists.txt                // Build project file
│   ├── data_utils.h                  // Data read/write utilities
│   ├── l2_cache_kernel.h             // Fixed workload parameters and shared device logic
│   ├── l2_cache_runner.h             // Shared host runtime
│   ├── README.md                     // Chinese sample documentation
│   └── README_en.md                  // English sample documentation
```

## Sample Description

- Sample function:
  The sample runs two in-place addition rounds on input `data`. Each round adds 1 to every `half` element, so the final result is `input + 2`. `SCENARIO_NUM` selects the no-split or L2 Cache split scenario at build time.

- Sample specification:

  <table>
  <tr><th align="center">OpType</th><th colspan="4" align="center">AddL2CacheSplit</th></tr>
  <tr><td rowspan="2" align="center">Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">data</td><td align="center">[201326592]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Output</td><td align="center">data</td><td align="center">[201326592]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Kernel name</td><td colspan="4" align="center">l2_cache_nosplit</td></tr>
  <tr><td colspan="4" align="center">l2_cache_split</td></tr>
  </table>

- Sample parameters:

  | Parameter | Value | Description |
  |------|-----:|------|
  | `totalByteSize` | 384MB | Total input data size |
  | `L2_TILE_THRESHOLD` | 100MB | Effective L2 Cache capacity threshold for one batch |
  | `l2BatchCount` | 4 | Number of L2 batches in scenario 2 |
  | `l2BatchSizeBytes` | 96MB | Size of one L2 batch in scenario 2 |
  | `vectorCoreCount` | 20 | Number of Vector Cores |
  | `computeRoundCount` | 2 | Number of compute rounds |

- Sample scenarios:

  | `SCENARIO_NUM` | Scenario | Source file | Kernel entry | Output file |
  |------|------|------|------|------|
  | `1` | No L2 Cache split | `add_l2cache_nosplit.asc` | `l2_cache_nosplit` | `output/output.bin` |
  | `2` | L2 Cache split enabled | `add_l2cache_split.asc` | `l2_cache_split` | `output/output.bin` |

- Sample implementation:

  - Kernel implementation
    The shared device logic splits the data range by 32-byte data blocks and assigns them to 20 Vector Cores. Inside each core, the workload is processed by UB tiles with `DataCopy`, `Adds`, and writes data back to GM.

    Scenario 1 processes the whole `data[0, 384MB)` range in each round:

    ```text
    Round 0: data[0, 384MB)
    Round 1: data[0, 384MB)
    ```

    Scenario 2 processes one L2 batch at a time and finishes both rounds of the same batch before moving on:

    ```text
    B0: Round 0 -> Round 1
    B1: Round 0 -> Round 1
    B2: Round 0 -> Round 1
    B3: Round 0 -> Round 1
    ```

    The whole 384MB range exceeds the sample's 100MB effective capacity threshold, so early data from Round 0 in scenario 1 can be evicted by later data in the same scan. In scenario 2, each batch is 96MB, so repeated accesses within the same batch are more likely to hit L2 Cache.

  - Synchronization
    Scenario 1 calls `AscendC::SyncAll()` after each full-data round. Scenario 2 calls `AscendC::SyncAll()` after each L2-batch/round stage, ensuring all cores finish current UB-to-GM stores before the next stage starts.

  - Invocation
    The host code launches kernels with `<<<>>>`. The build selects the source file by `SCENARIO_NUM`, and the generated executable is always `demo`.

## Build And Run

Run the following steps from the sample root directory.

- Configure environment variables
  Configure environment variables according to the [CANN installation guide](../../../../docs/zh/quick_start.md#prepare&install).

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN installation directory. If no installation directory is specified, the default path is `/usr/local/Ascend`.

- Run the sample

  ```bash
  SCENARIO_NUM=2
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # Build the project, NPU mode by default
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # Generate test input and golden data
  ./demo                           # Run the sample
  python3 ../scripts/verify_result.py output/output.bin ./output/golden.bin -scenarioNum=$SCENARIO_NUM  # Verify the output result
  ```

  To use CPU debug or NPU simulation mode, add `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim`.

  Examples:

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # CPU debug mode
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU simulation mode
  ```

  > **Note:** Clear the CMake cache before switching run mode, architecture, or scenario. For example, run `rm CMakeCache.txt` in the `build` directory and configure again.

- Build options

  | Option | Value | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Project mode: NPU run, CPU debug, or NPU simulation. The `sim` mode supports full execution verification, but the 384MB workload takes a long time to simulate and simulation time is not used to evaluate L2 Cache performance benefits. |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture. `dav-2201` maps to Atlas A2 training series products/Atlas A2 inference series products and Atlas A3 training series products/Atlas A3 inference series products, and `dav-3510` maps to Ascend 950PR/Ascend 950DT. |
  | `SCENARIO_NUM` | `1`, `2` | Scenario number: 1=no L2 Cache split, 2=L2 Cache split enabled. |

- Execution result

  Each scenario verifies its output after execution. The following output indicates that the result is correct:

  ```bash
  test pass!
  ```

  If runtime verification fails, `test failed!` is printed together with the first mismatched element index, input value, actual output value, and expected output value. If `verify_result.py` fails, it prints the first mismatched element index, actual output value, and expected output value.

  Output files:

  ```text
  output/output.bin
  output/golden.bin
  ```

## Profile Collection And Analysis

This sample is used to observe how L2 Cache splitting affects the data load path. Launch the two scenarios as separate processes so the first kernel's cache state does not affect the second.

```bash
SCENARIO_NUM=1
mkdir -p build && cd build
cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j
python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM
msopprof --output=./prof_384m_inplace_no_split_1 ./demo

SCENARIO_NUM=2
cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j
python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM
msopprof --output=./prof_384m_inplace_l2_split_1 ./demo
```

When analyzing results, first confirm that `GM_to_UB_datas(KB)` in `Memory.csv` is close between the two scenarios. Then compare the L2 Cache read hit rate in `L2Cache.csv`, MTE2 time in `PipeUtilization.csv`, and `Task Duration(us)` in `OpBasicInfo.csv`.

Common metrics:

| Field | Meaning | Source |
|------|------|---------|
| `Task Duration(us)` | Kernel task duration | `OpBasicInfo.csv` |
| `aiv_mte2_time(us)` | MTE2 load time | `PipeUtilization.csv` |
| `GM_to_UB_datas(KB)` | Logical GM-to-UB traffic | `Memory.csv` |
| `read_main_memory_datas(KB)` | Main-memory read counter. Semantics can vary by architecture and profiler version. | `Memory.csv` |
| `aiv_read_hit_rate(%)` | L2 Cache read hit rate | `L2Cache.csv` |

`aiv_gm_to_ub_bw(GB/s)` is effective bandwidth, not GM traffic. Performance benefits should be attributed by checking logical traffic fairness, L2 Cache read hit rate, MTE2 time, and total task duration together.
