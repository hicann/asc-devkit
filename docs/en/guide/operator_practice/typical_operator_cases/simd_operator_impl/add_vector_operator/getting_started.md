# Entry-Level Feature Implementation<a name="ZH-CN_TOPIC_000000_ADD_ENTRY"></a>

<!-- md-trans-meta sourceCommit=8d74eb48f7034c3670e29f4f3be39c4b19949dc0 translatedAt=2026-11-09T10:43:14.473Z pushedAt=2026-11-19T06:32:19.837Z nl=2 -->

This section uses the Add (vector addition) operator as an example to describe the basic method of implementing a vector operator based on the Ascend C programming model, helping developers quickly get started with SIMD operator development.

## Operator Overview

The Add operator performs element-wise addition on two vectors. The compute formula is as follows:

$$
z_i = x_i + y_i
$$

- x: input, with a shape of [8, 2048] and a data type of float;
- y: input, with a shape of [8, 2048] and a data type of float;
- z: output, with a shape of [8, 2048] and a data type of float;

Sample running parameters: eight cores are used for computation, each core processes 2048 float elements (`blockLength = 2048`), and the total data volume is 16384 float elements.

## Operator Implementation

The computation logic of the Add operator follows the "move-in, compute, move-out" three-stage pipeline structure:

1. Move the input data x and y from [GM](../../../../../api/SIMD-API/basic_api/data_structures/GlobalTensor/GlobalTensor_intro.md) (Global Memory, the global memory outside the chip) to the [Unified Buffer (UB)](../../../../../api/SIMD-API/basic_api/data_structures/LocalTensor/LocalTensor_intro.md) (the on-chip cache dedicated to vector computation);
2. Perform vector addition on xLocal and yLocal in the UB, and store the computation result in zLocal;
3. Move the computation result from the UB back to the GM.

**Prerequisites**:

- [GM (Global Memory)](../../../../../api/SIMD-API/basic_api/data_structures/GlobalTensor/GlobalTensor_intro.md): The global storage outside the AI Core. Data is accessed through [GlobalTensor](../../../../../api/SIMD-API/basic_api/data_structures/GlobalTensor/GlobalTensor_intro.md). It has a large capacity but a relatively slow access speed.
- [UB](../../../../../api/SIMD-API/basic_api/data_structures/LocalTensor/LocalTensor_intro.md): The on-chip cache dedicated to vector computation inside the AI Core. Data is accessed through [LocalTensor](../../../../../api/SIMD-API/basic_api/data_structures/LocalTensor/LocalTensor_intro.md). It has a limited capacity but a fast access speed.
- [DataCopy](../../../../../api/SIMD-API/basic_api/memory_vector_compute/data_move/DataCopy_GMAndUB_continuous.md): The API used to move data between the GM and the UB.
- [PipeBarrier](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/PipeBarrier_ISASI.md): A pipeline synchronization barrier that ensures subsequent operations are executed only after data movement is complete.
- `block_idx`: Obtains the current core sequence number through [GetBlockIdx()](../../../../../api/SIMD-API/basic_api/tool_interface/system_resources_and_variables/GetBlockIdx.md), which is used for data sharding in multi-core parallelism.

The core code is as follows:

```cpp
template <uint32_t blockLength>
__vector__ __global__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    AscendC::InitSocState();

    // Global Tensor: Allocate input/output buffers on GM, and implement per-core sharding by block_idx offset
    AscendC::GlobalTensor<float> xGm, yGm, zGm;
    xGm.SetGlobalBuffer(x + block_idx * blockLength, blockLength);
    yGm.SetGlobalBuffer(y + block_idx * blockLength, blockLength);
    zGm.SetGlobalBuffer(z + block_idx * blockLength, blockLength);

    // Local Tensor: Allocate computation buffers on UB
    AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    AscendC::LocalTensor<float> xLocal = ubAllocator.Alloc<float, blockLength>();
    AscendC::LocalTensor<float> yLocal = ubAllocator.Alloc<float, blockLength>();
    AscendC::LocalTensor<float> zLocal = ubAllocator.Alloc<float, blockLength>();

    // GM -> UB: Move in the input data
    AscendC::DataCopy(xLocal, xGm, blockLength);
    AscendC::DataCopy(yLocal, yGm, blockLength);
    AscendC::PipeBarrier<PIPE_ALL>();

    // Vector computation: z = x + y
    AscendC::Add(zLocal, xLocal, yLocal, blockLength);
    AscendC::PipeBarrier<PIPE_ALL>();

    // UB -> GM: Move out the computation result
    AscendC::DataCopy(zGm, zLocal, blockLength);
    AscendC::PipeBarrier<PIPE_ALL>();
}
```

## Implementation Process Analysis

| Stage | Data Flow/Behavior | Purpose/Reason |
|:---:|:---|:---|
| Initialization | `InitSocState()` | Initialize the AI Core hardware state |
| GM address allocation | `SetGlobalBuffer(x + block_idx * blockLength)` | Each core processes its own data segment by offsetting with `block_idx`, implementing multi-core parallelism |
| UB space allocation | `ubAllocator.Alloc<float, blockLength>()` | Allocate computation buffers for x, y, and z on UB |
| Move-in (Stage 1) | GM → UB: `DataCopy(xLocal, xGm)`, `DataCopy(yLocal, yGm)` | Move input data from GM to UB, because the vector computation unit can only access UB data |
| Pipeline synchronization | `PipeBarrier<PIPE_ALL>()` | Ensure move-in is complete before computation to avoid reading unready data |
| Compute (Stage 2) | Compute on UB: `Add(zLocal, xLocal, yLocal)` | Perform vector addition on UB, using the vector unit to process multiple elements in parallel |
| Pipeline synchronization | `PipeBarrier<PIPE_ALL>()` | Ensure computation is complete before move-out |
| Move-out (Stage 3) | UB → GM: `DataCopy(zGm, zLocal)` | Move the result from UB back to GM for subsequent use |
| Pipeline synchronization | `PipeBarrier<PIPE_ALL>()` | Ensure move-out is complete to guarantee data consistency |

## Optimization Direction Analysis

| Sequence Number | Optimization Direction | Problem in the Current Implementation | Expected Optimization Gain |
|------|-----------|--------------|------------|
| 1 | Dynamic multi-core allocation | Fixed use of 8 cores without dynamic allocation based on the actual number of available cores | Dynamically obtain the number of available cores to fully leverage multi-core parallelism and reduce end-to-end latency |
| 2 | Increase transfer granularity | Each transfer moves 2048 float elements (8 KB), which is a relatively small granularity | Increase the amount of data moved per transfer to reduce the number of transfers, amortize startup overhead, and improve bandwidth utilization |
| 3 | Double-buffering pipeline parallelism | The move-in, compute, and move-out stages execute strictly serially, so the hardware units (MTE2/V/MTE3) cannot work simultaneously | Adopt the Ping-Pong double-buffering mechanism so that move-in, compute, and move-out can execute in parallel, hiding transfer latency |
| 4 | L2 Cache bypass | The Add input data is read only once, but it passes through the L2 Cache by default, increasing cache pollution | Set L2 Cache bypass for streaming-accessed data to reduce unnecessary cache overhead and improve transfer efficiency |

For the complete sample code, see the [Add introduction sample](../../../../../../../examples/01_simd_cpp_api/00_introduction/01_add/add).

## Function Debugging

### printf

This API provides formatted output in CPU domain and NPU domain debugging scenarios.

In the operator kernel function implementation code, call the [printf](../../../../../api/Utils-API/tuning_interface/printf.md) API at the location where log information needs to be output to print the relevant content.

The following is an example:

```cpp
AscendC::printf("add blockIdx=%d\n", AscendC::GetBlockIdx());
```

> **Note:** The **printf** (PRINTF) API printing function affects the actual running performance of the operator, and is usually used in the debugging phase. Developers can disable the printing function by setting `ASCENDC_DUMP=0` as required.

### DumpTensor

For operators developed based on the operator project, you can use this API to dump the content of a specified [LocalTensor](../../../../../api/SIMD-API/basic_api/data_structures/LocalTensor/LocalTensor_intro.md). It also supports printing custom additional information (only information of the uint32\_t data type), such as the current line number.

In the operator kernel function implementation code, call the [DumpTensor](../../../../../api/SIMD-API/basic_api/debug_interface/onboard_print/DumpTensor.md) API at the location where tensor data needs to be printed. The following is an example:

```cpp
// Vector computation: z = x + y
AscendC::Add(zLocal, xLocal, yLocal, blockLength);
AscendC::DumpTensor(zLocal, 1, 32);
```

> **Note:** The DumpTensor API printing function affects the actual running performance of the operator, and is usually used in the debugging phase. Developers can disable the printing function by setting `ASCENDC_DUMP=0` as required.

## Performance Debugging

### msOpProf Tool Introduction

The msOpProf tool is a single-operator performance analysis tool. It provides two usage modes: msopprof and msopprof simulator. This tool helps you locate anomalies in operator memory, operator code, and operator instructions, enabling comprehensive operator tuning. It currently supports performance data collection and automatic parsing based on different running modes (on-board or simulation) and different file forms (executable files or operator binary .o files).

- On-board performance collection

    Through on-board performance collection, you can directly measure the running time of an operator on the Ascend AI Processor. This mode is suitable for quickly locating operator performance issues in the board environment.

    Perform operator tuning through msopprof based on the executable file demo:

    ```
    msopprof ./demo
    ```

    - Performance data description  
      After the command is executed, a folder named "OPPROF_{timestamp}_XXX" is generated in the default directory. The following shows an example of the performance data folder structure:

      ```bash
      ├──dump                       # Raw performance data, which you do not need to pay attention to
      ├──ArithmeticUtilization.csv  # Cycle ratio of cube/vector instructions
      ├──L2Cache.csv                # L2 cache hit rate, which affects MTE2. It is recommended to properly plan the data movement logic to increase the hit rate
      ├──Memory.csv                 # Read/write bandwidth rates of UB, L1, and main memory
      ├──MemoryL0.csv               # Read/write bandwidth rates of L0A, L0B, and L0C
      ├──MemoryUB.csv               # Read/write bandwidth rates from Vector and Scalar to UB
      ├──OpBasicInfo.csv            # Basic operator information
      ├──PipeUtilization.csv        # Time consumption and proportion of compute units and copy units
      ├──ResourceConflictRatio.csv  # Proportion of bank group, bank conflict, and resource conflict rates on UB among all instructions
      └──visualize_data.bin         # File presented by MindStudio Insight
      ```

To view the specific performance analysis results:

```bash
# View the task duration and various data
cat ./OPPROF_*/PipeUtilization.csv
```

For details, see the operator tuning (msOpProf) content in [msOpProf User Guide](https://gitcode.com/Ascend/msopprof/blob/master/docs/en/user_guide/msopprof_user_guide.md).

For the complete performance tuning process, see [Add performance tuning example](../../../../../../../examples/01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance).
