# Fixpipe (L0C Egress) Performance Test Example

## Overview

This example tests the performance of the matrix computation egress path. It covers the data path that moves Cube computation results from the L0C Buffer to the L1 Buffer or Unified Buffer (UB).

This is a non-functional performance test. It does not verify computation results and only collects the Fixpipe egress latency.

## Supported Products and CANN Versions

| Product | Architecture Code | CANN Version |
|---------|-------------------|--------------|
| Ascend 950PR&950DT products | dav-3510 | >= CANN 9.1.0 |
| Atlas A3 Training/Inference Series | dav-2201 | >= CANN 9.0.0 |
| Atlas A2 Training/Inference Series | dav-2201 | >= CANN 9.0.0 |

## Directory Structure

```text
├── fixpipe_perf
│   ├── CMakeLists.txt          // Build configuration file
│   ├── fixpipe_perf.asc        // Fixpipe egress performance test implementation and entry point
│   ├── perf.sh                 // Performance test script
│   ├── generate_roofline.py    // Roofline generation script
│   ├── README.md               // Example documentation
```

## Example Description

This example uses the runtime parameter `SCENARIO_NUM` to select different egress paths and data types. Matrix dimensions are passed at runtime through `./demo SCENARIO_NUM M K N`.

The two egress paths use different interfaces:

| Path | Interface | Header Path | Supported Architecture |
|------|-----------|-------------|------------------------|
| L0C Buffer to L1 Buffer | `DataCopy` | `basic_api/kernel_operator_data_copy_intf.h` | dav-2201, dav-3510 |
| L0C Buffer to UB | `Fixpipe` | `basic_api/kernel_operator_fixpipe_intf.h` | dav-3510 only |

The supported test scenarios vary by platform architecture:

### Atlas A3/A2 Training/Inference Platform Scenarios

| SCENARIO_NUM | Input Data Type | Data Source | Execution Path | Description | Theoretical Bandwidth (Byte/cycle) | Bandwidth Latency (cycle) |
|--------------|-----------------|-------------|----------------|-------------|-----------------------------------|---------------------------|
| 1 | float | L0C Buffer | L0C Buffer -> L1 Buffer | DataCopy egress with inline F322F16 conversion to half | 128 | 20 |
| 2 | float | L0C Buffer | L0C Buffer -> L1 Buffer | DataCopy egress with inline QF322B8_PRE quantization to int8_t | 64 | 20 |

### Ascend 950PR&950DT products Platform Scenarios

| SCENARIO_NUM | Input Data Type | Data Source | Execution Path | Description | Theoretical Bandwidth (Byte/cycle) | Bandwidth Latency (cycle) |
|--------------|-----------------|-------------|----------------|-------------|-----------------------------------|---------------------------|
| 11 | float | L0C Buffer | L0C Buffer -> L1 Buffer | DataCopy egress with inline F322F16 conversion to half | 128 | 26 |
| 12 | float | L0C Buffer | L0C Buffer -> L1 Buffer | DataCopy egress with inline QF322B8_PRE quantization to int8_t | 64 | 26 |
| 13 | float | L0C Buffer | L0C Buffer -> UB | Fixpipe egress for float, non-dual-target mode | 128 | 26 |
| 14 | float | L0C Buffer | L0C Buffer -> UB | Fixpipe egress for float, dual-target mode split along M dimension | 256 | 26 |

> Theoretical bandwidth is calculated based on hardware egress parallelism: the egress unit processes 64 output elements per cycle, so theoretical bandwidth = 64 × sizeof(destination data type) (Byte/cycle). Dual-target mode splits work across two sub-cores in parallel, effectively doubling the parallelism. The L0C Buffer to UB path is supported only on dav-3510 (scenarios 13 and 14). Scenario 14 splits the M×N matrix in the L0C Buffer along the M dimension into two halves and writes them simultaneously to the UB of two Vector cores.

## Build and Run

Run the following steps from the root directory of this example to build and run it.

### Configure Environment Variables

Configure environment variables based on the [installation method](../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit on your system.

```bash
source ${install_path}/cann/set_env.sh
```

`${install_path}` is the CANN package installation directory. If no installation directory is specified, the default installation path is `/usr/local/Ascend`.

### Build the Example

Build for Atlas A3/A2 Training/Inference Platform (dav-2201):

```bash
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..
make -j
cd ..
```

Build for Ascend 950PR&950DT products Platform (dav-3510):

```bash
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..
make -j
cd ..
```

### Run the Example

The runtime parameter order is `SCENARIO_NUM M K N`:

```bash
# Atlas A3/A2 Training/Inference Platform examples (scenarios 1, 2)
./build/demo 1 128 64 128
./build/demo 2 128 64 128

# Ascend 950PR&950DT products Platform examples (scenarios 11-14)
./build/demo 11 128 64 128
./build/demo 12 128 64 128
./build/demo 13 128 64 128
./build/demo 14 128 64 128
```

| Parameter | Description |
|-----------|-------------|
| `SCENARIO_NUM` | Test scenario number. Use 1, 2 for Atlas A3/A2 Training/Inference Platform; use 11-14 for Ascend 950PR&950DT products Platform |
| `M` | Number of matrix rows |
| `K` | Number of columns in matrix A (number of rows in matrix B) |
| `N` | Number of matrix columns |

Matrix dimensions must meet alignment requirements: M and N must be multiples of 16. For dual-target mode split along the M dimension, M must be a multiple of 2. For split along the N dimension, N must be a multiple of 32.

## Collecting Performance Data

Use the `msopprof` tool to collect detailed performance data:

```bash
msopprof build/demo 1 128 64 128
```

> 💡 The `msopprof` tool requires CANN Commercial or Community Edition. For details, refer to the [msOpProf Tool Installation Guide](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/install_guide/msopprof_install_guide.md).

After the command completes, a folder named `OPPROF_{timestamp}_XXX` is generated in the default directory. The performance data folder structure is as follows:

```bash
├── dump                       # Raw performance data
├── ArithmeticUtilization.csv  # Cube/vector instruction cycle ratio
├── L2Cache.csv                # L2 Cache hit rate, which affects MTE2. Plan data movement logic carefully to increase hit rate
├── Memory.csv                 # UB, L1 Buffer, and main memory read/write bandwidth
├── MemoryL0.csv               # L0A Buffer, L0B Buffer, and L0C Buffer read/write bandwidth
├── MemoryUB.csv               # Vector and Scalar to UB read/write bandwidth
├── OpBasicInfo.csv            # Operator basic information
├── PipeUtilization.csv        # Computation unit and transfer unit latency and ratio
├── ResourceConflictRatio.csv  # UB bank group, bank conflict, and resource conflict ratio
└── visualize_data.bin         # MindStudio Insight presentation file
```

This example focuses on L0C Buffer egress performance data. View the specific performance data results as follows:

```bash
cat ./OPPROF_*/PipeUtilization.csv
```

Key metrics to monitor:

| Metric | Description |
|--------|-------------|
| `aic_fixpipe_time(us)` | Latency of fixpipe-type instructions (L0C Buffer egress) |

## Performance Test Script

`perf.sh` performs batch building, runs `msopprof`, extracts Fixpipe egress latency, and generates a CSV summary.

```bash
# View help
./perf.sh --help

# Test scenario 1, using dav-2201 by default
./perf.sh 1

# Test scenario 13, using dav-3510 by default
./perf.sh 13

# Explicitly specify the platform. The platform must match the scenario; otherwise, an error is reported
./perf.sh 1 dav-2201
./perf.sh 13 dav-3510
```

The script uses a built-in default shape sequence. Egress performance depends only on M and N. K is fixed at 64 to allow Mmad to produce L0C Buffer data beforehand. The sequence starts from a small M·N value and gradually increases to full capacity. The L0C Buffer size is 128 KB for dav-2201 and 256 KB for dav-3510. The saturation points differ, so the last entry differs between the two architectures:

dav-2201 default shape sequence:

| Test_ID | M | K | N | M·N | L0C Buffer Usage (float) |
|---------|---|---|---|-----|--------------------------|
| 1 | 16  | 64 | 16  | 256   | 1 KB |
| 2 | 32  | 64 | 32  | 1024  | 4 KB |
| 3 | 64  | 64 | 64  | 4096  | 16 KB |
| 4 | 128 | 64 | 128 | 16384 | 64 KB |
| 5 | 128 | 64 | 256 | 32768 | 128 KB (dav-2201 saturation) |

dav-3510 default shape sequence:

| Test_ID | M | K | N | M·N | L0C Buffer Usage (float) |
|---------|---|---|---|-----|--------------------------|
| 1 | 16  | 64 | 16  | 256   | 1 KB |
| 2 | 32  | 64 | 32  | 1024  | 4 KB |
| 3 | 64  | 64 | 64  | 4096  | 16 KB |
| 4 | 128 | 64 | 128 | 16384 | 64 KB |
| 5 | 256 | 64 | 256 | 65536 | 256 KB (dav-3510 saturation) |

The L0C Buffer egress data volume is M × N × sizeof(destination type). K is used only to allow Mmad to produce the M×N result in the L0C Buffer beforehand and is not counted in the egress volume. Adjust K as needed.

To test specific shapes, run `./build/demo SCENARIO_NUM M K N` directly.

After testing, results are saved to `perf_data_${timestamp}_scenario${SCENARIO}/perf_result_scenario${SCENARIO}.csv`. Raw `msopprof` data is saved in the `test_${id}_${M}_${K}_${N}` subdirectory under the same directory.

## Performance Metrics

`perf.sh` extracts `aic_fixpipe_time(us)` from `PipeUtilization.csv` and calculates bandwidth based on the platform clock frequency and egress volume.

The computed columns in the CSV are as follows:

| Column | Calculation | Description |
|--------|-------------|-------------|
| `AIC_FixPipe_Time(us)` | Extracted from `aic_fixpipe_time(us)` in `PipeUtilization.csv` | Fixpipe egress latency |
| `Cycle` | `Time(us) × Frequency(MHz)` | Cycle count converted based on platform clock frequency |
| `Bandwidth(GB/s)` | `DataSize(bytes) / Time(us) / 1e3` | Data transfer bandwidth |

### Performance Metric Calculation Methods

The `aic_fixpipe_time(us)` collected by `msopprof` in `PipeUtilization.csv` is the egress latency in microseconds. `perf.sh` reads this time column and calculates the cycle count and measured bandwidth based on the platform clock frequency and egress data volume.

#### Converting Time to Cycles

The clock frequency unit is MHz, which represents cycles per microsecond. No additional conversion is needed:

```text
Cycle = Time(us) × Frequency(MHz)
```

For example, the Atlas A3/A2 Training/Inference Platform has a clock frequency of 1800 MHz. If `aic_fixpipe_time(us) = 0.050000`:

```text
Cycle = 0.050000 × 1800 = 90.00 cycles
```

#### Data Transfer Volume Calculation

`perf.sh` calculates the egress volume based on the destination data type for each scenario. This value is used as `DataSize(bytes)` for bandwidth calculation:

| Scenario | Data Volume Calculation | Data Type Size |
|----------|------------------------|----------------|
| 1, 11 | `M × N × sizeof(half)` | 2 bytes |
| 2, 12 | `M × N × sizeof(int8_t)` | 1 byte |
| 13, 14 | `M × N × sizeof(float)` | 4 bytes |

#### Measured Bandwidth Calculation

Bandwidth is output in GB/s. Since `Time(us)` is in microseconds, `DataSize(bytes) / Time(us)` yields MB/s. Dividing by `1e3` converts to GB/s:

```text
Bandwidth(GB/s) = DataSize(bytes) / Time(us) / 1e3
```

For example, scenario 11 with shape `[128, 64, 128]` and destination type half:

```text
DataSize = 128 × 128 × 2 = 32768 bytes
Time = 0.050000 us
Bandwidth = 32768 / 0.050000 / 1e3 = 655.360 GB/s
```

#### Theoretical Latency and Bandwidth Utilization

The "Theoretical Bandwidth (Byte/cycle)" and "Bandwidth Latency (cycle)" in the scenario table can be used to estimate theoretical latency. Theoretical bandwidth is derived from egress parallelism (the egress unit processes 64 output elements per cycle, theoretical bandwidth = 64 × sizeof(destination type), doubled for dual-target mode). The fixed latency represents the base startup overhead for a single transfer. The theoretical transfer time can be estimated as follows:

```text
TransferCycle = DataSize(bytes) / TheoreticalBandwidth(Byte/cycle)
TheoryCycle = Latency(cycle) + TransferCycle
TheoryTime(us) = TheoryCycle / Frequency(MHz)
TheoryBandwidth(GB/s) = DataSize(bytes) / TheoryTime(us) / 1e3
```

The ratio of measured bandwidth to theoretical bandwidth can be used to evaluate bandwidth utilization:

```text
BandwidthUtilization = MeasuredBandwidth(GB/s) / TheoryBandwidth(GB/s) × 100%
```

The platform clock frequency is set automatically by `perf.sh` based on the scenario:

| Platform | Architecture Code | Clock Frequency | Applicable Scenarios |
|----------|-------------------|-----------------|----------------------|
| Atlas A3/A2 Training/Inference Platform | dav-2201 | 1800 MHz | 1, 2 |
| Ascend 950PR&950DT products Platform | dav-3510 | 1650 MHz | 11-14 |

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
python3 generate_roofline.py --csv perf_data_xxx_scenario11/perf_result_scenario11.csv
```

The script has built-in egress parallelism for each scenario. You do not need to manually specify peak bandwidth. The first-instruction overhead defaults to the scenario value (20 cycles for dav-2201, 26 cycles for dav-3510), and the clock frequency is automatically selected based on the scenario.

### Chart Example

The following is a Roofline chart example generated for scenario 1:

![Scenario 1 Roofline Chart](figures/perf_data_scenario1_roofline.png)

## Notes

1. Scenario numbers must match the platform: use scenarios 1, 2 for dav-2201 and scenarios 11-14 for dav-3510. `perf.sh` validates the match and reports errors for mismatches.
2. The L0C Buffer to UB path (scenarios 13, 14) is supported only on dav-3510 and uses the `Fixpipe` interface with the hybrid programming framework.
3. Matrix dimensions must meet alignment requirements: M and N must be multiples of 16. For dual-target mode split along the M dimension, M must be a multiple of 2. For split along the N dimension, N must be a multiple of 32.
4. This is a pure performance test and does not verify computation results. The kernel function does not initialize data in the L0A Buffer or L0B Buffer. It retains a minimal Mmad operation to produce L0C Buffer data beforehand.
5. A `PipeBarrier<PIPE_ALL>` is inserted between the Mmad pre-operation and the egress instruction to prevent pipeline overlap from causing inaccurate `aic_fixpipe_time` statistics.
