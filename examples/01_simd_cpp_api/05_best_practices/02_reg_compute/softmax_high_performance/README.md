# Softmax 性能调优样例

## 概述

本样例以单核Softmax为例，展示从MemBase到RegBase、从基础循环到使用循环融合、循环展开等优化手段的完整性能调优路径，包含Case 0-5共六个逐步优化的版本。

**优化路径**：
- Case 0 → Case 1：MemBase → RegBase API（寄存器级计算）
- Case 1 → Case 2：循环融合 + ExpSub融合指令 + UpdateMask尾块处理
- Case 2 → Case 3：外层循环展开（UpdateMask模式）
- Case 2 → Case 4：主尾块模式（main-tail block）
- Case 4 → Case 5：主尾块模式 + 外层循环展开 + ExpSub融合指令

**前置阅读**：开始阅读前，建议对以下背景知识有初步了解，为后续各Case的优化策略提供理论支持：
- **[Reg矢量计算编程](../../../../../docs/zh/guide/programming_guide/programming_model/ai_core_simd_programming/cpp_tensor_programming/reg_vector_computation.md)**：
- **[指令双发优化](../../../../../docs/zh/guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/dual_issue_optimization.md)**：
- **[VF融合优化](../../../../../docs/zh/guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/vf_fusion_optimization.md)**：
- **[VF循环优化](../../../../../docs/zh/guide/operator_practice/simd_operator_optimization/vector_compute/vf_optimization/vf_loop_optimization.md)**：

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |

## 目录结构介绍

```
├── softmax_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── softmax.asc             // Ascend C样例实现（Case 0-5六个版本）
│   └── README.md               // 样例说明文档
```

## 样例描述

**样例功能**：

样例实现的是固定shape(m, n)为(128,128)的Softmax运算。

Softmax的计算公式为：

$$
\text{Softmax}(x_{i,j}) = \frac{e^{x_{i,j} - \max_i}}{\sum_{j=0}^{n-1} e^{x_{i,j} - \max_i}}
$$

- x：输入，形状为[128, 128]，数据类型为float；
- y：输出，形状为[128, 128]，数据类型为float；

**样例规格**：

<table border="2">
<tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">Softmax</td></tr>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[128, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">y</td><td align="center">[128, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">softmax_custom</td></tr>
</table>

## 版本概览

**表1 各版本性能及优化项说明表**
| 版本 | API类型 | 特点 | 性能 (折算后Vector指令开销) |
|:---:|:---|:---|:---:|
| **Case 0** | MemBase | 基础版本，单循环内完成所有计算 | 44113 cycles |
| **Case 1** | RegBase | 首次使用Reg矢量API，未融合，含冗余Load/Store/sync | 31894 cycles |
| **Case 2** | RegBase | UpdateMask模式 + 循环融合 + ExpSub融合指令 | 3413 cycles |
| **Case 3** | RegBase | UpdateMask模式 + 外层循环展开，提升指令双发能力 | 2197 cycles |
| **Case 4** | RegBase | 主尾块模式（main-tail block），替代UpdateMask | 3424 cycles |
| **Case 5** | RegBase | 主尾块模式 + 外层循环展开 + ExpSub融合指令（最优） | 1785 cycles |


**表2 AI Core 性能指标字段说明表**
| 字段名 | 字段含义 |
|:---:|:---|
| aiv_total_cycle | vector单算子端到端cycle数。 |
| aiv_vec_ratio | vector类型指令在端到端耗时中比率。 |

**说明**：
1. 本例针对单核Softmax内VF性能优化详解，当前不考虑多核切分、数据搬运、DoubleBuffer等因素影响。
2. Vector指令开销通过aiv_total_cycles及aiv_vec_ratio相乘折算得到。

## 优化策略分析

本节详细介绍从Case 0到Case 5的渐进优化策略。

## 第一阶段：MemBase (Case 0)

### Case 0: MemBase基础版本

**实现方式**：使用Ascend C基础API Memory矢量计算接口，在128行的for循环内依次完成ReduceMax、Duplicate、Sub、Exp、ReduceSum、Duplicate、Div全部7步操作。

**理论分析 — 为什么MemBase矢量计算接口慢**：

Case 0使用Memory矢量计算API，其计算过程中API通过封装Vector Function调用Reg矢量接口完成计算并实现代际兼容。每条指令的语义是"从UB读源操作数进入寄存器 → 在执行单元计算 → 将寄存器中的数据写回UB目的操作数"的完整流程。这意味着：

1. **每步中间结果必须写回UB**：ReduceMax的结果写入UB后，Duplicate需要从UB重新读出；Duplicate写入UB后，Sub需要从UB重新读出……以此类推。中间结果在UB上反复读写，性能较差。
2. **冗余的同步及指令**：由于数据会反复搬入搬出寄存器，缺少寄存器级别的数据复用，并且Vector Function间存在数据依赖，无法使能双发等能力。
3. **UB Bank冲突风险高**：连续的UB读写访问极易触发Bank冲突，导致访问延迟增加。

**关键代码**：
```cpp
for (uint32_t i = 0; i < aDim; i++) {
    AscendC::ReduceMax(expTensor[i * rDimAlign], srcTensor[i * rDimAlign], sumTensor[i * rDimAlign], rDim);
    AscendC::Duplicate(expTensor[i * rDimAlign], expTensor[i * rDimAlign], rDim);
    AscendC::Sub(expTensor[i * rDimAlign], srcTensor[i * rDimAlign], expTensor[i * rDimAlign], rDim);
    AscendC::Exp(expTensor[i * rDimAlign], expTensor[i * rDimAlign], rDim);
    AscendC::ReduceSum(sumTensor[i * rDimAlign], expTensor[i * rDimAlign], dstTensor[i * rDimAlign].ReinterpretCast<float>(), rDim);
    AscendC::Duplicate(sumTensor[i * rDimAlign], sumTensor[i * rDimAlign], rDim);
    AscendC::Div(dstTensor[i * rDimAlign], expTensor[i * rDimAlign], sumTensor[i * rDimAlign], rDim);
}
```

**性能**：44113 cycles（vector部分）

**说明**：MemBase接口可以通过编译期优化进行接口间的Vector Function融合来减少冗余的搬运开销，由于融合过程不可见，因此灵活性不如RegBase矢量计算接口。


## 第二阶段：MemBase → RegBase (Case 0 → Case 1)

### Case 1: Reg矢量API基线写法 — 循环未融合，含冗余操作

**优化动机**：MemBase API每次向量计算内部隐式执行 Load→Compute→Store，每次执行需要启动一次Vector Function。RegBase API可将数据加载到寄存器后常驻，中间结果暂存寄存器，多个计算可以通过一个Vector Function完成，仅需最终Store回UB。

**理论分析 — MemBase → RegBase 的关键收益**：

1. **Register编程减少冗余指令和VF启动开销**：
   在MemBase中，单个接口对应一个Vector Function。RegBase引入SIMD Register后，多个接口的计算可以通过启动一个Vector Function完成多条指令计算，从而减少VF启动开销：
   ```
   MemBase: UB → Load → Compute → Store → UB → Load → Compute → Store → UB ...
   RegBase: UB → Load → Compute(reg) → Compute(reg) → ... → Store → UB
   ```
   当前RegBase的基础写法仍会写入UB，未进行循环融合。

2. **显式Load/Store控制搬运时机**：
   MemBase中搬运动作由API隐式完成，开发者无法控制。RegBase通过 `LoadAlign` / `StoreAlign`（对齐搬运）或 `LoadUnAlign` / `StoreUnAlign`（非对齐搬运）显式控制搬运粒度与时机，可以精确调度数据流。

**关键变化**：

1. 使用 `Reg::LoadAlign` / `Reg::StoreAlign` 显式控制搬运
2. 使用 `Reg::Max` / `Reg::Sub` / `Reg::Exp` / `Reg::ReduceMax` / `Reg::ReduceSum` 等寄存器级API
3. 使用 `UpdateMask` 处理非对齐尾块
4. 通过 `LocalMemBar` 保证数据一致性

**Case 1存在的问题**：
- 多个循环未融合，存在冗余 Load/Store 指令
- 每次计算后插入 `LocalMemBar`，引入冗余同步
- 未使用 ExpSub 融合指令，Sub 和 Exp 分开执行

**性能**：31894 cycles（相比 Case 0 提升 34%）。RegBase带来的寄存器级计算消除了VF的启动开销，但每次计算通过一个单独的循环完成并写回UB，仍有很大的优化空间。

## 第三阶段：RegBase深度优化 (Case 1 → Case 2)

### Case 2: 循环融合 + ExpSub融合指令 + UpdateMask尾块处理

**理论分析 — 三项优化的深层原理**：

**1. 循环融合消除冗余Load/Store/同步**：

Case 1中ReduceMax循环和ExpSub循环各自独立从UB Load源数据，本质上是两次相同的UB→寄存器搬运。循环融合将两个循环的Load共享，从而消除冗余的搬运指令和同步指令，原理如下：

```
Case 1 (未融合):
  for i: for j: Load(src) → Max → ReduceMax     // Load src 第1次
  LocalMemBar()
  for i: for j: Load(src) → Sub+Exp → Store      // Load src 第2次 (冗余!)
  LocalMemBar()
  for i: for j: Load(exp) → Add → ReduceSum      // Load exp 第1次
  LocalMemBar()
  for i: for j: Load(exp) → Duplicate+Div → Store // Load exp 第2次 (冗余!)

Case 2 (融合):
  for i:
      for j: Load(src) → Max → ReduceMax          // Load src
      for j: Load(src) → ExpSub → Store           // 复用已Load的src (无需重复搬入)
  LocalMemBar()
  for i:
      for j: Load(exp) → Add → ReduceSum          // Load exp
      for j: Load(exp) → Duplicate+Div → Store    // 复用已Load的exp
```

Case 2只需2次Load（src和exp各一次），Case 1需要4次Load，融合后消除2次冗余UB搬运，并且消除了部分同步。

**2. 指令融合提升性能**：

Case 1中 `Sub` 与 `Exp` 为两条独立指令：
```cpp
// Case 1: Sub → Exp 顺序执行，Exp必须等待Sub结果写入dstReg
AscendC::Reg::Sub(dstReg, srcReg, maxReg, mask);
AscendC::Reg::Exp(dstReg, dstReg, mask);
```
ExpSub融合后为单条指令，消除了一条指令的开销，当Softmax的Shape最后一个维度越大，取得的收益越大：
```cpp
// Case 2: 一步融合，Sub→Exp在硬件内部完成
AscendC::Reg::ExpSub(expReg, srcReg, maxReg, mask);
```
在指令双发的执行队列中，减少一条依赖指令意味着后续独立指令可以更早被发射。

**3. UpdateMask vs 主尾块**：

UpdateMask模式在每次内层循环迭代中调用 `UpdateMask` 更新掩码，虽然满足Hardware Loop（无if/else），但每次调用仍产生标量计算开销。主尾块模式将数据切分为"主块（全Mask）"和"尾块（部分Mask）"分别处理，主块循环内无需任何掩码更新操作，完全消除UpdateMask的累加开销。

使用 `UpdateMask` 在每次内层循环中自动更新掩码，代码简洁：
```cpp
uint32_t count = rDim;
for (uint16_t j = 0; j < repeatTimes; j++) {
    mask = AscendC::Reg::UpdateMask<float>(count);
    LoadWithCastIfNeed(srcAddr, srcReg, mask, ...);
    AscendC::Reg::Max(maxReg, maxReg, srcReg, mask);
}
```

**效果**：31894 → 3413 cycles（提升 9.3x），融合收益显著。

## 第四阶段：循环展开优化、合理运用主尾块模式

### Case 2 → Case 3: 外层循环展开（UpdateMask模式）

**优化动机**：Case 2中128行依次串行处理，难以跨行调度指令，限制了VF双发效率。

**理论分析 — 外层展开如何提升指令双发**：

Case 2的执行模式为：行0的Load→Max→ReduceMax→Load→ExpSub→Store 全部执行完，再执行行1。此时执行队列中仅有一行（行i）的指令序列，而行i内部存在明显的依赖链：

```
Load(src_i) → Max(max_i, src_i) → ... → ReduceMax(max_i) → ... → Load(src_i) → ExpSub(...) → Store
```

依赖链上的指令必须等待前序指令完成，无法与任何其他指令并行发射。内部所有指令彼此依赖，实际可同时发射的无依赖指令很少。

**外层展开（展开因子=2）** 使用双套寄存器组（`*Reg` 和 `*Reg1`），每次处理两行：

```
行0: Load(src0) → Max(max0, src0) → ... → ReduceMax(max0) → Load(src0) → ExpSub(exp0) → Store
行1:   Load(src1) → Max(max1, src1) → ... → ReduceMax(max1) → Load(src1) → ExpSub(exp1) → Store
```

行0与行1之间**无任何数据依赖**，两条流水线可以同时发射：执行队列中同时填充两行无依赖的指令，双发率显著提升。

这一机制对应"指令双发优化"中的核心原则：为执行队列提供足够多且无依赖的并发指令。当前shape为128行，每个展开组处理2行，共64组，寄存器使用量为单行模式的约2倍，仍在硬件限制内（展开后约6-8个RegTensor，远未达到32上限）。

**优化措施**：手动展开最外层循环，每次处理两行数据，使用双套寄存器组（`*Reg` 和 `*Reg1`）：

```cpp
// Case 2: 串行处理
for (uint16_t i = 0; i < aDim; i++) {
    // 单行 ReduceMax → ExpSub
}
for (uint16_t i = 0; i < aDim; i++) {
    // 单行 ReduceSum → Div
}

// Case 3: 外层展开，每次处理两行
uint16_t halfA = aDim >> 1;
for (uint16_t i = 0; i < halfA; i++) {
    Duplicate(maxReg, ...);  Duplicate(maxReg1, ...);
    for (j) {
        Load(srcRow0, srcRow0+halfA_offset);  // 两行交错Load
        Max(maxReg, srcRow0);  Max(maxReg1, srcRow1);
    }
    ReduceMax(maxReg);  ReduceMax(maxReg1);
    for (j) {
        Load(srcRow0, srcRow0+halfA_offset);
        ExpSub(expReg);  ExpSub(expReg1);
        Store(...);
    }
}
```

**效果**：3413 → 2197 cycles（提升 35.6%），外层展开有效提升指令双发能力。

### Case 2/3 → Case 4/5: 主尾块模式

**优化动机**：UpdateMask虽然有掩码自动更新特性，但每次调用UpdateMask仍引入标量计算（更新计数值、生成掩码）。主尾块模式下，主块128行的全部内层循环全部使用编译期创建的 `CreateMask<ALL>()`（全Mask），完全消除UpdateMask的累加开销，循环内**无需任何条件判断或掩码更新**。当前shape下 `rDim=128`，每个float占4B，`VL=256B` 对应64元素，恰好整除 `128 / 64 = 2`，尾块为0，主尾块模式达到收益上限。

**关键代码变化**：
```cpp
// 先算出主块次数和尾块大小
uint16_t tail = rDim % (256 / sizeof(float));
uint16_t repeatTimesMain = rDim / (256 / sizeof(float));
uint16_t hasTail = (tail != 0);

// Case 4: 内层循环全部使用全Mask（maskFull），无需UpdateMask
for (uint16_t i = 0; i < aDim; i++) {
    for (uint16_t j = 0; j < repeatTimesMain; j++) {
        LoadWithCastIfNeed(srcAddr, srcReg, maskFull, ...);
        AscendC::Reg::Max(maxReg, maxReg, srcReg, maskFull);
    }
    ReduceMax(maxReg, maskFull);
    Duplicate(maxReg, maxReg, maskFull);
    for (uint16_t j = 0; j < repeatTimesMain; j++) {
        LoadWithCastIfNeed(srcAddr, srcReg, maskFull, ...);
        AscendC::Reg::ExpSub(expReg, srcReg, maxReg, maskFull);
    }
}
```

**理论分析 — 最大化双发效率的最优组合**：

Case 5同时应用了三个维度上互不冲突的优化手段，形成"1 + 1 + 1 > 3"的叠加效果：

| 优化手段 | 核心收益 | 对寄存器的影响 | 是否相互干扰 |
| --- | --- | --- | --- |
| 主尾块切分 | 主块全Mask，消除UpdateMask开销 | 无额外寄存器 | 不干扰 |
| 外层循环展开(×2) | 两行数据交错发射，填充执行队列 | ×2 RegTensor用量 | 不干扰 |
| ExpSub融合指令 | Sub+Exp合并为单指令，减依赖链 | 减少1个临时RegTensor | 不干扰 |

三项优化的组合机制：

1. **寄存器资源利用**：当前shape下单行需要约4-6个RegTensor（src、max、exp、sum、临时寄存器），展开2倍后约8-12个RegTensor，MaskReg使用量约2-3个，均在硬件限制内（≤32 RegTensor、≤8 MaskReg）。寄存器资源充足是三重优化可以叠加的前提。

2. **执行队列深度最大化**：外层展开向执行队列中填充两行无依赖的指令；主尾块确保主块循环内无条件分支（全Mask无UpdateMask调用），指令序列连续无跳转；ExpSub将Sub+Exp的两条有依赖指令合并为单条，使执行队列中指令总数减少、依赖关系更简短。

**性能**：1785 cycles（相比Case 0提升 27.2x），为本样例最优性能。

**结论**：当前shape下last轴仅128，尾块为0，主尾块模式展开后性能最优（Case 5），相应的，代码复杂度上升，UpdateMask模式代码更简洁，需根据场景权衡。

## 性能对比总结

### 优化要点总结

| 优化手段 | 核心原理 | 适用版本 |
|:---|:---|:---|
| MemBase→RegBase | 寄存器级计算减少UB↔寄存器往返和VF启动开销 | Case 0→Case 1 |
| ExpSub融合指令 | 单条指令替代Sub+Exp，减少延迟 | Case 1→Case 2 |
| 循环融合 | 共享Load/Store，消除冗余搬运和同步 | Case 1→Case 2 |
| UpdateMask | 自动更新掩码，简洁尾块处理 | Case 2, Case 3 |
| 主尾块模式 | 主块全Mask无更新开销，尾块独立处理 | Case 4, Case 5 |
| 外层循环展开 | 多套寄存器组交错执行，提升指令双发 | Case 2→Case 3, Case 4→Case 5 |

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 切换版本

  通过 `-DSCENARIO_NUM=N` (N=0~5) 指定要编译的版本：

  | SCENARIO_NUM | 版本 | 说明 | CMAKE_VF_MODE |
  |:---:|:---:|:---|:---:|
  | 0 | Case 0 | MemBase，单循环 | false |
  | 1 | Case 1 | RegBase，未融合 | false |
  | 2 | Case 2 | RegBase，UpdateMask+ExpSub+融合 | false |
  | 3 | Case 3 | RegBase，UpdateMask+循环展开 | false |
  | 4 | Case 4 | RegBase，主尾块模式 | false |
  | 5 | Case 5 | RegBase，主尾块+循环展开+ExpSub | false |

- 配置环境变量
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

- 单版本执行
  ```bash
  mkdir -p build && cd build
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=5 -DCMAKE_VF_MODE=false
  make -j
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin
  ```

- 执行结果

  ```bash
  error ratio: 0.0000, tolerance:0.0001
  test pass!
  ```

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |:---|:---|:---|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构 |
  | `SCENARIO_NUM` | `0`~`5` | 版本编号 Case 0-5 |
  | `CMAKE_VF_MODE` | `true`、`false` | VF融合模式：默认为false |

### 性能分析

使用 `msOpProf` 工具获取详细性能数据：

```bash
msopprof ./demo
cat ./OPPROF_*/PipeUtilization.csv
```
