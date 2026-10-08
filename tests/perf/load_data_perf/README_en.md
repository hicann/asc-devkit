# LoadData (L1->L0) Performance Test Sample

## Overview

This sample is used for performance testing of LoadData-related interfaces and covers the data transfer paths for moving matrices A/B from L1 to L0A/L0B.
This is a non-business test sample and does not include matrix computation instructions.

## Supported Products and CANN Versions

| Product | Architecture Code | CANN Version |
|------|----------|-------------|
| Ascend 950PR&950DT products | dav-3510 | >= CANN 9.1.0 |
| Atlas A3 products | dav-2201 | >= CANN 9.0.0 |
| Atlas A2 products | dav-2201 | >= CANN 9.0.0 |

## Directory Structure

```text
├── load_data_perf
│   ├── CMakeLists.txt                    // Build project file
│   ├── load_data_perf.asc                // LoadData performance test implementation and entry point
│   ├── perf.sh                           // Performance test script
│   ├── generate_roofline_with_latency.py // Roofline generation script
│   └── README.md                         // Sample documentation
```

## Sample Description

This sample selects different transfer paths through the runtime parameter `SCENARIO_NUM`. Matrix shapes are passed at runtime through `./demo SCENARIO_NUM M K N`, and the kernel function name is `load_data_perf_custom`.

The supported test scenarios vary by platform architecture:

| Platform Architecture | Supported Scenarios | Description |
|----------|----------|------|
| Atlas A3/A2 Training/Inference platform (dav-2201) | 1-9 | LoadData interface scenarios supported by the Atlas A3/A2 Training/Inference platform |
| Ascend 950PR&950DT products platform (dav-3510) | 11-19 | LoadData interface scenarios supported by the Ascend 950PR&950DT products platform |

### Atlas A3/A2 Training/Inference Platform Scenarios (1-9)

| SCENARIO_NUM | Input Data Type | Data Source | Execution Path | Description | Theoretical Bandwidth (Byte/cycle) | Bandwidth Latency (cycle) |
|--------------|--------------|--------|----------|------|----------------------|-----------------|
| 1 | bfloat16 | A matrix | L1 -> L0A | LoadData (2D matrix transfer) loads data from L1 to L0A | 256 | 28 |
| 2 | bfloat16 | B matrix | L1 -> L0B | LoadData (2D matrix transfer) loads data from L1 to L0B | 128 | 28 |
| 3 | bfloat16 | A matrix | L1 -> L0A (transpose) | LoadDataWithTranspose loads data from L1 to L0A (transpose) | 256 | 28 |
| 4 | bfloat16 | B matrix | L1 -> L0B (transpose) | LoadDataWithTranspose loads data from L1 to L0B (transpose) | 128 | 28 |
| 5 | bfloat16 | A matrix | L1 -> L0A | LoadData (convolution data transfer) v2 loads data from L1 to L0A | 32-256 | 50 |
| 6 | bfloat16 | B matrix | L1 -> L0B | LoadData (convolution data transfer) v2 loads data from L1 to L0B | 28.4-128 | 50 |
| 7 | int8_t | B matrix | L1 -> L0B | LoadDataWithSparse loads data from L1 to L0B (sparse load) | 128 | 30 |
| 8 | float | Bias | L1 -> BiasTable Buffer | Loads data from L1 to BiasTable Buffer | 32 | 20 |
| 9 | uint64_t | FixPipe | L1 -> Fixpipe Buffer | Loads data from L1 to Fixpipe Buffer | 32 | 20 |

### Ascend 950PR&950DT products Platform Scenarios (11-19)

| SCENARIO_NUM | Input Data Type | Data Source | Execution Path | Description | Theoretical Bandwidth (Byte/cycle) | Bandwidth Latency (cycle) |
|--------------|--------------|--------|----------|------|----------------------|-----------------|
| 11 | bfloat16 | A matrix | L1 -> L0A | LoadData (2D matrix transfer V2) loads data from L1 to L0A | 256 | 30 |
| 12 | bfloat16 | B matrix | L1 -> L0B | LoadData (2D matrix transfer V2) loads data from L1 to L0B | 256 | 30 |
| 13 | fp8_e4m3fn | A matrix + ScaleA | L1 -> L0A + L0A_MX | LoadData (MX matrix transfer) loads data from L1 to L0A and L0A_MX (with scale) | 256/32 | 30 |
| 14 | fp8_e4m3fn | B matrix + ScaleB | L1 -> L0B + L0B_MX | LoadData (MX matrix transfer) loads data from L1 to L0B and L0B_MX (with scale) | 256/32 | 30 |
| 15 | bfloat16 | A matrix | L1 -> L0A | LoadData (convolution data transfer) v2 loads data from L1 to L0A | 32-256 | 50 |
| 16 | bfloat16 | B matrix | L1 -> L0B | LoadData (convolution data transfer) v2 loads data from L1 to L0B | 32-256 | 50 |
| 17 | bfloat16 | B matrix | L1 -> L0B (transpose) | LoadDataWithTranspose loads data from L1 to L0B (transpose) | 256 | 28 |
| 18 | float | Bias | L1 -> BiasTable Buffer | Loads data from L1 to BiasTable Buffer | 32 | 20 |
| 19 | uint64_t | FixPipe | L1 -> Fixpipe Buffer | Loads data from L1 to Fixpipe Buffer | 32 | 26 |

## Building and Running

Run the following steps in the root directory of this sample to build and run the sample.

### Configure Environment Variables

Configure environment variables according to the [installation method](../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit package in the current environment.

```bash
source ${install_path}/cann/set_env.sh
```

`${install_path}` is the CANN package installation directory. If no installation directory is specified, the default installation path is `/usr/local/Ascend`.

### Build the Sample

```bash
mkdir -p build && cd build
cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..
make -j
```

To build for the Ascend 950PR&950DT products platform:

```bash
cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..
make -j
```

### Run the Sample

```bash
# Atlas A3/A2 Training/Inference platform examples (scenarios 1-9)
./demo 1 64 128 128
./demo 2 256 128 128

# Ascend 950PR&950DT products platform examples (scenarios 11-19)
./demo 11 64 128 128
./demo 13 256 128 128
```

The runtime parameter order is `SCENARIO_NUM M K N`:

| Parameter | Description |
|------|------|
| `SCENARIO_NUM` | Test scenario ID. Use 1-9 for the Atlas A3/A2 Training/Inference platform and 11-19 for the Ascend 950PR&950DT products platform |
| `M` | Number of rows in matrix A |
| `K` | Number of columns in matrix A, also the number of rows in matrix B |
| `N` | Number of columns in matrix B |

## Collecting Performance Data

Use the `msOpProf` tool to collect detailed performance data:

```bash
msopprof ./demo 1 128 128 128
```

  > **Note:** To use the `msOpProf` tool, install the CANN commercial/community edition. For details, see the [msopprof Tool Installation Guide](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/install_guide/msopprof_install_guide.md).

  After the command completes, a directory named in the format `OPPROF_{timestamp}_XXX` is generated in the default directory. The following is an example of the performance data directory structure:

  ```bash
  ├── dump                       # Raw performance data
  ├── ArithmeticUtilization.csv  # Cycle ratio of cube/vector instructions
  ├── L2Cache.csv                # L2 Cache hit rate, which affects MTE2. Plan data transfer logic properly to improve the hit rate
  ├── Memory.csv                 # Read/write bandwidth rates of UB, L1, and main memory
  ├── MemoryL0.csv               # Read/write bandwidth rates of L0A, L0B, and L0C
  ├── MemoryUB.csv               # Read/write bandwidth rates from Vector and Scalar to UB
  ├── OpBasicInfo.csv            # Operator basic information
  ├── PipeUtilization.csv        # Time and ratio of compute units and transfer units
  ├── ResourceConflictRatio.csv  # Ratio of UB bank group, bank conflict, and resource conflicts
  └── visualize_data.bin         # MindStudio Insight presentation file
  ```

This sample mainly focuses on L1->L0 performance data. Run the following command to view the detailed performance results:

```bash
cat ./OPPROF_*/PipeUtilization.csv
```

Focus on the following metrics:

| Metric | Description |
|------------------------|------------------------------------------------------------------------|
| aic_mte1_time(us) | Duration of mte1 type instructions, including L1 -> L0A/L0B transfers and L1 to Bias Table Buffer transfers |
| aic_fixpipe_time(us) | Duration of fixpipe type instructions, including L1 to Fixpipe Buffer transfers |

## Performance Test Script

`perf.sh` is used to batch build, run `msopprof`, extract performance metrics, and generate CSV summary results.

```bash
# Show help
./perf.sh --help

# Test scenario 1. dav-2201 is used by default
./perf.sh 1

# Test scenario 11. dav-3510 is used by default
./perf.sh 11

# Explicitly specify the platform
./perf.sh 1 dav-2201
./perf.sh 11 dav-3510
```

The script tests five shapes by default:

| Test_ID | M | K | N | Data Volume Description |
|---------|---|---|---|------------|
| 1 | 32 | 32 | 32 | Small data volume |
| 2 | 64 | 64 | 64 | Small-to-medium data volume |
| 3 | 64 | 128 | 64 | Small-to-medium data volume |
| 4 | 128 | 128 | 128 | Medium data volume |
| 5 | 256 | 128 | 256 | About 64 KB, close to full L0 capacity |

The transfer data volume for matrix A is `M * K`, the transfer data volume for matrix B is `K * N`, and the transfer data volume for Bias Table Buffer and Fixpipe Buffer is `N`. Adjust them as needed.

After the test completes, results are saved to `perf_data_${timestamp}_scenario${SCENARIO}/perf_result_scenario${SCENARIO}.csv`, and raw `msopprof` data is saved in the `test_${id}_${M}_${K}_${N}` subdirectory under the same directory.

## Performance Metric Description

`perf.sh` automatically selects the performance metric according to the scenario:

| Scenario | Metric | Description |
|------|------|------|
| 9, 19 | `aic_fixpipe_time(us)` | Execution time of FixPipe (L1 to Fixpipe Buffer transfer) |
| Other scenarios | `aic_mte1_time(us)` | Execution time of MTE1 (L1 to L0A/L0B transfer or L1 to Bias Table Buffer transfer) |

The calculated columns in the CSV are as follows:

| Column Name | Calculation Method | Description |
|------|----------|------|
| `Cycle` | `Time(us) * Frequency(MHz)` | Number of cycles converted based on the platform frequency |
| `Bandwidth(GB/s)` | `DataSize(bytes) / Time(us) / 1e3` | Data transfer bandwidth |

### Performance Metric Calculation Method

The `aic_mte1_time(us)` and `aic_fixpipe_time(us)` collected by `msopprof` in `PipeUtilization.csv` are durations in microseconds (us). `perf.sh` reads the corresponding time column and calculates the number of cycles and measured bandwidth based on the platform frequency and test data volume.

#### Converting Time(us) to Cycle

The frequency unit is MHz, which means cycles per microsecond. Therefore, no extra conversion is required when converting `Time(us)` to cycles:

```text
Cycle = Time(us) * Frequency(MHz)
```

For example, when the Atlas A3/A2 Training/Inference platform frequency is 1800 MHz and `msopprof` collects `aic_mte1_time(us) = 0.021111`:

```text
Cycle = 0.021111 * 1800 = 38.00 cycles
```

When the Ascend 950PR&950DT products platform frequency is 1650 MHz and `msopprof` collects `aic_mte1_time(us) = 0.030000`:

```text
Cycle = 0.030000 * 1650 = 49.50 cycles
```

#### Data Transfer Volume Calculation

`perf.sh` selects the data transfer volume by scenario as `DataSize(bytes)` for bandwidth calculation:

| Scenario | Data Volume Calculation | Data Type Size |
|------|------------|--------------|
| 1, 3, 5, 11, 15 | `M * K * sizeof(bfloat16)` | 2 bytes |
| 2, 4, 6, 12, 16, 17 | `K * N * sizeof(bfloat16)` | 2 bytes |
| 7 | `K * N * sizeof(int8_t)` | 1 byte |
| 8, 18 | `N * sizeof(float)` | 4 bytes |
| 13 | `M * K * sizeof(fp8_e4m3fn)` | 1 byte |
| 14 | `K * N * sizeof(fp8_e4m3fn)` | 1 byte |
| 9, 19 | `N * sizeof(uint64_t)` | 8 bytes |
| Other scenarios | Calculated as `M * K * sizeof(bfloat16)` by default | 2 bytes |

For bias scenarios, the kernel aligns the C1-to-C2 block length according to platform requirements. `perf.sh` calculates bandwidth by default based on the effective bias element count `N * sizeof(float)`. To analyze the actual hardware transfer granularity, separately convert it based on the `biasSizeAlign` alignment rule in `load_data_perf.asc`.

#### Measured Bandwidth Calculation

Bandwidth is output in GB/s. Because `Time(us)` is in microseconds, the unit of `DataSize(bytes) / Time(us)` is MB/s. Dividing by `1e3` gives GB/s:

```text
Bandwidth(GB/s) = DataSize(bytes) / Time(us) / 1e3
```

For example, in scenario 1, when the shape is `[128, 128, 128]` and the data type is bfloat16:

```text
DataSize = 128 * 128 * 2 = 32768 bytes
Time = 0.100000 us
Bandwidth = 32768 / 0.100000 / 1e3 = 327.680 GB/s
```

#### Theoretical Duration and Bandwidth Utilization

The "Theoretical Bandwidth (Byte/cycle)" and "Bandwidth Latency (cycle)" in the scenario tables can be used to estimate theoretical duration. The fixed latency indicates the basic startup overhead of one transfer, and the theoretical transfer time can be estimated as follows:

```text
TransferCycle = DataSize(bytes) / TheoreticalBandwidth(Byte/cycle)
TheoryCycle = Latency(cycle) + TransferCycle
TheoryTime(us) = TheoryCycle / Frequency(MHz)
TheoryBandwidth(GB/s) = DataSize(bytes) / TheoryTime(us) / 1e3
```

The ratio of measured bandwidth to theoretical bandwidth can be used to evaluate bandwidth utilization:

```text
BandwidthUtilization = MeasuredBandwidth(GB/s) / TheoryBandwidth(GB/s) * 100%
```

When the theoretical bandwidth is a range value, for example `32-256` Byte/cycle, or a composite value, for example `256/32` Byte/cycle, select the corresponding theoretical bandwidth based on the specific interface transfer mode, data layout, and scale data path.

The script automatically sets the platform frequency by scenario:

| Platform | Architecture Code | Frequency | Applicable Scenarios |
|------|----------|------|----------|
| Atlas A3/A2 Training/Inference platform | dav-2201 | 1800 MHz | 1-9 |
| Ascend 950PR&950DT products platform | dav-3510 | 1650 MHz | 11-19 |

## Roofline Analysis

This sample provides `generate_roofline_with_latency.py`, which can generate ASCII reports and images based on the CSV output generated by `perf.sh`.

### Python Package Dependencies

`generate_roofline_with_latency.py` uses the Python standard library to read CSV files and generate ASCII reports. To generate PNG/PDF charts at the same time, install `matplotlib` and `numpy`.

```bash
python3 -m pip install --user matplotlib numpy
```

If these dependencies are not installed, the script can still generate `.txt` ASCII analysis reports, but image generation is skipped.

```bash
# Automatically find results in the latest perf_data directory
python3 generate_roofline_with_latency.py

# Specify a CSV file
python3 generate_roofline_with_latency.py --csv perf_data_20260613_164620_scenario1/perf_result_scenario1.csv

# Customize theoretical parameters
python3 generate_roofline_with_latency.py \
  --csv perf_data_20260613_164620_scenario1/perf_result_scenario1.csv \
  --peak-bw 256 \
  --latency 30 \
  --frequency 1800 \
  --l0-size 64
```

The default output file prefix is `${perf_data_directory_name}_roofline`. The generated `.txt` file is a detailed ASCII analysis. If `matplotlib` is installed, a `.png` chart is also generated.

### Chart Example

The following is an example Roofline chart generated for scenario 1:

![Scenario 1 Roofline Chart](figures/perf_data_scenario1_roofline.png)

## Notes

1. The scenario ID must match the platform architecture: use scenarios 1-9 for the Atlas A3/A2 Training/Inference platform and scenarios 11-19 for the Ascend 950PR&950DT products platform.
2. Scenario 7 (LoadSparse) supports only the dav-2201 architecture.
3. Scenarios 13/14 (LoadData (MX matrix transfer)) support only the dav-3510 architecture.
4. Different scenarios have different matrix shape alignment requirements. Use matrix shapes that meet the alignment requirements, such as multiples of 16, 32, or 64.
