# DataCacheHint Performance Tuning Example

## Overview

This example uses table-lookup sin computation to demonstrate data cache optimization strategies in the Ascend C SIMT programming model. The example includes 1 baseline version and 1 optimized version. In the baseline version, all data is loaded using the default method, and input, output, and sin table data compete for DCache space. The optimized version specifies different cache strategies for different types of data, ensuring that the frequently accessed sin lookup table remains resident in DCache, reducing Global Memory access count, demonstrating the data cache optimization tuning path in the SIMT programming model.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version

- \>= CANN 9.1.0

## Directory Structure

```text
├── cache_hint
│   ├── CMakeLists.txt              // CMake build file
│   ├── data_cache_hint.asc         // SIMT data cache example implementation
│   ├── figures                     // Image resources for README
│   └── README.md
```

## Example Description

- Example Function

  Compute sin values using table lookup with linear interpolation for improved accuracy.

- Example Specifications:

  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">SinTableLookup</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[65536]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">sin_table</td><td align="center">[8192]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">output</td><td align="center">[65536]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">sin_table_lookup_baseline / sin_table_lookup_optimized</td></tr>
  </table>

## Example Implementation

### Table Lookup Computation Description

This example uses table lookup to compute sin values. The core idea is to pre-store sin values for the [0, pi] interval in a lookup table, and obtain approximate sin values for any angle through linear interpolation. The computation flow is as follows:

1. Reduce the input angle to the [0, pi] interval, handling negative values and periodicity
2. Compute the lookup table index: `index_float = x / pi * table_length`
3. Obtain the integer part `n` and fractional part `frac` of `index_float`
4. Read `table[n]` and `table[n+1]` from the lookup table
5. Perform linear interpolation to obtain the result: `y = table[n] + frac * (table[n+1] - table[n])`

### Example Implementation Description

This example implements different data cache strategies through 2 independent kernels, each corresponding to a specific case version.

| Case   | Implementation Characteristics                                                                 | Kernel Function Used               | Optimization Features           |
|--------|----------------------------------------------------------------------|----------------------------|---------------------|
| Case 0 | All data loaded using the default method; input, output, and sin table data compete for DCache space | sin_table_lookup_baseline  | Baseline version, no cache optimization |
| Case 1 | Input data `x` loaded directly from GM; lookup table `sin_table` is hot data, preferentially loaded from DCache, reducing GM access; output data written directly to GM | sin_table_lookup_optimized | Data cache optimization, sin table resident in DCache |

#### 1. Data Loading Characteristics

In this example, to demonstrate performance differences, a scenario with 32KB DCache space is constructed. Default data loading preferentially reads from DCache; if data does not exist in DCache, it is loaded from Global Memory (GM) and simultaneously cached in DCache. The access characteristics of the three types of data in this example are as follows:

| Data        | Size    | Access Characteristics              |
|-----------|-------|-------------------|
| input     | 256KB | Each element is read only once         |
| sin_table | 32KB  | Data is accessed repeatedly, classified as hot data    |
| output    | 256KB | Each element is written only once, not read after writing |

By default, loaded data is retained in DCache. During computation, some hot data is used multiple times by multiple threads, while other data is used only once during computation. This type of data does not need caching and should not compete with hot data for DCache space.

- **`asc_ldcg()`**: Loads data from Global Memory, suitable for input data that is traversed only once, reducing its occupation of DCache space
- **`asc_ldca()`**: Loads data preferentially from DCache, suitable for hot data that requires frequent access (such as lookup tables), ensuring hot data remains resident in DCache and reducing reloads from Global Memory
- **`asc_stcg()`**: Stores data directly to Global Memory space, suitable for data written to GM that will not be used again, avoiding unnecessary cache usage and preventing output data from occupying DCache space and affecting hot data caching
- Input data is loaded directly from GM via `asc_ldcg()`, not cached in DCache
- sin_table data is preferentially loaded from DCache via `asc_ldca()`, ensuring it remains resident in DCache
- Output data is written directly to GM via `asc_stcg()`, bypassing DCache caching

Since sin_table is 32KB in size, it can be cached in DCache without being evicted. Hot data continues to reside in DCache, reducing repeated loading of sin_table data from GM to DCache.

---

### Performance Metrics Description

| Metric                  | Description                          |
|---------------------|-----------------------------|
| Task Duration(us)   | Total execution time of the entire task; operator execution time is determined by this parameter     |
| DCache Read GM      | Number of times DCache reads data from Global Memory |
| DCache Read Vector  | Number of times Vector Core reads data from DCache   |
| DCache Write Vector | Number of times Vector Core writes data to DCache   |

---

### Case 0: Baseline Version

**Example Goal**: Establish a latency baseline for table-lookup sin computation, and observe the impact of input, output, and sin table data competing for DCache space on performance

**Core Implementation**:
- Directly read `input[idx]` and `sin_table[n]` / `sin_table[n+1]` data
- Use linear interpolation to compute the data result
- Threads directly write back to `output[idx]`
- All data access uses the default loading method; `input`, `sin_table`, and `output` data compete for DCache space

When accessing large amounts of input and output data, the sin lookup table data is evicted from DCache, causing subsequent table lookups to require reloading from GM, increasing memory access latency.

**Key Code**:

```cpp
float x = input[idx];
float index_float = x * static_cast<float>(table_length) / PI;
uint32_t n = static_cast<uint32_t>(floorf(index_float));
float frac = index_float - static_cast<float>(n);
float low_val = sin_table[n];
float high_val = 0.0f;
if (n + 1 >= table_length) {
    high_val = sin_table[0];
} else {
    high_val = sin_table[n + 1];
}
output[idx] = sign * (low_val + frac * (high_val - low_val));
```

**Performance Data**:

| Task Duration(us) | DCache Read GM | DCache Read Vector | DCache Write Vector |
|:-----------------:|:-----------------:|:---------------------:|:----------------------:|
|       56.82       |       5064        |         2048          |          6144          |

**Analysis**:

The Task Duration of Case 0 is 56.82us, and DCache Read GM is 5064 times, serving as the baseline version.

In the Case 0 scenario, loading input and output caches data in DCache, replacing some cached data in DCache. Assume the data corresponding to index 2 in sin_table is replaced. When a subsequent thread needs the data corresponding to index 2 in sin_table during computation, a cache miss occurs, requiring reloading from GM. Additionally, the input and output data cached in DCache is not actually used again, does not need caching, and should not occupy DCache space.

<img src="./figures/DCache默认加载.png" width="80%">

Optimization direction: When loading input and output, do not cache in DCache, ensuring table data remains resident in DCache. When the data corresponding to index 2 in sin_table is subsequently needed, it can be loaded directly from DCache without reloading from GM.

---

### Case 1: Data Cache Optimized Version

**Optimization Goal**: Differentiate cache strategies for different data through memory access functions, ensuring the sin lookup table remains resident in DCache, reducing GM access count, and lowering end-to-end latency

**Core Optimization**:
- Use `asc_ldcg()` to read data: each input data element is accessed only once, no DCache needed
- Use `asc_ldca()` to read sin table data: the sin table is hot data requiring frequent access, ensuring data remains resident in DCache
- Use `asc_stcg()` to write data: output data written to GM is not used again, no DCache needed

**Key Code**:

```cpp
float x = asc_ldcg(&input[idx]);
float index_float = x * static_cast<float>(table_length) / PI;
uint32_t n = static_cast<uint32_t>(floorf(index_float));
float frac = index_float - static_cast<float>(n);
float low_val = asc_ldca(&sin_table[n]);
float high_val = 0.0f;
if (n + 1 >= table_length) {
    high_val = asc_ldca(&sin_table[0]);
} else {
    high_val = asc_ldca(&sin_table[n + 1]);
}
float y = sign * (low_val + frac * (high_val - low_val));
asc_stcg(&output[idx], y);
```

**Performance Data**:

| Task Duration(us) | DCache Read GM | DCache Read Vector | DCache Write Vector |
|:-----------------:|:-----------------:|:---------------------:|:----------------------:|
|      50.895       |       3531        |         2048          |          6144          |

**Analysis**:
- Compared to the Case 0 baseline, Task Duration decreases from 56.82us to 50.895us, a reduction of approximately 10.4%
- DCache Read GM decreases from 5064 to 3531, a reduction of approximately 30.2% (1533 fewer Global Memory accesses), which is the key factor in performance improvement
- DCache Read Vector and DCache Write Vector remain unchanged, indicating the optimization does not add extra overhead
- After sin table data remains resident in DCache, it avoids being evicted by input/output data, reducing the number of sin table reloads from Global Memory
---

## Performance Comparison Summary

### Ascend 950PR Performance Data

**Overall Optimization Effect**:
- From Case 0 baseline to Case 1 optimized version, Task Duration decreases from 56.82us to 50.895us, a reduction of approximately 10.4%
- DCache Read GM decreases from 5064 to 3531, a reduction of approximately 30.2%, eliminating 1533 Global Memory accesses

| Case version | Task Duration(us) | Task Duration Relative to Case 0 | Optimization Points                   |
|--------------|-------------------|-----------------------|-----------------------|
| Case 0       | 56.82             | **1x**                | Baseline version, all data competes for DCache space |
| Case 1       | 50.895            | **0.90x latency**           | Data cache optimization, sin table resident in DCache space |

## Tuning Recommendations

1. **Identify hot data**: In table-lookup operators, the lookup table is frequently accessed hot data and should be prioritized for DCache residency.
2. **Differentiate data access patterns**: For data accessed only once (such as input data), use `asc_ldcg()` to avoid occupying DCache space; for data requiring frequent access (such as lookup tables), use `asc_ldca()` to preferentially allocate cache space.
3. **Avoid output data occupying cache**: For output data that is not read after writing, use `asc_stcg()` to avoid occupying DCache space.
4. **Monitor the DCache Read GM metric**: The reduction in DCache Read GM count directly reflects improved cache hit rate and reduced Global Memory access latency.

## Build and Run

Run the following steps in the root directory of this example to build and execute the example.
- Configure Environment Variables  
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, it defaults to `/usr/local/Ascend`.

- Run the Example

  Run the following commands in this example directory.

  ```bash
  SCENARIO_NUM=0                       # Select the execution scenario, options 0-1
  mkdir -p build && cd build;          # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;  # Build the project
  ./data_cache_hint                    # Run the example
  ```

- Build Options Description

  | Option                        | Values        | Description                                                |
  |---------------------------|------------|---------------------------------------------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |
  | `SCENARIO_NUM`            | `0`-`1`    | Example type, default is 0; 0: baseline version, 1: data cache optimized version                 |

  The following output indicates that the accuracy verification is successful.

  ```text
  [Success] Case accuracy is verification passed.
  ```

## Performance Analysis

Use the `msOpProf` tool to collect detailed performance data:

```bash
msopprof ./data_cache_hint   # Analyze case performance
```

After the command completes, a folder named "OPPROF_{timestamp}_XXX" is generated in the default directory. The performance data folder structure example is as follows:

```text
├──dump                       # Raw performance data, no user attention needed
├──ArithmeticUtilization.csv  # cube/vector instruction cycle ratio
├──L2Cache.csv                # L2 Cache hit rate
├──Memory.csv                 # UB, L1, and main memory read/write bandwidth
├──MemoryL0.csv               # L0A, L0B, and L0C read/write bandwidth
├──MemoryUB.csv               # Vector and Scalar to UB read/write bandwidth
├──OpBasicInfo.csv            # Operator basic information
├──PipeUtilization.csv        # Compute unit and transfer unit duration and ratio
├──ResourceConflictRatio.csv  # UB bank group, bank conflict, and resource conflict ratio across all instructions
└──visualize_data.bin         # MindStudio Insight presentation file
```
