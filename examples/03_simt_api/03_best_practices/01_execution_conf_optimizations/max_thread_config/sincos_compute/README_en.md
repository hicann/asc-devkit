# SinCosCompute Performance Tuning Example


## Overview

This example uses sincos computation to demonstrate thread configuration optimization strategies in the Ascend C SIMT programming model. The example includes 1 baseline version and 1 optimized version. In the baseline version, `__launch_bounds__` is not set, and the compiler allocates resources based on the default value of 1024 (that is, 1024 threads per Block), causing register spill. The optimized version configures `__launch_bounds__(512)` to inform the compiler that the maximum number of threads per Block is 512, allowing the compiler to fully utilize hardware resources during compilation and avoid register spill, demonstrating the tuning path of optimizing performance through proper thread count configuration in the SIMT programming model.

## Supported Products

- Ascend 950PR/Ascend 950DT

## Supported CANN Software Version

- \>= CANN 9.1.0

## Directory Structure

```text
sincos_compute/
│   ├── CMakeLists.txt              // CMake build file
│   ├── sincos_compute.asc          // sincos example implementation
│   └── README.md
```

## Example Description

- Example Function

  Use the `sincosf()` function to compute sin and cos results simultaneously

  ```
  sincosf(input[idx], output_sin + idx, output_cos + idx)
  ```

- Example Specifications:

  <table>
  <tr><td rowspan="1" align="center">Example Type (OpType)</td><td colspan="4" align="center">SinCosCompute</td></tr>
  <tr><td rowspan="2" align="center">Example Input</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[393216]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">Example Output</td><td align="center">output_sin</td><td align="center">[393216]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">output_cos</td><td align="center">[393216]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">Kernel Function Name</td><td colspan="4" align="center">sincos_thread_1024 / sincos_thread_512</td></tr>
  </table>

## Example Implementation

### sincos Computation Description

This example uses the sincosf interface provided by Ascend C to compute sin/cos results simultaneously. A fixed shape is set in the example, with each thread computing 16 input values. The computation flow is as follows:
1. Based on the operator data shape partitioning logic, compute the starting address of each core
2. Call the sincosf interface provided by Ascend C to compute sin/cos results simultaneously

### Thread Count and Register Relationship

In the SIMT programming model, the maximum thread count configured during kernel function definition directly affects the number of registers available per thread:

| Maximum Thread Count | Available Registers per Thread |
|--------------|--------------------------|
| 1025~2048 | 16 |
| 513~1024 | 32 |
| 257~512 | 64 |
| 1~256 | 127 |

**Key Principles**:
- The larger the configured maximum thread count, the fewer registers available per thread
- For compute-intensive operators, a single thread typically requires more registers; it is generally recommended to configure 512 or 1024 threads
- For data transfer operators, a single thread typically requires fewer registers; it is generally recommended to configure 2048 threads
- When registers are insufficient to hold all temporary variables, register spill occurs, and data spills to stack space (Global Memory), causing performance degradation

### Example Implementation Description

This example demonstrates the effect of `__launch_bounds__` through 2 independent kernels, each corresponding to a specific case version.

| Case   | Implementation Characteristics                                                                 | Kernel Function Used               | Optimization Features           |
|--------|----------------------------------------------------------------------|----------------------------|---------------------|
| Case 0 | Do not set launch bounds, use default value                                | sincos_thread_1024  | Baseline version, `__launch_bounds__` not configured          |
| Case 1 | Configure `__launch_bounds__(512)` based on the actual operator computation scale | sincos_thread_512 | Compiler uses user-specified configuration values for optimization |

---
### Performance Metrics Description

| Metric                  | Description                          |
|---------------------|-----------------------------|
| Task Duration(us)   | Total execution time of the entire task; operator execution time is determined by this parameter     |
| DCache Read GM      | Number of times DCache reads data from Global Memory |
| DCache Read Vector  | Number of times Vector Core reads data from DCache   |
| DCache Write Vector | Number of times Vector Core writes data to DCache   |

---

### Case 0: Baseline Version (Register Spill)

**Example Goal**: Do not configure `__launch_bounds__`, observe the impact of register spill on performance

**Core Implementation**:
- Default thread count is 1024, with only 32 registers available per Thread
- sincosf computation requires more registers, exceeding the 32-register limit

**Key Code**:

```cpp
__global__ void sincos_thread_1024(float* input, float* output_sin, 
                                    float* output_cos, uint64_t total_length)
{
    int32_t blk_start_idx = blockIdx.x * THREADS_PER_BLOCK * PER_THREAD_LOOP;
    
    // Each core computes PER_THREAD_LOOP * THREADS_PER_BLOCK operations
    for (int i = 0; i < PER_THREAD_LOOP; i++) {
        int idx = blk_start_idx + i * THREADS_PER_BLOCK + threadIdx.x;
        sincosf(input[idx], output_sin + idx, output_cos + idx);
    }
}
```

**Compilation Information**:

Use the `--cce-res-usage` compilation option to view register usage:

```
[BISHENG] Function properties for _Z18sincos_thread_1024PfS_S_m_simt_entry: Stack size: 32 bytes, Used register number: 32
```

**Analysis**:
- Stack size: 32 bytes -> register spill exists
- Used register number: 32 -> reaches the register limit at 1024 threads
- Register spill causes intermediate data to be stored in Global Memory, increasing memory access overhead

**Performance Data**:

| Task Duration(us) | DCache Read GM (count) | DCache Read Vector (count) | DCache Write Vector (count) |
|:-----------------:|:-----------------:|:---------------------:|:----------------------:|
|       102.47       |       256        |         640          |          768          |

**Performance Bottleneck**:
- Register spill to Global Memory
- Additional stack space access increases latency
- DCache Read Vector / DCache Write Vector counts are relatively high (640 / 768 times)

Optimization direction: Use `__launch_bounds__` to inform the compiler of the actual `blockDim`, fully utilize register resources, and avoid register spill.

---

### Case 1: Optimized Version (Avoid Register Spill)

**Optimization Goal**: Avoid register spill by configuring `__launch_bounds__(512)`, fully utilize register resources, and improve performance

**Core Optimization**:
- Specify `__launch_bounds__(512)`, with 64 registers available per Thread
- Registers required for sincosf computation are within the limit

**Key Code**:

```cpp
__global__ __launch_bounds__(512) void sincos_thread_512(float* input, 
                                    float* output_sin, float* output_cos,
                                    uint64_t total_length)
{
    int32_t blk_start_idx = blockIdx.x * THREADS_PER_BLOCK * PER_THREAD_LOOP;
    
    // Each core computes PER_THREAD_LOOP * THREADS_PER_BLOCK operations
    for (int i = 0; i < PER_THREAD_LOOP; i++) {
        int idx = blk_start_idx + i * THREADS_PER_BLOCK + threadIdx.x;
        sincosf(input[idx], output_sin + idx, output_cos + idx);
    }
}
```

**Compilation Information**:

```
[BISHENG] Function properties for _Z17sincos_thread_512PfS_S_m_simt_entry: Stack size: 0 bytes, Used register number: 48
```

**Analysis**:
- Stack size: 0 bytes -> no register spill
- Used register number: 48 -> within the 64-register limit
- All intermediate data is kept in registers, avoiding Global Memory access

**Performance Data**:

| Task Duration(us) | DCache Read GM (count) | DCache Read Vector (count) | DCache Write Vector (count) |
|:-----------------:|:-----------------:|:---------------------:|:----------------------:|
|      96.22       |       256        |         512          |          256          |

**Optimization Effect Analysis**:
- Task Duration decreases from 102.47us to 96.22us, a reduction of approximately **6.1%**
- DCache Read GM remains unchanged, indicating no additional overhead
- DCache Read Vector decreases from 640 to 512, DCache Write Vector decreases from 768 to 256, indicating that without register spill, Data Cache read/write counts decrease (the physical location of the stack is in Global Memory, so stack access during register spill is reflected in Data Cache access counts)
- Registers are fully utilized, avoiding data spill to Global Memory

---

## Performance Comparison Summary

### Ascend 950PR Performance Data

**Overall Optimization Effect**:
- From Case 0 baseline to Case 1 optimized version, Task Duration decreases from 102.47us to 96.22us, a reduction of approximately 6.1%
- DCache Read Vector decreases from 640 to 512, DCache Write Vector decreases from 768 to 256

| Case version | Task Duration(us) | Task Duration Relative to Case 0 | Optimization Points                   |
|--------------|-------------------|-----------------------|-----------------------|
| Case 0       | 102.47             | **1x**                | Baseline version, register spill to Global Memory |
| Case 1       | 96.22            | **0.94x latency**           | Configure launch bounds to avoid register spill |

## Tuning Recommendations

1. **Identify register spill**: Use the `--cce-res-usage` compilation option to view register usage
   - Stack size > 0: register spill exists
   - Stack size = 0: no register spill

2. **Configure thread count appropriately**:
   - Compute-intensive operators: recommend 512 or 1024 threads
   - Data transfer operators: recommend 2048 threads
   - Select appropriate thread count configuration based on the register requirement table

3. **Use `__launch_bounds__` to inform the compiler**:
   ```cpp
   __global__ __launch_bounds__(thread_count) void kernel_name(...)
   ```

4. **Verify optimization effect**:
   - Compare compilation information before and after optimization (Stack size and Used register number)
   - Compare performance data before and after optimization (Task Duration, DCache Read GM, DCache Read Vector, DCache Write Vector)

## Build and Run

Run the following steps in the root directory of this example to build and execute the example.

- Configure Environment Variables  
  Configure environment variables based on the [installation method](../../../../../../docs/en/quick_start.md#prepare&install) of the CANN development kit in the current environment.

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, the default installation path is `/usr/local/Ascend`.

- Run the Example

  ```bash
  mkdir -p build && cd build;          # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;  # Build the project
  ./sincos_compute 1024                    # Run the baseline example
  ./sincos_compute 512                     # Run the optimized example
  ```

- Build Options Description

  | Option                        | Values        | Description                                                |
  |---------------------------|------------|---------------------------------------------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU architecture: this example only supports dav-3510 (Ascend 950PR/Ascend 950DT) |

  The following output indicates that the accuracy verification is successful.

  ```text
  [Success] Case accuracy is verification passed.
  ```

## Performance Analysis

Use the `msOpProf` tool to collect detailed performance data:

```bash
msopprof ./sincos_compute 1024   # Analyze baseline case performance
msopprof ./sincos_compute 512    # Analyze optimized case performance
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

```bash
# For example, view operator basic information
cat ./OPPROF_*/OpBasicInfo.csv

# For example, view memory-related data
cat ./OPPROF_*/Memory.csv
```
