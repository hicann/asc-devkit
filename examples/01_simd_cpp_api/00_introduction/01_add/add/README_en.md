# Add Sample

## Overview

This sample demonstrates the basic usage of Ascend C vector addition.

## Supported Products and CANN Software Versions

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 Training Series Products/Atlas A3 Inference Series Products | >= CANN 9.0.0 |
| Atlas A2 Training Series Products/Atlas A2 Inference Series Products | >= CANN 9.0.0 |

## Directory Structure

```
├── add
│   ├── CMakeLists.txt      // Compilation project file
│   ├── add.asc             // Ascend C sample implementation & invocation sample
│   └── README.md           // Sample documentation
```

## Operator Overview

The Add operator implements element-wise addition of two vectors, with the computation formula:

$$
z_i = x_i + y_i
$$

- x: input, shape [8, 2048], data type float, data layout ND;
- y: input, shape [8, 2048], data type float, data layout ND;
- z: output, shape [8, 2048], data type float, data layout ND;

Sample running parameters: This sample uses 8 cores to complete the computation, each core processes 2048 elements (`blockLength = 2048`), with a total of 8×2048=16384 float elements.

## Operator Implementation

The Add operator's computation logic follows the three-stage pipeline structure of "load-compute-store":

1. Load input data x and y from [GM](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/GlobalTensor/GlobalTensor简介.md) (Global Memory, the external global memory of the chip) to [UB](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md) (Unified Buffer, the dedicated on-chip cache for vector computation);
2. Execute vector addition on xLocal and yLocal on UB, storing the result in zLocal;
3. Store the computation result from UB back to GM.

**Prerequisites**:

- [GM (Global Memory)](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/GlobalTensor/GlobalTensor简介.md): The global storage external to the AI Core, accessed via [GlobalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/GlobalTensor/GlobalTensor简介.md), with large capacity but slower access speed.
- [UB (Unified Buffer)](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md): The dedicated on-chip cache for vector computation inside the AI Core, accessed via [LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md), with limited capacity but fast access speed.
- [DataCopy](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopy（GM与UB连续数据搬运）.md): The API used for data transfer between GM and UB, with the transfer direction determined by the parameter order.
- [PipeBarrier](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/PipeBarrier(ISASI).md): Pipeline synchronization barrier, ensuring data transfer completes before subsequent operations to avoid read/write conflicts.
- `block_idx`: A built-in variable representing the current core's index (equivalent to [GetBlockIdx()](../../../../../docs/zh/api/SIMD-API/基础API/工具接口/系统资源与变量/GetBlockIdx.md)), used for data partitioning in multi-core parallel computation.

The core code is as follows:

```cpp
template <uint32_t blockLength>
__vector__ __global__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    AscendC::InitSocState();

    // Global Tensor: Allocate input/output buffers on GM
    AscendC::GlobalTensor<float> xGm, yGm, zGm;
    xGm.SetGlobalBuffer(x + block_idx * blockLength, blockLength);  // Each core processes its own data segment by block_idx offset
    yGm.SetGlobalBuffer(y + block_idx * blockLength, blockLength);
    zGm.SetGlobalBuffer(z + block_idx * blockLength, blockLength);

    // Local Tensor: Allocate computation buffers on UB
    AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    AscendC::LocalTensor<float> xLocal = ubAllocator.Alloc<float, blockLength>();
    AscendC::LocalTensor<float> yLocal = ubAllocator.Alloc<float, blockLength>();
    AscendC::LocalTensor<float> zLocal = ubAllocator.Alloc<float, blockLength>();

    // GM -> UB: Load input data
    AscendC::DataCopy(xLocal, xGm, blockLength);
    AscendC::DataCopy(yLocal, yGm, blockLength);
    AscendC::PipeBarrier<PIPE_ALL>();  // Ensure load completes before computation

    // Vector computation: z = x + y
    AscendC::Add(zLocal, xLocal, yLocal, blockLength);
    AscendC::PipeBarrier<PIPE_ALL>();  // Ensure computation completes before store

    // UB -> GM: Store computation result
    AscendC::DataCopy(zGm, zLocal, blockLength);
    AscendC::PipeBarrier<PIPE_ALL>();  // Ensure store completes
}
```

Invocation method: Use the kernel invocation operator `<<<numBlocks, 0, stream>>>` to call the kernel function, with `numBlocks=8` specifying 8 cores for parallel execution.

## Implementation Flow Analysis

| Stage | Data Flow/Behavior | Purpose/Reason |
|:---:|:---|:---|
| Initialization | `InitSocState()` | Initialize AI Core hardware state, prepare for subsequent operations |
| GM Address Allocation | `SetGlobalBuffer(x + block_idx * blockLength, blockLength)` | Each core calculates offset based on `block_idx` to process different data segments, enabling multi-core parallelism |
| UB Space Allocation | `ubAllocator.Alloc<float, blockLength>()` | Allocate contiguous memory blocks on UB for x, y, z for vector computation |
| Load (Stage 1) | GM → UB: `DataCopy(xLocal, xGm)`, `DataCopy(yLocal, yGm)` | Transfer input data from GM to UB, since the vector computation unit can only access data on UB |
| Pipeline Sync | `PipeBarrier<PIPE_ALL>()` | Ensure load completes before computation begins, preventing the computation unit from reading unready data |
| Compute (Stage 2) | UB computation: `Add(zLocal, xLocal, yLocal)` | Execute vector addition on UB, leveraging the vector unit to process multiple elements in parallel |
| Pipeline Sync | `PipeBarrier<PIPE_ALL>()` | Ensure computation completes before store begins, preventing storing incomplete results |
| Store (Stage 3) | UB → GM: `DataCopy(zGm, zLocal)` | Transfer computation results from UB back to GM for subsequent use or output |
| Pipeline Sync | `PipeBarrier<PIPE_ALL>()` | Ensure store completes, guaranteeing data consistency |

## Optimizable Directions Analysis

| No. | Optimizable Direction | Current Implementation Issue | Expected Optimization Benefit |
|------|-----------|--------------|------------|
| 1 | Multi-core dynamic allocation | Fixed use of 8 cores, not dynamically allocating based on actual available core count | Dynamically obtain available core count, fully utilize multi-core parallelism, reduce end-to-end latency |
| 2 | Increase transfer granularity | Each transfer of 2048 float elements (8KB), relatively small transfer granularity | Increase single transfer data volume, reduce transfer count, amortize startup overhead, improve bandwidth utilization |
| 3 | Dual-buffer pipeline parallelism | Load, compute, and store stages execute strictly sequentially, hardware units (MTE2/V/MTE3) cannot work simultaneously | Adopt Ping-Pong dual-buffer mechanism, enabling load, compute, and store to execute in parallel, hiding transfer latency |
| 4 | L2 Cache bypass | Add input data is read only once, but passes through L2 Cache by default, increasing Cache pollution | Set L2 Cache bypass for streaming access data, reduce unnecessary Cache overhead, improve transfer efficiency |

For the complete performance tuning process, please refer to the [Add Performance Tuning Sample](https://gitcode.com/cann/asc-devkit/tree/master/examples/01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance).

## Functional Debugging

### printf

This interface provides formatted output functionality for CPU/NPU domain debugging scenarios.

Call the [printf](../../../../../docs/zh/api/SIMD-API/基础API/调试接口/上板打印/printf.md) interface at the location in the operator kernel-side implementation code where log information needs to be output.

Example:

```cpp
AscendC::printf("add blockIdx=%d\n", AscendC::GetBlockIdx());
```

> **Note:** The printf (PRINTF) interface printing function has a certain impact on the actual running performance of the operator and is typically used during the debugging phase. Developers can optionally disable the printing function by setting `ASCENDC_DUMP=0`.

### DumpTensor

For operators developed based on operator projects, this interface can be used to Dump the contents of a specified [LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md). It also supports printing custom additional information (only uint32\_t data type information is supported), such as printing the current line number.

Call the [DumpTensor](../../../../../docs/zh/api/SIMD-API/基础API/调试接口/上板打印/DumpTensor.md) interface at the location in the operator kernel-side implementation code where Tensor data needs to be printed. Example:

```cpp
// Vector computation: z = x + y
AscendC::Add(zLocal, xLocal, yLocal, blockLength);
AscendC::DumpTensor(zLocal, 1, 32);
```

> **Note:** The DumpTensor interface printing function has a certain impact on the actual running performance of the operator and is typically used during the debugging phase. Developers can optionally disable the printing function by setting `ASCENDC_DUMP=0`.

## Performance Debugging

### Introduction to the msOpProf Tool

msOpProf is a single-operator performance analysis tool with two usage modes: `msopprof` and `msopprof simulator`. It helps users identify anomalies in operator memory, code, and instructions for comprehensive operator tuning. It currently supports performance data collection and automatic parsing for different run modes (on-device or simulation) and file types (executables or operator binary `.o` files).

- On-device performance collection

    On-device performance collection directly measures the operator's execution time on the Ascend AI Processor. This method is suitable for quickly locating operator performance issues in an on-device environment.

    Run msopprof on the `demo` executable for operator tuning:
    ```
    msopprof ./demo
    ```

    - Performance data description  
      After the command completes, a folder named "OPPROF_{timestamp}_XXX" will be generated in the default directory. The performance data folder structure is as follows:

      ```bash
      ├──dump                       # Raw performance data; users do not need to inspect it
      ├──ArithmeticUtilization.csv  # Cube/Vector instruction cycle proportions
      ├──L2Cache.csv                # L2 Cache hit rate; affects MTE2. Plan data transfer logic properly to increase the hit rate
      ├──Memory.csv                 # Read/write bandwidth rates of UB, L1, and main memory
      ├──MemoryL0.csv               # Read/write bandwidth rates of L0A, L0B, and L0C
      ├──MemoryUB.csv               # Read/write bandwidth rates from Vector and Scalar to UB
      ├──OpBasicInfo.csv            # Basic operator information
      ├──PipeUtilization.csv        # Durations and proportions of computation and data transfer units
      ├──ResourceConflictRatio.csv  # Proportions of UB bank groups, bank conflicts, and resource conflicts among all instructions
      └──visualize_data.bin         # MindStudio Insight presentation file
      ```

View the detailed performance analysis results:

```bash
# View Task Duration and other metrics
cat ./OPPROF_*/PipeUtilization.csv
```

For details, refer to the operator tuning (msOpProf) content in the [msOpProf User Guide](https://gitcode.com/Ascend/msopprof/blob/master/docs/zh/user_guide/msopprof_user_guide.md).

## Compilation and Execution

Execute the following steps in the root directory of this sample to compile and execute the sample.

- Configure Environment Variables
  Configure environment variables according to the [installation method](../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development kit package on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When the installation directory is not specified, it defaults to `/usr/local/Ascend`.

- Sample Execution

  Execute the following commands in this sample directory.
  ```bash
  mkdir -p build && cd build;      # Create and enter the build directory
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # Compile the project, default npu mode
  ./demo                           # Execute the sample
  ```

  When using CPU Debug or NPU Simulation mode, add the `-DCMAKE_ASC_RUN_MODE=cpu` or `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Examples:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU Debug mode
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU Simulation mode
  ```

  > **Attention:** You need to clear the cmake cache before switching compilation modes. Execute `rm CMakeCache.txt` in the build directory and then run cmake again.

- Compilation Options Description

  | Option | Available Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `cpu`, `sim` | Run mode: NPU execution, CPU debug, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` (default), `dav-3510` | NPU architecture: dav-2201 corresponds to Atlas A2 Training Series Products/Atlas A2 Inference Series Products and Atlas A3 Training Series Products/Atlas A3 Inference Series Products, dav-3510 corresponds to Ascend 950PR/Ascend 950DT |

- Execution Results
  The execution result is as follows, indicating that the accuracy comparison succeeds.
  ```bash
  test pass!
  ```
