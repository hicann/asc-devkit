# SIMT编程实现确定性计算样例

## 概述

本样例以一维 `float` 输入的归约求和为例，展示Ascend C SIMT编程场景的不确定计算问题与常用解决方案。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|---|---|
| Ascend 950PR&950DT系列产品 | \>= CANN 9.2.0 |

## 目录结构介绍

```text
├── deterministic_reduce
│   ├── deterministic_reduce.asc  // 三Case归约样例：原子不确定累加、确定性折半归并树与确定性蝶形归并树
│   ├── figures                   // README中的图片资源
│   ├── CMakeLists.txt            // 编译工程文件
│   ├── README.md
│   └── README_en.md              // 英文版样例说明
```

## SIMT场景的不确定性计算

### 不确定性计算根因

**根源一：原子操作不保证执行顺序**

原子操作保证的是**单次读-改-写过程不可被打断**，但不保证多个线程到达该原子操作的先后顺序。多个线程对同一地址发起原子操作时，请求只能依次执行，不能并行完成；至于哪个线程的请求先执行、哪个后执行，由硬件调度决定，软件无法指定。

**根源二：浮点加法不满足严格结合律**

对整数计数类原子累加，加法满足结合律，无论顺序如何最终计数值都相同，结果是确定的。

浮点则不同：由于浮点加法不满足严格结合律，`((a + b) + c) + d`与`(a + b) + (c + d)`可能得到不同结果。每次浮点加法都涉及有限精度表示下的舍入操作，不同累加顺序产生不同的中间舍入，进而影响最终结果的低位。

**两者叠加：不可控顺序作用在顺序敏感的运算上**

两个根源叠加后，多线程浮点原子累加的最终结果既依赖参与的数据，也依赖**本轮运行中各线程原子请求的实际执行顺序**。执行顺序随线程调度变化，因此：

- **同一次运行内**，最终结果的低位舍入路径由当时的调度顺序决定。
- **多次运行之间**，若调度顺序发生变化，低位结果可能随之漂移。

### 确定性计算的常见方法

SIMT场景下获得确定性计算，业界通用的思路要么让运算顺序与调度无关，要么让运算本身对顺序不敏感，常见方法如下：

- **固定顺序的分层归并树**：通过Warp能力、UB中间临时计算结果进行分层归并计算，每一层的配对顺序软件可见，不用原子浮点累加，不用执行顺序依赖调度的任何原语。

- **换用对顺序不敏感的运算或表示**：以更高精度的数据类型去承载中间计算结果，使各路径的舍入差被吸收到最终精度之外。

下文将以归约求和算子为例，展示Ascend C SIMT编程场景的不确定计算的解决方案。

## 样例描述

### 样例功能

对一维 `float` 输入进行累加计算，输出累加值。

### 样例规格

  <table border="1" align="center">
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">DeterministicReduce</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[8192]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[1]</td><td align="center">float</td><td align="center">ND</td></tr>
  </table>

- 数据切分：

  - gridDim：(4, 1, 1)
  - blockDim：(2048, 1, 1)
  - 单线程处理：1个输入元素

## 样例实现

### 精度比对标准

Host侧的golden使用最简单的顺序累加，每个Case运行10轮，逐轮记录结果相对golden的ULP误差。ULP（Unit in the Last Place）指最低有效位上的距离：两个同号float的位模式按整型相减即为ULP误差。


### 场景1：原子不确定累加场景

**核心实现**：每个线程读取自己的输入元素，全部8192个线程对同一GM float地址执行`asc_atomic_add`：

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void kernel_atomic_accumulate(const float* input, float* output)
{
    uint32_t idx = blockIdx.x * blockDim.x + threadIdx.x;
    asc_atomic_add(output, input[idx]);
}
```

以下仅以4个输入为例展示简单的原子累加过程：同一份输入两次运行的实际调度顺序不同，累加顺序随之不同，结果A与结果B可能不相等。

<img src="./figures/deterministic_case0_atomic.png">

#### 精度比对分析

Host侧回读每轮最终值并打印其二进制表示（bits）与相对顺序golden的ULP误差，与同Case其他轮次比对展示漂移分布。

- **精度不确定性**：原子累加的加法顺序由调度决定，ULP误差随轮次漂移，10轮里可能出现多个不同值。

当前4×2048配置下的一次10轮实测如下。golden的顺序累加结果为0x503ebc20（12800000000.0）：

| 计算结果（位模式） | 计算结果（十进制） | 与golden的ULP误差 | 10轮中出现次数 |
| --- | --- | --- | --- |
| 0x503ebc1e | 12799997952.0 | -2 | 1 |
| 0x503ebc1a | 12799993856.0 | -6 | 3 |
| 0x503ebc1b | 12799994880.0 | -5 | 2 |
| 0x503ebc1c | 12799995904.0 | -4 | 4 |

本次运行的ULP误差在-2至-6之间漂移，证明浮点数原子累加结果仍然受执行顺序影响。

### 场景2：折半树实现确定性归约

折半规约（halving reduction）是业界最通用的Block内归约写法：配置一块UB暂存区，参与归约的N个线程先把各自元素写入暂存区，N个线程分前后各一半，每轮前半部分的线程把后半部分线程的数据累加到各自元素位置，随后活跃线程数折半继续累加，log2(N)轮后得到全部N个值之和。

以下是以16个示意值为例，展示折半树计算的大致过程：

<img src="./figures/deterministic_case1_halving.png">

核心代码实现如下：

```cpp
smem[tid] = input[blockIdx.x * blockDim.x + tid];
asc_syncthreads();
for (uint32_t stride = blockDim.x / 2; stride > 0; stride /= 2) {
    if (tid < stride) {
        smem[tid] += smem[tid + stride];
    }
    asc_syncthreads();
}
if (tid == 0) {
    block_sums[blockIdx.x] = smem[0];
}
```

每轮“写回后同步、读取再同步”，保证一轮内全组先写入、后读取，读写不相交。最终每个Block的计算结果写入GM暂存区。

折半完成后进行Block间固定序合并：每个Block仅由0号线程写入本Block和。最后完成的Block由0号线程按顺序累加4个Block和并写回最终结果。

折半树计算固定了元素加法配对顺序，计算过程直观，是生态中最常见的归约写法，但每轮只有前半线程活跃，且每轮计算都要在UB暂存区读写数据，性能并非最优。下面将介绍使用Warp原语能力实现更高效的确定性归并计算。

#### 精度比对分析

每轮计算结果与Host侧顺序golden的ULP误差，结果如下：

| 观测项 | 实测值 |
| --- | --- |
| 全部10轮结果位模式 | 0x503ebc20（12800000000.0），计算结果恒定 |
| 与Golden的ULP误差 | +0（恒定） |

数据分析：

- **计算结果恒定，无不确定性**：折半树10轮ULP误差恒为定值，说明配对顺序由软件写死，与运行调度完全无关。

- **与Golden的ULP误差**：但是这里相对Host的Golden误差为0，属巧合，因为两者计算顺序不一样。由于Host的Golden实现是顺序累加，在某些输入场景，可能会出现折半树计算结果与Golden恒定非0的误差，均属于正常现象。

### 场景3：三层归并树实现确定性归约

先看整体流程：8192个输入由4个Block（每个2048个线程）分别归约出各自的Block和s0~s3，L1在warp内做蝶形归约，L2在Block内做二级蝶形，L3由末完成Block固定序合并得到最终结果，如下图所示。

<img src="./figures/deterministic_case2_struct.png">

归并树分为以下三层：

- **L1 warp内蝶形归约**：每个warp执行5轮`asc_shfl_down`，delta从16逐步减半，每轮lane与lane+delta相加，lane 0最终得到本warp的32个值之和。warp内线程天然共步，无需额外同步，代码如下所示：

  ```cpp
  __aicore__ inline float warp_butterfly_reduce(float val)
  {
      val += asc_shfl_down(val, 16, WARP_SIZE);
      val += asc_shfl_down(val, 8, WARP_SIZE);
      val += asc_shfl_down(val, 4, WARP_SIZE);
      val += asc_shfl_down(val, 2, WARP_SIZE);
      val += asc_shfl_down(val, 1, WARP_SIZE);
      return val;
  }
  ```

  蝶形树计算过程示意如下：

  <img src="./figures/deterministic_case2_butterfly.png">

- **L2 Block内二级蝶形**：配置一块UB暂存区存放每个Block的Warp计算和。2048线程对应64个Warp，先由两个Warp分别对两组32个部分和做蝶形，再由Warp 0合并两组结果，得到Block计算和并写入GM暂存区。
- **L3 固定序合并Block和**：最后一个完成的Block由0号线程将多个Block和按序累加，得到最终结果。

#### 精度比对分析

每轮计算结果相对顺序golden的ULP误差如下表所示：

| 观测项 | 实测值 |
| --- | --- |
| 全部10轮结果位模式 | 0x503ebc20（12800000000.0），计算结果恒定 |
| ULP vs golden | +0（恒定） |
| 相对误差 | 0（恒定） |

数据分析：

- **计算结果恒定，无不确定性**：蝶形树10轮ULP误差恒为定值，说明配对顺序由软件写死，与运行调度完全无关。

- **与Golden的ULP误差**：蝶形树相对Host的Golden误差为0，属巧合，因为两者计算顺序不一样。

### 确定性计算总结

上述Case 0-2说明了浮点数累加等计算场景，若使用atomic类API进行计算，可能出现多次运行计算结果不一致的问题，若需要保证确定性计算，则需要由用户实现代码来固定计算顺序。

不同的确定性计算算法，在易用性和性能表现上有差异，上述Case 1折半树和Case 2蝶形树中，处理的数据量一致，端到端耗时如下：

| Case | 核函数 | Task Duration(μs) |
| ---- | ---- | :----------------: |
| 1 | `kernel_halving_reduce` | 5.483 |
| 2 | `kernel_deterministic_reduce` | 3.482 |

蝶形树相对折半树性能更佳，两棵树完成的是同一份计算，差距主要来自数据交换路径与同步次数：

- **数据交换路径**：蝶形树L1的warp内32个元素与L2的Block内64个warp部分和的数据交换全部通过 `asc_shfl_down` 在寄存器间完成，全Block只有64个warp部分和经UB中转一次；折半树的2048个元素全部驻留动态UB暂存区，每轮活跃线程都要完成一次“读UB → 相加 → 写回UB”的往返。
- **同步次数**：折半树为保证每轮“先全组写入、再全组读取”，每Block执行11次；当前64 Warp蝶形实现需要2次Block级同步。

两棵树都满足逐bit确定性，选型时可按需取舍：折半树不依赖warp交换原语、概念直观；蝶形树寄存器交换、同步更少，对时延敏感的归约场景推荐使用。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：**`${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  mkdir build && cd build;                                      # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;          # 编译工程
  ./deterministic_reduce 0                                      # 执行场景1：原子不确定累加
  ./deterministic_reduce 1                                      # 执行场景2：确定性归约（折半树）
  ./deterministic_reduce 2                                      # 执行场景3：确定性归约（蝶形树）
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

  编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构：本样例仅支持dav-3510（Ascend 950PR&950DT系列产品） |
  | 运行入参 | `0`、`1`、`2` | 场景编号：0表示原子不确定累加场景，1表示确定性归约场景（折半树），2表示确定性归约场景（蝶形树） |

  - 场景1执行结果如下，ULP误差不固定，说明不确定性计算验证成功。若某次运行ULP误差恰好一致，只是该次运行的底层调度恰好掩盖了舍入差异，属偶现的合法输出。

    ```text
    Input: 8192 floats (fixed formula), 10 rounds, golden (sequential sum) = 12800000000.000000 (bits: 0x503ebc20)
    Case 0 (atomic accumulation, order decided by scheduling): 4 blocks x 2048 threads, 10 rounds, input = 8192 floats (fixed formula)

    ===== Case 0: atomic accumulation summary (10 rounds) =====
    Round  0: 12799997952.000000 (bits: 0x503ebc1e) ULP vs golden: -2
    Round  1: 12799993856.000000 (bits: 0x503ebc1a) ULP vs golden: -6
    Round  2: 12799993856.000000 (bits: 0x503ebc1a) ULP vs golden: -6
    Round  3: 12799994880.000000 (bits: 0x503ebc1b) ULP vs golden: -5
    Round  4: 12799993856.000000 (bits: 0x503ebc1a) ULP vs golden: -6
    Round  5: 12799995904.000000 (bits: 0x503ebc1c) ULP vs golden: -4
    Round  6: 12799995904.000000 (bits: 0x503ebc1c) ULP vs golden: -4
    Round  7: 12799994880.000000 (bits: 0x503ebc1b) ULP vs golden: -5
    Round  8: 12799995904.000000 (bits: 0x503ebc1c) ULP vs golden: -4
    Round  9: 12799995904.000000 (bits: 0x503ebc1c) ULP vs golden: -4
    Rounds bit-identical to round 0: 0/9
    Golden (sequential sum): 12800000000.000000 (bits: 0x503ebc20)
    Note: 9/9 rounds differ from round 0 due to non-deterministic atomic accumulation order (ULP errors vary across rounds).
    ```

  - 场景2执行结果如下，ULP误差为恒定值，确定性验证成功。

    ```text
    Input: 8192 floats (fixed formula), 10 rounds, golden (sequential sum) = 12800000000.000000 (bits: 0x503ebc20)
    Case 1 (deterministic reduction, fixed-order block halving): 4 blocks x 2048 threads, 10 rounds, input = 8192 floats (fixed formula)

    ===== Case 1: deterministic reduction (halving) summary (10 rounds) =====
    Round  0: 12800000000.000000 (bits: 0x503ebc20) ULP vs golden: +0
    ...
    Round  9: 12800000000.000000 (bits: 0x503ebc20) ULP vs golden: +0
    Rounds bit-identical to round 0: 9/9
    Golden (sequential sum): 12800000000.000000 (bits: 0x503ebc20)
    [Success] Deterministic path: all 10 rounds bit-identical; ULP error vs sequential golden is constant (+0), a fixed algorithm-order deviation without non-determinism.
    ```

  - 场景3执行结果如下，ULP误差为恒定值，确定性验证成功。

    ```text
    Input: 8192 floats (fixed formula), 10 rounds, golden (sequential sum) = 12800000000.000000 (bits: 0x503ebc20)
    Case 2 (deterministic reduction, fixed-order 3-layer butterfly tree): 4 blocks x 2048 threads, 10 rounds, input = 8192 floats (fixed formula)

    ===== Case 2: deterministic reduction (butterfly) summary (10 rounds) =====
    Round  0: 12800000000.000000 (bits: 0x503ebc20) ULP vs golden: +0
    ...
    Round  9: 12800000000.000000 (bits: 0x503ebc20) ULP vs golden: +0
    Rounds bit-identical to round 0: 9/9
    Golden (sequential sum): 12800000000.000000 (bits: 0x503ebc20)
    [Success] Deterministic path: all 10 rounds bit-identical; ULP error vs sequential golden is constant (+0), a fixed algorithm-order deviation without non-determinism.
    ```
