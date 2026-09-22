# Matmul Gelu最佳实践样例

## 概述

本样例基于静态Tensor编程范式，展示了Cube-Vector（CV）融合的实现方法，将Matmul矩阵乘法与GELU激活函数融合到同一AI Core中并行执行。AIC侧完成Matmul计算后通过Fixpipe将结果输出到GM或UB，AIV侧从GM或UB读取数据完成GELU向量计算。其中Matmul部分的优化细节参考[Matmul基础API最佳实践样例](../../01_matrix_compute/matmul_basic_api_high_performance/README.md)，GELU部分的优化细节参考[Gelu性能调优样例](../../02_reg_compute/gelu_high_performance/README.md)。

## 本样例支持的产品及CANN软件版本

| 产品                                        | CANN软件版本  |
| ------------------------------------------- | ------------- |
| Ascend 950PR/Ascend 950DT                   | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── matmul_gelu_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── figures                 // 图示
│   ├── mmad_gelu.asc           // Ascend C样例实现
│   └── README.md               // 样例说明文档
```

## 样例描述

Matmul + GELU计算公式：

$$
C = GELU(A * B)
$$

$$
GELU(x) \approx \frac{x}{1 + e^{-1.595769 \cdot (x + 0.044715 \cdot x^3)}}
$$

- A为左矩阵，形状为[M, K]，数据类型为float16；B为右矩阵，形状为[K, N]，数据类型为float16，B矩阵以转置方式存储
- C为目的操作数，存放Matmul+GELU计算结果，形状为[M, N]，数据类型为float32

- 样例规格：

<table>
<tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="5" align="center">Matmul + GELU</td></tr>
<tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
<tr><td align="center">A</td><td align="center">[M, K]</td><td align="center">float16</td><td align="center">ND</td><td align="center">false</td></tr>
<tr><td align="center">B</td><td align="center">[K, N]</td><td align="center">float16</td><td align="center">ND</td><td align="center">true</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">C</td><td align="center">[M, N]</td><td align="center">float32</td><td align="center">ND</td><td align="center">-</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="5" align="center">mmad_vector_custom</td></tr>
</table>

## 样例实现

### 性能指标说明

| 指标                 | 说明                                                                        |
| -------------------- | --------------------------------------------------------------------------- |
| Task Duration(μs)    | 整个任务执行的总时间，算子执行时间以该参数为准                              |
| Block Num            | 使用的核数（Block 数量）                                                    |
| aicore_time(μs)      | AI Core 的平均执行时间                                                      |
| aic_mac_time(μs)     | cube 类型指令（矩阵类运算指令）耗时                                         |
| aic_mac_ratio        | cube 类型指令（矩阵类运算指令）的 cycle 数在 total cycle 数中的占用比       |
| aic_mte1_time(μs)    | mte1 类型指令（L1 -> L0A/L0B 搬运）耗时                                     |
| aic_mte1_ratio       | mte1 类型指令（L1 -> L0A/L0B 搬运）的 cycle 数在 total cycle 数中的占用比   |
| aic_mte2_time(μs)    | mte2 类型指令（GM -> L1 搬运）耗时                                          |
| aic_mte2_ratio       | mte2 类型指令（GM -> L1 搬运）的 cycle 数在 total cycle 数中的占用比        |
| aic_fixpipe_time(μs) | fixpipe 类型指令（L0C -> GM/UB 搬运）耗时                                   |
| aic_fixpipe_ratio    | fixpipe 类型指令（L0C -> GM/UB 搬运）的 cycle 数在 total cycle 数中的占用比 |
| aiv_time(μs)         | Task 在 AI Vector Core 上的执行时间                                         |
| aiv_vec_time(μs)     | vec 类型指令（向量类运算指令）耗时                                          |
| aiv_vec_ratio        | vec 类型指令（向量类运算指令）的 cycle 数在 total cycle 数中的占用比        |
| aiv_scalar_time(μs)  | scalar 类型指令（标量类运算指令）耗时                                       |
| aiv_scalar_ratio     | scalar 类型指令（标量类运算指令）的 cycle 数在 total cycle 数中的占用比     |
| aiv_mte2_time(μs)    | mte2 类型指令（GM -> UB 搬运类指令）耗时                                    |
| aiv_mte2_ratio       | mte2 类型指令（GM -> UB 搬运类指令）的 cycle 数在 total cycle 数中的占用比  |
| aiv_mte3_time(μs)    | mte3 类型指令（UB -> GM 搬运类指令）耗时                                    |
| aiv_mte3_ratio       | mte3 类型指令（UB -> GM 搬运类指令）的 cycle 数在 total cycle 数中的占用比  |

### 数据流路径

本样例提供两种CV融合场景：

**Scenario 1：CV融合-GM中转（中级性能优化）**（A2/A3/Ascend 950PR均支持）

```
GM ──(MTE2)──> L1 ──(MTE1)──> L0A/L0B ──(Cube)──> L0C ──(Fixpipe)──> GM ──(MTE2)──> UB ──(Gelu)──> UB ──(MTE3)──> GM
```

**Scenario 2：CV融合-UB直通（高阶极限打磨）**（仅Ascend 950PR支持）

Ascend 950PR新增L0C->UB的通路，A2/A3不支持，芯片能力增强，因此在Ascend 950PR架构中可以不经过GM直接将矩阵乘结果输出到UB进行GELU计算

```
GM ──(MTE2)──> L1 ──(MTE1)──> L0A/L0B ──(Cube)──> L0C ──(Fixpipe)──> UB ──(Gelu)──> UB ──(MTE3)──> GM
```

### 中级性能优化

本样例的Matmul部分采用与[Matmul基础API最佳实践样例](../../01_matrix_compute/matmul_basic_api_high_performance/README.md)相同的优化手段（L1/L0双缓冲、大包搬运、细粒度流水同步、UnitFlag等），GELU部分在Ascend 950PR架构上采用RegBase方式，在A2/A3架构上采用MemBase方式。GELU公式的化简、RegBase API与VF融合的详细优化分析请参考[Gelu性能调优样例](../../02_reg_compute/gelu_high_performance/README.md)。

#### Matmul优化手段

| Case                                                                                                           | 优化手段        | 说明                                           |
| -------------------------------------------------------------------------------------------------------------- | --------------- | ---------------------------------------------- |
| [Case 1](../../01_matrix_compute/matmul_high_performance/README.md#case-1-单核tiling优化-single_core_tiling)   | 单核Tiling优化  | 优化base块参数，降低访存计算比                 |
| [Case 2](../../01_matrix_compute/matmul_high_performance/README.md#case-2-多核切分-2x12-multi_core_split_2_12) | 多核切分 2x12   | 引入多核并行计算，均衡负载分配                 |
| [Case 3](../../01_matrix_compute/matmul_high_performance/README.md#case-3-多核切分-4x6-multi_core_split_4_6)   | 多核切分 4x6    | 优化多核切分策略，地址512B对齐，减少同地址访问 |
| [Case 4](../../01_matrix_compute/matmul_high_performance/README.md#case-4-多核使用mdl模板-multi_core_mdl)      | 多核使用MDL模板 | 启用L1多块缓存，大包搬运，减少MTE2循环搬运次数 |

#### Gelu优化手段

| Case                                                                                              | 优化手段             | 说明                                                |
| ------------------------------------------------------------------------------------------------- | -------------------- | --------------------------------------------------- |
| [Case 1](../../02_reg_compute/gelu_high_performance/README.md#case-1-启用regbase-api和vf融合能力) | RegBase API + VF融合 | 寄存器级计算减少中间Load/Store，利用双发特性提升IPC |

#### GELU计算实现

**A2/A3架构**：使用MemBase方式（向量基础API + PipeBarrier），无需额外中间缓冲区，直接复用输出缓冲区：

```cpp
__aicore__ inline void GeluMemBaseCompute(
    const AscendC::LocalTensor<float>& xLocal, const AscendC::LocalTensor<float>& yLocal, uint32_t n)
{
    AscendC::Mul(yLocal, xLocal, xLocal, n);        // x²
    AscendC::PipeBarrier<PIPE_V>();
    AscendC::Mul(yLocal, yLocal, xLocal, n);        // x³
    ……
}
```

**Ascend 950PR架构**：使用RegBase方式（VF函数），中间计算在寄存器内完成，无需额外UB缓冲区，减少数据在UB和Reg之间的交互：

```cpp
__simd_vf__ inline void GeluVf(__ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t n, uint32_t loopNum)
{
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg, yReg;

    for (uint16_t i = 0; i < loopNum; ++i) {
        mask = AscendC::Reg::UpdateMask<float>(n);
        AscendC::Reg::LoadAlign(xReg, xAddr + i * oneRepeatSize);
        AscendC::Reg::Mul(yReg, xReg, xReg, mask);       // x²
        ……
        AscendC::Reg::StoreAlign(yAddr + i * oneRepeatSize, yReg, mask);
    }
}
```

以下重点说明AIC↔AIV的CV融合机制。

#### 1. CV融合架构：`__mix__(1,2)` 核函数

本样例使用 `__global__ __mix__(1, 2)` 声明核函数，表示每个逻辑核由1个AIC和2个AIV组成。AIC侧负责Matmul计算，AIV侧负责GELU向量计算，两者在同一AI Core内按(baseM × baseN)块粒度流水并行。

```cpp
__global__ __mix__(1, 2) void mmad_vector_custom(__gm__ uint8_t* xMatrix, __gm__ uint8_t* yMatrix, __gm__ uint8_t* zMatrix)
{
    AscendC::InitSocState();
    KernelMmadVector<...> op;
    op.Init(xMatrix, yMatrix, zMatrix);
    op.Process();
    AscendC::PipeBarrier<PIPE_ALL>();
}
```

在 `Process()` 中，通过 `ASCEND_IS_AIC` 和 `ASCEND_IS_AIV` 宏区分AIC和AIV的执行路径：

```cpp
if ASCEND_IS_AIC {
    // AIC侧：Buffer分配 + 计算循环 + Fixpipe搬出
    ProcessLoopAic(...);
}
if ASCEND_IS_AIV {
    // AIV侧：GELU向量计算 + 搬出至GM
    ProcessLoopAiv();
}
```

#### 2. 跨核同步：AIC→AIV 数据就绪通知

AIC侧每完成一个 (baseM × baseN)大小子块的Fixpipe输出后，通过 `CrossCoreSetFlag` 通知AIV侧数据已就绪；AIV侧通过 `CrossCoreWaitFlag` 等待该信号，然后开始GELU计算：

```cpp
// AIC侧 CopyOutAic():
AscendC::CrossCoreSetFlag<0x2, PIPE_FIX>(0x8);  // 通知AIV数据就绪

// AIV侧 GeluFromGM() / GeluFromUB():
AscendC::CrossCoreWaitFlag<0x2>(0x8);  // 等待AIC侧Fixpipe完成
```

#### 3. `__mix__(1,2)` 模式下的多核切分

`__mix__(1,2)` 模式下，`GetBlockIdx()` 在AIC侧和AIV侧各自独立编号：AIC侧返回0~numAICores-1，AIV侧返回0~numAIVCores-1。由于每个逻辑核对应2个AIV，AIV侧需要将 `GetBlockIdx() / 2` 映射回逻辑核ID，从而与AIC侧使用相同的多核切分策略：

```cpp
uint32_t logicCoreId;
if ASCEND_IS_AIC {
    logicCoreId = AscendC::GetBlockIdx();
} else {
    logicCoreId = AscendC::GetBlockIdx() / 2;  // __mix__(1,2): 每个逻辑核对应2个AIV
}
uint32_t mIterIdx = logicCoreId % mIter;
uint32_t nIterIdx = logicCoreId / mIter;
```

### 高阶极限打磨

在高阶极限打磨阶段，CV融合层面提供两种数据通路：Scenario 1通过GM中转实现CV融合（所有架构支持），Scenario 2通过Fixpipe直通UB实现CV融合（仅Ascend 950PR支持），省去GM中转的读写开销。
Matmul和Gelu相关高阶极限打磨手段请参考下方列表。

#### Matmul优化手段

| Case                                                                                                                                                                               | 优化手段                | 说明                                   |
| ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- | ----------------------- | -------------------------------------- |
| [Case 5](../../01_matrix_compute/matmul_high_performance/README.md#case-5-多核mdl--l1cache优化-multi_core_mdl_l1cache)                                                             | MDL + L1Cache优化       | 手动调整Tiling参数，充分利用L1缓存     |
| [Case 6](../../01_matrix_compute/matmul_high_performance/README.md#case-6-多核mdl--l1cache--l2cache-multi_core_mdl_l1cache_l2cache)                                                | MDL + L1Cache + L2Cache | 分块计算适配L2Cache，提高缓存命中率    |
| [Case 7](../../01_matrix_compute/matmul_high_performance/README.md#case-7-多核mdl--l1cache--l2cache--constants-tiling-multi_core_mdl_l1cache_l2cache_constants)                    | + Constants Tiling      | Tiling常量化，减少运行时Scalar计算开销 |
| [Case 8](../../01_matrix_compute/matmul_high_performance/README.md#case-8-多核mdl--l1cache--l2cache--constants-tiling--unitflag-multi_core_mdl_l1cache_l2cache_constants_unitflag) | + UnitFlag              | 细粒度流水同步，MMAD与FIXPIPE流水并行  |

#### Gelu优化手段

| Case                                                                                                          | 优化手段          | 说明                                     |
| ------------------------------------------------------------------------------------------------------------- | ----------------- | ---------------------------------------- |
| [Case 2](../../02_reg_compute/gelu_high_performance/README.md#case-2-启用regbase-apivf融合能力和循环展开优化) | VF融合 + 循环展开 | #pragma unroll展开循环，提高指令级并行度 |

#### CV融合-GM中转

**Scenario 1：CV融合-GM中转**（A2/A3/Ascend 950PR均支持）

AIC侧通过Fixpipe将L0C计算结果写入GM，AIV侧从GM读取数据到UB后进行GELU计算，再将结果写回GM。这种方式所有架构均支持。

数据流路径：

```
GM ──(MTE2)──> L1 ──(MTE1)──> L0A/L0B ──(Cube)──> L0C ──(Fixpipe)──> GM ──(MTE2)──> UB ──(Gelu)──> UB ──(MTE3)──> GM
```

**AIC侧实现**

Fixpipe L0C → GM

```cpp
// A2/A3架构
AscendC::FixpipeParamsV220 fixpipeParams;
fixpipeParams.nSize = curN;
fixpipeParams.mSize = curM;
fixpipeParams.srcStride = curMAlign;
fixpipeParams.dstStride = N;
AscendC::Fixpipe<float, float, CFG_ROW_MAJOR_GM>(cGM[...], cLocal, fixpipeParams);

// Ascend 950PR架构
AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::ROW_MAJOR> fixpipeParams;
fixpipeParams.mSize = curM;
fixpipeParams.nSize = curN;
fixpipeParams.srcStride = curMAlign;
fixpipeParams.dstStride = N;
AscendC::Fixpipe<float, float, CFG_ROW_MAJOR_GM>(cGM[...], cLocal, fixpipeParams);
```

**AIV侧实现**

GM → xUB → GELU → geluOutUB → GM

`__mix__(1,2)` 模式下每个逻辑核有2个AIV，Fixpipe输出到GM的M行数据按M/2行拆分给两个AIV处理。每个AIV通过 `GetSubBlockIdx() % 2` 获取自己的子块索引，独立完成各自M/2行的GELU计算：

```cpp
uint32_t localSubIdx = AscendC::GetSubBlockIdx() % 2;
uint32_t gmOffset = mBlockIdx * baseM * N + nBlockIdx * baseN + localSubIdx * baseM / 2 * N;

// Step 1: DataCopy GM → xUB
AscendC::DataCopyPad<float>(xUB, cGM[gmOffset], copyInParams, padParams);

// Step 2: GELU计算
// A2/A3: GeluMemBaseCompute(xUB, geluOutUB, computeLen);
// Ascend 950PR:    GeluRegBaseCompute(xUB, geluOutUB, computeLen);

// Step 3: DataCopy geluOutUB → GM
AscendC::DataCopyPad<float>(cGM[gmOffset], geluOutUB, copyOutParams);
```

**性能数据**

> 以下数据基于msOpProf工具测量，取5次中位数。矩阵规格M=8192, K=8192, N=8192，输入数据类型float16，输出数据类型float32。

##### Atlas A2训练系列芯片

- singleCoreM=2048, singleCoreN=1536，24核

| Scenario   | Task Type      | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
| ---------- | -------------- | ----------------- | --------- | ------------- | --------------- | ---------------- | ------------- | ------------------- | ---------------- | ----------------- | -------------- | ----------------- | -------------- | -------------------- | ----------------- | ------------ | ---------------- | ------------- | ------------------- | ---------------- | ----------------- | -------------- | ----------------- | -------------- |
| Gelu计算   | AI_VECTOR_CORE | 369.827           | 48        | 0             | 0               | 0                | 0             | 0                   | 0                | 0                 | 0              | 0                 | 0              | 0                    | 0                 | 358.91       | 236.506          | 0.659         | 8.293               | 0.023            | 316.672           | 0.882          | 150.033           | 0.418          |
| Matmul计算 | AI_CORE        | 4227.605          | 24        | 0             | 3760.55         | 3083.197         | 0.82          | 338.636             | 0.09             | 2516.081          | 0.669          | 3685.713          | 0.98           | 193.704              | 0.052             | 0            | 0                | 0             | 0                   | 0                | 0                 | 0              | 0                 | 0              |
| Scenario 1 | MIX_AIC        | 4236.944          | 24        | 48            | 3770.16         | 3083.235         | 0.818         | 531.971             | 0.141            | 2520.852          | 0.669          | 3687.839          | 0.978          | 193.933              | 0.051             | 3773.6       | 174.327          | 0.046         | 2.016               | 0.001            | 90.315            | 0.024          | 66.373            | 0.018          |

##### Ascend 950PR芯片

- singleCoreM=2048, singleCoreN=1024，32核

| Scenario          | Task Type      | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
| ----------------- | -------------- | ----------------- | --------- | ------------- | --------------- | ---------------- | ------------- | ------------------- | ---------------- | ----------------- | -------------- | ----------------- | -------------- | -------------------- | ----------------- | ------------ | ---------------- | ------------- | ------------------- | ---------------- | ----------------- | -------------- | ----------------- | -------------- |
| Gelu计算(RegBase) | AI_VECTOR_CORE | 348.868           | 64        | 0             | 0               | 0                | 0             | 0                   | 0                | 0                 | 0              | 0                 | 0              | 0                    | 0                 | 347.99       | 66.277           | 0.19          | 3.03                | 0.009            | 320.543           | 0.921          | 314.547           | 0.904          |
| Matmul计算        | AI_CORE        | 2601.311          | 32        | 0             | 2600.55         | 2593.264         | 0.997         | 104.582             | 0.04             | 816.91            | 0.314          | 1875.057          | 0.721          | 252.163              | 0.097             | 0            | 0                | 0             | 0                   | 0                | 0                 | 0              | 0                 | 0              |
| Scenario 1        | MIX_AIC        | 2606.91           | 32        | 64            | 2605.9          | 2597.856         | 0.997         | 105.131             | 0.04             | 818.781           | 0.314          | 1917.737          | 0.736          | 252.231              | 0.097             | 2605.93      | 59.897           | 0.023         | 1.783               | 0.001            | 57.145            | 0.022          | 50.516            | 0.019          |

##### 性能数据解读

以Ascend 950PR芯片数据为例：

- 单独Gelu任务耗时348.868μs，单独Matmul任务耗时2601.311μs
- Scenario 1融合后总耗时2606.91μs，与Matmul单独执行时间基本一致

这表明采用CV融合方式编写时，AIV的Vector计算时间基本被AIC的Cube计算时间完全覆盖。由于Cube和Vector可以并行执行，整体任务执行时间不是简单的C+V叠加，而是基本等于Cube计算时间（即Matmul时间），实现了高效的流水并行。

下图展示了Scenario 1（CV融合-GM中转）场景下AIC和AIV的流水并行执行过程。AIC侧完成Matmul计算后通过Fixpipe将结果写入GM，AIV侧从GM读取数据完成GELU计算，两者按(baseM × baseN)块粒度流水并行：

![Scenario1 CV融合-GM中转流水并行](figures/CVParallell_L0C_GM_UB.png)

> 💡 使用 msOpProf 工具需安装 CANN 商用/社区版，详细信息可参考[msOpProf工具安装指南](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/install_guide/msopprof_install_guide.md)。流水图基于msOpProf工具上板获取，获取命令：`msopprof --aic-metrics=PipeTimeline ./demo`

**MTE2时间显著减少的原因：**

对比数据可以发现：

- 单独Gelu计算的aiv_mte2_time为320.543μs
- Scenario 1中aiv_mte2_time仅为57.145μs，减少约82.2%

原因分析：虽然Scenario 1中AIV也需要从GM将数据读取到UB，但AIC的Fixpipe将Matmul计算结果写入GM后，数据会被缓存在L2 Cache中。当AIV发起MTE2操作从GM读取数据时，由于数据已在L2 Cache中命中，实际是从L2 Cache读取而非从GM读取，因此MTE2时间显著减少。这也是CV融合场景下的一个隐式性能优化点。

#### CV融合-UB直通

**Scenario 2：CV融合-UB直通**（仅Ascend 950PR支持）

AIC侧通过Fixpipe将L0C计算结果直接写入UB，AIV侧无需从GM读取数据，直接在UB上进行GELU计算后将结果写回GM。这种方式省去了GM中转的读写开销，但仅Ascend 950PR架构支持Fixpipe从L0C到UB。

Ascend 950PR新增L0C->UB的通路，A2/A3不支持，芯片能力增强，因此在Ascend 950PR架构中可以不经过GM直接将矩阵乘结果输出到UB进行GELU计算。

数据流路径：

```
GM ──(MTE2)──> L1 ──(MTE1)──> L0A/L0B ──(Cube)──> L0C ──(Fixpipe)──> UB ──(Gelu)──> UB ──(MTE3)──> GM
```

**AIC侧实现**

Fixpipe L0C → UB（通过 `dualDstCtl=0b01` 开启双目标模式，按M维度拆分给2个AIV）

```cpp
AscendC::FixpipeParamsArch3510<AscendC::CO2Layout::ROW_MAJOR> fixpipeParams;
fixpipeParams.mSize = DivCeil(curM, 2) * 2;
fixpipeParams.nSize = curN;
fixpipeParams.srcStride = curMAlign;
fixpipeParams.dstStride = curN;
fixpipeParams.dualDstCtl = 0b01;  // 按M维度拆分到2个AIV的UB
AscendC::Fixpipe<float, float, CFG_ROW_MAJOR_UB>(xUB, cLocal, fixpipeParams);
```

**AIV侧实现**

UB → GELU → geluOutUB → GM

AIV侧无需DataCopy GM→UB，直接对xUB进行GELU计算：

```cpp
// GELU计算：xUB作为输入，geluOutUB作为输出
GeluRegBaseCompute(xUB, geluOutUB, computeLen);

// DataCopy: geluOutUB → GM
uint32_t localSubIdx = AscendC::GetSubBlockIdx() % 2;
uint32_t offset = baseM / 2 * N;
AscendC::DataCopyPad<float>(cGM[mBlockIdx * baseM * N + nBlockIdx * baseN + localSubIdx * offset], geluOutUB, copyParams);
```

**性能数据**

> 以下数据基于msOpProf工具测量，取5次中位数。矩阵规格M=8192, K=8192, N=8192，输入数据类型float16，输出数据类型float32。

##### Ascend 950PR芯片

- singleCoreM=2048, singleCoreN=1024，32核

| Scenario   | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
| ---------- | --------- | ----------------- | --------- | ------------- | --------------- | ---------------- | ------------- | ------------------- | ---------------- | ----------------- | -------------- | ----------------- | -------------- | -------------------- | ----------------- | ------------ | ---------------- | ------------- | ------------------- | ---------------- | ----------------- | -------------- | ----------------- | -------------- |
| Scenario 2 | MIX_AIC   | 2573.584          | 32        | 64            | 2572.67         | 2565.251         | 0.997         | 104.669             | 0.041            | 819.934           | 0.319          | 2096.38           | 0.815          | 212.403              | 0.083             | 2572.69      | 60.438           | 0.023         | 1.138               | 0                | 0.005             | 0              | 68.76             | 0.027          |

##### 性能数据解读

以Ascend 950PR芯片数据为例：

- Scenario 2融合后总耗时2573.584μs，相比Scenario 1的2606.91μs进一步减少约33μs
- Scenario 2中aiv_mte2_time仅为0.005μs，相比Scenario 1的57.145μs几乎降为零

下图展示了Scenario 2（CV融合-UB直通）场景下AIC和AIV的流水并行执行过程。AIC侧完成Matmul计算后通过Fixpipe将结果直接写入UB（不经过GM中转），AIV侧直接在UB上进行GELU计算，省去了GM→UB的MTE2搬运开销：

![Scenario2 CV融合-UB直通流水并行](figures/CVParallell_L0C_UB.png)

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 切换Case

  在 cmake 编译时通过 `-DSCENARIO_NUM=N` 指定要编译的场景，各场景说明：
  - `1`: CV融合-GM中转（A2/A3/Ascend 950PR均支持）
  - `2`: CV融合-UB直通（仅Ascend 950PR支持）

  > **注意：** A2/A3架构仅支持Scenario 1（GM中转），选择Scenario 2会触发编译期 `static_assert` 报错。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如：

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式或Scenario前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项                      | 可选值                 | 说明                                                             |
  | ------------------------- | ---------------------- | ---------------------------------------------------------------- |
  | `CMAKE_ASC_RUN_MODE`      | `npu`（默认）、`sim`   | 运行模式：NPU运行、NPU仿真                                       |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`、`dav-3510` | NPU硬件架构：dav-2201对应A2/A3，dav-3510对应Ascend 950PR         |
  | `SCENARIO_NUM`            | `1`、`2`               | 场景编号：1=CV融合-GM中转，2=CV融合-UB直通（仅Ascend 950PR支持） |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```

## 功能调试

### printf

该接口提供CPU域/NPU域调试场景下的格式化输出功能。

在算子kernel侧实现代码中需要输出日志信息的地方调用printf接口打印相关内容。

示例如下：

```cpp
AscendC::printf("matmul blockIdx=%d\n", AscendC::GetBlockIdx());
```

> **注意：** printf（PRINTF）接口打印功能会对算子实际运行的性能带来一定影响，通常在调测阶段使用。开发者可以按需通过设置ASCENDC_DUMP=0的方式关闭打印功能。

### DumpTensor

基于算子工程开发的算子，可以使用该接口Dump指定[LocalTensor](../../../../../docs/zh/api/SIMD-API/basic_api/data_structures/LocalTensor/LocalTensor_intro.md)的内容。同时支持打印自定义的附加信息（仅支持uint32_t数据类型的信息），比如打印当前行号等。

在算子kernel侧实现代码中需要打印Tensor数据的地方调用DumpTensor接口打印相关内容。样例如下：

```cpp
// Vector核LeakyRelu结果
AscendC::Div(yLocal, xLocal, yLocal, n);
AscendC::DumpTensor(yLocal, 1, 16);
```

> **注意：** [DumpTensor](../../../../../docs/zh/api/SIMD-API/basic_api/debug_interface/onboard_print/DumpTensor.md)接口打印功能会对算子实际运行的性能带来一定影响，通常在调测阶段使用。开发者可以按需通过设置ASCENDC_DUMP=0来关闭打印功能。

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
