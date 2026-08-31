# QuantGroupMatmul最佳实践样例


## 概述

本样例实现了per-token量化的分组矩阵乘法（QuantGroupMatmul），展示CV融合时Vector bound场景下的性能调优方法。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── quant_group_matmul_high_performance
│   ├── CMakeLists.txt                     // 编译工程文件
│   ├── data_utils.h                       // 数据读入写出函数
│   ├── figures                            // README中图片资源目录
│   ├── quant_group_matmul_custom.asc      // Ascend C样例实现 & 调用样例
│   ├── README.md                          // 样例说明文档
│   └── scripts
│       ├── gen_data.py                    // 输入数据和真值数据生成脚本
│       └── verify_result.py               // 验证输出数据和真值数据是否一致的验证脚本
```

## 样例描述

- 样例功能：  
  样例实现了分组的per-token量化matmul计算，分组轴为m轴，并对结果进行激活函数Gelu计算。

  QuantGroupMatmul的计算公式为：

  $$
  s_i = \sum_{t=0}^{i-1} group[t]
  $$

  $$
  y[s_i:s_i + group[i], :] =
  \operatorname{Gelu}\left(
  \left(x[s_i:s_i + group[i], :] \times weight[i]\right)
  \odot scale[i] \odot perTokenScale[s_i:s_i + group[i]]
  \right), \quad 0 \le i < g
  $$

  其中，$s_i$ 表示第 $i$ 组在m轴上的起始位置，$\odot$ 表示按广播规则逐元素相乘。

  - x：左矩阵，形状为[m, k]，数据类型为int8；
  - weight：右矩阵，逻辑形状为[g, k, n]，实际输入文件为NZ格式，数据类型为int8；
  - group：记录每组m的大小，数据类型为int64；
  - scale：右矩阵的量化参数，形状为[g, n]，数据类型为float，用于矩阵乘结果的反量化，对第i次矩阵乘结果采用scale[i]进行反量化；
  - perTokenScale：左矩阵的per-token scale，形状为[m]，数据类型为float，用于矩阵乘结果的反量化，采用与x行相同的索引范围进行反量化；
  - y：输出，存放矩阵乘结果的矩阵，形状为[m, n]，数据类型为float16；

- 样例规格：

<table>
<tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">QuantGroupMatmul</td></tr>
<tr><td rowspan="6" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[8192, 1024]</td><td align="center">int8</td><td align="center">ND</td></tr>
<tr><td align="center">weight</td><td align="center">[8, 1024, 8192]</td><td align="center">int8</td><td align="center">NZ</td></tr>
<tr><td align="center">group</td><td align="center">[8]</td><td align="center">int64</td><td align="center">ND</td></tr>
<tr><td align="center">scale</td><td align="center">[8, 8192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">perTokenScale</td><td align="center">[8192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">y</td><td align="center">[8192, 8192]</td><td align="center">float16</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">quant_group_matmul_custom</td></tr>
</table>

- 调用实现  
  使用内核调用符<<<>>>调用核函数。

## 样例实现

### 场景说明

本样例通过3个递进式场景展示Vector bound场景下的完整优化路径：

| 场景 | SCENARIO_NUM | __mix__模式 | PIPELINE_DEPTH | 说明 |
|------|-------------|------------|----------------|------|
| Scenario 0 | 0 | `__mix__(1, 1)` | 1 | 基准：单AIV，无流水重叠，Cube与Vector串行 |
| Scenario 1 | 1 | `__mix__(1, 2)` | 1 | 双AIV，无流水重叠，多AIV分摊Vector任务 |
| Scenario 2 | 2 | `__mix__(1, 2)` | 4 | 双AIV + 4级流水，Cube-Vector细粒度流水重叠 |

**关键代码**：
```cpp
constexpr uint32_t scenarioNum = SCENARIO_NUM;
// ...
constexpr uint32_t PIPELINE_DEPTH = (scenarioNum >= 2) ? 4 : 1;

template <...>
#if SCENARIO_NUM == 0
__global__ __mix__(1, 1)
#else
__global__ __mix__(1, 2)
#endif
void quant_group_matmul_custom(...)
```

### 性能指标说明

| 指标                    | 说明                                                                 |
|------------------------|----------------------------------------------------------------------|
| Task Duration(μs)      | 整个任务执行的总时间，样例执行时间以该参数为准                         |
| Block Num              | 使用的核数（Block 数量）                                              |
| aicore_time(μs)        | AI Core 的平均执行时间                                                |
| aic_mac_time(μs)       | cube 类型指令（矩阵类运算指令）耗时                                    |
| aic_mac_ratio          | cube 类型指令（矩阵类运算指令）的 cycle 数在 total cycle 数中的占用比    |
| aic_mte1_time(μs)      | mte1 类型指令（L1 -> L0A/L0B 搬运）耗时                               |
| aic_mte1_ratio         | mte1 类型指令（L1 -> L0A/L0B 搬运）的 cycle 数在 total cycle 数中的占用比 |
| aic_mte2_time(μs)      | mte2 类型指令（GM -> L1 搬运）耗时                                    |
| aic_mte2_ratio         | mte2 类型指令（GM -> L1 搬运）的 cycle 数在 total cycle 数中的占用比    |
| aic_fixpipe_time(μs)   | fixpipe 类型指令（L0C -> GM/UB 搬运）耗时                             |
| aic_fixpipe_ratio      | fixpipe 类型指令（L0C -> GM/UB 搬运）的 cycle 数在 total cycle 数中的占用比 |
| aiv_time(μs)           | Task 在 AI Vector Core 上的执行时间                               |
| aiv_vec_time(μs)       | vec 类型指令（向量类运算指令）耗时                                    |
| aiv_vec_ratio          | vec 类型指令（向量类运算指令）的 cycle 数在 total cycle 数中的占用比    |
| aiv_scalar_time(μs)    | scalar 类型指令（标量类运算指令）耗时                                 |
| aiv_scalar_ratio       | scalar 类型指令（标量类运算指令）的 cycle 数在 total cycle 数中的占用比 |
| aiv_mte2_time(μs)      | mte2 类型指令（GM -> UB 搬运类指令）耗时                              |
| aiv_mte2_ratio         | mte2 类型指令（GM -> UB 搬运类指令）的 cycle 数在 total cycle 数中的占用比 |
| aiv_mte3_time(μs)      | mte3 类型指令（UB -> GM 搬运类指令）耗时                              |
| aiv_mte3_ratio         | mte3 类型指令（UB -> GM 搬运类指令）的 cycle 数在 total cycle 数中的占用比 |

### 数据流路径

```text
存储位置                        数据类型           流水

GM (x, weight)                   int8
  |                                               MTE2
L1                               int8
  |                                               MTE1
L0A / L0B                        int8
  |  Iterate                                     MMAD
L0C                              int32
  |                                               FIXPIPE
GM (workspace)                   int32
  |  DataCopyPad2D                                MTE2
UB vecInQueue                    int32
  |  AscendDequant (×scale)   int32 → float       VECTOR
UB dequantMiddleResult            float
  |  Mul (×perTokenScale)     float → float       VECTOR
UB mulsResultLocal                float
  |  FasterGelu               float → float       VECTOR
UB actResultLocal                 float
  |  Cast                     float → half        VECTOR
UB vecOutQueue                    half
  |  DataCopyPad2D                                MTE3
GM (y)                            half

注：scale[float] 和 perTokenScale[float] 通过 MTE2 独立搬入UB，
    分别参与 AscendDequant 和 Mul 步骤。
```

### 核心特性

#### Case 0：基准实现（`__mix__(1, 1)`，PIPELINE_DEPTH=1）

Case 0是本样例中的基础场景，采用`__mix__(1, 1)`模式（1个AIC + 1个AIV），PIPELINE_DEPTH=1。Cube与Vector以Iterate粒度串行同步——Cube完成一次Iterate后通知Vector，Vector处理完后通知Cube，无流水重叠。在此基础上，Case 0已包含以下优化特性：

**① Cube+Vector融合计算**

本样例采用`__mix__`核函数模式，AIC（Cube核）和AIV（Vector核）并行运行在同一个物理核上。AIC执行矩阵乘法，AIV执行反量化+激活+类型转换，两者通过`CrossCoreSetFlag`/`CrossCoreWaitFlag`实现同步：

```cpp
// AIC执行路径: MMCompute → Iterate循环 → SetFlag(SYNC_AIC_TO_AIV)
// AIV执行路径: 跳过MMCompute → VectorCompute → WaitFlag(SYNC_AIC_TO_AIV)
if ASCEND_IS_AIC {
    matmulObj.Iterate();    // Cube计算
    CrossCoreSetFlag<2, PIPE_FIX>(SYNC_AIC_TO_AIV);  // 通知Vector
}
if ASCEND_IS_AIV {
    CrossCoreWaitFlag<2>(SYNC_AIC_TO_AIV);  // 等待Cube完成
    // 处理数据...
    CrossCoreSetFlag<2, PIPE_MTE2>(SYNC_AIV_TO_AIC);
}
```

Case 0中PIPELINE_DEPTH=1，workspace仅1个Slot（即 `CORE_NUM×BASE_M×BASE_N×sizeof(int32_t)` 大小的一段连续空间），Cube每次Iterate后必须等Vector处理完才能复用该Slot，Cube与Vector串行执行。

**② 跨Group连续轮询（preCount机制）**

Process函数采用**跨Group连续轮询**策略分配子块给多个Core。核心思想是将所有Group的子块在逻辑上连续编号，每个Core按`CORE_NUM`步长轮询取块，用`preCount`变量记录上一Group末尾未被`CORE_NUM`整除的余数，实现跨Group的连续分配，避免核间负载不均。

**分块维度说明**：

- **GroupM**：当前Group的M维度行数，从groupList动态读取。本样例每组GroupM=1024
- **numBlocksM = Ceil(GroupM, SINGLE_M)**：当前Group在M方向的子块数。Atlas A2下 Ceil(1024, 128) = 8，Ascend 950PR下 Ceil(1024, 256) = 4
- **numBlocksN = Ceil(N, SINGLE_N)**：N方向的子块数，所有Group相同。Ceil(8192, 1024) = 8

每个Group共 `numBlocksM × numBlocksN` 个子块，通过`(mIdx, nIdx)`二维坐标定位。每个子块大小为 `[SINGLE_M, SINGLE_N]`，Cube内部按`[BASE_M, BASE_N]`粒度计算（每个子块需 `SINGLE_N/BASE_N × SINGLE_M/BASE_M` 次Iterate）。

```cpp
for (groupIdx = 0, preCount = 0; groupIdx < GROUP_NUM; ++groupIdx) {
    groupM = groupListGlobal.GetValue(groupIdx);          // 本组行数, 本例=1024
    numBlocksM = Ceil(groupM, SINGLE_M);                   // M方向分块数
    numBlocksN = Ceil(N, SINGLE_N);                         // N方向分块数, 所有组相同=8
    curCount = preCount + numBlocksM * numBlocksN;          // 含上组余数的连续编号上界

    // 核心分配公式：确定当前Core在本Group中的第一个块编号
    curBlock = coreIdx >= preCount ? coreIdx : coreIdx + CORE_NUM;

    while (curBlock < curCount) {
        mIdx = (curBlock - preCount) / numBlocksN;          // 组内M方向块索引
        nIdx = (curBlock - preCount) % numBlocksN;          // 组内N方向块索引
        MMCompute(groupIdx, groupConfig);                    // Cube计算
        VectorCompute(groupIdx, groupConfig);                // Vector计算
        curBlock += CORE_NUM;                                // 轮询步进，步长为CORE_NUM
    }
    preCount = curCount % CORE_NUM;                          // 余数传递给下一组
    globalRowOffset += groupM;                               // 全局行偏移累积
}
```

**preCount跨组连续分配机制**：`curBlock`的初始值由`preCount`决定——若`coreIdx >= preCount`则直接从`coreIdx`取块，否则跳过本组前`preCount`个块从`coreIdx + CORE_NUM`开始。这确保了上组末尾多分配的Core在下组开头少分配。

**Atlas A2样例示例**（CORE_NUM=24，每组64块，64 % 24 = 16）：

```
Group 0: preCount=0,  curCount=64  → Core0~15处理3块, Core16~23处理2块 → preCount_out=16
Group 1: preCount=16, curCount=80  → Core0~7处理3块, Core8~15处理2块, Core16~23处理3块 → preCount_out=8
Group 2: preCount=8,  curCount=72  → Core0~7处理3块, Core8~23处理2块 → preCount_out=0
Group 3~7: preCount以3组为周期循环 0 → 16 → 8 → 0 → ...
```

8组共512块，Core0~7各处理22块，Core8~23各处理21块，最大负载差仅1块。

**③ Tiling常量化**

所有Tiling参数通过`MatmulApiStaticTiling`在编译期确定，运行时无需Scalar动态计算，减少Scalar开销：

```cpp
constexpr static auto CONSTANT_CFG = GetCustomConstantCFG();
matmul::Matmul<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE, CONSTANT_CFG> matmulObj;
```


**④ Matmul UnitFlag优化**

本样例通过`mmCFG.enUnitFlag = true`开启Matmul的UnitFlag优化，实现Cube计算与搬运的512B细粒度流水同步：

```cpp
static constexpr MatmulShapeParams shapeParams = {SINGLE_M, SINGLE_N, K, BASE_M, BASE_N, BASE_K};
MatmulConfig mmCFG = GetMMConfig<MatmulConfigMode::CONFIG_MDL>(shapeParams);
mmCFG.enUnitFlag = true;  // 开启UnitFlag
auto constantCFG = AscendC::GetMatmulApiTiling<A_TYPE, B_TYPE, C_TYPE, BIAS_TYPE>(mmCFG);
```

默认情况下，Cube的MMAD计算和FixPipe搬运是粗粒度同步的——必须等待整个`[BASE_M, BASE_N]`块的计算完成后，FixPipe才能开始搬出数据。开启UnitFlag后，MMAD和FixPipe实现细粒度（512B）流水并行：每当Cube完成一个512B数据结果的计算，FixPipe立即搬出该数据，Cube计算与结果写回流水重叠。

**⑤ UB Double Buffer**

本样例所有TQue队列均设置`BUFFER_NUM=2`开启double buffer，实现Vector侧数据搬运与计算的流水并行：

```cpp
constexpr uint32_t BUFFER_NUM = 2; // UB队列Buffer数, 设为2开启double buffer

pipe->InitBuffer(scaleInQueue, BUFFER_NUM, BASE_N * sizeof(float));          // 2 × 1024B
pipe->InitBuffer(perTokenScaleInQueue, BUFFER_NUM, BASE_M * sizeof(float));   // DAV_2201(BASE_M=128): 2 × 512B；DAV_3510(BASE_M=256): 2 × 1024B
pipe->InitBuffer(vecInQueue, BUFFER_NUM, UB_CAL_SIZE * sizeof(int32_t));      // 2 × 24,576B
pipe->InitBuffer(vecOutQueue, BUFFER_NUM, UB_CAL_SIZE * sizeof(half));        // 2 × 12,288B
```

double buffer的工作原理是：当Vector在处理Buffer A的数据时，MTE2/MTE3可以同时搬运Buffer B的数据，反之亦然。

```
时间 ──────────────────────────────────────────────────────>
MTE2:  |─ 搬入Buffer0 ──|─ 搬入Buffer1 ──|─ 搬入Buffer0 ──| ...
VEC:                    |─ 处理Buffer0 ──|─ 处理Buffer1 ──|─ 处理Buffer0 ──| ...
MTE3:                                    |─ 搬出Buffer0 ──|─ 搬出Buffer1 ─| ...
```

**性能数据**

以Ascend 950PR实测性能为例：

| Scenario | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|------|-----------|-------------------|-----------|---------------|-----------------|------------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|------------------|----------------|---------------------|-------------------|--------------|-----------------|---------------|---------------------|------------------|------------------|----------------|
| Scenario 0 | MIX_AIC | 282.717 | 32 | 32 | 281.84 | 167.952 | 0.596 | 34.137 | 0.121 | 50.792 | 0.18 | 148.151 | 0.526 | 59.619 | 0.212 | 281.84 | 229.166 | 0.813 | 77.027 | 0.273 | 129.698 | 0.46 | 87.937 | 0.312 |

**优化效果分析**：
- 端到端耗时：**282.717μs**
- Cube计算耗时：167.952μs，占比 **59.6%**
- Vec计算耗时：229.166μs，占比 **81.3%**

Case 0作为基准场景，Vector计算耗时（229.166μs）显著超过Cube计算（167.952μs），Vector耗时较长，需优化。

**下一步优化方向**：提升Vector侧计算并行度，从`__mix__(1, 1)`升级为`__mix__(1, 2)`双AIV模式。

#### Case 1：双AIV并行（`__mix__(1, 2)`，PIPELINE_DEPTH=1）

Case 1在Case 0基础上将核函数模式从`__mix__(1, 1)`升级为`__mix__(1, 2)`，每个物理核包含1个AIC和2个AIV，通过多AIV分摊Vector侧计算任务，提升Vector吞吐。PIPELINE_DEPTH仍为1，Cube与Vector仍串行同步。

**`__mix__`双AIV模式**

`__mix__(1, 2)`模式下，每个物理核包含1个AIC和2个AIV。两个AIV通过`taskRation`轮询机制分摊Vector侧的offsetM迭代任务：

```cpp
uint32_t taskRation = AscendC::GetTaskRation();  // __mix__(1,2)下为2
if (taskRation != 0 && vecCount % taskRation != subBlockIdx) {
    continue;  // 跳过不属于当前AIV的任务
}
```

两个AIV共享同一个coreIdx，通过`subBlockIdx`（由`GetSubBlockIdx()`获取）区分身份。在offsetM内层循环中，`vecCount`递增，每个AIV只处理`vecCount % taskRation == subBlockIdx`的迭代，即偶数迭代由AIV0处理，奇数迭代由AIV1处理，实现负载均分。

相比Case 0的`__mix__(1, 1)`单AIV模式，双AIV可将Vector计算耗时减少一半，有效缓解Vector侧的计算瓶颈。

**性能数据**：
| Scenario | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|------|-----------|-------------------|-----------|---------------|-----------------|------------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|------------------|----------------|---------------------|-------------------|--------------|-----------------|---------------|---------------------|------------------|------------------|----------------|
| Scenario 1 | MIX_AIC | 224.125 | 32 | 64 | 223.27 | 168.201 | 0.753 | 33.531 | 0.15 | 50.367 | 0.226 | 153.263 | 0.686 | 104.334 | 0.467 | 223.29 | 112.441 | 0.504 | 39.701 | 0.178 | 77.206 | 0.346 | 43.409 | 0.194 |


**优化效果分析**：
- 端到端耗时：**224.125μs**，耗时减少20.72%
- Cube计算耗时：168.201μs，本次优化为优化Vector计算并行，Cube侧计算耗时没有明显提升
- Vec计算耗时：112.441μs，耗时减少50.93%，符合预期收益

从耗时关系看，端到端耗时小于Cube计算耗时与Vector计算耗时之和（168.201μs + 112.441μs），说明Cube和Vector之间已经存在部分并行；但端到端耗时仍明显高于两者的较大值168.201μs，说明并行重叠不充分。

**PIPE流水图展示**：

通过PIPE流水图可以看到，Case 1 虽然已经引入双 AIV 分摊 Vector 任务，但由于 `PIPELINE_DEPTH=1`，Cube 与 Vector 仍然按照 Iterate 粒度串行同步，workspace 只有单个 Slot 可复用，因此流水中仍能看到明显的等待和断点。

![Case 1 的 PIPE 流水图](./figures/CV流水优化1.png)

**下一步优化方向**：引入4级Cube-Vector流水，提高Cube-Vector并行度，进一步提升整体计算效率。

#### Case 2：4级Cube-Vector流水（`__mix__(1, 2)`，PIPELINE_DEPTH=4）

Case 2在Case 1基础上将PIPELINE_DEPTH从1提升为4，引入workspace环形Slot复用机制，实现Cube-Vector细粒度流水重叠。

**Workspace环形Slot复用**

workspace是Cube和Vector之间的中转站，采用环形分槽设计。这里的 Slot 指由 `workSpaceOffset = BASE_N × BASE_M × (coreIdx + (cubeTaskIdx % PIPELINE_DEPTH) × CORE_NUM)` 定位的一段连续空间，单个 core 的 Slot 大小为 `BASE_M×BASE_N×sizeof(int32_t)`，每层 pipeline 的 Slot 总空间为 `CORE_NUM×BASE_M×BASE_N×sizeof(int32_t)`，`PIPELINE_DEPTH` 个 Slot 轮流使用，Cube 写入当前 Slot 的同时，Vector 可以处理之前 Slot 的数据：

```
workspace布局:
Slot 0: Core0[BASE_M×BASE_N] + Core1[BASE_M×BASE_N] + ... + Core(CORE_NUM-1)[BASE_M×BASE_N]
Slot 1: Core0[BASE_M×BASE_N] + Core1[BASE_M×BASE_N] + ... + Core(CORE_NUM-1)[BASE_M×BASE_N]
Slot 2: ...
Slot 3: ...

workSpaceOffset = BASE_N × BASE_M × (coreIdx + (cubeTaskIdx % PIPELINE_DEPTH) × CORE_NUM)
```

- 当`cubeTaskIdx < PIPELINE_DEPTH`时，Cube无需等待，直接写入新Slot
- 当`cubeTaskIdx >= PIPELINE_DEPTH`时，Cube需等待Vector释放对应Slot（`CrossCoreWaitFlag<2>(SYNC_AIV_TO_AIC)`）
- 同步粒度为Iterate（=BASE_M×BASE_N），而非子块（=SINGLE_M×SINGLE_N）

**4级流水时序**：

```
时间 ──────────────────────────────────────────────────────────────>
Cube:  |─Iter0→Slot0─|─Iter1→Slot1─|─Iter2→Slot2─|─Iter3→Slot3─|─Iter4→Slot0─| ...
                      (Wait)                                     (Wait)
Vec:                 |─Slot0处理─|─Slot1处理─|─Slot2处理─|─Slot3处理─| ...
```

前4次Iterate（Slot 0~3）Cube无需等待，连续写入；第5次Iterate复用Slot 0时，需等Vector释放。由此形成Cube-Vector流水重叠，相比Case 1的串行模式，显著减少Cube空闲等待时间。

**性能数据**：
| Scenario | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|------|-----------|-------------------|-----------|---------------|-----------------|------------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|------------------|----------------|---------------------|-------------------|--------------|-----------------|---------------|---------------------|------------------|------------------|----------------|
| Scenario 2 | MIX_AIC | 204.129 | 32 | 64 | 203.26 | 180.347 | 0.887 | 36.598 | 0.18 | 51.726 | 0.254 | 172.252 | 0.847 | 185.875 | 0.914 | 203.29 | 112.218 | 0.552 | 39.816 | 0.196 | 79.51 | 0.391 | 50.123 | 0.247 |

**优化效果分析**：
- 端到端耗时：**204.129μs**，相对Case 1减少8.92%，相对Case 0减少**27.80%**。
- Cube计算耗时：180.347μs，较Case 1略有增加，但`aic_mac_ratio`从0.753提升至0.887，说明Cube计算连续性更好。
- Vec计算耗时：112.218μs，与Case 1的112.441μs基本持平，说明本轮优化没有改变Vector侧单次计算开销，主要收益来自Cube和Vector之间的流水重叠。

**PIPE流水图展示**：
Case 2 在 Case 1 的基础上将 `PIPELINE_DEPTH` 提升到 4，Cube 可以在多个 Iterate 上连续写入不同 Slot，Cube计算连续性更好。

![Case 2 的 PIPE 流水图](./figures/CV流水优化2.png)

## 性能数据分析

### Atlas A2训练系列芯片性能数据

| Scenario | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|------|-----------|-------------------|-----------|---------------|-----------------|------------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|---------------------|-------------------|--------------|-----------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|
| Scenario 0 | MIX_AIC | 583.512 | 24 | 24 | 564.06 | 192.02 | 0.34 | 115.317 | 0.204 | 168.645 | 0.299 | 141.648 | 0.251 | 79.47 | 0.141 | 565.12 | 471.71 | 0.835 | 228.275 | 0.404 | 139.25 | 0.246 | 94.682 | 0.168 |
| Scenario 1 | MIX_AIC | 353.887 | 24 | 48 | 344.01 | 192.612 | 0.56 | 143.518 | 0.417 | 170.031 | 0.494 | 160.396 | 0.466 | 107.662 | 0.313 | 345.22 | 235.871 | 0.683 | 122.424 | 0.355 | 107.715 | 0.312 | 56.494 | 0.164 |
| Scenario 2 | MIX_AIC | 299.286 | 24 | 48 | 288.65 | 193.576 | 0.671 | 196.668 | 0.681 | 173.003 | 0.599 | 170.193 | 0.59 | 167.936 | 0.582 | 285.21 | 235.872 | 0.827 | 126.227 | 0.443 | 99.321 | 0.348 | 56.645 | 0.199 |


### Ascend 950PR芯片性能数据
| Scenario | Task Type | Task Duration(μs) | Block Num | Mix Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|------|-----------|-------------------|-----------|---------------|-----------------|------------------|---------------|---------------------|------------------|------------------|----------------|------------------|----------------|------------------|----------------|---------------------|-------------------|--------------|-----------------|---------------|---------------------|------------------|------------------|----------------|
| Scenario 0 | MIX_AIC | 282.717 | 32 | 32 | 281.84 | 167.952 | 0.596 | 34.137 | 0.121 | 50.792 | 0.18 | 148.151 | 0.526 | 59.619 | 0.212 | 281.84 | 229.166 | 0.813 | 77.027 | 0.273 | 129.698 | 0.46 | 87.937 | 0.312 |
| Scenario 1 | MIX_AIC | 224.125 | 32 | 64 | 223.27 | 168.201 | 0.753 | 33.531 | 0.15 | 50.367 | 0.226 | 153.263 | 0.686 | 104.334 | 0.467 | 223.29 | 112.441 | 0.504 | 39.701 | 0.178 | 77.206 | 0.346 | 43.409 | 0.194 |
| Scenario 2 | MIX_AIC | 204.129 | 32 | 64 | 203.26 | 180.347 | 0.887 | 36.598 | 0.18 | 51.726 | 0.254 | 172.252 | 0.847 | 185.875 | 0.914 | 203.29 | 112.218 | 0.552 | 39.816 | 0.196 | 79.51 | 0.391 | 50.123 | 0.247 |


### 性能分析总结

本样例固定shape为`M=8192, N=8192, K=1024`，分组只改变M轴调度方式，总Cube计算量仍为：

$$
Ops_{\text{cube}} = \sum_{i=0}^{G-1} group_i \times N \times K = M \times N \times K
$$

对于int8矩阵乘，Cube单cycle按`16 × 32 × 16`次乘加估算，理论Cube计算耗时为：

$$
T_{\text{cube}} =
\frac{M \times N \times K}{16 \times 32 \times 16 \times f \times CORE\_NUM}
$$

最终优化场景为Scenario 2（`__mix__(1, 2)` + 4级Cube-Vector流水）。该场景下Cube侧利用率和瓶颈判断如下：

| 芯片 | CORE_NUM | 理论Cube耗时(μs) | aic_mac_time(μs) | aic_mac_ratio | aiv_vec_time(μs) | 达到理论峰值比例 | 瓶颈判断 |
|------|----------|------------------|------------------|---------------|------------------|----------------|----------|
| Atlas A2/A3（DAV_2201） | 24 | 188.93 | 193.576 | 0.671 | 235.872 | 97.60% | 仍偏Vector侧 |
| Ascend 950PR（DAV_3510） | 32 | 158.88 | 180.347 | 0.887 | 112.218 | 88.10% | 不再是Vector侧 |


从最终结果看，Ascend 950PR上Vector侧计算耗时已经通过双AIV并行降低到112.218μs，明显低于Cube计算耗时180.347μs，且`aic_mac_ratio`达到0.887，说明性能瓶颈已经不再是Vector计算本身。此时端到端耗时主要受Cube计算、workspace复用同步、FIXPIPE搬运和流水收尾开销共同影响。

Atlas A2/A3上表现不同：Scenario 2中`aiv_vec_time`为235.872μs，高于`aic_mac_time`的193.576μs，`aiv_vec_ratio`也达到0.827，说明Vector侧的反量化、per-token缩放、FasterGelu和Cast链路仍然占据较大比例，整体仍偏Vector侧瓶颈。与此同时，A2实测Cube计算耗时193.576μs已经接近理论Cube耗时188.93μs，达到理论峰值约97.60%，说明Cube矩阵乘本身已经较充分利用，继续优化端到端性能需要优先关注Vector侧计算链路和Cube-Vector同步开销。

虽然Atlas A2/A3最终仍偏Vector bound，但三级优化仍然有效：Scenario 1通过双AIV并行将Vector计算耗时从471.710μs降低到235.871μs，端到端耗时从583.512μs降低到353.887μs；Scenario 2在Vector计算耗时基本不变的情况下，通过4级workspace流水提升Cube和Vector的重叠度，将端到端耗时进一步降低到299.286μs，相比Scenario 0总耗时减少48.71%。


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
  SCENARIO_NUM=1                                                       # 选择执行场景
  mkdir -p build && cd build;                                          # 创建并进入 build 目录
  cmake -DCMAKE_ASC_RUN_MODE=npu -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;  # 编译工程
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用NPU仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `SCENARIO_NUM` | `0`、`1`、`2` | 场景编号：0-基准单AIV，1-双AIV无流水，2-双AIV+4级流水 |
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
