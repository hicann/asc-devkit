# Gelu+Element-wise高性能优化样例


## 概述

本样例以Gelu+Element-wise计算为例（在Gelu后面再引入一系列的Element-wise计算操作，增加计算深度），介绍RegBase的向量性能调优方法，样例展示并行度调整、循环拆分和循环展开之后的性能收益情况。

> **建议**：在阅读本样例之前，建议先阅读 [Gelu性能优化样例](../gelu_high_performance/README.md)，了解VF融合的基本优化原理，再学习本样例中更复杂的Loop拆分优化方法。

**优化路径**：
- Case 0: 使用单loop计算，并行度调整为128 bytes
- Case 1: 使用单loop计算，提升并行度到256 bytes
- Case 2: 将VF的loop循环拆分为两个loop，分别执行Gelu和Element-wise计算
- Case 3: 在Case 2基础上，启用unroll循环展开优化

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

## 目录结构介绍

```
├── gelu_eltwise_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── gelu_eltwise.asc        // Ascend C样例实现（包含4个优化case）
│   └── README.md               // 样例说明文档
```

## 样例描述

**样例功能**：

本样例包含两个计算步骤：Gelu计算和Element-wise计算。

**Gelu近似公式**：

$$
GELU(x) \approx \frac{x}{1 + e^{-1.595769 \cdot (x + 0.044715 \cdot x^3)}}
$$

其中，$-2 \cdot \sqrt{\frac{2}{\pi}} \approx -1.595769$，$0.044715$ 为Gelu近似公式的系数。

**Element-wise计算公式**：

基于Gelu输出结果进行一系列Element-wise计算：

$$
y = e^{-0.5 \cdot (GELU(x) + 1)^2 + 2}
$$

**样例规格**：

<table border="2">
<tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Gelu+Element-wise</td></tr>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[8192, 8192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">y</td><td align="center">[8192, 8192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">gelu_eltwise_custom</td></tr>
</table>

## 样例实现

### 性能指标说明

**表1 AI Core 性能指标字段说明表**

| 字段名 | 字段含义 |
|:---|:---|
|Task Duration(μs)|Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间。|
|aiv_time(μs)|Task在AI Vector Core上的执行时间。|
|aiv_total_cycles|所有vector核的总共计算的cycle数量。|
|aiv_vec_time(μs)|vec类型指令（向量类运算指令）耗时。|
|aiv_vec_ratio|vec类型指令（向量类运算指令）的cycle数在total cycle数中的占用比。|
|aiv_scalar_time(μs)|scalar类型指令（标量类运算指令）耗时。|
|aiv_scalar_ratio|scalar类型指令（标量类运算指令）的cycle数在total cycle数中的占用比。|
|aiv_mte2_time(μs)|mte2类型指令（GM->UB搬运类指令）耗时。|
|aiv_mte2_ratio|mte2类型指令（GM->UB搬运类指令）的cycle数在total cycle数中的占用比。|
|aiv_mte3_time(μs)|mte3类型指令（UB->GM搬运类指令）耗时。|
|aiv_mte3_ratio|mte3类型指令（UB->GM搬运类指令）的cycle数在total cycle数中的占用比。|

### Case 0: 使用单loop计算，并行度调整为128 bytes

**实现方式**：参考 `KernelGeluEltwise::GeluEltwiseBasic()` 函数实现

基准程序使用RegBase API实现Gelu和Element-wise计算，按照能融合尽量融合的原则，把两种计算放在同一个VF的loop中计算；作为对比，将每次并行度设置为128 bytes（标准计算并行度为256 bytes）。

**关键代码**：
```cpp
__simd_vf__ inline static void GeluEltwiseBasic(
    __ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t n, uint32_t loopNum, uint32_t oneRepeatSize)
{
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg, yReg;

    for (uint16_t i = 0; i < loopNum; ++i) {
        // Case 0（scenarioNum == 0）固定使用 VL32 掩码（32 个 float、128 bytes 并行度）；
        // 非 Case 0 才使用 UpdateMask<float>(n) 按剩余元素数更新掩码
        mask = AscendC::Reg::CreateMask<float, AscendC::Reg::MaskPattern::VL32>();
        // Gelu计算
        AscendC::Reg::LoadAlign(xReg, xAddr + i * oneRepeatSize);
        AscendC::Reg::Mul(yReg, xReg, xReg, mask);
        AscendC::Reg::Mul(yReg, yReg, xReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_A, mask);
        AscendC::Reg::Add(yReg, xReg, yReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_B, mask);
        AscendC::Reg::Exp(yReg, yReg, mask);
        AscendC::Reg::Adds(yReg, yReg, 1.0f, mask);
        AscendC::Reg::Div(yReg, xReg, yReg, mask);
        // Element-wise计算
        AscendC::Reg::Adds(yReg, yReg, COEFF_C, mask);
        AscendC::Reg::Mul(yReg, yReg, yReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_D, mask);
        AscendC::Reg::Adds(yReg, yReg, COEFF_E, mask);
        AscendC::Reg::Exp(yReg, yReg, mask);
        AscendC::Reg::StoreAlign(yAddr + i * oneRepeatSize, yReg, mask);
    }
}
```

**样例配置**：
- 多核切分：M方向分32份，N方向分2份，共计64份数据，分布到64core上计算
- `tileLen = 8192` 为每次搬运和计算的数据元素个数
- `oneRepeatSize = 128 / sizeof(float) = 32` 个元素

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| 334.437 | 333.47 | 34582356 | 244.299 | 0.733 | 3.108 | 0.009 | 275.719 | 0.827 | 252.305 | 0.757 |

**优化效果分析**：
- 端到端耗时：**334.437μs**
- 向量指令耗时：244.299μs，占比 **73.3%**
- 数据搬运耗时：275.719μs（读）+ 252.305μs（写），搬运占比超过 **80%**
- 总计算cycle数：34582356

**下一步优化方向**：
- 提升并行度到256 bytes，提高数据处理效率

---

### Case 1: 使用单loop计算，提升并行度到256 bytes

**实现方式**：参考 `KernelGeluEltwise::GeluEltwiseBasic()` 函数实现

与Case 0使用相同实现方式，但将并行度提升到256 bytes（默认值）。

**样例配置**：
- 多核切分：M方向分32份，N方向分2份，共计64份数据，分布到64core上计算
- `tileLen = 8192` 为每次搬运和计算的数据元素个数
- `oneRepeatSize = 256 / sizeof(float) = 64` 个元素

**优化手段**：
- **并行度提升**：
  - 从128 bytes提升到256 bytes，提高单指令计算的吞吐能力

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| 324.862 | 324.09 | 33758796 | 125.203 | 0.386 | 3.187 | 0.01 | 292.819 | 0.904 | 280.044 | 0.864 |

**优化效果分析**：
- 端到端耗时：**324.862μs**，相比Case 0耗时减少 **2.9%**
- 向量指令耗时：125.203μs，相比Case 0耗时减少 **48.7%**
- 并行度提升后，计算指令吞吐增加，向量计算效率显著提升

**下一步优化方向**：
- 通过计算可知，IPC明显低于Gelu样例（后面章节有详细计算），将loop拆分，缩短单loop中的指令计算深度，提高指令双发效率

---

### Case 2: 将VF的loop循环拆分为两个loop

**实现方式**：参考 `KernelGeluEltwise::GeluEltwiseLoopSplit()` 函数实现

将Gelu和Element-wise计算拆分为两个独立的loop循环，分别执行步骤1-8和步骤9-13。

**关键代码**：
```cpp
__simd_vf__ inline static void GeluEltwiseLoopSplit(
    __ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t n, uint32_t loopNum, uint32_t oneRepeatSize)
{
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg, yReg;
    uint32_t n2 = n;

    // Loop 1: Gelu计算
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

    // Loop 2: Element-wise计算
    for (uint16_t i = 0; i < loopNum; ++i) {
        mask = AscendC::Reg::UpdateMask<float>(n2);
        AscendC::Reg::LoadAlign(yReg, yAddr + i * oneRepeatSize);
        AscendC::Reg::Adds(yReg, yReg, COEFF_C, mask);
        AscendC::Reg::Mul(yReg, yReg, yReg, mask);
        AscendC::Reg::Muls(yReg, yReg, COEFF_D, mask);
        AscendC::Reg::Adds(yReg, yReg, COEFF_E, mask);
        AscendC::Reg::Exp(yReg, yReg, mask);
        AscendC::Reg::StoreAlign(yAddr + i * oneRepeatSize, yReg, mask);
    }
}
```

**样例配置**：
- 多核切分：M方向分32份，N方向分2份，共计64份数据，分布到64core上计算
- `tileLen = 8192` 为每次搬运和计算的数据元素个数
- `oneRepeatSize = 256 / sizeof(float) = 64` 个元素

**优化手段**：
- **Loop拆分原理**：
  - 将13步计算拆分为两个loop：Gelu（8步）和Element-wise（5步）
  - VF函数中单loop中的指令计算深度较大，但是执行队列深度有限，执行队列无法容纳更多的指令，导致无法最大限度发挥指令双发能力

- **Loop拆分收益分析**：
  - 拆分点选择：建议选择在reduce操作结束点或长latency指令（如Exp、Div）结束点
  - Gelu计算以Div结束，Element-wise以Exp结束，均为长latency指令
  - 拆分后每个loop指令依赖链更短，有利于指令双发

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| 324.039 | 323.32 | 33794573 | 97.184 | 0.301 | 3.21 | 0.01 | 292.68 | 0.905 | 279.178 | 0.863 |

**优化效果分析**：
- 端到端耗时：**324.039μs**，相比Case 1耗时减少 **0.25%**
- 向量指令耗时：97.184μs，相比Case 1耗时减少 **22.7%**
- Loop拆分后，指令依赖链缩短，指令双发效率提升，向量计算效率进一步提高

**下一步优化方向**：
- 启用循环展开优化，进一步提高指令发射并行度

---

### Case 3: 启用unroll循环展开优化

**实现方式**：参考 `KernelGeluEltwise::GeluEltwiseLoopSplit()` 函数实现，添加 `#pragma unroll 6` 循环展开优化

在Case 2基础上，为两个loop循环分别添加unroll优化。

**关键代码**：
```cpp
__simd_vf__ inline static void GeluEltwiseLoopSplit(
    __ubuf__ float* xAddr, __ubuf__ float* yAddr, uint32_t n, uint32_t loopNum, uint32_t oneRepeatSize)
{
    AscendC::Reg::MaskReg mask;
    AscendC::Reg::RegTensor<float> xReg, yReg;
    uint32_t n2 = n;

    #pragma unroll 6
    // Loop 1: Gelu计算
    for (uint16_t i = 0; i < loopNum; ++i) {
        // ...
    }

    #pragma unroll 6
    // Loop 2: Element-wise计算
    for (uint16_t i = 0; i < loopNum; ++i) {
        // ...
    }
}
```

**样例配置**：
- 多核切分：M方向分32份，N方向分2份，共计64份数据，分布到64core上计算
- `tileLen = 8192` 为每次搬运和计算的数据元素个数
- `oneRepeatSize = 256 / sizeof(float) = 64` 个元素

**优化手段**：
- **循环展开优化原理**：
  - 通过 `#pragma unroll 6` 告诉编译器展开循环，每次展开6个迭代
  - 提高指令级并行度，使更多VF指令能够连续发射

- **循环展开收益分析**：
  - 展开因子选择：`unroll 6` 为经验值，需要根据实际场景调优
  - 展开过多：可能增加寄存器压力，导致性能劣化，同时代码大小也会增加
  - 展开过少：优化效果不明显
  - 建议：用户可以采用逐次尝试的方法，找到最优的循环展开次数

**性能数据**：

| Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|:---|
| 324.748 | 323.97 | 33905446 | 94.339 | 0.291 | 4.932 | 0.015 | 288.144 | 0.889 | 279.082 | 0.861 |

**优化效果分析**：
- 端到端耗时：**324.748μs**，相比Case 2耗时增加 **0.22%**
- 向量指令耗时：94.339μs，相比Case 2耗时减少 **2.9%**
- 循环展开后，更多指令能够并行执行，向量计算效率略有提升

---

## 性能对比总结

### Ascend 950PR性能对比

下表为本样例在Ascend 950系列产品上运行的性能数据对比：

| Case | 优化策略 | 核数 | tileLen | Task Duration(μs) | aiv_vec_time(μs) | 端到端耗时相对Case 0 | vector耗时相对Case 0 |
|:---|:---|:---|:---|:---|:---|:---|:---|
| 0 | 单loop计算，并行度128 bytes（基准） | 64 | 8192 | 334.437 | 244.299 | 1x | 1x |
| 1 | 单loop计算，并行度256 bytes | 64 | 8192 | 324.862 | 125.203 | 1.03x | 1.95x |
| 2 | Loop拆分（Gelu + Element-wise） | 64 | 8192 | 324.039 | 97.184 | 1.03x | 2.51x |
| 3 | Loop拆分 + unroll循环展开 | 64 | 8192 | 324.748 | 94.339 | 1.03x | 2.58x |

> **注意：** 该样例为MTE2 bound，性能瓶颈为数据搬运。端到端耗时变化较小，但向量计算效率显著提升。

### 优化要点总结

| 优化手段 | 核心原理 | 使用建议 |
|:---|:---|:---|
| 并行度调整 | 最大限度地用满指令并行处理能力 | 用满指令计算并行度 |
| Loop拆分 | 缩短单loop中指令计算深度，提高指令双发效率 | 选择reduce结束或长latency指令结束点作为拆分点 |
| 循环展开 | 提高指令发射并行度，减少循环控制开销 | 使用`#pragma unroll N`，N需根据实际调优 |

### 理论性能分析

下表为本样例的vector计算指令的并行度：

**Gelu计算指令并行度**：

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

**Element-wise计算指令并行度**：

| 序号 | 指令 | 数据类型 | 计算并行度(bytes/cycle) |
|:---:|:---:|:---:|:---:|
| 9 | Adds | float | 256 |
| 10 | Mul | float | 256 |
| 11 | Muls | float | 256 |
| 12 | Adds | float | 256 |
| 13 | Exp | float | 64 |

**理论vector耗时计算（不考虑VF融合指令双发特性）**：

本样例包含13步计算，各指令的并行度如下：
- 10条线性运算指令（Mul×3、Muls×3、Add×1、Adds×3）：并行度256 bytes/cycle
- 3条非线性运算指令（Exp×2、Div×1）：并行度64 bytes/cycle

处理1 byte数据需要的cycle数：

$$
Cycles_{\text{per\_byte}} = 10 \times \frac{1}{256} + 3 \times \frac{1}{64} = \frac{10}{256} + \frac{12}{256} = \frac{22}{256}
$$

等效每cycle处理数据量：

$$
P_{\text{bytes}} = \frac{256}{22} \approx 11.64 \text{ bytes/cycle}
$$

本样例在Ascend 950PR系列产品上运行，硬件参数：
- 主频 $f = 1650 \text{ MHz}$
- AIV核数 $N_{\text{core}} = 64$
- 数据shape $M = N = 8192$，总数据量 $D = M \times N \times 4 = 268435456 \text{ bytes}$

理论vector耗时计算公式：

$$
T_{\text{theory}} = \frac{D}{P_{\text{bytes}} \times f \times N_{\text{core}}}
$$

代入数值：

$$
T_{\text{theory}} = \frac{268435456}{\frac{256}{22} \times 1650 \times 10^6 \times 64} \approx 219.0 \text{ μs}
$$

**VF融合场景下的IPC分析**：

在VF融合场景下，vector指令具有双发特性，采用IPC(Instructions Per Cycle，每cycle指令发射数量)来衡量性能。

IPC计算公式：

$$
IPC = \frac{N_{\text{instr}}}{Cycles_{\text{vec}}}
$$

式中：
- $N_{\text{instr}}$：单核vector计算指令数量
- $Cycles_{\text{vec}} = T_{\text{vec}} \times f$：单核vector计算cycle数
- $f = 1650 \text{ MHz}$：硬件主频

**Case 0: 单loop计算，并行度128 bytes**

单核vector计算指令数量：

$$
N_{\text{instr}} = N_{\text{VF}} \times N_{\text{loop}} \times N_{\text{op}} = 128 \times 256 \times 13 = 425984
$$

式中：
- $N_{\text{VF}} = 128$：VF函数调用次数（Process外层循环）
- $N_{\text{loop}} = 256$：VF内部循环次数（oneRepeatSize=32时）
- $N_{\text{op}} = 13$：VF内计算指令数（Gelu×8 + Element-wise×5）

vector计算的cycle数：

$$
Cycles_{\text{vec}} = T_{\text{vec}} \times f = 244.299 \text{ μs} \times 1650 \text{ MHz} = 403093
$$

IPC计算：

$$
IPC = \frac{425984}{403093} \approx 1.06
$$

---

**Case 1: 单loop计算，并行度256 bytes**

单核vector计算指令数量：

$$
N_{\text{instr}} = N_{\text{VF}} \times N_{\text{loop}} \times N_{\text{op}} = 128 \times 128 \times 13 = 212992
$$

式中：
- $N_{\text{VF}} = 128$：VF函数调用次数
- $N_{\text{loop}} = 128$：VF内部循环次数（oneRepeatSize=64时）
- $N_{\text{op}} = 13$：VF内计算指令数

vector计算的cycle数：

$$
Cycles_{\text{vec}} = T_{\text{vec}} \times f = 125.203 \text{ μs} \times 1650 \text{ MHz} = 206585
$$

IPC计算：

$$
IPC = \frac{212992}{206585} \approx 1.03
$$

**IPC分析**：Case 1相比Case 0，IPC略有降低（1.03 vs 1.06），但vector耗时显著减少（减少48.7%）。这里要注意，虽然一般采用IPC衡量VF的性能，但是前提是要尽量用满指令并行度。

---

**Case 2: Loop拆分（Gelu + Element-wise）**

单核vector计算指令数量保持不变：

$$
N_{\text{instr}} = N_{\text{VF}} \times N_{\text{loop}} \times N_{\text{op}} = 128 \times 128 \times 13 = 212992
$$

式中各参数与Case 1相同，Loop拆分不影响总指令数量。

vector计算的cycle数：

$$
Cycles_{\text{vec}} = T_{\text{vec}} \times f = 97.184 \text{ μs} \times 1650 \text{ MHz} = 160354
$$

IPC计算：

$$
IPC = \frac{212992}{160354} \approx 1.33
$$

**IPC分析**：Case 2相比Case 1，IPC从1.03提升到1.33，提升了 **29.1%**。Loop拆分后，指令依赖链缩短，指令双发效率显著提高。

---

**Case 3: Loop拆分 + unroll循环展开**

单核vector计算指令数量保持不变：

$$
N_{\text{instr}} = N_{\text{VF}} \times N_{\text{loop}} \times N_{\text{op}} = 128 \times 128 \times 13 = 212992
$$

式中各参数与Case 2相同，循环展开不影响总指令数量。

vector计算的cycle数：

$$
Cycles_{\text{vec}} = T_{\text{vec}} \times f = 94.339 \text{ μs} \times 1650 \text{ MHz} = 155659
$$

IPC计算：

$$
IPC = \frac{212992}{155659} \approx 1.37
$$

**IPC分析**：Case 3相比Case 2，IPC从1.33提升到1.37，提升了 **3.0%**。循环展开后，更多指令能够并行执行，进一步提高指令发射效率。

---

### IPC对比总结

| Case | 优化策略 | vector耗时(μs) | IPC | IPC相对Case 0 |
|:---:|:---|:---:|:---:|:---:|
| 0 | 单loop计算，并行度128 bytes（基准） | 244.299 | 1.06 | 1x |
| 1 | 单loop计算，并行度256 bytes | 125.203 | 1.03 | 0.97x |
| 2 | Loop拆分（Gelu + Element-wise） | 97.184 | 1.33 | 1.25x |
| 3 | Loop拆分 + unroll循环展开 | 94.339 | 1.37 | 1.29x |

**IPC优化建议**：
- IPC越高越好，理论极限接近2.0，大多数情况下达成1.4~1.5则表示取得较优性能
- Case 3的IPC为1.37，已接近理想性能水平
- Loop拆分是提升IPC的一种优化手段，需要结合实际VF的计算深度进行分析和尝试

---

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 切换Case

  在 cmake 编译时通过 `-DSCENARIO_NUM=N` 指定要编译的 case，各 case 说明：
  - `0`: 单loop计算，并行度128 bytes
  - `1`: 单loop计算，并行度256 bytes
  - `2`: Loop拆分（Gelu + Element-wise）
  - `3`: Loop拆分 + unroll循环展开

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
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如：
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式或Case前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　　　　　　　　　　　　　　　　　　 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0`、`1`、`2`、`3`　　　　　| Case编号：0=单loop+128bytes，1=单loop+256bytes，2=loop拆分，3=loop拆分+unroll |

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
