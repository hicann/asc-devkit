# GELU样例

## 算子概述

**计算公式**：

GELU 近似计算公式为：

$$
GELU(x) \approx 0.5 \cdot x \cdot \left(1 + \tanh\left(\sqrt{\frac{2}{\pi}} \cdot \left(x + 0.044715 \cdot x^3\right)\right)\right)
$$

对公式进行简化，采用如下方式展开为具体的向量运算步骤：

$$
GELU(x) \approx \frac{x}{1 + e^{-1.595769 \cdot x - 0.071405 \cdot x^3}}
$$

**输入/输出定义**：

| 矩阵名称 | Shape | Data Type | Format | 说明 |
|-----------|-------|-----------|--------|------|
| x（输入） | [256, 32] | float | ND | 输入张量 |
| y（输出） | [256, 32] | float | ND | 输出张量 |

**样例运行参数**：

- 输入 shape 为 `[256, 32]`，总元素数为 8192
- 固定使用 2 个 Vector 核，仅按 M 方向（即 shape 的第一维，行方向）分核
- `totalM = 256`（输入数据总行数），`singleCoreM = 128`（每个核处理的行数），每个核处理 `128 × 32 = 4096` 个元素
- 前 128 行由第 1 个核处理，后 128 行由第 2 个核处理

**说明**：GELU 是神经网络中常用的激活函数，用于替代 ReLU 以获得更平滑的梯度特性。

本样例采用 RegBase 编程方式实现 GELU 计算——与 [01_add/add 样例](https://gitcode.com/cann/asc-devkit/blob/master/examples/01_simd_cpp_api/00_introduction/01_add/add/README.md) 使用的 MemBase（基于 [LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md) + Compute API）方式相比，RegBase 允许中间计算结果暂存在寄存器而非 UB 中，减少了 UB 读写次数，适合多步骤融合计算场景。

完整的数据通路为：GM → UB → 寄存器（逐步计算）→ UB → GM。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 算子实现

**实现流程概述**：

本样例遵循"搬入 — 同步 — 计算 — 同步 — 搬出"的执行流程。输入数据从 [GM](../../../../../docs/zh/guide/技术附录/概念原理和术语/术语表.md)（Global Memory，全局内存）搬运到 [UB](../../../../../docs/zh/guide/技术附录/概念原理和术语/术语表.md)（Unified Buffer，片上统一缓冲区），再通过 [asc_vf_call](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/VF调用/asc_vf_call.md) 调用 VF 函数将数据从 UB 加载到寄存器完成逐步计算，最后将结果从寄存器写回 UB，再从 UB 搬运回 GM。

**前置说明**：

- **内存层级与数据通路**：Ascend C 编程涉及的核心内存层级包括 GM（全局内存，位于芯片外部，容量大但访问延迟高）、UB（片上统一缓冲区，位于芯片内部，访问延迟低）和寄存器（最靠近计算单元，延迟最低但容量最小）。数据需逐级搬运：GM → UB → 寄存器 → UB → GM。
- **同步机制**：由于数据搬运和计算由不同硬件单元异步执行，需要通过 [SetFlag/WaitFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) 机制进行流水同步。`SetFlag` 在某一硬件单元完成操作后写入事件标志，`WaitFlag` 让后续硬件单元等待该事件完成后再开始执行，从而保证数据依赖关系的正确性。本样例中使用了两种事件：
  - `MTE2_V`：表示 MTE2（内存搬运引擎）完成 GM→UB 搬运后，V（向量计算单元）再开始读取 UB 数据
  - `V_MTE3`：表示 V 完成计算后，MTE3（回写引擎）再将 UB 结果写回 GM
- [PipeBarrier](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/PipeBarrier(ISASI).md) 用于等待本核所有流水阶段全部完成后再退出 kernel，确保数据写回完成。
- **RegBase 编程范式**：与传统 MemBase（基于 [LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md) + Compute API）不同，RegBase 通过 [asc_vf_call](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/VF调用/asc_vf_call.md) 调用 VF 函数，在函数内使用 [RegTensor](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/寄存器数据类型/RegTensor.md)（寄存器张量）完成计算。VF 函数以 `__simd_vf__` 声明，参数和局部变量通过 `__ubuf__` 标记 UB 地址空间。VF 函数内数据通过 [LoadAlign](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬入/连续对齐搬入（LoadAlign）.md)（UB→寄存器）和 [StoreAlign](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬出/连续对齐搬出（StoreAlign）.md)（寄存器→UB）搬运，使用 [MaskReg](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/寄存器数据类型/MaskReg.md)（掩码寄存器）控制每次计算的元素数量，通过 [UpdateMask](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/寄存器数据类型/MaskReg.md) 根据剩余元素数更新掩码。优势是中间结果可暂存在寄存器，减少 UB 读写次数。

**核心代码示例**：

```cpp
// GELU 公式系数
constexpr float COEFF_LINEAR = -1.595769f;
constexpr float COEFF_CUBIC = -0.071405f;

// VF 函数：在寄存器中完成 GELU 逐步计算
__simd_vf__ inline static void GeluVfMethod2(
    __ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t count, uint32_t loopNum)
{
    // 一次向量计算repeat可处理的float元素数（如dav-3510为256字节/4字节=64个元素）
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg;
    AscendC::Reg::RegTensor<float> yReg;
    AscendC::Reg::RegTensor<float> tmpReg;
    for (uint32_t i = 0; i < loopNum; ++i) {
        mask = AscendC::Reg::UpdateMask<float>(count);
        AscendC::Reg::LoadAlign(xReg, xAddr + i * oneRepeatSize);   // UB → 寄存器
        AscendC::Reg::Mul(yReg, xReg, xReg, mask);                  // x²
        AscendC::Reg::Mul(yReg, yReg, xReg, mask);                  // x³
        AscendC::Reg::Muls(yReg, yReg, COEFF_CUBIC, mask);          // -0.071405 * x³
        AscendC::Reg::Muls(tmpReg, xReg, COEFF_LINEAR, mask);       // -1.595769 * x
        AscendC::Reg::Add(yReg, tmpReg, yReg, mask);                // -1.595769x - 0.071405x³
        AscendC::Reg::Exp(yReg, yReg, mask);                        // exp(...)
        AscendC::Reg::Adds(yReg, yReg, 1.0f, mask);                 // 1 + exp(...)
        AscendC::Reg::Div(yReg, xReg, yReg, mask);                  // x / (1 + exp(...))
        AscendC::Reg::StoreAlign(yAddr + i * oneRepeatSize, yReg, mask); // 寄存器 → UB
    }
}
```

上述 VF 函数封装了 GELU 在寄存器中的逐步计算逻辑。下面是完整的核函数，负责分核、数据搬运、调用 VF 函数和同步管理。

```cpp
// 核函数：分核逻辑 + 搬入/计算/搬出
template <uint32_t singleCoreLength>
__global__ __vector__ void gelu_custom(__gm__ uint8_t* x, __gm__ uint8_t* y)
{
    AscendC::InitSocState();
    // 分核：通过 GetBlockIdx 获取当前核索引，计算数据偏移
    AscendC::GlobalTensor<float> xGm;
    AscendC::GlobalTensor<float> yGm;
    xGm.SetGlobalBuffer((__gm__ float*)x + AscendC::GetBlockIdx() * singleCoreLength);
    yGm.SetGlobalBuffer((__gm__ float*)y + AscendC::GetBlockIdx() * singleCoreLength);

    // 分配 UB 缓存
    AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    AscendC::LocalTensor<float> xLocal = ubAllocator.Alloc<float, singleCoreLength>();
    AscendC::LocalTensor<float> yLocal = ubAllocator.Alloc<float, singleCoreLength>();

    // Stage 1: GM → UB 搬入
    AscendC::DataCopy(xLocal, xGm[0], singleCoreLength);
    // 同步：等待 GM→UB 搬运完成
    // EVENT_ID0 是硬件事件通道编号（0-7），每个事件通道独立计数，本样例仅使用一个通道故固定使用 0
    AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);

    // Stage 2: 寄存器计算
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);
    uint32_t loopNum = DivCeil(singleCoreLength, oneRepeatSize);
    __ubuf__ float* xAddr = reinterpret_cast<__ubuf__ float*>(xLocal.GetPhyAddr());
    __ubuf__ float* yAddr = reinterpret_cast<__ubuf__ float*>(yLocal.GetPhyAddr());
    asc_vf_call<GeluVfMethod2>(xAddr, yAddr, singleCoreLength, loopNum);
    // 同步：等待寄存器计算完成
    AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
    AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);

    // Stage 3: UB → GM 搬出
    AscendC::DataCopy(yGm[0], yLocal, singleCoreLength);
    AscendC::PipeBarrier<PIPE_ALL>();
}
```

## 实现流程解析

| 阶段 | 数据流动/行为 | 实现目的/原因 |
|------|-------------|-------------|
| 初始化 | 调用 `InitSocState()` 初始化硬件状态 | 确保核运行前硬件状态正确复位 |
| 分核 | 通过 [GetBlockIdx](../../../../../docs/zh/api/SIMD-API/基础API/工具接口/系统资源与变量/GetBlockIdx.md) 获取当前核索引，计算 `xGm`/`yGm` 的偏移地址 | 每个核只处理自己对应的连续数据段，避免数据重叠 |
| UB 分配 | 使用 [LocalMemAllocator](../../../../../docs/zh/api/SIMD-API/基础API/资源管理/LocalMemAllocator/LocalMemAllocator简介.md) 为当前核申请 UB 缓存 `xLocal`/`yLocal` | UB 是片上高速缓存，为后续寄存器计算提供数据暂存区 |
| GM → UB 搬入 | 调用 [DataCopy](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/数据搬运.md) 将输入数据从 GM 搬运到 UB | 寄存器无法直接访问 GM，必须先将数据搬运到 UB |
| MTE2_V 同步 | `SetFlag<HardEvent::MTE2_V>` + `WaitFlag<HardEvent::MTE2_V>` | GM→UB 搬运由 MTE2 引擎异步执行，必须等待搬运完成后 V 单元才能读取 UB 中的数据，否则会读到未完成搬运的脏数据 |
| 寄存器计算 | 通过 [asc_vf_call](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/VF调用/asc_vf_call.md) 调用 `GeluVfMethod2`，在 VF 函数内完成 [LoadAlign](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬入/连续对齐搬入（LoadAlign）.md) → 多步 Reg 计算 → [StoreAlign](../../../../../docs/zh/api/SIMD-API/基础API/Reg矢量计算/Reg数据搬出/连续对齐搬出（StoreAlign）.md) | RegBase API 在寄存器级别执行计算，延迟最低；GELU 公式需分解为 Mul/Muls/Add/Exp/Adds/Div 共 8 步逐步完成 |
| V_MTE3 同步 | `SetFlag<HardEvent::V_MTE3>` + `WaitFlag<HardEvent::V_MTE3>` | 寄存器计算由 V 流水异步执行，必须等待计算完成后 MTE3 引擎才能将 UB 结果写回 GM，否则会写出不完整的计算结果 |
| UB → GM 搬出 | 调用 `DataCopy` 将结果从 UB 搬运回 GM | 将计算结果从片上缓存写回全局内存，供后续使用 |
| 流水同步 | [PipeBarrier](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/PipeBarrier(ISASI).md)`<PIPE_ALL>()` | 等待本核所有流水阶段（MTE2/V/MTE3）全部完成后再退出 kernel，确保数据写回完成 |

## 可优化方向分析

| 可优化方向 | 当前实现的问题 | 预期优化收益 |
|-----------|--------------|------------|
| VF融合双发优化 | 当前 VF 函数内 GELU 计算依赖路径较长，虽然已使用 `asc_vf_call` 调用 RegBase API，但 VF 融合的双发特性未充分利用，IPC（每 cycle 指令发射数量）较低 | 利用 VF 融合双发特性，常规计算指令（Mul/Muls/Add/Adds）并行度可达 512 bytes/cycle，向量化指令耗时可减少 **55%** 以上。详见[Gelu性能调优样例 Case 1](https://gitcode.com/cann/asc-devkit/blob/master/examples/01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_high_performance/README.md) |
| 循环展开优化 | VF 函数内循环为简单 for 循环，编译器无法充分调度指令级并行，执行队列中可双发的指令数受限 | 使用 `#pragma unroll N` 展开循环，提高指令发射并行度和 IPC，在 VF 融合基础上向量指令耗时可进一步减少约 **4.6%**。展开因子 N 需根据实际场景调优，过大会增加寄存器压力。详见[Gelu性能调优样例 Case 2](https://gitcode.com/cann/asc-devkit/blob/master/examples/01_simd_cpp_api/05_best_practices/02_reg_compute/gelu_high_performance/README.md) |
| 流水线串行执行 | 当前搬入、计算、搬出三个阶段严格串行执行，各硬件单元（MTE2/V/MTE3）无法同时工作，数据搬运占比超过 90%，性能瓶颈为 MTE2 bound | 采用多缓冲（Double Buffer/Triple Buffer）机制，使搬入、计算、搬出可并行执行，提升硬件利用率和吞吐 |
| 核数固定 | 固定使用 2 个核，未根据实际可用核数动态分配 | 动态获取可用核数（[GetBlockNum](../../../../../docs/zh/api/SIMD-API/基础API/工具接口/系统资源与变量/GetBlockNum.md)），充分利用多核并行能力 |

## 功能调试

### printf

该接口提供 CPU 域/NPU 域调试场景下的格式化输出功能。

在算子 kernel 侧实现代码中需要输出日志信息的地方调用 printf 接口打印相关内容。

示例如下：

```cpp
AscendC::printf("gelu blockIdx=%d, singleCoreLength=%d\n", AscendC::GetBlockIdx(), singleCoreLength);
```

> **注意：** printf（PRINTF）接口打印功能会对算子实际运行的性能带来一定影响，通常在调测阶段使用。开发者可以按需通过设置 `ASCENDC_DUMP=0` 的方式关闭打印功能。

### DumpTensor

基于算子工程开发的算子，可以使用该接口 Dump 指定 [LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md) 的内容。同时支持打印自定义的附加信息（仅支持 uint32_t 数据类型的信息），比如打印当前行号等。

在算子 kernel 侧实现代码中需要打印 Tensor 数据的地方调用 DumpTensor 接口打印相关内容。样例如下：

```cpp
// GELU 计算完成后 Dump 输出结果
asc_vf_call<GeluVfMethod2>(xAddr, yAddr, singleCoreLength, loopNum);
AscendC::SetFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
AscendC::WaitFlag<AscendC::HardEvent::V_MTE3>(EVENT_ID0);
AscendC::DumpTensor(yLocal, 0, 16);
```

> **注意：** DumpTensor 接口打印功能会对算子实际运行的性能带来一定影响，通常在调测阶段使用。开发者可以按需通过设置 `ASCENDC_DUMP=0` 来关闭打印功能。

## 性能调试

### msOpProf工具介绍
msOpProf工具是单算子性能分析工具。包含msopprof和msopprof simulator两种使用方式。该工具协助用户定位算子内存、算子代码以及算子指令的异常，实现全方位的算子调优。当前支持基于不同运行模式（上板或仿真）和不同文件形式（可执行文件或算子二进制.o文件）进行性能数据的采集和自动解析。

- 上板性能采集

    通过上板性能采集，可以直接测定算子昇腾AI处理器上的运行时间。该方式适合在板环境中快速定位算子性能问题。

    基于可执行文件demo通过msopprof执行算子调优：
    ```
    msopprof ./demo
    ```

    - 性能数据说明  
      命令完成后，会在默认目录下生成以“OPPROF_{timestamp}_XXX”命名的文件夹,性能数据文件夹结构示例如下：

      ```bash
      ├──dump                       # 原始的性能数据，用户无需关注
      ├──ArithmeticUtilization.csv  # cube/vector指令cycle占比
      ├──L2Cache.csv                # L2 Cache命中率，影响MTE2，建议合理规划数据搬运逻辑，增加命中率
      ├──Memory.csv                 # UB，L1和主存储器读写带宽速率
      ├──MemoryL0.csv               # L0A，L0B，和L0C读写带宽速率
      ├──MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
      ├──OpBasicInfo.csv            # 算子基础信息
      ├──PipeUtilization.csv        # 采集计算单元和搬运单元耗时和占比
      ├──ResourceConflictRatio.csv  # UB上的bank group、bank conflict和资源冲突率在所有指令中的占比
      └──visualize_data.bin         # MindStudio Insight呈现文件
      ```

查看具体的性能分析结果：

```bash
# 查看Task Duration 以及各项数据
cat ./OPPROF_*/PipeUtilization.csv
```

## 目录结构介绍

``` 
├── gelu
│   ├── scripts
│   │   ├── gen_data.py      // 输入数据和真值数据生成脚本
│   │   └── verify_result.py // 输出结果和真值数据校验脚本
│   ├── CMakeLists.txt       // 编译工程文件
│   ├── data_utils.h         // 数据读入写出函数
│   ├── gelu.asc             // Ascend C样例实现 & 调用样例
│   └── README.md            // 样例说明文档
```

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin
  ```

  使用 NPU 仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j
  ```

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（默认） | NPU架构：Ascend 950PR/Ascend 950DT |
  | `CMAKE_VF_MODE` | `true`（默认）、`false` | VF融合模式：启用或禁用 `--cce-simd-vf-fusion` |


- 执行结果  
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
