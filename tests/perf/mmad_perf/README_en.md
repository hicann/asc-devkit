# Mmad (Cube Matrix Multiplication) Performance Test Example

## Overview

This example tests the performance of matrix multiply-add operations using Mmad series interfaces. It covers the computation path where matrices A and B are multiplied by the Cube computation unit from the L0A Buffer/L0B Buffer and the result is written to the L0C Buffer. This is a non-functional performance test. It does not verify computation results and only collects Cube computation latency.

## Supported Products and CANN Versions

| Product | Architecture Code | CANN Version |
|---------|-------------------|--------------|
| Ascend 950PR&950DT products | dav-3510 | >= CANN 9.1.0 |
| Atlas A3 Training/Inference Series | dav-2201 | >= CANN 9.0.0 |
| Atlas A2 Training/Inference Series | dav-2201 | >= CANN 9.0.0 |

## Directory Structure

```text
├── mmad_perf
│   ├── CMakeLists.txt                    // Build configuration file
│   ├── mmad_perf.asc                     // Mmad performance test implementation and entry point
│   ├── perf.sh                           // Performance test script
│   ├── generate_roofline.py              // Roofline generation script
│   ├── README.md                         // Example documentation
```

## Example Description

This example uses the runtime parameter `SCENARIO_NUM` to select different data types and computation interfaces. Matrix dimensions are passed at runtime through `./demo SCENARIO_NUM M K N`. The kernel function name is `mmad_perf_custom`.

The supported test scenarios vary by platform architecture:

| Platform Architecture | Supported Scenarios | Description |
|-----------------------|---------------------|-------------|
| Atlas A3/A2 Training/Inference Platform (dav-2201) | 1-4 | Standard Mmad for b8, b16, b32, and structured sparse b8 |
| Ascend 950PR&950DT products Platform (dav-3510) | 11-15 | Standard Mmad for b8, b16, b32, and MX quantization mxfp8, mxfp4 |

### Atlas A3/A2 Training/Inference Platform Scenarios

| SCENARIO_NUM | Interface | Left Matrix A | Right Matrix B | Result Matrix C | Description |
|--------------|-----------|---------------|----------------|-----------------|-------------|
| 1 | `Mmad`           | int8_t | int8_t | int32_t | b8 standard matrix multiplication |
| 2 | `Mmad`           | half   | half   | float   | b16 standard matrix multiplication |
| 3 | `Mmad`           | float  | float  | float   | b32 standard matrix multiplication |
| 4 | `MmadWithSparse` | int8_t | int8_t | int32_t | b8 4:2 structured sparse matrix multiplication |

### Ascend 950PR&950DT products Platform Scenarios

| SCENARIO_NUM | Interface | Left Matrix A | Right Matrix B | Scale | Result Matrix C | Description |
|--------------|-----------|---------------|----------------|-------|-----------------|-------------|
| 11 | `Mmad`   | int8_t       | int8_t       | -          | int32_t | b8 standard matrix multiplication |
| 12 | `Mmad`   | half         | half         | -          | float   | b16 standard matrix multiplication |
| 13 | `Mmad`   | float        | float        | -          | float   | b32 standard matrix multiplication |
| 14 | `MmadMx` | fp8_e4m3fn_t | fp8_e4m3fn_t | fp8_e8m0_t | float   | mxfp8 quantized matrix multiplication |
| 15 | `MmadMx` | fp4x2_e2m1_t | fp4x2_e2m1_t | fp8_e8m0_t | float   | mxfp4 quantized matrix multiplication |

None of the scenarios include bias.

## Build and Run

Run the following steps from the root directory of this example to build and run it.

### Configure Environment Variables

Configure environment variables based on the [installation method](../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit on your system.

```bash
source ${install_path}/cann/set_env.sh
```

`${install_path}` is the CANN package installation directory. If no installation directory is specified, the default installation path is `/usr/local/Ascend`. After running `source`, verify that the CANN package path is ready by running `echo ${ASCEND_HOME_PATH}`. During `perf.sh` build, `ASC_DIR` is derived from this path by default.

To run this example in a simulation environment, configure the simulator dynamic library path. Different architectures correspond to different simulator models:

```bash
# dav-2201 (Atlas A3/A2 Training/Inference Platform)
export LD_LIBRARY_PATH=${ASCEND_HOME_PATH}/tools/simulator/Ascend910B3/lib:$LD_LIBRARY_PATH

# dav-3510 (Ascend 950PR&950DT products Platform)
export LD_LIBRARY_PATH=${ASCEND_HOME_PATH}/tools/simulator/Ascend950PR_9589/lib:$LD_LIBRARY_PATH
```

`perf.sh` automatically selects the corresponding model and configures this path based on the scenario. When running `msopprof simulator` manually, export this path yourself.

### Build the Example

Build for Atlas A3/A2 Training/Inference Platform (dav-2201):

```bash
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DCMAKE_ASC_RUN_MODE=sim ..
make -j
cd ..
```

Build for Ascend 950PR&950DT products Platform (dav-3510):

```bash
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DCMAKE_ASC_RUN_MODE=sim ..
make -j
cd ..
```

### Run the Example

The runtime parameter order is `SCENARIO_NUM M K N`:

```bash
# Atlas A3/A2 Training/Inference Platform examples (scenarios 1-4)
./build/demo 1 128 128 128
./build/demo 4 128 256 128

# Ascend 950PR&950DT products Platform examples (scenarios 11-15)
./build/demo 11 128 128 128
./build/demo 14 128 512 128
```

| Parameter | Description |
|-----------|-------------|
| `SCENARIO_NUM` | Test scenario number. Use 1-4 for Atlas A3/A2 Training/Inference Platform; use 11-15 for Ascend 950PR&950DT products Platform |
| `M` | Number of rows in left matrix A |
| `K` | Number of columns in left matrix A, which equals the number of rows in right matrix B |
| `N` | Number of columns in right matrix B |

Matrix dimensions must meet alignment requirements: M and N must be multiples of 16, and K must be a multiple of 64. For MX scenarios, K must be a multiple of 64. For consistency, K should be a multiple of 64 in all scenarios.

## Collecting Performance Data

Use the `msopprof simulator` tool to collect detailed performance data in the simulation environment:

```bash
msopprof simulator build/demo 1 128 128 128
```

> 💡 The `msopprof` tool requires CANN Commercial or Community Edition. For details, refer to the [msOpProf Tool Installation Guide](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/install_guide/msopprof_install_guide.md).

After the command completes, a folder named `OPPROF_{timestamp}_XXX` is generated in the default directory. The simulation performance data folder structure is as follows:

```text
├── dump                                          # Raw performance data
└── simulator
    ├── trace.json                                # Overall timing trace
    ├── visualize_data.bin                        # MindStudio Insight presentation file
    └── core0.cubecore0
        ├── core0.cubecore0_instr_exe.csv         # Cube core instruction-by-instruction execution details (including cycles and running_time)
        ├── core0.cubecore0_code_exe.csv          # Execution details aggregated by code line
        └── trace.json                            # Cube core instruction-by-instruction timing trace (including high-precision dur)
```

This example focuses on MMAD instruction performance data. View the specific performance data results as follows:

```bash
cat ./OPPROF_*/simulator/core0.cubecore0/core0.cubecore0_instr_exe.csv
```

Key metrics to monitor:

| Metric | Source | Description |
|--------|--------|-------------|
| `dur(us)` | `dur` field of `MMAD` events in `core0.cubecore0/trace.json` | MMAD instruction duration (high precision) |
| `cycles` | `cycles` column of the `MMAD` row in `core0.cubecore0_instr_exe.csv` | MMAD instruction cycle count |

## Performance Test Script

`perf.sh` performs batch simulation builds, runs `msopprof simulator`, extracts the duration and cycle count of MMAD instructions, and generates a CSV summary.

```bash
# View help
./perf.sh --help

# Test scenario 1, using dav-2201 by default
./perf.sh 1

# Test scenario 11, using dav-3510 by default
./perf.sh 11

# Explicitly specify the platform. The platform must match the scenario; otherwise, an error is reported
./perf.sh 1 dav-2201
./perf.sh 14 dav-3510
```

Built-in default shape sequences:

| Scenario | Data Type | Default Shape Sequence (M K N) |
|----------|-----------|-------------------------------|
| 1 | b8 (dav-2201) | (32,32,32) (64,64,64) (128,128,128) (128,256,128) (128,512,128) |
| 2 | b16 (dav-2201) | (32,32,32) (64,64,64) (64,128,64) (128,128,128) (128,256,128) |
| 3 | b32 (dav-2201) | (32,32,32) (64,64,64) (64,128,64) (128,128,128) |
| 4 | sparse b8 (dav-2201) | (64,64,64) (64,128,64) (128,128,128) (128,256,128) (128,512,128) |
| 11 | b8 (dav-3510) | (32,32,32) (64,64,64) (128,128,128) (128,256,128) (256,256,256) |
| 12 | b16 (dav-3510) | (32,32,32) (64,64,64) (64,128,64) (128,128,128) (256,128,256) |
| 13 | b32 (dav-3510) | (32,32,32) (64,64,64) (64,128,64) (128,128,128) |
| 14 | mxfp8 (dav-3510) | (64,64,64) (64,128,64) (128,128,128) (128,256,128) (256,256,256) |
| 15 | mxfp4 (dav-3510) | (64,64,64) (128,128,128) (128,256,128) (128,512,128) (256,512,256) |

To test specific shapes, run `./build/demo SCENARIO_NUM M K N` directly.

After testing, results are saved to `perf_data_${timestamp}_scenario${SCENARIO}/perf_result_scenario${SCENARIO}.csv`. Raw `msopprof` data is saved in the `test_${id}_${M}_${K}_${N}` subdirectory under the same directory.

## Performance Metrics

`perf.sh` extracts the duration and cycle count of MMAD instructions from the simulation output `core0.cubecore0/` and writes them to a CSV. The columns in the CSV are as follows:

| Column | Source | Description |
|--------|--------|-------------|
| `MMAD_Dur(us)` | `dur` field of `MMAD` events in `core0.cubecore0/trace.json` | MMAD instruction duration (high precision, measured) |
| `Cycles` | `cycles` column of the `MMAD` row in `core0.cubecore0_instr_exe.csv` | MMAD instruction cycle count (measured) |

### Performance Metric Calculation Methods

In the simulation output, `Cycles` is the measured cycle count for MMAD instructions, and `MMAD_Dur(us)` is the corresponding duration. `perf.sh` reads these two columns directly. Measured performance and compute utilization are derived by `generate_roofline.py` from `Cycles` based on hardware parallelism. No clock frequency conversion is needed.

#### Mmad Performance Calculation

Mmad performance is expressed in `MAC/cycle`. The computation volume is counted as MAC operations `M×N×K` (one multiply-add counts as 1 MAC). The hardware parallelism (element block `cube_m×cube_n×cube_k` processed per cycle) and equivalent peak compute for each data type are as follows:

| Architecture | Data Type | cube_m | cube_n | cube_k | k_divisor | Equivalent Peak Compute (MAC/cycle) |
|--------------|-----------|--------|--------|--------|-----------|--------------------------------------|
| dav-2201 | b8 | 16 | 16 | 32 | 1 | 8192 |
| dav-2201 | b16 | 16 | 16 | 16 | 1 | 4096 |
| dav-2201 | b32 | 16 | 16 | 4  | 1 | 1024 |
| dav-2201 | sparse b8 | 16 | 16 | 32 | 2 | 16384 |
| dav-3510 | b8 | 16 | 16 | 32 | 1 | 8192 |
| dav-3510 | b16 | 16 | 16 | 16 | 1 | 4096 |
| dav-3510 | b32 | 16 | 16 | 1  | 1 | 256 |
| dav-3510 | mxfp8 | 16 | 16 | 32 | 1 | 8192 |
| dav-3510 | mxfp4 | 16 | 16 | 64 | 1 | 16384 |

`k_divisor` is the K-dimension effective reduction factor. In sparse 4:2 structured sparsity, the right matrix is densified to `[K/2, N]`, and the hardware traverses only `K/2` fractals in the K direction. Therefore, the computation cycle is halved and the equivalent peak compute is doubled (`16×16×32×2=16384`). For standard scenarios, `k_divisor=1`.

Theoretical and measured calculation formulas (first-instruction overhead is treated as 0):

```text
Computation cycles = ceil(M/cube_m) × ceil(N/cube_n) × ceil(K / (cube_k × k_divisor))
Theoretical total cycles = Computation cycles                   (first-instruction overhead is 0)
Theoretical time (us) = Theoretical total cycles / Frequency(MHz)
Equivalent peak = cube_m × cube_n × cube_k × k_divisor     (MAC/cycle)
Theoretical performance = M×N×K / Theoretical total cycles           (MAC/cycle, equals equivalent peak)

Measured cycles = Cycles (from MMAD in simulation instr_exe.csv, read directly)
Measured performance = M×N×K / Measured cycles             (MAC/cycle)
Compute utilization = Measured performance / Equivalent peak × 100%
```

For example, scenario 1 (dav-2201 b8, parallelism 16×16×32, k_divisor=1) with shape `[128, 128, 128]`:

```text
Computation cycles = ceil(128/16) × ceil(128/16) × ceil(128/32) = 8 × 8 × 4 = 256
Theoretical total cycles = 256                              (first-instruction overhead is 0)
Theoretical time = 256 / 1800 = 0.1422 us
Theoretical performance = 128×128×128 / 256 = 8192 MAC/cycle (equals peak 8192)
```

The platform clock frequency is set automatically by `perf.sh` based on the scenario:

| Platform | Architecture Code | Clock Frequency | Applicable Scenarios |
|----------|-------------------|-----------------|----------------------|
| Atlas A3/A2 Training/Inference Platform | dav-2201 | 1800 MHz | 1-4 |
| Ascend 950PR&950DT products Platform | dav-3510 | 1650 MHz | 11-15 |

## Roofline Analysis

This example provides `generate_roofline.py`, which generates ASCII reports and charts from the CSV output produced by `perf.sh`.

### Python Package Dependencies

`generate_roofline.py` uses Python standard libraries to read CSV and generate ASCII reports. To generate PNG/PDF charts, install `matplotlib` and `numpy`.

```bash
python3 -m pip install --user matplotlib numpy
```

If these dependencies are not installed, the script still generates `.txt` ASCII analysis reports but skips chart generation.

```bash
# Automatically find the latest perf_data directory results
python3 generate_roofline.py

# Specify a CSV file
python3 generate_roofline.py --csv perf_data_xxx_scenario1/perf_result_scenario1.csv
```

The default output file prefix is `${perf_data directory name}_cube_roofline`. The generated `.txt` file contains detailed ASCII analysis. After installing `matplotlib`, a `.png` chart is also generated.

### Chart Example

The following is a Roofline chart example generated for scenario 1:

![Scenario 1 Roofline Chart](figures/perf_data_scenario1_roofline.png)


## Notes

1. Scenario numbers must match the platform: use scenarios 1-4 for dav-2201 and scenarios 11-15 for dav-3510. `perf.sh` validates the match and reports errors for mismatches.
2. Scenario 4 (MmadWithSparse) is supported only on dav-2201. Scenarios 14 and 15 (MmadMx) are supported only on dav-3510.
3. MmadMx constraints: K must be a multiple of 64. The start addresses of A and B must be aligned to 1024 bytes for fp8 scenarios and 512 bytes for fp4 scenarios.
4. This is a pure performance test and does not verify computation results. The kernel function does not initialize data in the L0 Buffer. The execution and latency of Mmad instructions depend only on the shape and architecture, not on data content.
