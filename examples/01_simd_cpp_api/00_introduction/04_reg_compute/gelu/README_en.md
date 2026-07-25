# GELU Example

## Operator Overview

**Calculation Formula**:

The GELU approximation formula is:

$$
GELU(x) \approx 0.5 \cdot x \cdot \left(1 + \tanh\left(\sqrt{\frac{2}{\pi}} \cdot \left(x + 0.044715 \cdot x^3\right)\right)\right)
$$

Simplifying the formula, it expands into specific vector computation steps as follows:

$$
GELU(x) \approx \frac{x}{1 + e^{-1.595769 \cdot x - 0.071405 \cdot x^3}}
$$

**Input/Output Definition**:

| Matrix Name | Shape | Data Type | Format | Description |
|-----------|-------|-----------|--------|------|
| x (input) | [256, 32] | float | ND | Input tensor |
| y (output) | [256, 32] | float | ND | Output tensor |

**Example Running Parameters**:

- Input shape is `[256, 32]`, with a total of 8192 elements
- Fixed use of 2 Vector cores, with core partitioning only along the M direction (that is, the first dimension of shape, row direction)
- `totalM = 256` (total number of input data rows), `singleCoreM = 128` (number of rows processed by each core), each core processes `128 × 32 = 4096` elements
- The first 128 rows are processed by the first core, and the last 128 rows are processed by the second core

**Note**: GELU is a commonly used activation function in neural networks, used to replace ReLU for smoother gradient characteristics.

This example implements GELU computation using the RegBase programming approach. Compared to the MemBase approach (based on [LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md) + Compute API) used in the [01_add/add example](https://gitcode.com/cann/asc-devkit/blob/master/examples/01_simd_cpp_api/00_introduction/01_add/add/README.md), RegBase allows intermediate computation results to be temporarily stored in registers rather than in UB, reducing the number of UB read/write operations, which is suitable for multi-step fusion computation scenarios.

The complete data path is: GM → UB → Register (step-by-step computation) → UB → GM.

## Supported Products and CANN Software Versions for This Example

| Product | CANN Software Version |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## Operator Implementation

**Implementation Process Overview**:

This example follows the execution flow of "copy in — synchronize — compute — synchronize — copy out". Input data is copied from [GM](../../../../../docs/zh/guide/技术附录/概念原理和术语/术语表.md) (Global Memory) to [UB](../../../../../docs/zh/guide/技术附录/概念原理和术语/术语表.md) (Unified Buffer), then loaded from UB to registers through [asc_vf_call](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/VF调用/asc_vf_call.md) calling the VF function to complete step-by-step computation, and finally the results are written back from registers to UB, then copied from UB back to GM.

**Prerequisites**:

- **Memory Hierarchy and Data Path**: The core memory hierarchies involved in Ascend C programming include GM (Global Memory, located outside the chip with large capacity but high access latency), UB (Unified Buffer, located inside the chip with low access latency), and registers (closest to the compute units with the lowest latency but smallest capacity). Data needs to be moved level by level: GM → UB → Register → UB → GM.
- **Synchronization Mechanism**: Since data movement and computation are executed asynchronously by different hardware units, synchronization through the [SetFlag/WaitFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) mechanism is required for pipeline synchronization. `SetFlag` writes an event flag after a hardware unit completes an operation, and `WaitFlag` makes subsequent hardware units wait for that event to complete before starting execution, thus ensuring the correctness of data dependencies. This example uses two types of events:
  - `MTE2_V`: Indicates that after MTE2 (memory transfer engine) completes GM→UB transfer, V (vector compute unit) starts reading UB data
  - `V_MTE3`: Indicates that after V completes computation, MTE3 (write-back engine) writes UB results back to GM
- [PipeBarrier](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/PipeBarrier(ISASI).md) is used to wait for all pipeline stages in the current core to complete before exiting the kernel, ensuring data write-back completion.
- **RegBase Programming Paradigm**: Unlike traditional MemBase (based on [LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md) + Compute API), RegBase calls VF functions through [asc_vf_call](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/VF调用/asc_vf_call.md), using [RegTensor](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/寄存器数据类型/RegTensor.md) (register tensor) for computation within the function. VF functions are declared with `__simd_vf__`, and parameters and local variables are marked with `__ubuf__` for UB address space. Within VF functions, data is moved through [LoadAlign](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬入/连续对齐搬入（LoadAlign）.md) (UB→Register) and [StoreAlign](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬出/连续对齐搬出（StoreAlign）.md) (Register→UB), using [MaskReg](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/寄存器数据类型/MaskReg.md) (mask register) to control the number of elements for each computation, and updating the mask through [UpdateMask](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/寄存器数据类型/MaskReg.md) based on remaining element count. The advantage is that intermediate results can be temporarily stored in registers, reducing the number of UB read/write operations.

**Core Code Example**:

```cpp
// GELU formula coefficients
constexpr float COEFF_LINEAR = -1.595769f;
constexpr float COEFF_CUBIC = -0.071405f;

// VF function: Complete GELU step-by-step computation in registers
__simd_vf__ inline static void GeluVfMethod2(
    __ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t count, uint32_t loopNum)
{
    // Number of float elements that can be processed in one vector computation repeat (e.g., dav-3510: 256 bytes / 4 bytes = 64 elements)
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg;
    AscendC::Reg::RegTensor<float> yReg;
    AscendC::Reg::RegTensor<float> tmpReg;
    for (uint32_t i = 0; i < loopNum; ++i) {
        mask = AscendC::Reg::UpdateMask<float>(count);
        AscendC::Reg::LoadAlign(xReg, xAddr + i * oneRepeatSize);   // UB → Register
        AscendC::Reg::Mul(yReg, xReg, xReg, mask);                  // x²
        AscendC::Reg::Mul(yReg, yReg, xReg, mask);                  // x³
        AscendC::Reg::Muls(yReg, yReg, COEFF_CUBIC, mask);          // -0.071405 * x³
        AscendC::Reg::Muls(tmpReg, xReg, COEFF_LINEAR, mask);       // -1.595769 * x
        AscendC::Reg::Add(yReg, tmpReg, yReg, mask);                // -1.595769x - 0.071405x³
        AscendC::Reg::Exp(yReg, yReg, mask);                        // exp(...)
        AscendC::Reg::Adds(yReg, yReg, 1.0f, mask);                 // 1 + exp(...)
        AscendC::Reg::Div(yReg, xReg, yReg, mask);                  // x / (1 + exp(...))
        AscendC::Reg::StoreAlign(yAddr + i * oneRepeatSize, yReg, mask); // Register → UB
    }
}
```

The VF function above encapsulates the step-by-step computation logic of GELU in registers. Below is the complete kernel function, responsible for core partitioning, data movement, calling VF functions, and synchronization management.

```cpp
// Kernel function: Core partitioning logic + copy in/compute/copy out
template <uint32_t singleCoreLength>
__global__ __vector__ void gelu_custom(__gm__ uint8_t* x, __gm__ uint8_t* y)
{
    AscendC::InitSocState();
    // Core partitioning: Get current core index through GetBlockIdx, calculate data offset
    AscendC::GlobalTensor<float> xGm;
    AscendC::GlobalTensor<float> yGm;
    xGm.SetGlobalBuffer((__gm__ float*)x + AscendC::GetBlockIdx() * singleCoreLength);
    yGm.SetGlobalBuffer((__gm__ float*)y + AscendC::GetBlockIdx() * singleCoreLength);

    // Allocate UB buffer
    AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    AscendC::LocalTensor<float> xLocal = ubAllocator.Alloc<float, singleCoreLength>();
    AscendC::LocalTensor<float> yLocal = ubAllocator.Alloc<float, singleCoreLength>();

    // Stage 1: GM → UB copy in
    AscendC::DataCopy(xLocal, xGm[0], singleCoreLength);
    // Synchronization: Wait for GM→UB transfer to complete
    // EVENT_ID0 is the hardware event channel number (0-7), each event channel counts independently, this example uses only one channel so it is fixed to 0
    AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

    // Stage 2: Register computation
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);
    uint32_t loopNum = DivCeil(singleCoreLength, oneRepeatSize);
    __ubuf__ float* xAddr = reinterpret_cast<__ubuf__ float*>(xLocal.GetPhyAddr());
    __ubuf__ float* yAddr = reinterpret_cast<__ubuf__ float*>(yLocal.GetPhyAddr());
    asc_vf_call<GeluVfMethod2>(xAddr, yAddr, singleCoreLength, loopNum);
    // Synchronization: Wait for register computation to complete
    AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);

    // Stage 3: UB → GM copy out
    AscendC::DataCopy(yGm[0], yLocal, singleCoreLength);
    AscendC::PipeBarrier<PIPE_ALL>();
}
```

## Implementation Process Analysis

| Stage | Data Flow/Behavior | Implementation Purpose/Reason |
|------|-------------|-------------|
| Initialization | Call `InitSocState()` to initialize hardware state | Ensure hardware state is correctly reset before the core runs |
| Core Partitioning | Get current core index through [GetBlockIdx](../../../../../docs/zh/api/SIMD-API/基础API/工具接口/系统资源与变量/GetBlockIdx.md), calculate offset addresses for `xGm`/`yGm` | Each core only processes its own continuous data segment to avoid data overlap |
| UB Allocation | Use [LocalMemAllocator](../../../../../docs/zh/api/SIMD-API/基础API/资源管理/LocalMemAllocator/LocalMemAllocator简介.md) to request UB buffers `xLocal`/`yLocal` for the current core | UB is on-chip high-speed cache, providing data staging area for subsequent register computation |
| GM → UB Copy In | Call [DataCopy](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/数据搬运.md) to copy input data from GM to UB | Registers cannot directly access GM, data must first be copied to UB |
| MTE2_V Synchronization | `SetFlag<HardEvent::MTE2_V>` + `WaitFlag<HardEvent::MTE2_V>` | GM→UB transfer is executed asynchronously by the MTE2 engine, must wait for transfer to complete before the V unit can read UB data, otherwise incomplete transferred data will be read |
| Register Computation | Call `GeluVfMethod2` through [asc_vf_call](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/VF调用/asc_vf_call.md), complete [LoadAlign](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬入/连续对齐搬入（LoadAlign）.md) → multi-step Reg computation → [StoreAlign](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬出/连续对齐搬出（StoreAlign）.md) within VF function | RegBase API executes computation at register level with lowest latency; GELU formula needs to be decomposed into 8 computation steps, using the following APIs: Mul/Muls/Add/Exp/Adds/Div |
| V_MTE3 Synchronization | `SetFlag<HardEvent::V_MTE3>` + `WaitFlag<HardEvent::V_MTE3>` | Register computation is executed asynchronously by V pipeline, must wait for computation to complete before MTE3 engine can write UB results back to GM, otherwise incomplete computation results will be written out |
| UB → GM Copy Out | Call `DataCopy` to copy results from UB back to GM | Write computation results from on-chip cache back to global memory for subsequent use |
| Pipeline Synchronization | [PipeBarrier](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/PipeBarrier(ISASI).md)`<PIPE_ALL>()` | Wait for all pipeline stages (MTE2/V/MTE3) in the current core to complete before exiting the kernel, ensuring data write-back completion |

## Optimization Direction Analysis

| Optimization Direction | Problem with Current Implementation | Expected Optimization Benefit |
|-----------|--------------|------------|
| VF Fusion Dual-Issue Optimization | The current VF function has a long dependency path for GELU computation, although RegBase API is already called through `asc_vf_call`, the dual-issue characteristic of VF fusion is not fully utilized, and IPC (instructions issued per cycle) is low | Utilize VF fusion dual-issue characteristic, regular computation instructions (Mul/Muls/Add/Adds) can achieve parallelism of 512 bytes/cycle, vectorization instruction time can be reduced by **55%** or more. See [Gelu Performance Tuning Example Case 1](https://gitcode.com/cann/asc-devkit/blob/master/examples/01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_high_performance/README_en.md) |
| Loop Unrolling Optimization | The loop inside VF function is a simple for loop, the compiler cannot fully schedule instruction-level parallelism, and the number of instructions that can be dual-issued in the execution queue is limited | Use `#pragma unroll N` to unroll loops, improve instruction issue parallelism and IPC, on top of VF fusion, vector instruction time can be further reduced by about **4.6%**. The unrolling factor N needs to be tuned according to actual scenarios, too large will increase register pressure. See [Gelu Performance Tuning Example Case 2](https://gitcode.com/cann/asc-devkit/blob/master/examples/01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_high_performance/README_en.md) |
| Pipeline Serial Execution | The current copy in, compute, and copy out stages execute strictly serially, each hardware unit (MTE2/V/MTE3) cannot work simultaneously, data movement accounts for more than 90%, and the performance bottleneck is MTE2 bound | Adopt multi-buffer (Double Buffer/Triple Buffer) mechanism to enable parallel execution of copy in, compute, and copy out, improving hardware utilization and throughput |
| Fixed Core Count | Fixed use of 2 cores, not dynamically allocated according to actual available cores | Dynamically get available core count ([GetBlockNum](../../../../../docs/zh/api/SIMD-API/基础API/工具接口/系统资源与变量/GetBlockNum.md)), fully utilize multi-core parallel capability |

## Functional Debugging

### printf

This interface provides formatted output functionality for CPU domain/NPU domain debugging scenarios.

Call the printf interface in the operator kernel implementation code where log information needs to be output.

Example:

```cpp
AscendC::printf("gelu blockIdx=%d, singleCoreLength=%d\n", AscendC::GetBlockIdx(), singleCoreLength);
```

> **Note:** The printf (PRINTF) interface printing functionality has some impact on the actual performance of the operator and is typically used during the debugging phase. Developers can disable the printing functionality by setting `ASCENDC_DUMP=0` as needed.

### DumpTensor

For operators developed based on operator projects, this interface can be used to Dump the contents of a specified [LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md). It also supports printing custom additional information (only supports uint32_t data type information), such as printing the current line number.

Call the DumpTensor interface in the operator kernel implementation code where Tensor data needs to be printed. Example:

```cpp
// Dump output results after GELU computation completes
asc_vf_call<GeluVfMethod2>(xAddr, yAddr, singleCoreLength, loopNum);
AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
AscendC::DumpTensor(yLocal, 0, 16);
```

> **Note:** The DumpTensor interface printing functionality has some impact on the actual performance of the operator and is typically used during the debugging phase. Developers can disable the printing functionality by setting `ASCENDC_DUMP=0` as needed.

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

## Directory Structure

``` 
├── gelu
│   ├── scripts
│   │   ├── gen_data.py      // Input data and golden data generation script
│   │   └── verify_result.py // Output result and golden data verification script
│   ├── CMakeLists.txt       // Build project file
│   ├── data_utils.h         // Data read/write functions
│   ├── gelu.asc             // Ascend C example implementation & calling example
│   └── README.md            // Example documentation
```

## Build and Run

Execute the following steps in the root directory of this example to build and run the example.

- Configure environment variables  
  Configure environment variables according to the [installation method](../../../../../docs/zh/quick_start.md#prepare&install) of the CANN development toolkit on the current environment.
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **Note:** `${install_path}` is the CANN package installation directory. When no installation directory is specified, it defaults to `/usr/local/Ascend`.

- Execute example

  Execute the following commands in this example directory.
  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin
  ```

  When using NPU simulation mode, add the `-DCMAKE_ASC_RUN_MODE=sim` parameter.

  Example:
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j
  ```

- Build option description

  | Option | Available Values | Description |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu` (default), `sim` | Run mode: NPU execution, NPU simulation |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` (default) | NPU architecture: Ascend 950PR/Ascend 950DT |
  | `CMAKE_VF_MODE` | `true` (default), `false` | VF fusion mode: enable or disable `--cce-simd-vf-fusion` |


- Execution result  
  The execution result is as follows, indicating that the accuracy comparison succeeded.
  ```bash
  test pass!
  ```
