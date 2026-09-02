# Short Vector Add Operator Tuning Example

## Overview

This example uses vector addition to demonstrate performance tuning methods for short vectors in Ascend C SIMT programming. Through comparison of two scenarios (Case 1-2), it shows the optimization process from basic half-type element-wise addition to half2-type addition. The focus is on comparing the data processing efficiency differences between the original type (half) and the short vector type (half2).

**Optimization Path**:

- Case 1: half-type data processing (baseline)
- Case 2: half2-type data processing

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version

- \>= CANN 9.1.0

## Directory Structure

```
short_vector_add
│   ├── scripts/             // Test script directory
│   │   ├── gen_data.py      // Generate test input and golden data
│   │   └── verify_result.py // Verify output results
│   ├── CMakeLists.txt       // Build project file
│   ├── data_utils.h         // Data read/write utility functions
│   └── short_vector_add.asc // Ascend C operator implementation & invocation example
```

## Example Description

- Example Function:

  The example computes half-type data, implementing vector addition using both basic half type and short vector half2 type. Through comparison of different scenarios, it demonstrates the impact of short vector computation and thread allocation optimization on performance. The computation formula is:
  $$z_i = x_i + y_i$$

- Example Specifications
  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">SIMT Example</td></tr>
  <tr><td rowspan="3" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center" colspan="2">data type</td></tr>
  <tr><td align="center">x</td><td align="center">[64 * 256 * 1024]</td><td align="center" colspan="2">half</td></tr>
  <tr><td align="center">y</td><td align="center">[64 * 256 * 1024]</td><td align="center" colspan="2">half</td></tr>
  <tr><td rowspan="1" align="center">Example Output</td><td align="center">z</td><td align="center">[64 * 256 * 1024]</td><td align="center" colspan="2">half</td></tr>
  </table>

## Example Implementation

### Performance Metrics Description

**Table 1 AI Core Performance Metrics Field Description**

|             Field Name             | Field Description                                             |
|:---------------------------:|:-------------------------------------------------|
|      Task Duration(us)      | Total task duration, including scheduling time to the accelerator, execution time on the accelerator, and response completion time.          |
|      aiv_total_cycles       | Total CPU cycles consumed by the task executing on the Vector Core.          |
|          aiv_time           | Theoretical execution time of the task on the AI Vector Core, in microseconds.               |
|      aiv_vec_time(us)       | Duration of vec-type instructions (vector computation instructions), in microseconds.                         |
|        aiv_vec_ratio        | Ratio of vec-type instruction cycles to total cycles.       |
|     aiv_scalar_time(us)     | Duration of scalar-type instructions (scalar computation instructions), in microseconds.                      |
|      aiv_scalar_ratio       | Ratio of scalar-type instruction cycles to total cycles.    |
|      aiv_read_hit_rate      | Read cache hit rate (file: L2Cache.csv).                     |
|     aiv_write_hit_rate      | Write cache hit rate (file: L2Cache.csv).                          |
| aiv_main_mem_read_bw(GB/s)  | Bandwidth rate for the corresponding total cycle when main memory reads data from all other units (file: Memory.csv). |
| aiv_main_mem_write_bw(GB/s) | Bandwidth rate for the corresponding total cycle when main memory writes data to all other units (file: Memory.csv). |

The performance data in this section is obtained on Ascend 950PR series products.

### Case 1: half-type Data Processing (Baseline)

**Implementation**:

The baseline implements addition of two groups of `half`-type input data.

**Key Code**:

```cpp
template<typename T>
__global__ void add(T* x, T* y, T* z, uint64_t size)
{
    // gridDim.x * blockDim.x total threads (64 * 1024)
    int32_t stride = gridDim.x * blockDim.x;
    // Each thread starts from its own global idx, incrementing by total threads until all elements are processed
    for (int32_t idx = blockIdx.x * blockDim.x + threadIdx.x; idx < size; idx += stride) {
        z[idx] = x[idx] + y[idx];
    }
}
```

**Example Configuration**:

- blockNum: 64
- threadNumPerBlock: 1024
- Total threads: 65536
- Elements per thread: 256

**Performance Data**:

| Task Duration(us) | aiv_time(us) | aiv_total_cycles | aiv_vec_time(us) | aiv_vec_ratio | aiv_scalar_time(us) | aiv_scalar_ratio | aiv_read_hit_rate | aiv_write_hit_rate | aiv_main_mem_read_bw(GB/s) | aiv_main_mem_write_bw(GB/s) |
|:-----------------:|:------------:|:----------------:|:----------------:|:-------------:|:-------------------:|:----------------:|:-----------------:|:------------------:|:--------------------------:|:---------------------------:|
|      86.478       |    86.217    |     8832432      |      85.735      |     0.995     |        0.47         |      0.005       |         0         |       0.493        |            7.77            |            6.24             |


**Performance Data Analysis**:

- End-to-end latency: **86.478 us**
- Write cache hit rate: **49.3%**
- Read/write bandwidth: **7.77(GB/s) | 6.24(GB/s)**
- Performance analysis: The Add operator performs simple X+Y=Z. When writing back results, the hit rate is below 50%, indicating that write cache miss rate exceeds 50%, triggering additional memory read-back (RMW) operations, classified as write misses. The typical cause is data misalignment or small data type bit width preventing Warp from filling completely.

**Performance Optimization Recommendation**:
> **Introduce half2 type**
>
> For 16-bit half-type data, the underlying instructions support computing two data elements at once, which improves data computation efficiency and reduces scalar instruction count such as for loop iterations.

---

### Case 2: half2-type Data Processing

**Implementation**:

Replace the original `half`-type data with `half2`-type data for two groups of input. When the total number of elements is odd, the last `half` element must be handled separately.

**Key Code**:

```cpp
template<typename T, typename U>
__global__ void add2(T* x, T* y, T* z, uint64_t size)
{
    // Convert to vector type
    const U* x2 = reinterpret_cast<const U*>(x);
    const U* y2 = reinterpret_cast<const U*>(y);
    U* z2 = reinterpret_cast<U*>(z);

    // Compute the for-loop boundary once via division here
    uint64_t vector_size = size / 2;
    int32_t stride = gridDim.x * blockDim.x;

    // Vectorized loop processes even pairs (stride is still the total number of threads)
    for (int32_t idx = blockIdx.x * blockDim.x + threadIdx.x; idx < vector_size; idx += stride) {
        z2[idx] = x2[idx] + y2[idx];
    }

    // Handle last odd element (if size is odd, processed by global thread 0 at the end)
    if (blockIdx.x == 0 && threadIdx.x == 0 && (size % 2 != 0)) {
        uint64_t last_idx = size - 1;
        z[last_idx] = x[last_idx] + y[last_idx];
    }
}
```

**Example Configuration**:

- blockNum: 64
- threadNumPerBlock: 1024
- Total threads: 65536
- half2 elements per thread: 128

**Performance Data**:

| Task Duration(us) | aiv_time(us) | aiv_total_cycles | aiv_vec_time(us) | aiv_vec_ratio | aiv_scalar_time(us) | aiv_scalar_ratio | aiv_read_hit_rate | aiv_write_hit_rate | aiv_main_mem_read_bw(GB/s) | aiv_main_mem_write_bw(GB/s) |
|:-----------------:|:------------:|:----------------:|:----------------:|:-------------:|:-------------------:|:----------------:|:-----------------:|:------------------:|:--------------------------:|:---------------------------:|
|      45.371       |    45.111    |     4584309      |      44.582      |     0.988     |        0.517        |      0.011       |         0         |       0.891        |           14.51            |            7.25             |

**Optimization Effect Analysis**:

- End-to-end latency: **45.371 us**
- Write cache hit rate: **89.1%**
- Read/write bandwidth: **14.51(GB/s) | 7.25(GB/s)**

**Principle Description**:  
In SIMT programming mode, L2 Cache accesses data in units of Cache Line = 128 bytes.

- Case 1 (scalar half): Unable to fill entire cache line causes RMW write amplification  
  Using scalar half (2B). When a single Warp (32 threads) issues instructions simultaneously, the total width concatenated spatially is:  
  $$
  32 * 2B = 64B
  $$
  The total width of simultaneously issued write instructions is only 64 bytes, which does not meet the Cache Line requirement, resulting in 50% bandwidth utilization.


- In Case 2, the short vector type half2 (4B) is used instead. The total write width per Warp (32 threads) per issue reaches:
  $$
  32 * 4B = 128B
  $$
  This width covers an entire physical Cache Line boundary, achieving 100% bandwidth utilization and maximizing transfer efficiency. Under the same physical fetch cycles, read bandwidth increases from 7.77 GB/s to 14.51 GB/s.


**Final Performance Summary**:
- **Latency**: Decreases from **86.478 us** to **45.371 us**, a reduction of **47.5%**, equivalent to an overall performance improvement of **1.91x**.
- **Cycle count**: Total CPU cycles decrease from **8,832,432** to **4,584,309**, a reduction of **48.1%**, confirming improved instruction issue density.
- **Main memory read/write bandwidth**: Both read and write bandwidth improve, with read bandwidth increasing by **86.7%**, from **7.77 GB/s** to **14.51 GB/s**, improving bandwidth utilization for memory-access-intensive operators.

---

### Optimization Key Points Summary

| Optimization Method | Core Principle              | Applicable Scenarios                                                                                                                                                                                                                                                                                                                                               |
|:-----|:------------------|:---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Short vector  | Underlying instructions support computing two data elements at once | 1. half/bfloat16_t type elements performing [basic operations](../../../../../docs/zh/guide/programming_guide/language_extension/simt_builtin_keywords.md#运算符), more short vector types see [complete type list](../../../../../docs/zh/api/SIMT-API/SIMT_programming_intro/extended_syntax/builtin_data_types.md#短向量类型) |

---

## Build and Run

Run the following steps in the root directory of this example to build and execute the example.
- Configure Environment Variables  
  Configure environment variables based on the [installation method](../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  ```bash
  # Scenario 2: Use half2 type
  SCENARIO_NUM=2
  mkdir -p build && cd build;                                               # Create and enter the build directory
  cmake .. -DSCENARIO_NUM=$SCENARIO_NUM; make -j;                           # Build the project
  python3 ../scripts/gen_data.py;                                           # Generate test input data
  ./demo                                                                    # Run the example
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin;  # Verify output results
  ```

  When using NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # NPU simulation mode
  ```

  > **Notice:** Before switching build modes, clean the cmake cache by running `rm CMakeCache.txt` in the build directory and then re-run cmake.

- Build Options Description

  | Parameter                        | Values             | Description                                                |
  |:--------------------------|:----------------|:--------------------------------------------------|
  | `SCENARIO_NUM`            | `1` (default), `2`     | 1: half Add computation; <br/>2: short vector half2 Add computation;             |
  | `CMAKE_ASC_RUN_MODE`      | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation                                 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`      | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |

- Execution Result  
  The following output indicates that the accuracy verification is successful.

  ```bash
  test pass!
  ```

## Performance Data Collection

Use the `msOpProf` tool to collect performance data on a single component:

```bash
msopprof ./demo   # Analyze case performance
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

View specific performance analysis results:

```
# For example, view L2 Cache related data
cat ./OPPROF_*/L2Cache.csv
```
