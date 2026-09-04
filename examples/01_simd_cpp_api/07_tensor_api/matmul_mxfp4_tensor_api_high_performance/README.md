# MxFP4 Matmul Tensor API 高性能样例

## 概述

本样例介绍如何基于 Ascend C Tensor API 和静态 Tensor 编程方式，通过 L1/L0 双缓冲机制、大包搬运、细粒度流水同步等多种优化手段，实现一个高性能的 MxFP4 Matmul kernel。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |

> **说明：** 该样例依赖尚未正式发布的CANN特性，请使用最新的CANN master包。

## 目录结构

```text
├── matmul_mxfp4_tensor_api_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── figures                 // 图示
│   ├── mmad_mx.asc             // Ascend C Tensor API 样例实现
│   └── README.md               // 样例说明文档
```

## MxMatmul 介绍

普通 Matmul 只有两路矩阵输入：

$$
C = A * B
$$

MxMatmul 在此基础上额外引入两路 ScaleA/ScaleB 输入。A、B 为 MxFP4 低比特数据，ScaleA、ScaleB 为缩放因子。计算时，矩阵数据与对应 ScaleA/ScaleB 共同参与运算：

$$
C = (\text{ScaleA} \otimes A) \times (\text{ScaleB} \otimes B)
$$

在MxMatmul中，K 方向每 32 个元素共享一个 scale。也就是说，scale 的数量远少于矩阵元素数量，但它会沿 K 方向广播到对应的一组 MxFP4 数据上，如图所示：

  <img src="figures/MxMatmul.png"  width="80%">

## 样例规格

本样例实现固定 shape 为 `8192 x 8192 x 8192` 的 MxFP4 Matmul，输出数据类型为 `bfloat16_t`。

### 输入输出

| 输入/输出 | 逻辑形状 | 数据类型 | 数据排布类型 | 说明 |
|------|------|----------|------|------|
| A | `[M, K]` | `fp4x2_e1m2_t` | ND | 左矩阵，每个字节打包 2 个 fp4 元素 |
| ScaleA | `[M, scaleK]` | `fp8_e8m0_t` | ND | A 矩阵的缩放因子，A矩阵在K 方向每 32 个元素共享一个 scale |
| B | `[N, K]` | `fp4x2_e1m2_t` | DN | 右矩阵，按 `[N, K]` 形式输入 kernel |
| ScaleB | `[N, scaleK]` | `fp8_e8m0_t` | DN | B 矩阵的缩放因子，B矩阵在K 方向每 32 个元素共享一个 scale |
| C | `[M, N]` | `bfloat16_t` | ND | 输出矩阵 |

其中：

- 两个`fp4x2_e1m2_t`打包存储在一个字节中，因此当数据类型为`fp4`时，K 需要为偶数
- `scaleK = align_even(ceil(K / 32))`，表示先向上整除再对齐到 2 的倍数。由于硬件约束要求 scale 数据在 K 方向满足 2Byte 连续对齐，因此 scaleK 必须为偶数
- 由于硬件约束，当 ScaleB 矩阵为 `[scaleK, N]` 输入时，需要 K 方向 2 Byte连续，因此推荐 ScaleB 采用`[N, scaleK]`输入

#### 四路输入说明

与普通 Matmul 相比，MxMatmul 在 kernel 中多了 scale 的搬运、加载和参与计算路径。A/B 与 ScaleA/ScaleB 必须按相同的 K block 节奏进入计算流水，否则 Cube 侧拿不到匹配的缩放信息。

- 四路输入的数据排布格式如下图所示：

  <img src="figures/formatOfMx.png">

- 四路输入的搬运与计算如下图所示：

  <img src="figures/InputOfMxMatmul.png">

### 关键参数

| 参数 | 值 | 说明 |
|------|----|------|
| `M` | `8192` | 矩阵 M 维度规模 |
| `N` | `8192` | 矩阵 N 维度规模 |
| `K` | `8192` | 矩阵 K 维度规模 |
| `baseM` | `256` | Cube 计算基本块 M 维度大小 |
| `baseK` | `256` | Cube 计算基本块 K 维度大小 |
| `baseN` | `256` | Cube 计算基本块 N 维度大小 |
| `singleCoreM` | `2048` | 单核 M 方向计算范围 |
| `singleCoreN` | `1024` | 单核 N 方向计算范围 |
| `singleCoreK` | `8192` | 单核 K 方向计算范围 |
| `stepK` | `2` | GM->L1搬运中，A/B 在 K 方向的大包搬运步长 |
| `scaleFactorK` | `4` | GM->L1搬运中，ScaleA/ScaleB 相对 A/B 的 K 方向搬运比例 |
| `Block Num` | `32` | 使用核数 |

> **约束说明**：本样例暂时不支持 K 方向存在尾块的场景，即要求 `K` 能被 `baseK` 整除。

## 样例实现

整个 kernel 的过程为：多核切分后，每个核负责一个 singleCoreM * singleCoreN 子矩阵，在 M/N 方向上循环，将 A/B 与 ScaleA/ScaleB 搬入 L1，再在K方向循环，加载到 L0A/L0B 和 L0ScaleA/L0ScaleB，完成 Cube 累加计算，最后把结果写回 GM。

### 数据流路径

整体数据流如下：

```text
物理地址                               流水
  GM
  |  A:copy_gm_to_l1 / ScaleA:copy_gm_to_l1   MTE2
  |  B:copy_gm_to_l1 / ScaleB:copy_gm_to_l1   MTE2
  v
  L1
  |  A:copy_l1_to_l0a / ScaleA:copy_l1_to_l0scalea        MTE1
  |  B:copy_l1_to_l0b / ScaleB:copy_l1_to_l0scaleb        MTE1
  v
  L0A / L0B / L0ScaleA / L0ScaleB
  |
  |  mmad                              M
  v
  L0C
  |
  |  copy_l0c_to_gm: float -> bfloat16       FIX
  v
  GM
```

#### 1. 多核切分：把大矩阵拆成 32 个子任务

样例使用 32 核并行。`M=8192` 按 `singleCoreM=2048` 切成 4 份，`N=8192` 按 `singleCoreN=1024` 切成 8 份，正好形成 `4 x 8 = 32` 个子矩阵。

```text
M direction: 8192 / 2048 = 4 blocks
N direction: 8192 / 1024 = 8 blocks

Total blocks = 4 * 8 = 32
```

每个核只处理自己负责的 `2048 x 1024` 输出区域。核内按 `baseK=256` 分块在 K 方向循环完成累加。

对应的核索引计算如下：

```cpp
constexpr uint32_t mIter = AscendC::Std::ceil_div(M, singleCoreM);
uint32_t mIterIdx = block_idx % mIter;
uint32_t nIterIdx = block_idx / mIter;
```

#### 2. GM 到 L1：矩阵和 scale 一起进流水

MxMatmul 的输入不是两路，而是四路：A、B、ScaleA、ScaleB。因此每个核 GM 到 L1 的搬运也分成四类：

| 数据 | 搬运函数 | GM 数据shape | L1 数据shape |
|------|----------|-----------|---------|
| A | `copy` | `[singleCoreM, singleCoreK]` | `[baseM, stepK * baseK]` |
| B | `copy` | `[singleCoreN, singleCoreK]` | `[baseN, stepK * baseK]` |
| ScaleA | `copy` | `[singleCoreM, singleCoreSK]` | `[baseM, scaleFactorK * stepK * baseSK]` |
| ScaleB | `copy` | `[singleCoreN, singleCoreSK]` | `[baseN, scaleFactorK * stepK * baseSK]` |

通过 `stepK=2` 一次搬入 `stepK * baseM * baseK` 的 A 矩阵数据 和 `stepK * baseN * baseK` 的 B 矩阵数据，减少搬运指令数量，提高 GM 到 L1 的搬运效率。

scale 的搬运粒度由 `scaleFactorK=4` 控制，一次搬入 `scaleFactorK * stepK * baseM * baseSK` 的 ScaleA 矩阵数据 和 `scaleFactorK * stepK * baseN * baseSK` 的 ScaleB 矩阵数据，其中`baseSK = baseK / 32`。它的作用是让 ScaleA/ScaleB 在 K 方向相比于 A/B 一次覆盖更大的范围，减少 scale 反复搬运带来的 MTE2 压力。

以 A 矩阵数据搬运为例，`copy` 一次搬入 `stepK` 个 K 方向 base 块，每个base块为 `baseM * baseK`：

```cpp
asc::te::copy(gm2L1Atom, l1TensorAPing, a.slice(asc::te::make_coord(mBlockIdx * baseM, dataNextKChunkIdx * baseK), asc::te::make_shape(curM, stepCurK)));
```

以 ScaleA 矩阵数据搬运为例。`copy` 一次搬入 `scaleFactorK * stepK` 个 K 方向 base 块，每个base块为 `baseM * baseSK`（其中 `baseSK = align_even(ceil(baseK / 32))`）：

```cpp
asc::te::copy(gm2L1Atom, l1TensorAsPing, as.slice(asc::te::make_coord(mBlockIdx * baseM, scaleNextKChunkIdx * baseScaleK), asc::te::make_shape(curM, stepCurScaleK)));
```

#### 3. L1 到 L0 进入 MX 计算路径

普通 Matmul 只需要加载 A/B；MxMatmul 还需要加载对应的 ScaleA/ScaleB，每次搬运一个 base 块。

当 A/B 和 ScaleA/ScaleB 进入 L0 后，Cube 计算单元就可以按照 MX 语义完成带缩放的矩阵乘累加。

以 A 矩阵数据搬运为例，`copy` 一次搬入 1 个`baseM * baseK` 大小的 base 块：

```cpp
asc::te::copy(l12l0AAtom, l0TensorA, l1ReadBufA.slice(asc::te::make_coord(0, kOffsetInDataChunk * baseK), asc::te::make_shape(curM, curK)));
```

以 ScaleA 矩阵数据搬运为例，`copy` 一次搬入 1 个`baseM * baseSK` 大小的 base 块：

```cpp
asc::te::copy(l12l0ScaleAAtom, l0TensorAs, l1ReadBufAs.slice(asc::te::make_coord(0, kOffsetInScaleChunk * baseScaleK), asc::te::make_shape(curM, curScaleK)));
```

#### 4. L0 双缓冲：让加载和计算交叠

样例在 L1 和 L0 都使用 Ping-Pong 双缓冲。这样做的核心目标是让搬运、加载、计算尽量交叠，减少等待。

本样例的主要缓冲区布局如下：

| 层级 | 缓冲区 | 内容 | 作用 |
|------|--------|------|------|
| L1 | `A Ping/Pong` | A 的大包数据 | 存放 GM->L1 的 A 数据 |
| L1 | `B Ping/Pong` | B 的大包数据 | 存放 GM->L1 的 B 数据 |
| L1 | `ScaleA Ping/Pong` | A 对应的 scale 数据 | 存放 A 侧 MXScale  |
| L1 | `ScaleB Ping/Pong` | B 对应的 scale 数据 | 存放 B 侧 MXScale  |
| L0A | `A Ping/Pong` | 当前 K block 的 A 数据 | Cube `mmad` 左操作数 |
| L0B | `B Ping/Pong` | 当前 K block 的 B 数据 | Cube `mmad` 右操作数 |
| L0C | `C` | float 累加结果 | `mmad` 输出，供写回GM |

一个简化的流水节奏如下：

```text
time     |---------------------------------------------------------------------------->

GM->L1   | A/B/ScaleA/ScaleB Ping | A/B/ScaleA/ScaleB Pong | A/B/ScaleA/ScaleB Ping |
L1->L0                            | L0 Ping load --|       | L0 Pong load --|
Cube                                               | mmad Ping ---|   | mmad Pong ---|
L0C->GM                                                           | copy L0C to GM ---
```

图中横线长度只用于表达不同阶段耗时可能不同，并不代表实测比例；真实耗时需要以 `msopprof` 采集结果为准。

在 K 循环中，当前 K block 进入 Cube 计算时，下一批 A/B/ScaleA/ScaleB 可以提前发起搬运。Ping 和 Pong 交替使用，生产者和消费者通过事件同步确认缓冲区是否可写、数据是否可读。

#### 5. Cube 计算：按 K block 累加

每个输出子矩阵会在最内层先沿 K 方向循环。每一轮处理一个 `baseK=256` 的 K block，再沿M/N方向循环：

```text
for nBlock in N blocks:
  for mBlock in M blocks:
    for kBlock in K blocks:
        copy(A block, ScaleA block)
        copy(B block, ScaleB block)
        mmad accumulate
```

`mmad` 的输入来自 L0A/L0B，输出累加到 L0C。第一轮 K block 初始化累加结果，后续 K block 持续累加，直到完整 K 方向计算完成。

计算阶段的关键参数包括 `m/n/k` 尺寸、是否初始化 C 矩阵：

```cpp
asc::te::mmad_params mmadParams{curM, curN, curK, asc::te::unit_flag_mode::disable, true};
mmadParams.init_with_zero = (kBlockIdx == 0);
asc::te::mmad(mmadAtom.with(mmadParams), l0TensorC, l0TensorA, l0TensorB);
```

其中 `init_with_zero` 用于控制第一轮 K block 初始化累加结果，后续 K block 在已有 L0C 数据上继续累加。

#### 6. L0C 写回 GM：从 float 转换到 bfloat16 输出

Cube 侧累加结果保存在 L0C 中，数据类型为 float。计算结束后，`copy` 负责把结果转换为 `bfloat16_t` 并写回 GM 中。

```text
L0C float result
      |
      | copy, F32 -> BF16
      v
GM C [M, N]
```

```cpp
asc::te::l0c_to_gm_params fixpipeParams;
asc::te::copy(l0c2GmAtom.with(fixpipeParams), c.slice(asc::te::make_coord(mBlockIdx * baseM, nBlockIdx * baseN), asc::te::make_shape(curM, curN)), l0TensorC);
```

### 流水同步

使用静态Tensor编程时，同步是性能和正确性的关键。本样例通过`asc_lock`和`asc_unlock`管理不同流水之间的生产者和消费者依赖，并为各级Ping/Pong Buffer分配独立slot：

| 数据 | 写流水 | 读流水 | slot |
|------|--------|--------|------|
| A/B Data Ping/Pong | `PIPE_MTE2` | `PIPE_MTE1` | `0/1` |
| ScaleA/ScaleB Ping/Pong | `PIPE_MTE2` | `PIPE_MTE1` | `2/3` |
| L0A/L0B Ping/Pong | `PIPE_MTE1` | `PIPE_M` | `4/5` |
| L0C Buffer | `PIPE_M` | `PIPE_FIX` | `6` |

A/B Data的生命周期一致，因此按Ping/Pong相位绑定到同一组slot；ScaleA/ScaleB的生命周期一致，但scale chunk通常比data chunk更大，因此使用另一组Ping/Pong slot。

大包粒度的同步只在大包首元素锁定待读Buffer，在大包末元素释放Buffer：

```cpp
if (kOffsetInDataChunk == 0) {
    asc_lock(PIPE_MTE1, GetDataSlot(dataReadIdx));
}
if (((kOffsetInDataChunk + 1) == dataChunkStep) || (kBlockIdx + 1 == kLoopCount)) {
    asc_unlock(PIPE_MTE1, GetDataSlot(dataReadIdx));
}
if (kOffsetInScaleChunk == 0) {
    asc_lock(PIPE_MTE1, GetScaleSlot(scaleReadIdx));
}
if (((kOffsetInScaleChunk + 1) == scaleChunkStep) || (kBlockIdx + 1 == kLoopCount)) {
    asc_unlock(PIPE_MTE1, GetScaleSlot(scaleReadIdx));
}
```

从流水角度看，这些事件串起了三个阶段：

```text
copy(GM->L1)
        |
        | MTE2_MTE1
        v
copy(L1->L0)
        |
        | MTE1_M
        v
mmad(Cube)
        |
        | M_MTE1
        v
next LoadData
```

反向事件也很重要。比如 `MTE1_MTE2` 表示 L1 中某个 Ping/Pong 缓冲已经被消费完，可以被下一次 GM->L1 搬运覆盖。没有这些反向同步，流水很容易出现覆盖未消费数据或等待不完整数据的问题。

## 性能优化总结

本样例的性能优化围绕三件事展开：

- **多核切分**：把 `8192 x 8192` 输出矩阵拆到 32 核并行计算。
- **大包搬运**：A/B 按多个 K block 搬运，ScaleA/ScaleB 按更大的 K 覆盖范围搬运，降低 MTE2 指令和数据搬运压力。
- **双缓冲流水**：L1 和 L0 都采用 Ping-Pong，让 MTE2、MTE1、CUBE、FIXPIPE 流水尽量交叠执行。

## 理论性能对比

性能指标说明表：

| 指标 | 说明 |
|------|------|
| `Task Duration(μs)` | 整个任务执行的总时间，算子端到端执行时间以该参数为准 |
| `Block Num` | 使用的核数，也就是 kernel 启动的 block 数量 |
| `aicore_time(μs)` | AI Core 的平均执行时间 |
| `aic_mac_time(μs)` | Cube 计算单元执行时间，主要对应 `mmad` 阶段 |
| `aic_mac_ratio` | Cube 计算单元时间占比，反映计算单元利用率 |
| `aic_scalar_time(μs)` | Scalar 指令执行时间，反映循环调度、地址计算、参数配置等开销 |
| `aic_scalar_ratio` | Scalar 时间占比 |
| `aic_mte1_time(μs)` | MTE1 执行时间，主要对应 L1 到 L0A/L0B 的 `copy` |
| `aic_mte1_ratio` | MTE1 时间占比，反映 L1 到 L0 的数据搬运压力 |
| `aic_mte2_time(μs)` | MTE2 执行时间，主要对应 GM 到 L1 的 `copy` |
| `aic_mte2_ratio` | MTE2 时间占比，反映 GM 到 L1 的数据加载压力 |
| `aic_fixpipe_time(μs)` | L0C->GM 的 `copy` 执行时间，主要对应 L0C 到 GM 的结果写回 |
| `aic_fixpipe_ratio` | L0C->GM 的 `copy` 时间占比，反映结果写回的访存压力 |

Ascend 950PR 芯片性能数据如下：

| Case version | Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------|------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| Tensor API MxMatmul | 682.316 | 32 | 681.54 | 640.591 | 0.94 | 63.466 | 0.093 | 315.894 | 0.464 | 589.308 | 0.865 | 31.646 | 0.046 |

可以看到，本样例的 Cube 计算时间占比（即表中的`aic_mac_ratio`）达到 `94.0%`。

### Cube计算性能分析

本样例的性能数据在 Ascend 950PR 上运行得到，该处理器的主频为 1.65GHz，对于 MX-FP4 的数据类型，每 cycle 处理 16×64×16 次乘加运算。

Cube 理论运算时间 $T_{cube}$ 为：

$$T_{cube} = \frac{M \times N \times K}{16 \times 64 \times 16 \times 1.65 \times 10^9 \times \text{核数}} = \frac{8192 \times 8192 \times 8192}{16384 \times 1.65 \times 10^9 \times 32} = 635.5 μs$$

从表中可以看出 `aic_mac_time` 为 `640.591 μs`，相对理论值 `635.5 μs`，误差 $E_{cube}$ 为：

$$E_{cube} = \frac{T_{actual} - T_{cube}}{T_{cube}} = \frac{640.591 - 635.5}{635.5} = 0.80 \%$$

### MTE2 带宽分析

**数据复用原理**：

矩阵乘法中，输出矩阵 C 的每个元素 $C_{i,j}$ 需要 A 的第 i 行和 B 的第 j 列参与计算。分块计算时，同一输入数据块会被多个输出块复用：

- A 矩阵按 M 方向切成 `M/baseM` 个行块，每个 A 行块参与 N 方向 `N/baseN` 个输出块的计算
- B 矩阵按 N 方向切成 `N/baseN` 个列块，每个 B 列块参与 M 方向 `M/baseM` 个输出块的计算

MxFP4 Matmul 还包含两路 ScaleA/ScaleB 输入，K 方向每 32 个元素共享一个 scale，因此 `scaleK = K/32`：

- ScaleA 形状 `[M, scaleK]`，按 M 方向切成 `M/baseM` 个行块，每个 ScaleA 行块参与 N 方向 `N/baseN` 个输出块的计算
- ScaleB 形状 `[N, scaleK]`，按 N 方向切成 `N/baseN` 个列块，每个 ScaleB 列块参与 M 方向 `M/baseM` 个输出块的计算

由于 L1/L2Cache 容量有限，无法缓存所有输入数据，同一数据块会被多次从 HBM 搬运到 L2Cache/L1，导致数据重复搬运。

**读入数据总量**：

MxFP4 Matmul 的输入包含四路数据：A、B、ScaleA、ScaleB。A/B 矩阵使用 `fp4x2_e1m2_t`，每 2 个 fp4 元素打包成 1 byte，单元素 `sizeof = 0.5B`；ScaleA/ScaleB 使用 `fp8_e8m0_t`，单元素 `sizeof = 1B`。

本样例参数 `M=N=K=8192`，`scaleK=256`，分块参数 `baseM=baseN=256`。

读入数据总量 $D_{total}$ 为：

$$D_{total} = \frac{N}{baseN} \times M \times K \times 0.5B + \frac{M}{baseM} \times K \times N \times 0.5B + \frac{N}{baseN} \times M \times scaleK \times 1B + \frac{M}{baseM} \times scaleK \times N \times 1B$$

$$= (32 \times 8192 \times 8192 \times 0.5 + 32 \times 8192 \times 8192 \times 0.5 + 32 \times 8192 \times 256 \times 1 + 32 \times 256 \times 8192 \times 1) B$$

$$= (1GB + 1GB + 64MB + 64MB) = 2.125GB$$

**MTE2 理论耗时**：

Ascend 950PR 芯片：L2Cache 峰值带宽约 5TB/s，HBM（对应 GM）带宽约 1.6TB/s。最理想情况下首次访问从 HBM 获取数据并缓存到 L2Cache，后续访问直接从 L2Cache 读取。

> **单位说明**：带宽单位采用十进制，1 TB/s = 10^12 B/s。

首次从 HBM 读入的数据总量 $D_{HBM}$ 为：

$$D_{HBM} = M \times K \times 0.5B + K \times N \times 0.5B + M \times scaleK \times 1B + N \times scaleK \times 1B = 32MB + 32MB + 2MB + 2MB = 68MB$$

从 L2Cache 读入的数据总量 $D_{L2Cache}$ 为：

$$D_{L2Cache} = D_{total} - D_{HBM} = 2.125GB - 68MB \approx 2.057GB$$

MTE2 理论耗时 $T_{MTE2}$ 为：

$$T_{MTE2} = \frac{D_{HBM}}{1.6TB/s} + \frac{D_{L2Cache}}{5TB/s} = \frac{68MB}{1.6TB/s} + \frac{2.057GB}{5TB/s} \approx 42.5μs + 411.4μs = 453.9μs$$

MTE2 耗时误差 $E_{MTE2}$ 为：

$$E_{MTE2} = \frac{T_{actual} - T_{MTE2}}{T_{MTE2}} = \frac{589.308μs - 453.9μs}{453.9μs} = 29.8\%$$

Ascend 950PR中 L2Cache 大小为 128MB，无法缓存所有输入数据，部分数据在搬运时会发生 L2Cache miss，需要从 HBM 获取。用户可进一步优化 L2Cache 切分策略以提高 MTE2 带宽。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

> **注意事项**：本样例使用的 `ml_dtypes` 库需要版本 `0.2.0`，`en_dtypes` 库需要版本 `0.0.4`。安装命令为

```bash
python3 -m pip install ml_dtypes==0.2.0 en_dtypes==0.0.4
```

### 配置环境变量

请根据当前环境上 CANN 开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量，**当前仅支持使用[CANN master](../../../../docs/zh/quick_start.md#cann-install)**。

```bash
source ${install_path}/cann/set_env.sh
```

> **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

### 样例执行

在本样例目录下执行如下命令。

```bash
mkdir -p build && cd build;  # 创建并进入 build 目录
cmake .. -DCMAKE_ASC_RUN_MODE=npu -DCMAKE_ASC_ARCHITECTURES=dav-3510; make -j;
python3 ../scripts/gen_data.py
./demo
python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin
```

使用 NPU 仿真模式时，设置 `-DCMAKE_ASC_RUN_MODE=sim` 即可。

```bash
cmake .. -DCMAKE_ASC_RUN_MODE=npu -DCMAKE_ASC_ARCHITECTURES=dav-3510; make -j; # NPU 模式
cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510; make -j; # NPU 仿真模式
```

编译选项说明：

| 参数 | 可选值 | 说明 |
|------|--------|------|
| `CMAKE_ASC_RUN_MODE` | `npu` / `sim` | 运行模式：NPU 运行、NPU 仿真 |
| `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | 目标 SoC 架构 |

> **注意：** 切换 `CMAKE_ASC_RUN_MODE` / `CMAKE_ASC_ARCHITECTURES` 前需清理 CMake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

执行结果如下，说明精度对比成功。

```bash
test pass!
```

### NPU模式性能分析

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
cat ./OPPROF_*/PipeUtilization.csv
```
