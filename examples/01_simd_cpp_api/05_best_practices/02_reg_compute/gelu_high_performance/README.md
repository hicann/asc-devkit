# Gelu性能调优样例

## 概述

本样例以Gelu计算为例，介绍RegBase的向量性能调优方法，样例展示使能VF融合和循环展开之后的性能收益情况。

> **前置阅读**：[Gelu算子入门样例](../../../00_introduction/04_reg_compute/gelu/README.md)，本样例基于入门样例进行性能优化，建议先阅读入门样例了解基础概念。

**优化路径**：
- Case 0: Gelu未使能VF融合能力（基准）
- Case 1: 启用RegBase API和VF融合能力
- Case 2: 启用RegBase API、VF融合能力和循环展开优化

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```
├── gelu_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── gelu.asc                // Ascend C样例实现（包含2个优化case）
│   └── README.md               // 样例说明文档
```

## 样例描述

**样例功能**：

Gelu近似公式计算为：

$$
GELU(x) \approx 0.5 \cdot x \cdot \left(1 + \tanh\left(\sqrt{\frac{2}{\pi}} \cdot \left(x + 0.044715 \cdot x^3\right)\right)\right) \tag{1}
$$

tanh的计算公式为：

$$
\tanh(u) = \frac{e^{2u} - 1}{e^{2u} + 1} \tag{2}
$$

其中，$u = \sqrt{\frac{2}{\pi}} \cdot (x + 0.044715 \cdot x^3)$。

将tanh公式代入Gelu公式并化简，得到：

$$
GELU(x) \approx \frac{x}{1 + e^{-2 \cdot \sqrt{\frac{2}{\pi}} \cdot (x + 0.044715 \cdot x^3)}} \tag{3}
$$

其中，$-2 \cdot \sqrt{\frac{2}{\pi}} \approx -1.595769$。

本样例以公式（3）进行编程计算，用户在设计向量算子时，需要考虑对原始计算进行化简，可以有效减少计算步骤和内存占用。

## 计算步骤分析

为说明公式化简对算子性能的影响，对比三种不同的Gelu计算实现方式。

**方式1：**

$$
GELU(x) \approx 0.5 \cdot x \cdot \left(1 + \frac{e^{2 \cdot \sqrt{\frac{2}{\pi}} \cdot (x + 0.044715 \cdot x^3)} - 1}{e^{2 \cdot \sqrt{\frac{2}{\pi}} \cdot (x + 0.044715 \cdot x^3)} + 1}\right)
$$

**方式2：**

$$
GELU(x) \approx \frac{x}{1 + e^{-1.595769 \cdot x - 0.071405 \cdot x^3}}
$$

**方式3：**

$$
GELU(x) \approx \frac{x}{1 + e^{-1.595769 \cdot (x + 0.044715 \cdot x^3)}}
$$

| 计算方式 | 计算指令数 | Unified Buffer（UB）内存份数 |
|:---|:---:|:---:|
| 方式1 | 13 | 5 |
| 方式2 | 8 | 3 |
| 方式3 | 8 | 2 |

令输入UB内存为xLocal，输出UB内存为yLocal，临时UB内存统一用tmp0、tmp1、tmp2等表示

**方式1计算步骤分解**（共13条指令，需5份内存）：

| 步骤 | 计算内容 | 计算指令 | 内存占用 |
|:---:|:---|:---:|:---|
| 1 | yLocal = x² | Mul | xLocal, yLocal |
| 2 | yLocal = x³ | Mul | xLocal, yLocal |
| 3 | yLocal = 0.044715 * x³ | Muls | yLocal |
| 4 | yLocal = x + 0.044715 * x³ | Add | xLocal, yLocal |
| 5 | yLocal = √(2/π) * (x + 0.044715 * x³) | Muls | yLocal |
| 6 | yLocal = 2u | Muls | yLocal |
| 7 | tmp0 = e^(2u) | Exp | tmp0, yLocal |
| 8 | tmp1 = e^(2u) - 1 | Adds | tmp1, yLocal |
| 9 | tmp2 = e^(2u) + 1 | Adds | tmp2, yLocal |
| 10 | tmp1 = tanh(u) = (e^(2u) - 1) / (e^(2u) + 1) | Div | tmp1, tmp2 |
| 11 | yLocal = 1 + tanh(u) | Adds | yLocal, tmp1 |
| 12 | yLocal = x * (1 + tanh(u)) | Mul | xLocal, yLocal |
| 13 | yLocal = 0.5 * x * (1 + tanh(u)) | Muls | yLocal |

**方式2计算步骤分解**（共8条指令，需3份内存）：

| 步骤 | 计算内容 | 计算指令 | 内存占用 |
|:---:|:---|:---:|:---|
| 1 | yLocal = x² | Mul | xLocal, yLocal |
| 2 | yLocal = x³ | Mul | xLocal, yLocal |
| 3 | yLocal = -0.071405 * x³ | Muls | yLocal |
| 4 | tmp0 = -1.595769 * x | Muls | tmp0, xLocal |
| 5 | yLocal = -1.595769 * x + (-0.071405 * x³) | Add | yLocal, tmp0 |
| 6 | yLocal = e^(-1.595769 * x - 0.071405 * x³) | Exp | yLocal |
| 7 | yLocal = 1 + e^(...) | Adds | yLocal |
| 8 | yLocal = x / (1 + e^(...)) | Div | yLocal, xLocal |

**方式3计算步骤分解**（共8条指令，需2份内存）：

| 步骤 | 计算内容 | 计算指令 | 内存占用 |
|:---:|:---|:---:|:---|
| 1 | yLocal = x² | Mul | yLocal, xLocal |
| 2 | yLocal = x³ | Mul | yLocal, xLocal |
| 3 | yLocal = 0.044715 * x³ | Muls | yLocal |
| 4 | yLocal = x + 0.044715 * x³ | Add | xLocal, yLocal |
| 5 | yLocal = -1.595769 * (x + 0.044715 * x³) | Muls | yLocal |
| 6 | yLocal = e^(-1.595769 * (x + 0.044715 * x³)) | Exp | yLocal |
| 7 | yLocal = 1 + e^(...) | Adds | yLocal |
| 8 | yLocal = x / (1 + e^(...)) | Div | yLocal, xLocal |

**对比总结**：

- **计算指令数**：方式2和方式3均比方式1减少 **5条指令**
- **内存份数**：方式2比方式1减少 **2份内存**，方式3比方式1减少 **3份内存**
- **实现选择**：方式3是最优实现，计算指令数最少，内存占用最低，本样例采用方式3实现

**样例规格**：

<table border="2">
<tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Gelu</td></tr>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[8192, 8192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">y</td><td align="center">[8192, 8192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">gelu_custom</td></tr>
</table>

## 样例实现

### 性能指标说明

**表1 AI Core 性能指标字段说明表**

| 字段名 | 字段含义 |
|:---|:---|
|Task Duration(μs)|Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间。|
|aiv_time(μs)|Task在AI Vector Core上的执行时间。|
|aiv_vec_time(μs)|vec类型指令（向量类运算指令）耗时。|
|aiv_vec_ratio|vec类型指令（向量类运算指令）的cycle数在total cycle数中的占用比。|
|aiv_scalar_time(μs)|scalar类型指令（标量类运算指令）耗时。|
|aiv_scalar_ratio|scalar类型指令（标量类运算指令）的cycle数在total cycle数中的占用比。|
|aiv_mte2_time(μs)|mte2类型指令（GM->UB搬运类指令）耗时。|
|aiv_mte2_ratio|mte2类型指令（GM->UB搬运类指令）的cycle数在total cycle数中的占用比。|
|aiv_mte3_time(μs)|mte3类型指令（UB->GM搬运类指令）耗时。|
|aiv_mte3_ratio|mte3类型指令（UB->GM搬运类指令）的cycle数在total cycle数中的占用比。|

### Case 0: Gelu未使能VF融合能力

**实现方式**：参考 `KernelGelu::GeluCompute()` 函数实现

基准程序使用Ascend C基础API实现Gelu计算，包括Mul、Muls、Add、Exp、Adds、Div等向量指令。

**关键代码**：
```cpp
__aicore__ inline void GeluCompute(
        const AscendC::LocalTensor<float>& xLocal, const AscendC::LocalTensor<float>& yLocal, uint32_t n)
{
    // yLocal = x * x = x²
    AscendC::Mul(yLocal, xLocal, xLocal, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = x² * x = x³
    AscendC::Mul(yLocal, yLocal, xLocal, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = x³ * 0.044715 = 0.044715 * x³
    AscendC::Muls(yLocal, yLocal, COEFF_A, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = x + 0.044715 * x³
    AscendC::Add(yLocal, xLocal, yLocal, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = -1.595769 * (x + 0.044715 * x³)
    AscendC::Muls(yLocal, yLocal, COEFF_B, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = e^(-1.595769 * (x + 0.044715 * x³))
    AscendC::Exp(yLocal, yLocal, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = 1 + e^(-1.595769 * (x + 0.044715 * x³))
    AscendC::Adds(yLocal, yLocal, (float)1.0, n);
    AscendC::PipeBarrier<PIPE_V>();
    // yLocal = x / (1 + e^(-1.595769 * (x + 0.044715 * x³)))
    AscendC::Div(yLocal, xLocal, yLocal, n);
}
```

**样例配置**：
- 多核切分：M方向（shape 第一维，行方向）分32份，N方向（shape 第二维，列方向）分2份，共计64份数据，分布到64core上计算
- `tileLen = 8192` 为每次搬运和计算的数据元素个数

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| 351.525 | 350.64 | 147.895 | 0.422 | 9.513 | 0.027 | 320.283 | 0.913 | 303.647 | 0.866 |

**优化效果分析**：
- 端到端耗时：**351.525μs**
- 向量指令耗时：147.895μs，占比 **42.2%**
- 数据搬运耗时：320.283μs（读）+ 303.647μs（写），搬运占比超过 **90%**

**原理说明**：
- 计算流程按照公式(3)执行，包含8次向量运算，使用基础API进行向量计算
- 每次向量计算内部会进行load -> compute -> store操作，每次计算数据均需要在Unified Buffer(UB)和Reg之间交互

**下一步优化方向**：
- 启用RegBase API和VF融合能力减少数据在UB和Reg之间的交互，在Reg内部完成最终计算结果的生成

---

## 中级性能优化

### Case 1: 启用RegBase API和VF融合能力 

### 性能瓶颈分析

基于 Case 0 的 Profiling 数据，当前入门级代码的核心性能瓶颈如下：

**Profiling 数据抓取**：

使用 `msOpProf` 工具对 Case 0 进行性能采集，关键指标如下：

| 指标 | 数值 | 占比 |
|:---|:---|:---|
| Task Duration | 351.525μs | - |
| aiv_vec_time | 147.895μs | 42.2% |
| aiv_scalar_time | 9.513μs | 2.7% |
| aiv_mte2_time | 320.283μs | 91.3% |
| aiv_mte3_time | 303.647μs | 86.6% |

**核心瓶颈解读**：

- **痛点1：向量计算开销占比高**：向量指令耗时 147.895μs，占比 42.2%。每次基础 API 向量计算内部会进行 load → compute → store 操作，数据需在 UB 和寄存器之间反复交互，8 次向量计算产生 8 次 Load/Store 开销。
- **痛点2：指令级并行度低**：基础 API 每条向量指令间需插入 `PipeBarrier<PIPE_V>()` 同步，无法利用 VF 融合的双发特性，标量开销占比 2.7%。

### 优化手段解析与实施：RegBase API + VF融合

**MemBase vs RegBase 概念对比**：

| 维度 | MemBase（基础API） | RegBase（VF融合API） |
|:---|:---|:---|
| 编程接口 | Compute API（如 `Mul(yLocal, xLocal, xLocal, n)`） | Reg API（如 `Reg::Mul(yReg, xReg, xReg, mask)`） |
| 数据驻留 | 每条指令隐式 Load/Store，中间结果写回 UB | 数据显式加载到寄存器后常驻，中间结果暂存寄存器 |
| 同步方式 | 每条指令间需 `PipeBarrier<PIPE_V>()` 同步 | VF 函数内无需显式同步，硬件自动管理依赖 |
| 调用方式 | 直接调用 Compute API | 通过 `asc_vf_call` 调用 VF 函数 |
| 适用场景 | 简单计算、单步或少步运算 | 多步骤融合计算，减少 UB 读写次数 |
| 性能特征 | 每步 Load/Store 开销大，IPC 低 | 仅 1 次 Load + 1 次 Store，支持双发，IPC 高 |

> **选择建议**：当计算步骤较多（≥3步）且中间结果无需写回 UB 时，优先选择 RegBase + VF 融合，可显著减少 UB 读写次数并利用双发特性提升 IPC。

**RegBase API 关键概念**：

| 关键字/概念 | 含义 | 详细文档 |
|:---|:---|:---|
| `__simd_vf__` | VF 函数声明修饰符，表示该函数运行在 VF（Vector Function）执行域 | [asc_vf_call](../../../../../docs/zh/api/SIMD-API/basic_api/reg_vector_compute/vf_call/asc_vf_call.md) |
| `__ubuf__` | UB 地址空间限定符，标记指针指向 UB 内存区域 | [asc_vf_call](../../../../../docs/zh/api/SIMD-API/basic_api/reg_vector_compute/vf_call/asc_vf_call.md) |
| `RegTensor<T>` | 寄存器级 Tensor 对象，数据驻留在向量寄存器中（区别于驻留 UB 的 `LocalTensor`） | [RegTensor](../../../../../docs/zh/api/SIMD-API/basic_api/reg_vector_compute/register_data_types/RegTensor.md) |
| `MaskReg` | 向量掩码寄存器，控制每次计算参与运算的元素数量 | [MaskReg](../../../../../docs/zh/api/SIMD-API/basic_api/reg_vector_compute/register_data_types/MaskReg.md) |
| `LoadAlign` | 连续对齐搬运，将数据从 UB 加载到寄存器 | [连续对齐搬入](../../../../../docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reg_data_load/LoadAlign_continuous.md) |
| `StoreAlign` | 连续对齐搬运，将数据从寄存器写回 UB | [连续对齐搬出](../../../../../docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reg_data_store/StoreAlign_continuous.md) |
| `asc_vf_call` | VF 函数调用入口，在核函数中调用 VF 函数 | [asc_vf_call](../../../../../docs/zh/api/SIMD-API/basic_api/reg_vector_compute/vf_call/asc_vf_call.md) |
| `UpdateMask` | 根据剩余元素数更新掩码寄存器 | [UpdateMask](../../../../../docs/zh/api/SIMD-API/basic_api/reg_vector_compute/register_data_types/MaskReg.md) |

**原理解析**：

RegBase API 提供寄存器级别的向量计算接口，配合 [asc_vf_call](../../../../../docs/zh/api/SIMD-API/basic_api/reg_vector_compute/vf_call/asc_vf_call.md) 调用 VF 函数，可实现 VF 融合。VF 融合的核心优势：
- 在 VF 函数内，数据从 UB 加载到寄存器后，所有中间计算均在寄存器中完成，仅需一次 [LoadAlign](../../../../../docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reg_data_load/LoadAlign_continuous.md) 和一次 [StoreAlign](../../../../../docs/zh/api/SIMD-API/basic_api/reg_vector_compute/reg_data_store/StoreAlign_continuous.md)，消除中间结果的 Load/Store 开销
- 支持 VF 双发特性，常规计算指令并行度可达 512 bytes/cycle，指令发射效率（IPC）大幅提升

```
基础API模式：                    VF融合模式：
Load → Mul → Store              LoadAlign → Mul
Load → Mul → Store                   → Mul
Load → Muls → Store                   → Muls
Load → Add → Store                    → Add
Load → Muls → Store                   → Muls
Load → Exp → Store                    → Exp
Load → Adds → Store                   → Adds
Load → Div → Store                    → Div → StoreAlign
（8次Load/Store）                 （1次Load/1次Store）
```

**代码改造点**：

对比入门级 Case 0 代码，关键改造如下：
- 将基础 API 的 8 次独立向量计算替换为 `asc_vf_call` 调用的 VF 函数
- VF 函数内使用 `Reg::LoadAlign`/`Reg::StoreAlign` 替代反复的 Load/Store
- 移除 `PipeBarrier<PIPE_V>()` 同步，VF 函数内计算无需显式同步

```cpp
// Case 0: 基础API，每次计算需Load/Store + PipeBarrier同步
AscendC::Mul(yLocal, xLocal, xLocal, n);
AscendC::PipeBarrier<PIPE_V>();
AscendC::Mul(yLocal, yLocal, xLocal, n);
AscendC::PipeBarrier<PIPE_V>();
// ... 共8次计算，8次同步

// Case 1: RegBase API + VF融合，寄存器内完成全部计算
__simd_vf__ inline void GeluVfBasic(__ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t n, uint32_t loopNum)
{
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg, yReg;

    for (uint16_t i = 0; i < loopNum; ++i) {
        mask = AscendC::Reg::UpdateMask<float>(n);
        AscendC::Reg::LoadAlign(xReg, xAddr + i * oneRepeatSize);  // 仅一次Load
        AscendC::Reg::Mul(yReg, xReg, xReg, mask);
        AscendC::Reg::Mul(yReg, yReg, xReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_A, mask);
        AscendC::Reg::Add(yReg, xReg, yReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_B, mask);
        AscendC::Reg::Exp(yReg, yReg, mask);
        AscendC::Reg::Adds(yReg, yReg, 1.0f, mask);
        AscendC::Reg::Div(yReg, xReg, yReg, mask);
        AscendC::Reg::StoreAlign(yAddr + i * oneRepeatSize, yReg, mask);  // 仅一次Store
    }
}
```

**VF 函数调用上下文**：

上述 VF 函数定义了 GELU 在寄存器中的计算逻辑，在核函数中通过 `asc_vf_call` 调用，调用示例如下：

```cpp
// 核函数中调用 VF 函数的代码片段
constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);
uint32_t loopNum = AscendC::Std::ceil_div(n, oneRepeatSize);
__ubuf__ float* xAddr = reinterpret_cast<__ubuf__ float*>(xLocal.GetPhyAddr());
__ubuf__ float* yAddr = reinterpret_cast<__ubuf__ float*>(yLocal.GetPhyAddr());
// 通过 asc_vf_call 调用 VF 函数，传入 UB 地址、元素数 n 和循环次数
asc_vf_call<GeluVfBasic>(xAddr, yAddr, n, loopNum);
```

**样例配置**：
- 多核切分：M方向分32份，N方向2份，共计64份数据，分布到64core上计算
- `tileLen = 8192` 为每次搬运和计算的数据元素个数

### 优化效果评估

**性能提升对比**：

| 指标 | Case 0（入门版） | Case 1（中级优化版） | 提升幅度 |
|:---|:---:|:---:|:---:|
| Task Duration(μs) | 351.525 | 348.868 | 0.76% |
| aiv_vec_time(μs) | 147.895 | 66.277 | **55.2%** |
| aiv_vec_ratio | 0.422 | 0.19 | - |
| aiv_scalar_time(μs) | 9.513 | 3.03 | **68.2%** |
| aiv_scalar_ratio | 0.027 | 0.009 | - |
| aiv_mte2_time(μs) | 320.283 | 320.543 | - |
| aiv_mte3_time(μs) | 303.647 | 314.547 | - |

> 本样例为 MTE2 bound，数据搬运占比超过 90%，因此端到端收益不明显，但向量计算耗时的优化效果显著。

**本阶段结论**：
- 通过 VF 融合，向量指令耗时从 147.895μs 降至 66.277μs，减少 **55.2%**，IPC 达到 1.20
- 当前样例为 MTE2 bound，性能瓶颈仍在数据搬运，VF 计算效率仍有提升空间
- VF 函数内 GELU 计算依赖路径较长，循环内指令双发效率未充分发挥，IPC 仅为 1.20，距离理论极限 2.0 仍有差距，需进一步优化指令级并行度

---

## 高阶极限打磨

### Case 2: 启用RegBase API、VF融合能力和循环展开优化

### 极限性能目标与残余瓶颈深剖

**距离理论峰值的差距**：

Case 1 的 IPC 为 1.20，距离 VF 融合理论极限 IPC=2.0 仍有较大差距。VF 融合场景下，常规计算指令并行度可达 512 bytes/cycle，但当前循环执行方式限制了指令双发效率。

**微架构级 Profiling 分析**：

基于 Case 1 的性能数据，残余瓶颈主要体现在：
- **瓶颈1：VF 函数循环内指令依赖链过长**：GELU 计算包含 8 条向量指令（Mul×2、Muls×2、Add×1、Exp×1、Adds×1、Div×1），形成较长的依赖链。循环执行时，编译器在有限执行队列中调度的循环次数太少，导致每 cycle 可双发的指令数不足，IPC 仅为 1.20
- **瓶颈2：循环控制标量开销**：for 循环的变量更新、条件判断引入标量指令，在向量指令大幅缩短后相对占比上升

### 极致打磨手段：循环展开优化

**原理**：

VF 函数内的 for 循环为简单迭代结构，编译器默认按序生成循环控制指令（比较、跳转），导致循环迭代之间存在标量开销。通过 `#pragma unroll N` 告诉编译器将循环展开 N 份，消除循环控制开销，同时使多组迭代的 VF 指令可交错排布，提高指令级并行度（ILP），使更多指令能够连续双发射。

```
未展开：                         展开后：
Iter0: Load→Mul→...→Store       Iter0: Load→Mul→...→Store
  ↓ (循环控制开销)               Iter1: Load→Mul→...→Store
Iter1: Load→Mul→...→Store       Iter2: Load→Mul→...→Store
  ↓ (循环控制开销)               Iter3: Load→Mul→...→Store
Iter2: Load→Mul→...→Store       ...（无循环控制开销，指令可交错排布）
```

**优化方案**：

在 VF 函数的 for 循环前添加 `#pragma unroll 6`，展开因子 6 为经验值，需根据实际场景调优：

```cpp
__simd_vf__ inline static void GeluVfBasic(__ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t n, uint32_t loopNum)
{
    constexpr uint32_t oneRepeatSize = AscendC::GetVecLen() / sizeof(float);
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg, yReg;
    #pragma unroll 6  // 循环展开优化，提高指令级并行度
    for (uint16_t i = 0; i < loopNum; ++i) {
        mask = AscendC::Reg::UpdateMask<float>(n);
        AscendC::Reg::LoadAlign(xReg, xAddr + i * oneRepeatSize);
        AscendC::Reg::Mul(yReg, xReg, xReg, mask);
        AscendC::Reg::Mul(yReg, yReg, xReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_A, mask);
        AscendC::Reg::Add(yReg, xReg, yReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_B, mask);
        AscendC::Reg::Exp(yReg, yReg, mask);
        AscendC::Reg::Adds(yReg, yReg, 1.0f, mask);
        AscendC::Reg::Div(yReg, xReg, yReg, mask);
        AscendC::Reg::StoreAlign(yAddr + i * oneRepeatSize, yReg, mask);
    }
}
```

**展开因子选择建议**：
- 展开过多：增加寄存器压力，可能导致溢出，性能反而劣化，同时代码大小增加
- 展开过少：优化效果不明显
- 建议：逐次尝试 2、4、6、8 等值，找到最优展开次数

**样例配置**：
- 多核切分：M方向分32份，N方向分2份，共计64份数据，分布到64core上计算
- `tileLen = 8192` 为每次搬运和计算的数据元素个数

### 终极性能达成验证

**Profiling 报告**：

| 指标 | Case 0（基准） | Case 1（VF融合） | Case 2（VF融合+循环展开） |
|:---|:---:|:---:|:---:|
| Task Duration(μs) | 351.525 | 348.868 | **344.436** |
| aiv_vec_time(μs) | 147.895 | 66.277 | **63.655** |
| aiv_vec_ratio | 0.422 | 0.19 | 0.185 |
| aiv_scalar_time(μs) | 9.513 | 3.03 | 4.757 |
| aiv_scalar_ratio | 0.027 | 0.009 | 0.014 |
| aiv_mte2_time(μs) | 320.283 | 320.543 | 315.468 |
| aiv_mte3_time(μs) | 303.647 | 314.547 | 306.069 |
| IPC | - | 1.20 | **1.25** |

**关键验证指标**：
- 向量指令耗时从 Case 0 的 147.895μs 降至 **63.655μs**，减少 **56.9%**
- IPC 从 Case 1 的 1.20 提升至 **1.25**，提升 **4.2%**，指令发射效率进一步逼近理论极限
- 循环展开有效提高了指令级并行度，多组迭代的 VF 指令可交错双发射

**终极性能总结**：
- 在 VF 融合 + 循环展开的组合优化下，向量计算效率已达较优水平
- 当前 IPC 为 1.25，距离理论极限 2.0 仍有空间，主要受限于 GELU 8 条指令的依赖链长度
- 本样例为 MTE2 bound（数据搬运占比超过 90%），端到端收益受限于搬运瓶颈
- 进一步提升 IPC 的方向：将过长的 loop 循环切分为多个短循环，在长 latency 指令（如 Exp、Div）结束点进行拆分，使每组依赖链更短，双发效率更高。本样例 loop 循环较短，不适用于该优化项

---

## 性能对比总结

### Ascend 950PR性能对比

下表为本样例在Ascend 950系列产品上运行的性能数据对比：

| Case | 优化策略 | 核数 | tileLen | Task Duration(μs) | aiv_vec_time(μs) | 理论vector耗时(μs) | 端到端耗时相对Case 0 | vector耗时相对Case 0 |
|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| 0 | Gelu未使能VF融合能力（基准） | 64 | 8192 | 351.525 | 147.895 | 139.02 | 1x | 1x |
| 1 | 启用RegBase API和VF融合能力 | 64 | 8192 | 348.868 | 66.277 | NA | 1.01x | 2.23x |
| 2 | 启用RegBase API、VF融合和循环展开 | 64 | 8192 | 344.436 | 63.655 | NA | 1.02x | 2.32x |

> **注意：** 该样例为MTE2 bound，性能瓶颈为数据搬运，下述分析主要针对vector耗时计算，方便用户分析使能VF融合和循环展开的性能收益。Case 2相比Case 1，vector耗时减少 **4.6%**。

### 理论性能分析

下表为本样例的vector计算指令的并行度：

| 序号 | 指令 | 数据类型 | 计算并行度(bytes/cycle) |
|:---:|:---:|:---:|:---:|
| 1 | Mul | float | 256 |
| 2 | Mul | float | 256 |
| 3 | Muls | float | 256 |
| 4 | Add | float | 256 |
| 5 | Muls | float | 256 |
| 6 | Exp | float | 64 |
| 7 | Adds | float | 256 |
| 8 | Div | float | 64 |

**理论vector耗时计算**：

**Step 1: 计算Gelu计算流程的等效每cycle数据处理量（不考虑VF融合指令双发特性）**

Gelu计算包含8条向量指令，各指令的并行度如下：
- 6条线性运算指令（Mul×2、Muls×2、Add×1、Adds×1）：并行度256 bytes/cycle
- 2条非线性运算指令（Exp×1、Div×1）：并行度64 bytes/cycle

处理1 byte数据需要的cycle数：
- 线性运算指令：每条需要 1/256 cycles
- 非线性运算指令：每条需要 1/64 cycles

处理1 byte数据所需总cycle数：

$$
Cycles_{\text{per\_byte}} = 6 \times \frac{1}{256} + 2 \times \frac{1}{64} = \frac{6}{256} + \frac{2}{64} = \frac{14}{256}
$$

等效每cycle处理数据量：

$$
P_{\text{bytes}} = \frac{256}{14} = \frac{128}{7} \approx 18.29 \text{ bytes/cycle}
$$

**Step 2: 计算理论vector耗时（不考虑VF融合指令双发特性）**

本样例在Ascend 950PR系列产品上运行，硬件参数：
- 主频 $f = 1650 \text{ MHz} = 1.65 \times 10^9 \text{ Hz}$
- AIV核数 $N_{\text{core}} = 64$
- 数据shape $M = N = 8192$，总数据量 $D = M \times N \times 4 = 268435456 \text{ bytes}$

理论vector耗时计算公式：

$$
T_{\text{theory}} = \frac{D}{P_{\text{bytes}} \times f \times N_{\text{core}}}
$$

代入数值：

$$
T_{\text{theory}} = \frac{268435456}{\frac{128}{7} \times 1.65 \times 10^9 \times 64} = \frac{268435456 \times 7}{128 \times 1.056 \times 10^{11}} = \frac{1879048192}{1.35168 \times 10^{11}} \approx 139.02 \times 10^{-6} \text{ s} = 139.02 \text{ μs}
$$

**Step 3: 计算VF融合场景下理论vector耗时**

在VF融合场景下，vector指令具有双发特性，即常规计算指令并行度可以达到512 bytes/cycle，但是exp和div在双发场景下不能简单按照128 bytes/cycle进行估计。在VF融合场景下，更多采用IPC(Instructions Per Cycle，每cycle指令发射数量)来衡量性能。

case 1场景，单核vector计算指令数量：

$$
N_{\text{instr}} = N_{\text{VF}} \times N_{\text{loop}} \times N_{\text{op}} = 128 \times 128 \times 8 = 131072
$$

式中：
- $N_{\text{VF}} = 128$：VF函数调用次数
- $N_{\text{loop}} = 128$：VF内部循环次数
- $N_{\text{op}} = 8$：VF内计算指令数（Mul×2、Muls×2、Add×1、Adds×1、Exp×1、Div×1）

vector计算的cycle数：

$$
Cycles_{\text{vec}} = T_{\text{vec}} \times f = 66.277 \text{ μs} \times 1650 \text{ MHz} = 109357
$$

式中：
- $T_{\text{vec}} = 66.277 \text{ μs}$：实测vector计算耗时
- $f = 1650 \text{ MHz}$：硬件主频

IPC计算：

$$
IPC = \frac{N_{\text{instr}}}{Cycles_{\text{vec}}} = \frac{131072}{109357} \approx 1.20
$$

式中：
- $N_{\text{instr}} = 131072$：单核vector计算指令数量
- $Cycles_{\text{vec}} = 109357$：vector计算cycle数

IPC越高越好，理论极限接近2。

**Case 2: 循环展开优化后的IPC分析**

case 2场景，在VF融合基础上添加循环展开优化，单核vector计算指令数量保持不变：

$$
N_{\text{instr}} = N_{\text{VF}} \times N_{\text{loop}} \times N_{\text{op}} = 128 \times 128 \times 8 = 131072
$$

式中各参数与Case 1相同，循环展开不影响总指令数量。

vector计算的cycle数：

$$
Cycles_{\text{vec}} = T_{\text{vec}} \times f = 63.655 \text{ μs} \times 1650 \text{ MHz} = 105031
$$

IPC计算：

$$
IPC = \frac{N_{\text{instr}}}{Cycles_{\text{vec}}} = \frac{131072}{105031} \approx 1.25
$$

**循环展开对IPC的影响分析**：
- Case 2的IPC为1.25，相比Case 1的IPC（1.20）提升了 **4.2%**
- 循环展开优化提高了vector指令发射效率，使得更多的指令能够并行执行

**IPC优化建议**：
- 在VF融合场景下，IPC是衡量vector指令发射效率的重要指标
- 理想IPC极限接近2.0，大多数情况下达成1.4~1.5则表示取得较优性能
- 当一个VF函数loop循环内包含的指令依赖链过长时，会导致执行队列中存放的循环次数太少，从而导致每cycle可以双发的指令数变少，导致性能下降。此时可以将过长的loop循环切分成多个，比如可以选择reduce结束，或者长latency指令的结束点（比如div、exp），将一个loop循环拆分成2~3个loop循环。本样例给出的示例loop循环较短，不适用于该优化项。

### 优化要点总结

| 优化手段 | 核心原理 | 使用建议 |
|:---|:---|:---|
| 公式化简 | 减少计算步骤，降低计算开销 | 优先进行公式推导和化简 |
| RegBase API + VF融合 | 寄存器级计算减少中间Load/Store，利用双发特性提升性能 | 使用asc_vf_call调用VF函数，利用双发特性提高IPC |
| 循环展开 | 提高指令发射并行度 | 使用`#pragma unroll N`，N需根据实际调优 |

---

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 切换Case

  在 cmake 编译时通过 `-DSCENARIO_NUM=N` 指定要编译的 case，各 case 说明：
  - `0`: Gelu未使能VF融合能力（需设置 `-DCMAKE_VF_MODE=false`）
  - `1`: 启用RegBase API和VF融合能力
  - `2`: 启用RegBase API、VF融合能力和循环展开优化

  > **注意：** 编译器有VF自动融合的能力，默认会使能VF自动融合，本样例为方便对比分析性能，在case 0时需要关闭VF自动融合能力。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  SCENARIO_NUM=0
  CMAKE_VF_MODE=false
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_VF_MODE=$CMAKE_VF_MODE;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如：
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_VF_MODE=$CMAKE_VF_MODE;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式或Case前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　　　　　　　　　　　　　　　　　　 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0`、`1`、`2`　　　　　| Case编号：0=Gelu未使能VF融合，1=启用RegBase API和VF融合，2=启用RegBase API、VF融合和循环展开 |
  | `CMAKE_VF_MODE` | `true`、`false`　　　　　| VF融合模式：case 0时需设置为false关闭VF自动融合，case 1/2时需设置为true使能VF融合 |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  error ratio: 0.0000, tolerance:0.0001
  test pass!
  ```

### 性能分析

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
