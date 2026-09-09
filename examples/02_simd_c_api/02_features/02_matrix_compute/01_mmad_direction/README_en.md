# Mmad Computation Direction Optimization Sample

## Overview

This sample demonstrates the effect of the Mmad computation direction on performance when UnitFlag is enabled. Better performance can be obtained when the result fractal production order matches the output data-movement order.

With UnitFlag enabled, when the Mmad result fractal production order matches the read order of the output data-movement instruction, the output data-movement instruction can read generated result fractals earlier. Computation and output data movement can then overlap at fractal granularity. When the orders do not match, the output data-movement instruction must wait for a result fractal that has not yet been generated, which creates additional pipeline wait overhead. The following figure illustrates computation and output data movement both writing and reading in the M direction:

![Computation and output data movement both write and read in the M direction](figures/mmad_fixpipe_direction.png)

The sample includes five scenario groups. Every group provides a recommended example and a counterexample that changes only the Mmad computation direction configuration:

1. float result with Nz2Nz output: `asc_set_mmad_direction_m()` is recommended.
2. B4 quantization with Nz2Nz output and hardware-enabled ChannelMerge: `asc_set_mmad_direction_n()` is recommended.
3. float result with on-the-fly Nz2ND format conversion: `asc_set_mmad_direction_n()` is recommended.
4. float result with Nz2DN output: `asc_set_mmad_direction_m()` is recommended.
5. B8 quantization with Nz2Nz output and hardware-enabled ChannelMerge: `asc_set_mmad_direction_n()` is recommended.

## Supported Products and CANN Versions

**Table 1** Supported Products and CANN Versions

| Product | CANN Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## Directory Structure

```text
01_mmad_direction
├── figures
│   └── mmad_fixpipe_direction.png    // Diagram
├── scripts
│   ├── gen_data.py                   // Generates random input data and golden data
│   └── verify_result.py              // Verifies output data
├── CMakeLists.txt                    // Build configuration file
├── data_utils.h                      // Binary data read and write functions
├── mmad_direction.asc                // Kernel and host implementation
├── README.md
└── README_en.md
```

## Sample Description

The sample computes `C = A x B`. The data types of A and B are both half, and the data type of C in L0C Buffer is float.

**Table 2** Input and Output Data Specifications

| Data | Input/Output | Data Type | Dimensions | Data Layout |
| --- | --- | --- | --- | --- |
| A | Input | half | `[M, K]` | ND |
| B | Input | half | `[N, K]` | ND |
| C, scenarios 1/2 | Output | float | `[M, N]` | Nz |
| C, scenarios 3/4 | Output | int4b_t | `[M, N]` | Nz |
| C, scenarios 5/6 | Output | float | `[M, N]` | ND |
| C, scenarios 7/8 | Output | float | `[M, N]` | DN |
| C, scenarios 9/10 | Output | int8_t | `[M, N]` | Nz |

The Kernel executes in the following order:

1. Call `asc_set_gm2l1_nz_para` and `asc_copy_gm2l1_nd2nz` to move A and B from GM to L1 Buffer and convert them to Nz.
2. Call `asc_copy_l12l0a` and `asc_copy_l12l0b` to load A and B from L1 Buffer to L0A Buffer and L0B Buffer.
3. Preconfigure `asc_set_l0c_copy_prequant` or `asc_set_l0c_copy_nz_para` according to the output scenario.
4. Call the direction configuration API before `asc_mmad`, and enable UnitFlag with `asc_unit_flag_mode::ENABLE_UPDATE`.
5. Call `asc_mmad` once to complete matrix multiplication, then call `asc_copy_l0c2gm` once with `asc_unit_flag_mode::ENABLE_UPDATE` to enable UnitFlag and move out the result.

### Scenario Configuration

The `SCENARIO_NUM` build parameter selects a recommended example or counterexample. Within each scenario group, every configuration except the Mmad computation direction is identical.

**Table 3** Scenario Information

| `SCENARIO_NUM` | Type | Output Scenario | `[M, N, K]` | Mmad Direction | Description |
| ---: | --- | --- | --- | --- | --- |
| 1 | Recommended | float Nz2Nz + UnitFlag | `[256, 256, 128]` | `asc_set_mmad_direction_m()` | Normal Nz output reads in the M direction. |
| 2 | Counterexample | float Nz2Nz + UnitFlag | `[256, 256, 128]` | `asc_set_mmad_direction_n()` | The production order does not match the output order. |
| 3 | Recommended | B4 ChannelMerge + UnitFlag | `[192, 64, 128]` | `asc_set_mmad_direction_n()` | ChannelMerge reads in the N direction. |
| 4 | Counterexample | B4 ChannelMerge + UnitFlag | `[192, 64, 128]` | `asc_set_mmad_direction_m()` | The production order does not match the output order. |
| 5 | Recommended | float Nz2ND + UnitFlag | `[256, 256, 128]` | `asc_set_mmad_direction_n()` | Nz2ND reads in the N direction. |
| 6 | Counterexample | float Nz2ND + UnitFlag | `[256, 256, 128]` | `asc_set_mmad_direction_m()` | The production order does not match the output order. |
| 7 | Recommended | float Nz2DN + UnitFlag | `[256, 256, 128]` | `asc_set_mmad_direction_m()` | Nz2DN reads in the M direction. |
| 8 | Counterexample | float Nz2DN + UnitFlag | `[256, 256, 128]` | `asc_set_mmad_direction_n()` | The production order does not match the output order. |
| 9 | Recommended | B8 ChannelMerge + UnitFlag | `[256, 256, 64]` | `asc_set_mmad_direction_n()` | ChannelMerge reads in the N direction. |
| 10 | Counterexample | B8 ChannelMerge + UnitFlag | `[256, 256, 64]` | `asc_set_mmad_direction_m()` | The production order does not match the output order. |

### Scenarios 1/2: float Nz Output

Scenarios 1 and 2 use float as the destination type, the quantization mode is `NoQuant`, and Nz2ND and Nz2DN are not enabled. The result therefore retains the Nz layout and ChannelMerge is not enabled. Normal Nz output reads L0C Buffer in the M direction. Thus, scenario 1 uses `asc_set_mmad_direction_m()`, while scenario 2 uses the N direction as a counterexample.

### Scenarios 3/4: B4 ChannelMerge

Scenarios 3 and 4 use `QF322S4_PRE` to quantize float to `int4b_t` and retain Nz2NZ output. Hardware automatically enables ChannelMerge and merges four consecutive 16 x 16 fractals in the N direction into one 16 x 64 fractal. ChannelMerge reads L0C Buffer in the N direction. Thus, scenario 3 uses `asc_set_mmad_direction_n()`, while scenario 4 uses the M direction as a counterexample.

### Scenarios 5/6: float Nz2ND

Scenarios 5 and 6 configure on-the-fly format conversion parameters through `asc_set_l0c_copy_nz_para` and enable Nz2ND in `asc_copy_l0c2gm`. Nz2ND reads L0C Buffer in the N direction. Thus, scenario 5 uses `asc_set_mmad_direction_n()`, while scenario 6 uses the M direction as a counterexample.

### Scenarios 7/8: float Nz2DN

Scenarios 7 and 8 configure on-the-fly format conversion parameters through `asc_set_l0c_copy_nz_para` and enable Nz2DN in `asc_copy_l0c2gm`. Nz2DN reads L0C Buffer in the M direction. Thus, scenario 7 uses `asc_set_mmad_direction_m()`, while scenario 8 uses the N direction as a counterexample.

### Scenarios 9/10: B8 ChannelMerge

Scenarios 9 and 10 use `QF322B8_PRE` to quantize float to `int8_t` and retain Nz2NZ output. Hardware automatically enables ChannelMerge and merges two consecutive 16 x 16 fractals in the N direction into one 16 x 32 fractal. ChannelMerge reads L0C Buffer in the N direction. Thus, scenario 9 uses `asc_set_mmad_direction_n()`, while scenario 10 uses the M direction as a counterexample.

## Performance Comparison

**Table 4** Test Configuration

| Configuration Item | Value |
| --- | --- |
| Block count | 1 |
| Kernel pipeline repetitions | 50 |
| Mean source | Mean sample end-to-end execution time collected by the msOpProf tool |
| Performance improvement | `(counterexample mean - recommended mean) / counterexample mean x 100%` |

**Table 5** Multiple-Run Means of Recommended Examples and Counterexamples

| Scenario | Recommended Mean (us) | Counterexample Mean (us) | Performance Improvement |
| --- | ---: | ---: | ---: |
| float Nz2Nz + UnitFlag | 61.295 +/- 0.084 | 61.948 +/- 0.141 | **1.055%** |
| B4 ChannelMerge + UnitFlag | 13.154 +/- 0.053 | 15.149 +/- 0.043 | **13.172%** |
| float Nz2ND + UnitFlag | 61.935 +/- 0.113 | 112.891 +/- 0.056 | **45.137%** |
| float Nz2DN + UnitFlag | 60.937 +/- 0.170 | 61.653 +/- 0.191 | **1.162%** |
| B8 ChannelMerge + UnitFlag | 60.909 +/- 0.157 | 61.174 +/- 0.112 | **0.435%** |

Performance gains depend on data volume, shape, partitioning strategy, and the complete operator pipeline. The percentages in Table 5 cannot be directly extrapolated to other scenarios; they only demonstrate the relationship between computation direction and performance optimization.

### Performance Test Method

### msOpProf Tool

msOpProf can collect single-operator performance data on real NPUs or in a simulation environment. After the command completes, an `OPPROF_{timestamp}_*` directory is generated, containing:

- `OpBasicInfo.csv`, which records basic Task information and end-to-end execution time.
- `PipeUtilization.csv`, which records the duration and utilization of Cube, Scalar, MTE, and Fixpipe pipelines.
- `Memory.csv` and `MemoryL0.csv`, which record access metrics for each memory level.

Use the following procedure to collect Kernel performance data for this sample:

```bash
SCENARIO_NUM=1
rm -rf build
mkdir build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..
make -j
python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM
msopprof ./c_api_mmad_direction_example
python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM \
    output/output.bin output/golden.bin
```

## Build and Run

Configure CANN environment variables:

```bash
source ${install_path}/cann/set_env.sh
```

> **Note:** `${install_path}` is the CANN package installation directory. The default is `/usr/local/Ascend` for the root user and `${HOME}/Ascend` for non-root users.

Build and run a selected scenario in the sample directory:

```bash
SCENARIO_NUM=1
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..
make -j
python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM
./c_api_mmad_direction_example
python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM \
    output/output.bin output/golden.bin
```

Clear the CMake cache in the `build` directory before changing scenarios.

**Table 6** Build Options

| Build Option | Supported Values | Description |
| --- | --- | --- |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture. |
| `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | NPU execution or NPU simulation; CPU debugging is not supported. |
| `SCENARIO_NUM` | `1` to `10`, default: `1` | Selects a scenario in Table 3. |

Successful result verification prints:

```text
test pass!
```
