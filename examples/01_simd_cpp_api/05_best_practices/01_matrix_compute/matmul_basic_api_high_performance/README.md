# Matmul 基础API最佳实践样例

## 概述

本样例基于静态Tensor编程范式，通过 L1/L0 双缓冲机制、大包搬运、细粒度流水同步、UnitFlag、L2Cache等多种优化手段，实现高性能矩阵乘法运算。本样例基于基础API实现，与高阶API版本所使用的优化方法相同，样例目标为基于静态Tensor编程范式展示调优实现细节。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── matmul_basic_api_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本文件
│   │   └── verify_result.py    // 真值对比文件
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── mmad.asc                // Ascend C样例实现
│   └── README.md               // 样例说明文档
```

## 样例描述

  Matmul计算公式：
  $$
  C = A * B
  $$
  - A、B为源操作数，A为左矩阵，形状为[M, K]；B为右矩阵，形状为[K, N]
  - C为目的操作数，存放矩阵乘结果的矩阵，形状为[M, N]

- 样例规格：

<table>
<tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="5" align="center">Matmul</td></tr>
<tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td><td align="center">isTrans</td></tr>
<tr><td align="center">A</td><td align="center">[M, K]</td><td align="center">half</td><td align="center">ND</td><td align="center">false</td></tr>
<tr><td align="center">B</td><td align="center">[K, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">true</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">C</td><td align="center">[M, N]</td><td align="center">half</td><td align="center">ND</td><td align="center">-</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="5" align="center">mmad_custom</td></tr>
</table>

## 样例实现

### 性能指标说明

| 指标 | 说明 |
|------|------|
| Task Duration(μs) | 整个任务执行的总时间，算子执行时间以该参数为准 |
| Block Num | 使用的核数（Block数量） |
| aicore_time(μs) | AI Core 的平均执行时间 |
| aic_mac_time(μs) | Cube 计算单元的执行时间 |
| aic_mac_ratio | Cube 计算单元的时间占比，反映计算单元利用率 |
| aic_mte1_time(μs) | MTE1（L1 到 L0A/L0B 搬运）的执行时间 |
| aic_mte1_ratio | MTE1 的时间占比，反映 L1 到 L0 的数据搬运压力 |
| aic_mte2_time(μs) | MTE2（[GM](../../../../../docs/zh/guide/编程指南/编程模型/AI-Core-SIMD编程/抽象硬件架构.md)（Global Memory） 到 L1 搬运）的执行时间 |
| aic_mte2_ratio | MTE2 的时间占比，反映 GM 到 L1 的数据加载压力 |
| aic_fixpipe_time(μs) | [Fixpipe](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md)（L0C 到 GM 搬运）的执行时间 |
| aic_fixpipe_ratio | [Fixpipe](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md) 的时间占比，反映结果写回的访存压力 |


### 数据流路径：

```
GM ──(MTE2, DataCopy)──> L1 ──(MTE1, LoadData)──> L0A/L0B ──(Cube, Mmad)──> L0C ──(Fixpipe)──> GM
       DataCopyInA/B             DataLoadA/B                   Compute             CopyOut
```

### 中级性能优化

#### 1. L1/L0 双缓冲 Ping-Pong 布局

L1 和 L0 均采用 Ping-Pong 双缓冲，使得 DataCopyIn（GM→L1）、DataLoad（L1→L0）与 Compute 三个阶段形成三级流水，每个阶段处理不同缓冲区的数据，互不阻塞。

```
时间 ──────────────────────────────────────────────────────────────>

MTE2:  |─ A1 Ping ──|─ A1 Pong ──|─ A1 Ping ──| ...
MTE1:               |─ A2 Ping ──|─ A2 Pong ──|─ A2 Ping ──| ...
Cube:                            |─ Mmad ─────|─ Mmad ─────| ...
Fixpipe:                           |─ CopyOut ──|(unitflag)
```

**L1 双缓冲布局**：A1 占 L1 前半（0~256KB），B1 占 L1 后半（256~512KB），各自再分为 Ping/Pong 两块：

```
L1 (512KB):
├── A1 Ping: [0, 128KB)
├── A1 Pong: [128KB, 256KB)
├── B1 Ping: [256KB, 384KB)
└── B1 Pong: [384KB, 512KB)
```

**L0 双缓冲布局**：A2/B2 各自独立64KB空间，各自再分为 Ping/Pong：

```
L0A/L0B (64KB):
├── A2 Ping: [0, 16KB)
├── A2 Pong: [32KB, 48KB)
├── B2 Ping: [0, 32KB)
└── B2 Pong: [32KB, 64KB)
```

```cpp
// A1: L1 Ping/Pong
AscendC::LocalTensor<half> a1LocalPing(AscendC::TPosition::A1, 0, a1PingpongSize);
AscendC::LocalTensor<half> a1LocalPong(AscendC::TPosition::A1, a1PingpongSize * sizeof(half), a1PingpongSize);
// A2: L0 Ping/Pong
AscendC::LocalTensor<half> a2LocalPing(AscendC::TPosition::A2, 0, a2PingpongSize);
AscendC::LocalTensor<half> a2LocalPong(AscendC::TPosition::A2, L0_PINGPONG_BYTES, a2PingpongSize);
```

#### 2. 细粒度流水同步

使用四类硬件事件标志实现精确的流水线同步，分为正向同步（数据就绪通知）和反向同步（缓冲区释放通知）：

| 事件类型 | 方向 | 用途 | flag 编号 |
|---------|------|------|----------|
| [MTE2_MTE1](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) | 正向 | L1 数据就绪通知，DataCopyIn 通知 DataLoad 可读取 | 0/1: A1 Ping/Pong; 2/3: B1 Ping/Pong |
| [MTE1_MTE2](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) | 反向 | L1 缓冲区释放通知，DataLoad 通知 DataCopyIn 可写入 | 同上 |
| [MTE1_M](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) | 正向 | L0 数据就绪通知，DataLoad 通知 Compute 可计算 | mte1DBFlag (0/1 交替) |
| [M_MTE1](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) | 反向 | L0 缓冲区释放通知，Compute 通知 DataLoad 可写入 | mte1DBFlag (0/1 交替) |

**反向同步需预置**：由于反向同步是"消费方 [SetFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md) → 生产方 [WaitFlag](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/SetFlag-WaitFlag(ISASI).md)"，首次使用前必须预置 SetFlag，否则首次 WaitFlag 会死锁：

```cpp
// 初始化：预置反向同步 flag，防止首次 WaitFlag 死锁
AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(EVENT_ID0);  // A1 Ping 可写
AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(EVENT_ID1);  // A1 Pong 可写
AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(EVENT_ID2);  // B1 Ping 可写
AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(EVENT_ID3);  // B1 Pong 可写
AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(EVENT_ID0);     // L0 Ping 可写
AscendC::SetFlag<AscendC::HardEvent::M_MTE1>(EVENT_ID1);     // L0 Pong 可写
```

**大包粒度的正向同步**：DataLoad 从 L1 大包中按K方向逐个base块读取数据，只需在读取大包首个base块时等待数据就绪，后续base块与首个在同一个大包内，无需重复等待：

```cpp
// 正向同步：仅大包首元素需等待数据就绪
if (kOffsetInChunkA == 0) {
    AscendC::WaitFlag<AscendC::HardEvent::MTE2_MTE1>(a1ReadIdx);
}
// 反向同步：仅大包末元素消费完后通知 DataCopyIn 可覆盖
if ((kOffsetInChunkA + 1) == stepKa) {
    AscendC::SetFlag<AscendC::HardEvent::MTE1_MTE2>(a1ReadIdx);
}
```

#### 3. 多核并行切分

按 M/N 方向均匀切分矩阵到多核并行计算。以 Atlas A2/A3（dav-2201）为例，采用 4×6 切分策略（M 方向 4 块、N 方向 6 块，共 24 核）满足地址 512B 对齐，并减少同地址访问冲突；Ascend 950PR/950DT（dav-3510）下为 32 核切分（参见后文性能数据小节）：

```cpp
constexpr uint32_t mIter = DivCeil(M, singleCoreM);
uint32_t mIterIdx = AscendC::GetBlockIdx() % mIter;
uint32_t nIterIdx = AscendC::GetBlockIdx() / mIter;
```

#### 4. DataCopyIn 优先搬运 B 矩阵

当 `stepKa > stepKb` 时，B 矩阵每 `stepKb` 个 baseK 就需要切换到下一个 L1 缓冲区（Pong），而 A 矩阵需要 `stepKa` 个 baseK 才切换。因此 B 的数据需求更紧迫。如果先搬运 A，MTE2 流水被 A 占用，B 的搬运要等 A 搬完才能开始，导致需要 B 数据时 B 尚未就绪。

本样例在 Compute 之后触发 DataCopyIn 时，**先 B 后 A**，优先搬运更紧迫的 B 数据：

```
k=0:  Compute → DataCopyIn(B1 Pong) → DataCopyIn(A1 Pong)
                 ↑ MTE2 先搬 B        ↑ 再搬 A
k=stepKb:       需要 B1 Pong → 已就绪 ✓（B 已有 stepKb 轮时间搬运）
k=stepKa:       需要 A1 Pong → 已就绪 ✓（A 有 stepKa 轮时间搬运，更充裕）
```

### 高级极限打磨

#### 5. 大包搬运

通过 `stepKa`/`stepKb` 参数将多个基本块打包为一次 DataCopyIn 操作（称为"大包"），减少 MTE2 搬运次数。例如 `stepKa=8` 表示一次将 8 个 baseM * baseK 块从 GM 搬入 L1。

```cpp
// DataCopyInA: 一次搬入 stepKa 个 baseK 块
AscendC::Nd2NzParams nd2nzParams;
nd2nzParams.nValue = curM;
nd2nzParams.dValue = baseK * stepKa;  // 大包包含 stepKa 个 baseM * baseK
```

#### 6. LoadData3D 替代 LoadData2D——减少指令队列占用

在 Atlas A2/A3 架构上，本样例使用 [`LoadData3DParamsV2`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_3D.md)（即 [LoadData3D](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_3D.md)）替代 [`LoadData2DParams`](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬入/LoadData_2D.md)（即 LoadData2D）完成 L1→L0 的数据搬运。这是一个关键的指令队列优化。

**问题背景**：MTE1 指令队列深度为 32。使用 LoadData2D 时，由于单条 LoadData2D 指令搬运粒度有限，搬运一个 baseM×baseK 的切片需要用 for 循环发射多条 LoadData2D 指令。例如 baseM=128、baseK=64 时，最少需要发射 `baseK/16 = 4` 条 LoadData2D 指令。

**代码实现**：使用 LoadData2D 时，需要按 `CUBE_BLOCK` 粒度循环发射多条搬运指令；改为 LoadData3D 后，可用一条指令描述完整的 base 切片。

优化前，`baseK=64` 时循环次数为 4：

```cpp
AscendC::LoadData2DParams loadDataParams;
for (int i = 0; i < DivCeil(baseK, CUBE_BLOCK); ++i) {
    AscendC::LoadData(b2Local[i * dstOffset], b1Local[srcAddr + i * srcOffset], loadDataParams);
}
```

优化后，使用 `LoadData3DParamsV2` 一次性描述 `baseM * baseK` 切片：

```cpp
AscendC::LoadData3DParamsV2<half> loadDataParams;
loadDataParams.l1H = 1;
loadDataParams.l1W = baseM;
loadDataParams.channelSize = baseK;
loadDataParams.kExtension = baseK;
loadDataParams.mExtension = curMAlign;
loadDataParams.mStartPt = 0;
loadDataParams.kStartPt = 0;
AscendC::LoadData(a2Local, a1Local[srcAddr], loadDataParams);
```

这样每个 base 切片的 MTE1 指令数从多条 `LoadData2D` 降为 1 条 `LoadData3D`，可以降低 MTE1 队列占用。B 矩阵非转置输入时，同样通过 `LoadData3DParamsV2` 配置 `enTranspose = true` 完成转置搬运。

> **注意**：Atlas A5 芯片提供了 `LoadData2DParamsV2` 接口，单条指令即可完成搬运，无需 LoadData3D。因此本样例通过 `__NPU_ARCH__` 条件编译区分两种架构的 LoadData 实现。

#### 7. 常量 Tiling

所有 Tiling 参数（baseM/baseK/baseN、singleCoreM/K/N、stepKa/stepKb）通过模板参数在编译期确定，运行时无需 Scalar 动态计算，减少 Scalar 开销：

```cpp
template <uint32_t M, uint32_t K, uint32_t N, uint32_t baseM, uint32_t baseK, uint32_t baseN,
          uint32_t singleCoreM, uint32_t singleCoreK, uint32_t singleCoreN,
          uint32_t stepKa, uint32_t stepKb>
class KernelMmad { ... };
```

#### 8. UnitFlag 优化

开启 UnitFlag 后，[MMAD](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/Mmad计算/Mmad.md) 和 [FIXPIPE](../../../../../docs/zh/api/SIMD-API/基础API/矩阵计算（ISASI）/矩阵计算的搬出/Fixpipe（L0C到GM数据搬运）.md) 实现细粒度（512B）流水并行，而非指令级同步。每当 Cube 完成一个 512B 数据结果的计算，FIXPIPE 立即搬出该数据，Cube 计算与结果写回流水重叠：

```cpp
mmadParams.unitFlag = (kBlockIdx != kLoopCount - 1) ? 2 : 3;  // 开启 UnitFlag
```

- `unitFlag = 2`：中间 K 块，MMAD 计算结果不立即写回，而是等待下一个 512B 完成后流水搬出
- `unitFlag = 3`：最后一个 K 块，通知 FIXPIPE 将所有剩余结果写回 GM

#### 9. L2Cache 优化

L2Cache 是 AI Core 共享的外部缓存，纯读带宽约为 GM 的 3 到 4 倍。若数据无法命中 L2Cache，需要访问 GM，带宽利用效率较低，导致 MTE2 成为性能瓶颈。

L2Cache 切分的具体实现与[高阶 API Matmul 样例](../matmul_high_performance/README.md)中的 Case 6 一致，核心思路均为将 A 矩阵 M 轴切分，使 B 矩阵跨轮驻留 L2Cache。本样例通过 `ProcessL2Cache()` 方法实现双重外层循环，按 `outerMIdx` 分轮调度，每轮内 24 核并行计算各自的子块：

```cpp
// ProcessL2Cache: 按 M 方向分轮，每轮 24 核覆盖 mIterPerRound 份 M 子块
constexpr uint32_t mIterPerRound = DivCeil(M, singleCoreM * 2);
constexpr uint32_t outerMLoopCount = DivCeil(mIterTotal, mIterPerRound);

for (uint32_t outerMIdx = 0; outerMIdx < outerMLoopCount; outerMIdx++) {
    uint32_t mIterIdx = AscendC::GetBlockIdx() % mIterPerRound + outerMIdx * mIterPerRound;
    uint32_t nIterIdx = AscendC::GetBlockIdx() / mIterPerRound;
    if (mIterIdx >= mIterTotal || nIterIdx >= nIterTotal) continue;
    InitComputeParamsL2Cache(mIterIdx, nIterIdx);
    ProcessLoop(...);
}
```

#### K 方向主循环完整流程

以 stepKa=8、stepKb=4 为例，展示一个完整 (mBlockIdx, nBlockIdx) 子块内 K 方向循环的详细执行流程：

```
预处理：
  SetFlag(MTE1_MTE2, 0/1/2/3)  // 预置反向同步：L1 Ping/Pong 均可写
  SetFlag(M_MTE1, 0/1)         // 预置反向同步：L0 Ping/Pong 均可写
  DataCopyIn(A1 Ping, k=0)     // 搬入首个 A 大包到 Ping
  DataCopyIn(B1 Ping, k=0)     // 搬入首个 B 大包到 Ping
  SetFlag(MTE2_MTE1, 0/2)      // 通知 A1/B1 Ping 数据就绪

K循环 kBlockIdx = 0, 1, ..., kLoopCount-1:
  ┌─ a1ReadIdx = (kBlockIdx / stepKa) % 2        // 当前读 L1 A 的 Ping/Pong
  │  b1ReadIdx = (kBlockIdx / stepKb) % 2        // 当前读 L1 B 的 Ping/Pong
  │  kOffsetInChunkA = kBlockIdx % stepKa        // 当前 baseK 在 A 大包内的偏移
  │  kOffsetInChunkB = kBlockIdx % stepKb        // 当前 baseK 在 B 大包内的偏移
  │
  │  WaitFlag(M_MTE1, mte1DBFlag)                // 等待上一轮 Compute 释放 L0
  │  if (kOffsetInChunkA == 0)
  │      WaitFlag(MTE2_MTE1, a1ReadIdx)          // 等待 L1 A 大包数据就绪（仅首元素）
  │  if (kOffsetInChunkB == 0)
  │      WaitFlag(MTE2_MTE1, b1ReadIdx + 2)      // 等待 L1 B 大包数据就绪（仅首元素）
  │
  │  DataLoadA(A1 → A2)                          // L1 → L0
  │  DataLoadB(B1 → B2)                          // L1 → L0
  │
  │  if (kOffsetInChunkA + 1 == stepKa)
  │      SetFlag(MTE1_MTE2, a1ReadIdx)           // A 大包末元素：通知 L1 A 可写
  │  if (kOffsetInChunkB + 1 == stepKb)
  │      SetFlag(MTE1_MTE2, b1ReadIdx + 2)       // B 大包末元素：通知 L1 B 可写
  │
  │  Compute(Mmad)                               // Cube 计算（M 指令）
  │  SetFlag(M_MTE1, mte1DBFlag)                 // 通知 L0 可覆盖
  │  mte1DBFlag ^= 1                             // 切换 L0 Ping/Pong
  │
  │  // DataCopyIn 放在 Compute 之后，先 B 后 A
  │  if (B大包末元素 && 还有B数据):
  │      WaitFlag(MTE1_MTE2, b1WriteIdx + 2)     // 等待 L1 B 缓冲区可写
  │      DataCopyInB(下一个 B 大包)               // GM → L1（MTE2 指令）
  │      SetFlag(MTE2_MTE1, b1WriteIdx + 2)      // 通知 L1 B 数据就绪
  │  if (A大包末元素 && 还有A数据):
  │      WaitFlag(MTE1_MTE2, a1WriteIdx)         // 等待 L1 A 缓冲区可写
  │      DataCopyInA(下一个 A 大包)               // GM → L1（MTE2 指令）
  │      SetFlag(MTE2_MTE1, a1WriteIdx)          // 通知 L1 A 数据就绪
  └─
```

**时序示意**（stepKa=8, stepKb=4）：

```
预取: DataCopyIn(A1 Ping) + DataCopyIn(B1 Ping)                     ← K循环前

k=0:  WaitFlag(A1Ping, B1Ping) → DataLoad → Compute → DataCopyIn(B1Pong) → DataCopyIn(A1Pong)
k=1:  DataLoad → Compute
k=2:  DataLoad → Compute
k=3:  DataLoad(释放B1Ping) → Compute → DataCopyIn(搬运B1Ping)

k=4:  WaitFlag(B1Pong) → DataLoad(B1Pong) → Compute
k=5:  DataLoad → Compute
k=6:  DataLoad → Compute
k=7:  DataLoad(释放A1Ping B1Pong) → Compute → DataCopyIn(B1Pong) → DataCopyIn(A1Ping)
k=8:  WaitFlag(A1Pong已就绪✓) → WaitFlag(B1Ping已就绪✓) → DataLoad → Compute
...
```

### 性能数据分析

#### Atlas A2训练系列芯片性能数据

- Scenario 1：不使能 L2Cache 切分，singleCoreM=2048, singleCoreN=1536，24 核一轮全覆盖
- Scenario 2：使能 L2Cache 切分，singleCoreM=1024, singleCoreN=1536，24 核分 2 轮计算

| Scenario | Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------|------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| Scenario 1 | 4121.16 | 24 | 3670.7 | 3081.664 | 0.84 | 337.343 | 0.092 | 2538.348 | 0.692 | 3552.248 | 0.968 | 160.405 | 0.044 |
| Scenario 2 | 4081.64 | 24 | 3636.85 | 3082.158 | 0.847 | 345.139 | 0.095 | 2553.064 | 0.702 | 3487.068 | 0.959 | 161.812 | 0.044 |

除去启动开销，已达成该芯片 84.7% 的峰值算力。

使能 L2Cache 切分后，aic_mte2_time 从 3552.248μs 降低到 3487.068μs，降低了 1.84%。当前切分策略较简单，用户可进一步优化 L2Cache 切分策略以提高 MTE2 带宽。

#### Ascend 950PR芯片性能数据

- Scenario 1：不使能 L2Cache 切分，singleCoreM=2048, singleCoreN=1024，32 核一轮全覆盖
- Scenario 2：使能 L2Cache 切分，singleCoreM=1024, singleCoreN=1024，32 核分 2 轮计算

| Scenario | Task Duration(μs) | Block Num | aicore_time(μs) | aic_mac_time(μs) | aic_mac_ratio | aic_scalar_time(μs) | aic_scalar_ratio | aic_mte1_time(μs) | aic_mte1_ratio | aic_mte2_time(μs) | aic_mte2_ratio | aic_fixpipe_time(μs) | aic_fixpipe_ratio |
|------|------------------|-----------|----------------|-----------------|---------------|-------------------|-----------------|------------------|----------------|------------------|----------------|--------------------|-------------------|
| Scenario 1 | 2572.047 | 32 | 2571.44 | 2564.813 | 0.997 | 144.604 | 0.056 | 828.001 | 0.322 | 1874.267 | 0.729 | 221.997 | 0.086 |
| Scenario 2 | 2574.492 | 32 | 2573.39 | 2564.147 | 0.996 | 104.845 | 0.041 | 819.207 | 0.318 | 1892.742 | 0.736 | 223.129 | 0.087 |

已达成该芯片 99.7% 的峰值算力。

使能 L2Cache 切分后在 Ascend 950PR 芯片上没有明显效果，原因如下：L2Cache 优化的目标是缓解 MTE2 bound，但当前瓶颈为 Cube 计算而非数据搬运，因此减少 MTE2 耗时无法提升整体性能；此外 L2Cache 切分将计算分为 2 轮调度，引入了额外的 Scalar 开销和调度开销，导致 Scenario 2 的 Task Duration 略高于 Scenario 1。同时，Scenario 2 的 aic_mte2_time（1892.742μs）反而略高于 Scenario 1（1874.267μs），这是因为当样例处于 Cube bound 时，MTE2 流水线被 Cube 计算阻塞，profiler 采集的 aic_mte2_time 包含了流水线等待时间而非纯粹的数据搬运时间，L2Cache 优化虽然减少了实际数据访问延迟，但被 Cube 计算瓶颈掩盖，无法在 MTE2 指标上体现。

### 理论性能分析

#### Cube 计算性能分析

**Atlas A2 训练系列芯片**：样例参数 M=N=K=8192，baseM=128，baseN=256，baseK=64，该计算芯片主频为 1.85GHz，每 cycle 处理 16×16×16 次乘加运算。

$$cube\_time = \frac{M \times N \times K}{16 \times 16 \times 16 \times core\_num \times cube\_freq} = \frac{8192 \times 8192 \times 8192}{16 \times 16 \times 16 \times 24 \times 1850} = 3022.92\mu s$$

Cube 计算耗时误差：

$$误差 = \frac{aic\_mac\_time - cube\_time}{cube\_time} = \frac{3082.158 - 3022.92}{3022.92} = 1.96\%$$

**Ascend 950PR 芯片**：样例参数 M=N=K=8192，baseM=256，baseN=256，baseK=64，该处理器主频为 1.65GHz，每 cycle 处理 16×16×16 次乘加运算。

$$cube\_time = \frac{M \times N \times K}{16 \times 16 \times 16 \times core\_num \times cube\_freq} = \frac{8192 \times 8192 \times 8192}{16 \times 16 \times 16 \times 32 \times 1650} = 2542.00\mu s$$

Cube 计算耗时误差：

$$误差 = \frac{aic\_mac\_time - cube\_time}{cube\_time} = \frac{2564.813 - 2542.00}{2542.00} = 0.90\%$$

#### MTE2 带宽分析

**读入数据总量**：

Atlas A2 训练系列芯片（baseM=128，baseN=256）：

$$读入数据总量 = \left(\frac{N}{baseN} \times M \times K + \frac{M}{baseM} \times K \times N\right) \times sizeof(half) = (32 \times 8192 \times 8192 + 64 \times 8192 \times 8192) \times 2B = 12GB$$

Ascend 950PR 芯片（baseM=256，baseN=256）：

$$读入数据总量 = \left(\frac{N}{baseN} \times M \times K + \frac{M}{baseM} \times K \times N\right) \times sizeof(half) = (32 \times 8192 \times 8192 + 32 \times 8192 \times 8192) \times 2B = 8GB$$

**MTE2 理论耗时**：

Atlas A2 训练系列芯片：L2Cache 峰值带宽约 5TB/s，HBM 带宽约 1.8TB/s。首次从 HBM 读入数据，后续从 L2Cache 读取。

$$第一次从HBM读入的数据总量 = M \times K \times sizeof(half) + K \times N \times sizeof(half) = 256MB$$

$$MTE2理论耗时 = \frac{HBM读入数据总量}{1.8TB/s} + \frac{L2Cache读入数据总量}{5TB/s}$$

MTE2 耗时误差：

$$MTE2耗时误差 = \frac{3487.068 - 2672.44}{2672.44} = 30.48\%$$

当前MTE2耗时与理论值相差较大，因为实际芯片L2Cache大小为192MB，当前L2Cache切分策略较简单；另一方面当MTE2搬运场景为ND2NZ（GM数据Layout为ND，搬运到L1时需做ND→NZ格式转换）时，L2Cache带宽会降低。用户可进一步优化L2Cache切分策略以提高MTE2带宽。

Ascend 950PR 芯片：L2Cache 峰值带宽约 5TB/s，HBM 带宽约 1.6TB/s。

$$MTE2理论耗时 = \frac{HBM读入数据总量}{1.6TB/s} + \frac{L2Cache读入数据总量}{5TB/s}$$

MTE2 耗时误差：

$$MTE2耗时误差 = \frac{1874.267 - 1832.10}{1832.10} = 2.30\%$$

相比于 Atlas A2 训练系列芯片，Ascend 950PR 芯片数据搬运更为高效，MTE2 带宽利用率更高。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 切换Case

  在 cmake 编译时通过 `-DSCENARIO_NUM=N` 指定要编译的场景，各场景说明：
  - `1`: 不使能L2Cache切分
  - `2`: 使能L2Cache切分

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
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # 编译工程，默认npu模式
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

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`、`dav-3510` | NPU硬件架构：dav-2201对应Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510对应Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`、`2` | 场景编号：1=不使能L2Cache切分，2=使能L2Cache切分 |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```

## 功能调试

### printf

该接口提供CPU域或NPU域调试场景下的格式化输出功能。

在算子kernel侧实现代码中需要输出日志信息的地方调用printf接口打印相关内容。

示例如下：

```cpp
AscendC::printf("matmul blockIdx=%d\n", AscendC::GetBlockIdx());
```

> [!CAUTION]注意 
>printf（PRINTF）接口打印功能会对算子实际运行的性能带来一定影响，通常在调测阶段使用。开发者可以按需通过设置ASCENDC\_DUMP=0的方式关闭打印功能。

### DumpTensor

基于算子工程开发的算子，可以使用该接口Dump指定Tensor的内容。同时支持打印自定义的附加信息（仅支持uint32\_t数据类型的信息），比如打印当前行号等。

在算子kernel侧实现代码中需要打印Tensor数据的地方调用DumpTensor接口打印相关内容。样例如下：

```cpp
AscendC::DumpTensor(cLocal, baseM * baseN);
```

> [!CAUTION]注意 
>DumpTensor接口打印功能会对算子实际运行的性能带来一定影响，通常在调测阶段使用。开发者可以按需通过设置ASCENDC\_DUMP=0来关闭打印功能。

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
