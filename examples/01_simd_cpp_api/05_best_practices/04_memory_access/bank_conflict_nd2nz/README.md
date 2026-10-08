# ND2NZ Bank 冲突调优样例


## 概述

本样例以 `8192 × 8192` half ND 矩阵到全局紧凑 NZ 布局的转换为例，介绍UB 上向量写阶段的 bank 冲突如何产生、如何规避。整个调优过程分为两个步骤（case 1-2），通过调整 `dstNzC0Stride` 这一个参数对比性能。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```text
├── bank_conflict_nd2nz
│   ├── bank_conflict_nd2nz.asc    // Ascend C 样例实现（含 2 个 case）
│   ├── bank_conflict_nd2nz_3510.h // dav-3510 Regbase ND2NZ 实现
│   ├── CMakeLists.txt             // 编译工程文件
│   ├── data_utils.h               // 数据读入写出函数
│   ├── figures                    // 图示
│   ├── README.md                  // 样例说明文档
│   └── scripts
│       ├── gen_data.py            // 输入数据和真值数据生成脚本文件
│       └── verify_result.py       // 真值对比文件
```

## 样例描述

**样例功能**：

  样例实现的是固定 shape 为 `8192 × 8192` 的 half ND 矩阵到全局紧凑 NZ 布局的转换。

  ND → NZ 转换的本质是一次大规模向量重排写：把 tile 内每行的 8 个 C0 块（每个 C0 块 = 16 half = 32B = 1 个 DataBlock）"撒"到 NZ 布局里 8 个不同 C0 列的对应行槽里。`dstNzC0Stride` 控制 UB 内相邻 C0 列的落点间距，是 bank 冲突的核心旋钮：

  - x：输入，shape `[8192, 8192]`，half，ND 行优先布局
  - z：输出，shape `[8192, 8192]`，half，全局紧凑 NZ 布局

**分块与分核**：

当前实现先把 `8192 × 8192` 输入按 N / D 两个方向切给各 Vector 核，每个 block 负责一个行切分 × 列切分区域。切分方式按架构配置：2201 使用 `6 行切分 × 8 列切分 = 48` 个 block，3510 使用 `8 行切分 × 8 列切分 = 64` 个 block。block 内部再切成 `144 × 128` half 小 tile 跑流水；由于 `8192` 不能被 `144` 整除，最后一个全局行 tile 的有效高度为 `128`，在代码中通过 `actualTileH` 处理尾块。

```text
2201:
      colSplit →  0       1       2     ...       7

rowSplit ↓      ┌───────┬───────┬───────┬─────┬───────┐
      0         │  #0   │  #1   │  #2   │ ... │  #7   │
      1         │  #8   │  #9   │ #10   │ ... │ #15   │
     ...        │                  ...                │
      5         │ #40   │ #41   │ #42   │ ... │ #47   │
                └───────┴───────┴───────┴─────┴───────┘

3510:
      colSplit →  0       1       2     ...       7
rowSplit ↓      ┌───────┬───────┬───────┬─────┬───────┐
      0         │  #0   │  #1   │  #2   │ ... │  #7   │
      1         │  #8   │  #9   │ #10   │ ... │ #15   │
     ...        │                  ...                │
      7         │ #56   │ #57   │ #58   │ ... │ #63   │
                └───────┴───────┴───────┴─────┴───────┘

```

**关键参数**：

| 参数 | 值 | 含义 |
|:---:|:---:|:---|
| `rowSplits` | dav-2201场景 = 6 / dav-3510场景 = 8 | N 方向切分数，不同的架构切分逻辑不同，dav-2201 指 Atlas A2/A3 系列产品，dav-3510 指 Ascend 950PR&950DT系列产品 |
| `colSplits` | 8 | D 方向切分数 |
| `totalBlocks` | dav-2201场景 = 48 / dav-3510场景 = 64 | 启动 block 数，等于 `rowSplits * colSplits` |
| `tileH` | 144 | 单次搬运 tile  N 方向最大行数；尾块通过 `actualTileH` 变为 128 |
| `tileW` | 128 | 单次搬运 tile  D 方向 half 数 |
| `C0_ELEMS` | 16 | 固定布局参数：一个 C0 块包含的 half 元素数，也是一个 DataBlock 的 half 元素数，不是可调 tiling 参数 |
| `vecLenHalf` | 128 | tiling 参数：单条向量指令一次处理的 half 元素数 |
| `tileC0Cols` | 8 | 派生参数：单次搬运 tile  D 方向 C0 列数，等于 `tileW / C0_ELEMS` |
| `dstNzNStride` | 1 | 单次搬运 UB 内 N 方向步长，以 DataBlock 为单位 |
| `dstNzC0Stride` | case1 = 144 / case2 = 145 | 单次搬运 UB 内相邻 C0 列起始位置的间距，以 DataBlock 为单位。对应 `Copy()` 接口 `CopyRepeatParams` 中的 `dstStride` —— 同一迭代内 datablock 的地址步长。本样例通过切换 144 / 145 来对比 bank 冲突场景。 |
| `vecLenDbs` | 8 | 派生参数：单条向量指令一次处理的 DataBlock 数，等于 `vecLenHalf / C0_ELEMS` |

> **术语说明 · DataBlock**
>
> DataBlock 是**一条 NPU 矢量计算指令处理的数据单元**，大小为 **32 字节**（half 类型下即 16 个 half）；一条矢量指令一次 Repeat 可同时处理多个 DataBlock。后文所有 stride 类参数（`dstNzNStride` / `dstNzC0Stride`）取值都以 DataBlock 为单位；行文中的"32B 块"指的就是 DataBlock。

**ND → NZ 数据排布**：

ND和NZ的数据排布如图所示

**图：ND 与 NZ 数据排布示意图**

<img src="figures/nd2nz.png" width="80%">

## 样例实现

### 性能指标说明

**表 1 AI Core 性能指标字段说明表**

| 字段名 | 字段含义 |
|:---:|:---|
| Task Duration(μs)|Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间。|
| aiv_time|Task在AI Vector Core上的执行时间，单位为μs。|
| aiv_vec_time(μs) | vec类型指令（向量类运算指令）耗时，单位μs。 |
| aiv_vec_ratio | vec类型指令（向量类运算指令）的cycle数在total cycle数中的占用比。 |
| aiv_scalar_time(μs) | scalar类型指令（标量类运算指令）耗时，单位μs。 |
| aiv_scalar_ratio | scalar类型指令（标量类运算指令）的cycle数在total cycle数中的占用比。 |
| aiv_mte2_time(μs) | mte2类型指令（GM->UB搬运类指令）耗时，单位μs。 |
| aiv_mte2_ratio | mte2类型指令（GM->UB搬运类指令）的cycle数在total cycle数中的占用比。 |
| aiv_mte3_time(μs) | mte3类型指令（UB->GM搬运类指令）耗时，单位μs。 |
| aiv_mte3_ratio | mte3类型指令（UB->GM搬运类指令）的cycle数在total cycle数中的占用比。 |

---

### Atlas A2/A3 系列产品（dav-2201）

> 编译时通过 `-DCMAKE_ASC_ARCHITECTURES=dav-2201` 选择 Atlas A2/A3 系列产品。

#### UB Bank 结构与冲突类型

Atlas A2/A3 系列产品上，Unified Buffer 总大小 192KB，划分为 48 个 bank（每个 bank 128 行 × 32B = 4KB）；这 48 个 bank 进一步组织为 **16 个 bank group**，每个 bank group 包含 3 个 bank。Vector 计算单元每拍（一个指令周期）能从每个 bank group 中读取或写入一行数据。当一拍内的多个读 / 写操作落到同一个 bank 或 bank group 时，硬件无法在一个周期内全部处理，需要排队等待，即 bank 冲突。

**图：Atlas A2/A3 UB Bank 结构示意图**（箭头方向表示内存排布顺序）

<img src="figures/ubBankStruct2201.png" width="80%">

bank 冲突主要分为三类：

- **读写冲突**：读操作和写操作同时尝试访问同一个 bank；
- **写写冲突**：多个写操作同时尝试访问同一个 bank group；
- **读读冲突**：多个读操作同时尝试访问同一个 bank group。

本样例的 ND→NZ 转换主要触发**写写冲突**：向量写 NZ 时一拍要落 8 个 DataBlock，落点的 bank group 由 `dstNzC0Stride` 决定。下面对比两种 case。

#### Case 1: 紧凑排放

**实现方式**：编译时 `-DSCENARIO_NUM=1`。

代码中每次取最多 `144×128` 的 `Tile` 进行 ND→NZ 重排，转置后的 8 个 C0 列在 UB 里**依次紧密排开、一列接一列首尾相连**，这是 NZ 布局的"自然写法"。尾块使用 `actualTileH=128`，但 UB 内相邻 C0 列的最大 stride 仍按 `tileH=144` 保持固定。

**关键代码**：

**ND2NZ 实现**：

`Copy()` 自带 repeat，单条 Copy 覆盖 `actualTileH` 行 × 8 个 C0 列；非尾块时即 `144×128` half，一行 8 个 C0 列只需要 1 次 `Copy()`，因此在`tileW`方向进行循环：

```cpp
AscendC::CopyRepeatParams copyParams;
copyParams.dstStride     = dstNzC0Stride;    // = 144，相邻 C0 列起始位置的间距
copyParams.srcStride     = 1;
copyParams.dstRepeatSize = 1;
copyParams.srcRepeatSize = tileW / C0_ELEMS;

for (uint32_t k = 0; k < tileW / vecLenHalf; ++k) {
    AscendC::Copy(nzBuf[k * vecLenDbs * dstNzC0Stride * C0_ELEMS],
                  ndBuf[k * vecLenHalf],
                  static_cast<uint64_t>(vecLenHalf),
                  static_cast<uint8_t>(actualTileH),
                  copyParams);
}
```

一拍取的 8 个 datablock 数据在 NZ 矩阵中的排布如下图所示，非尾块中原本相邻的两个 datablock 在 NZ 排布中起始地址需间隔 `144 * 32B`：

**图：Case 1 紧凑排放下 ND 到 NZ 示意图**

<img src="figures/datand2nzS1.png" width="80%">

**Ping-Pong 流水**：

样例在 `Process` 中开启双缓冲机制，UB 上同时分配 `ndPing/ndPong` 和 `nzPing/nzPong` 两套 buffer，相邻 tile 交替使用两组 buffer。每组 buffer 使用独立 EVENTID。

**UB 上的摆放顺序**：4 个 buf 按 `[ndPing | ndPong | nzPing | nzPong]` 顺序摆放。

> 下文 Case 2 / Ascend 950PR&950DT系列产品 各 case 的 Ping-Pong 流水实现完全一致，不再重复展开。

**冲突原理**：

UB 的 16 个 bank group 各自可以并行接收写入；地址按 32B 一块为单位顺序循环分配：第 1 个 32B 块属于 bank group 0、第 2 个属于 bank group 1、……、第 17 个又回到 bank group 0 重新一轮。**一拍内多个写如果都要去同一个 bank group，就只能排队**。

单条 copy 一拍要把 8 个 32B 块写到 UB，相邻两个落点之间隔 `dstNzC0Stride = 144` 个 32B 块。**144 刚好是 16 的整数倍**——每跳 144 步都会回到同一个 bank group。结果是 8 个写全部挤到 bank group 0，硬件只能依次处理，单条 copy 从 1 拍拉长为 **8 拍**，aiv_vec_time 显著膨胀。

**S1 落点示意图**：

```text
单条 copy() 的 8 个写在 UB 上的落点：
  DataBlock 0 → 第   0 个 32B 块  → bank group 0
  DataBlock 1 → 第 144 个 32B 块  → bank group 0   ◀ 同一 bank group
  DataBlock 2 → 第 288 个 32B 块  → bank group 0   ◀ 同一 bank group
  DataBlock 3 → 第 432 个 32B 块  → bank group 0   ◀ 同一 bank group
  DataBlock 4 → 第 576 个 32B 块  → bank group 0   ◀ 同一 bank group
  DataBlock 5 → 第 720 个 32B 块  → bank group 0   ◀ 同一 bank group
  DataBlock 6 → 第 864 个 32B 块  → bank group 0   ◀ 同一 bank group
  DataBlock 7 → 第1008 个 32B 块  → bank group 0   ◀ 同一 bank group
```

下图中蓝色部分表示一拍从 NDBUF 中读取数据，红色部分表示一拍写入 NZBUF 的数据，可以看到 8 个 datablock 全部落入同一个 bank group：

**图：Atlas A2/A3 Case 1 的 UB bank group 冲突示意图**

<img src="figures/s1bank2201.png" width="80%">

**性能数据**（Atlas A2/A3 系列产品，当前 `144×128` 配置，48 核）：

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 151.42 | 138.00 | **47.790** | 0.346 | 6.586 | 0.048 | 113.856 | 0.825 | 76.760 | 0.556 |

**优化效果分析**：
- `aiv_vec_time = 47.790 μs`。
- 主要冲突点在 Vec 写阶段：copy 的 8 个 32B 块全打 bank group 0。

#### Case 2: nzBuf 多申请一行

**实现方式**：编译时 `-DSCENARIO_NUM=2`。

在 Case 1 紧凑排放的基础上，**将 `dstNzC0Stride` 从 `tileH` 调整为 `tileH + 1`**，相当于 `nzBuf` 多申请一行空间，让向量写落点能在不同 bank group 间轮转。

**关键代码**：

```cpp
constexpr uint32_t dstNzC0Stride = (SCENARIO == 1) ? tileH : (tileH + 1);
```

Ping-Pong 流水与 Case 1 完全一致，参见上文。

**规避原理**：

把 `dstNzC0Stride` 从 144 改成 145，相比于 Case1 在 UB 上多申请了一行空间，相邻两个写落点之间隔 145 个 32B 块。每 145 步，相当于在 16 个 bank group 里走完 9 整轮后又额外多迈 1 步，落点依次进入 bank group 0、1、2、……、7，**8 个写分散到 8 个不同 bank group**，硬件可以并发处理，单条 copy **1 拍**完成。

多申请的这一行空间不作为有效数据写回 GM，只用于改变 UB 内写落点的 bank 分布。

**图：Case 2 nzBuf 多申请一行后的 ND 到 NZ 示意图**

<img src="figures/datand2nzS2.png" width="80%">

**S2 落点示意图**：

```text
单条 copy() 的 8 个写在 UB 上的落点：
  DataBlock 0 → 第   0 个 32B 块  → bank group 0
  DataBlock 1 → 第 145 个 32B 块  → bank group 1
  DataBlock 2 → 第 290 个 32B 块  → bank group 2
  DataBlock 3 → 第 435 个 32B 块  → bank group 3
  DataBlock 4 → 第 580 个 32B 块  → bank group 4
  DataBlock 5 → 第 725 个 32B 块  → bank group 5
  DataBlock 6 → 第 870 个 32B 块  → bank group 6
  DataBlock 7 → 第1015 个 32B 块  → bank group 7
```

**图：Atlas A2/A3 Case 2 的 UB bank 落点示意图**

<img src="figures/s2bank2201.png" width="80%">

**UB 占用变化**：

`ndBuf` 占用 = `tileH * tileW * sizeof(half) = 144 * 128 * 2 = 36864 B = 36 KB`

`nzBuf` DataBlock 数统计的是 UB 实际申请空间，不是写回 GM 的有效数据量。S1 按 144 行紧凑申请，为 `144 × 8 = 1152` 个 DataBlock；S2 的 `dstNzC0Stride = 145`，相当于 `nzBuf` 多申请一行空间，因此为 `145 × 8 = 1160` 个 DataBlock。

| 场景 | `ndBuf` 占用 | `nzBuf` DataBlock 数 | `nzBuf` 占用 |
|:---:|:---:|:---:|:---:|
| S1 (stride=144) | 36864 B = 36 KB | 1152 | 1152 × 32 B = 36864 B = 36 KB（紧凑） |
| S2 (stride=145) | 36864 B = 36 KB | 1160 | 1160 × 32 B = 37120 B ≈ 36.3 KB（按 145 行申请） |

多申请的这一行空间在 UB→GM 的 MTE3 阶段由 `outParams.srcStride = dstNzC0Stride - tileH` 跳过，不会写回 GM。

双缓冲后，每套 buffer 包含一个 `ndBuf` 和一个 `nzBuf`。S1 总 UB 占用为 `2 × (36864 B + 36864 B) = 147456 B = 144 KB`，S2 总 UB 占用为 `2 × (36864 B + 37120 B) = 147968 B ≈ 144.5 KB`，不会超过 UB Bank 内存。

**性能数据**（Atlas A2/A3 系列产品，当前 `144×128` 配置，48 核）：

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 144.00 | 131.44 | **7.133** | 0.054 | 7.760 | 0.059 | 115.405 | 0.878 | 76.532 | 0.582 |

**优化效果分析**：
- `aiv_vec_time` 从 47.790 → 7.133 μs（提升 **6.70×**）， bank group 分散后 Vec 写冲突明显降低。
- 代价是每 tile 多占用 8 × 32B = 256B 的 UB 空间（约 0.7% 浪费），完全可接受。

---

### Ascend 950PR&950DT系列产品（dav-3510）

> 编译时通过 `-DCMAKE_ASC_ARCHITECTURES=dav-3510` 选择 Ascend 950PR&950DT系列产品。

#### UB Bank 结构与冲突类型

Ascend 950PR&950DT系列产品 上，Unified Buffer 总大小 256KB，划分为 16 个 bank（每个 bank 512 行 × 32B = 16KB）；这 16 个 bank 进一步组织为 **8 个 bank group**，每个 bank group 包含 2 个 bank（bank `i` 与 bank `i+8` 同属 bank group `i`，即 `bank group = bank % 8`）。Vector 计算单元每拍能从每个 bank group 中读取或写入一行数据。

**图：Ascend 950PR&950DT系列产品 UB Bank 结构示意图**

<img src="figures/ubBankStruct3510.png" width="80%">

**地址采用低位交织**：UB 内连续地址以 32B 为单位在 bank0~bank15 间循环 —— 第 1 个 32B 块落到 bank0、第 2 个落到 bank1、……、第 16 个落到 bank15，第 17 个回到 bank0 的下一行，依次类推。

**图：Ascend 950PR&950DT系列产品 UB Bank 内存排布示意图**（其中箭头方向表示内存排布顺序）
<img src="figures/UB-3510.png" width="80%">

bank 冲突主要分为三类：

- **读写冲突**：读操作和写操作同时尝试访问同一个 bank；
- **写写冲突**：多个写操作同时尝试访问同一个 bank group；
- **读读冲突**：两个读操作同时尝试访问同一个 bank，或两个以上读操作同时尝试访问同一个 bank group。

本样例的 ND→NZ 转换主要触发**写写冲突**：向量写 NZ 时一拍要落 8 个 DataBlock，落点的 bank group 由 `dstNzC0Stride` 决定。下面对比两种 case。

#### Case 1: 紧凑排放

**实现方式**：编译时 `-DSCENARIO_NUM=1`。

代码中每次取最多 `144×128` 的 `Tile` 进行 ND→NZ 重排，转置后的 8 个 C0 列在 UB 里**依次紧密排开、一列接一列首尾相连**，这是 NZ 布局的"自然写法"。尾块使用 `actualTileH=128`。

**关键代码**：

```cpp
constexpr uint32_t dstNzC0Stride = (SCENARIO == 1) ? tileH : (tileH + 1);
```

Ping-Pong 流水的实现与 Atlas A2/A3 Case 1 完全相同（参见上文 [Atlas A2/A3 → Case 1 → Ping-Pong 流水](#case-1-紧凑排放)）。

**ND2NZ 实现**：

Ascend 950PR&950DT系列产品 基于 Reg 编程接口实现 Nd2Nz 重排，使用 `__simd_vf__`，外层向量列组、内层行循环：

```cpp
for (uint16_t k = 0; k < vecsPerRow; ++k) {
    const uint32_t dstColGroupOffset = k * vecLenDbs * dstNzC0Stride * C0_ELEMS;

    for (uint16_t r = 0; r < actualTileH; ++r) {
        AscendC::Reg::LoadAlign<half>(reg, srcAddr + r * tileW + k * vecLenHalf);

        __ubuf__ half* dstRowAddr = dstAddr + dstColGroupOffset + r * C0_ELEMS;
        AscendC::Reg::StoreAlign<half, AscendC::Reg::DataCopyMode::DATA_BLOCK_COPY>(
            dstRowAddr, reg, dstNzC0Stride, mask);
    }
}
```

一拍取的 8 个 datablock 数据在 NZ 矩阵中的排布如下图所示，非尾块中原本相邻的两个 datablock 在 NZ 排布中起始地址需间隔 `144 * 32B`：

**图：Case 1 紧凑排放下 ND 到 NZ 的示意图**

<img src="figures/datand2nzS1.png" width="80%">

**冲突原理**：

按低位交织规则，地址每跳 1 个 32B 块就换一个物理 bank（16 步走完一轮），每跳 8 步走完一轮 bank group。**一拍内多个写如果都要去同一个 bank group，就只能排队**。

单条 `Reg::StoreAlign<DATA_BLOCK_COPY>` 一拍要把 8 个 32B 块写到 UB，相邻两个落点之间隔 `dstNzC0Stride = 144` 个 32B 块。**144 = 16 × 9，是 16 的整数倍**——每跳 144 步不仅回到同一个 bank group，更回到完全相同的物理 bank（bank 0）。结果是 8 个写全部挤到 bank 0，硬件只能依次处理，单条 StoreAlign 从 1 拍拉长为 **8 拍**，aiv_vec_time 显著膨胀。

**S1 落点示意图**：

```text
单条 StoreAlign 的 8 个写在 UB 上的落点：
  DataBlock 0 → 第   0 个 32B 块  → bank 0 → bank group 0
  DataBlock 1 → 第 144 个 32B 块  → bank 0 → bank group 0   ◀ 同一 bank
  DataBlock 2 → 第 288 个 32B 块  → bank 0 → bank group 0   ◀ 同一 bank
  DataBlock 3 → 第 432 个 32B 块  → bank 0 → bank group 0   ◀ 同一 bank
  DataBlock 4 → 第 576 个 32B 块  → bank 0 → bank group 0   ◀ 同一 bank
  DataBlock 5 → 第 720 个 32B 块  → bank 0 → bank group 0   ◀ 同一 bank
  DataBlock 6 → 第 864 个 32B 块  → bank 0 → bank group 0   ◀ 同一 bank
  DataBlock 7 → 第1008 个 32B 块  → bank 0 → bank group 0   ◀ 同一 bank
```

**图：Ascend 950PR&950DT系列产品 Case 1 的 UB bank group 冲突示意图**

<img src="figures/s1bank3510.png" width="80%">

**性能数据**（Ascend 950PR&950DT系列产品，当前 `144×128` 配置，64 核）：

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 141.691 | 140.89 | **45.25** | 0.321 | 2.638 | 0.019 | 134.915 | 0.958 | 104.571 | 0.742 |

**优化效果分析**：
- `aiv_vec_time = 45.25 μs`，是 Case 2 的 ~4.95 倍。
- 冲突点在 Vec 写阶段：VEC_STORE 流水内每条 StoreAlign 都因 bank-group 写写冲突排队 8 拍。

**性能瓶颈定位**：
> ⚠️ **bank 冲突在 Vec 流水**
>
> 在 Regbase 架构下，`Reg::LoadAlign` 走 **VEC_LOAD** 子流水、`Reg::StoreAlign` 走 **VEC_STORE** 流水，不在 MTE 上。常规 MTE 类 bank-conflict 计数器看不到，需要看 vec 流水或单指令周期数。

#### Case 2: nzBuf 多申请一行

**实现方式**：编译时 `-DSCENARIO_NUM=2`。

在 Case 1 紧凑排放的基础上，**将 `dstNzC0Stride` 从 `tileH` 调整为 `tileH + 1`**，相当于 `nzBuf` 多申请一行空间，让向量写落点能在不同 bank group 间轮转。

**关键代码**：

```cpp
constexpr uint32_t dstNzC0Stride = (SCENARIO == 1) ? tileH : (tileH + 1);
```

**规避原理**：

把 `dstNzC0Stride` 从 144 改成 145，相比于 Case1 在 UB 上多申请了一行空间，相邻两个写落点之间隔 145 个 32B 块。**145 = 16 × 9 + 1**，每跳一次物理 bank 索引 +1，8 个落点依次落到 8 个相邻物理 bank；又因 `bank group = bank % 8`，这 8 个 bank 同时分属 8 个不同 bank group，**8 个写正好分散至 8 个 bank group**，硬件可以并发处理，单条 StoreAlign **1 拍**完成。本实现中 nzBuf 起始位置已偏移到 bank 8（见下方"补充"小节），因此 8 个落点实际是 bank 8 ~ bank 15。

多申请的这一行空间不作为有效数据写回 GM，只用于改变 UB 内写落点的 bank 分布。

**图：Case 2 nzBuf 多申请一行后的 ND 到 NZ 示意图**

<img src="figures/datand2nzS2.png" width="80%">

**S2 落点示意图**（nzBuf 已偏移到 bank 8 起步）：

```text
单条 StoreAlign 的 8 个写在 UB 上的落点（"第 N 个 32B 块"按 nzBuf 起点算）：
  DataBlock 0 → 第   0 个 32B 块  → bank 8  → bank group 0
  DataBlock 1 → 第 145 个 32B 块  → bank 9  → bank group 1
  DataBlock 2 → 第 290 个 32B 块  → bank 10 → bank group 2
  DataBlock 3 → 第 435 个 32B 块  → bank 11 → bank group 3
  DataBlock 4 → 第 580 个 32B 块  → bank 12 → bank group 4
  DataBlock 5 → 第 725 个 32B 块  → bank 13 → bank group 5
  DataBlock 6 → 第 870 个 32B 块  → bank 14 → bank group 6
  DataBlock 7 → 第1015 个 32B 块  → bank 15 → bank group 7
```

**图：Ascend 950PR&950DT系列产品 Case 2 的 UB bank group 分散落点示意图**

<img src="figures/s2bank3510.png" width="80%">

> **补充：nzBuf 起始地址偏移到 bank 8**
>
> 950PR 上 S2 还把 nzBuf 起始地址在紧贴 ndBuf 末尾的基础上**再多偏移 8 个 datablock = 256B**，让 nzBuf 起步于 bank 8。这样一拍 V_LOAD 读 ndBuf 行 0 落在 bank 0~7，V_STORE 落在 bank 8~15，物理 bank 不相交，可降低同拍 V_LOAD / V_STORE 读写冲突（同一个Bank内读写冲突）。

**UB 占用变化**：

`ndBuf` 占用 = `tileH * tileW * sizeof(half) = 144 * 128 * 2 = 36864 B = 36 KB`

`nzBuf` DataBlock 数统计的是 UB 实际申请空间，不是写回 GM 的有效数据量。S1 按 144 行紧凑申请，为 `144 × 8 = 1152` 个 DataBlock；S2 的 `dstNzC0Stride = 145`，相当于 `nzBuf` 多申请一行空间，因此为 `145 × 8 = 1160` 个 DataBlock。

| 场景 | `ndBuf` 占用 | `nzBuf` DataBlock 数 | `nzBuf` 占用 |
|:---:|:---:|:---:|:---:|
| S1 (stride=144) | 36864 B = 36 KB | 1152 | 1152 × 32 B = 36864 B = 36 KB（紧凑） |
| S2 (stride=145) | 36864 B = 36 KB | 1160 | 1160 × 32 B = 37120 B ≈ 36.3 KB（按 145 行申请） |

多申请的这一行空间在 UB→GM 的 MTE3 阶段由 `outParams.srcStride = dstNzC0Stride - tileH` 跳过，不会写回 GM。

双缓冲后，每套 buffer 包含一个 `ndBuf` 和一个 `nzBuf`。S1 总 UB 占用为 `2 × (36864 B + 36864 B) = 147456 B = 144 KB`；S2 还为两块 `nzBuf` 各引入一段 256 B 的 bank-8 起步偏移（详见上文"补充"小节），总 UB 占用为 `2 × (36864 B + 37120 B) + 2 × 256 B = 148480 B ≈ 145 KB`，满足 Ascend 950PR&950DT系列产品 256 KB UB 的约束。

**性能数据**（Ascend 950PR&950DT系列产品，当前 `144×128` 配置，64 核）：

| Task Duration(μs) | aiv_time(μs) | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 140.403 | 139.68 | **9.14** | 0.065 | 2.765 | 0.020 | 135.115 | 0.967 | 116.921 | 0.837 |

**优化效果分析**：
- `aiv_vec_time` 从 45.25 → 9.14 μs（提升 **4.95×**）。
- 代价是每 tile 多占用 8 × 32B = 256B 的 UB 空间（约 0.7% 浪费），完全可接受。

## 理论性能分析

### Atlas A2/A3 系列产品（dav-2201）

当前实现选择 `144×128`。该配置下 S2 相比 S1 已经能体现出规避 Vec bank 冲突后的收益，端到端耗时从 `151.42μs` 降到 `144.00μs`。

| Tile | Case | dstNzC0Stride | Task Duration(μs) | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio | icache_miss_rate |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 144×128 | S1 | 144 | 151.42 | 138.00 | 12254207 | **47.790** | 0.346 | 6.586 | 0.048 | 113.856 | 0.825 | 76.760 | 0.556 | 0.007 |
| 144×128 | S2 | 145 | **144.00** | 131.44 | 11671939 | **7.133** | 0.054 | 7.760 | 0.059 | 115.405 | 0.878 | 76.532 | 0.582 | 0.005 |

Atlas A2/A3 系列产品上，单条 vcopy 每拍处理 `128` 个 half，主频按 `1.85GHz`、核数按 `48` 计算。无冲突基础耗时为：

$$
T_{\text{base}} = \frac{M \times N}{128 \times 1.85 \times 10^9 \times 48}
                = \frac{8192 \times 8192}{1.13664 \times 10^{13}}
                \approx 5.904\mu s
$$

Case 1 中 `dstNzC0Stride = 144`，8 个写落到同一个 bank group，单条向量写从 1 拍拉长到 8 拍，因此：

$$
T_{\text{S1}} = 8 \times T_{\text{base}} \approx 47.233\mu s
$$

Case 2 中 `dstNzC0Stride = 145`，8 个写均匀分散到不同 bank group，因此：

$$
T_{\text{S2}} = 1 \times T_{\text{base}} \approx 5.904\mu s
$$

Case 1 因 V 自身被拉长至 8 拍写, MTE2/V 之间的干扰会被掩盖; Case 2 V 砍到 1 拍写后, ping-pong 机制下 MTE2 写 ndPong 和 V 读 ndPing 落到同一 bank 引发的读写冲突变得可见，因此 Case2 实测 7.133μs 比理论 5.904μs 高约 21%。端到端 Task Duration 从 `151.42μs` 降到 `144.00μs`，降低约 4.9%；整体仍包含 MTE2/MTE3 搬运和同步开销，bank 冲突优化主要体现在 Vec 子流水。

### Ascend 950PR&950DT系列产品（dav-3510）

当前实现选择 `144×128`，S2 的 Vec 时间明显下降，但当前端到端主要受 MTE2/MTE3 限制，因此 Task Duration 只从 `141.691μs` 降到 `140.403μs`。

| Tile | Case | dstNzC0Stride | Task Duration(μs) | Block Num  | aiv_time(μs) | aiv_total_cycles | aiv_vec_time(μs) | aiv_vec_ratio | aiv_scalar_time(μs) | aiv_scalar_ratio | aiv_mte2_time(μs) | aiv_mte2_ratio | aiv_mte3_time(μs) | aiv_mte3_ratio | aiv_icache_miss_rate |
|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|:---:|
| 144×128 | S1 | 144 | 141.691 | 64  | 140.89 | 14125772 | **45.25** | 0.321 | 2.638 | 0.019 | 134.915 | 0.958 | 104.571 | 0.742 | 0.006 |
| 144×128 | S2 | 145 | **140.403** | 64  | 139.68 | 14094639 | **9.14** | 0.065 | 2.765 | 0.020 | 135.115 | 0.967 | 116.921 | 0.837 | 0.006 |

Ascend 950PR&950DT系列产品 上，Reg 路径每处理一组 `128` 个 half 数据，需要执行一次 `LoadAlign` 和一次 `StoreAlign`。

单条 `LoadAlign` 或 `StoreAlign` 每拍处理 `128` 个 half，跨循环的 `StoreAlign` 与 `LoadAlign` 可以落到不同物理 reg 上、互不阻塞，理想状态下每拍处理 `256 bytes/cycle`。主频按 `1.65GHz`、核数按 `64` 计算，单遍处理全量数据的基础耗时为：

$$
T_{\text{base}} = \frac{M \times N}{128 \times 1.65 \times 10^9 \times 64}
                = \frac{8192 \times 8192}{1.35168 \times 10^{13}}
                \approx 4.965\mu s
$$

实测 `aiv_vec_time` 为 `9.14μs`，与 4.965μs 理论值的差距来自两个叠加因素：(1) V_STORE stride=145 每次起始 bank +1，多轮循环后还会与V_LOAD落至一个bank，仅靠 buffer 偏移无法逐行完美错开；(2) vloop 指令双发，发射槽位不是每个 cycle 都能凑出"本轮 store + 下一轮 load"的理想组合，带来冲突。本样例主要演示如何解决 NZ 写的写写冲突，端到端 Task Duration 已被 MTE2/MTE3 主导（`aiv_mte2_ratio`高达96.7%），vec 时间不在关键路径上。

### 优化要点总结

> **核心结论**
>
> - 向量写如果总是落到同一组 bank group，就会产生写写冲突。
> - 通过多申请内存，让写的落点不在同一bank group来解冲突。

---

## 编译运行

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build   # 创建并进入build目录
  cmake -DSCENARIO_NUM=1 -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;  # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                           # 执行（使用编译时指定的case）
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU 调试 或 NPU 仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim`：

  ```bash
  cmake -DSCENARIO_NUM=1 -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;  # CPU 调试模式
  cmake -DSCENARIO_NUM=1 -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;  # NPU 仿真模式
  ```

  > **注意：** 切换编译模式或场景前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU 调试、NPU 仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |
  | `SCENARIO_NUM` | `1`（默认）、`2` | bank 冲突场景编号 |

  执行结果如下，说明精度对比成功：

  ```bash
  test pass!
  ```

## 性能分析

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
