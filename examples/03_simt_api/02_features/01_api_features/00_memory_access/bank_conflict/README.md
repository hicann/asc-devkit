# SIMT UB Bank冲突样例

## 概述

本样例介绍Ascend C SIMT编程方式下同一个Warp内多个线程访问UB时的bank冲突现象。样例通过5个case构造不同的UB读地址模式，展示同地址合并、跨bank/subbank分散访问、连续float访问中的同一8B范围合并、同subbank不同行冲突，以及同一bank group内同编号subbank冲突等典型场景。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.1.0

## 目录结构介绍

```text
├── bank_conflict
│   ├── figures              // README中的图片资源
│   ├── bank_conflict.asc    // SIMT UB Bank冲突样例实现
│   ├── CMakeLists.txt       // cmake编译文件
│   ├── README.md            // 样例说明文档
│   └── README_en.md         // 样例说明文档
```

## 样例描述

### 样例功能

本样例用于验证Ascend C SIMT编程方式下同一个Warp内多个线程访问UB时的bank冲突现象。样例通过编译期参数`SCENARIO_NUM`选择不同case，构造同地址合并、跨subbank分散访问、连续float访问中的同一8B范围合并、同subbank不同行冲突，以及同一bank group内同编号subbank冲突等典型访问模式。

Host侧构造输入数组并拷贝到GM，kernel先将输入数据从GM搬入UB，再按照当前case对应的UB地址模式重复读取数据，并使用[`clock()`](../../../../../../docs/zh/api/Utils-API/tuning_interface/clock.md)统计读取UB数据的cycles。

### 样例规格

<table>
<tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">BankConflict</td></tr>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">input</td><td align="center">[4096]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">样例输出</td><td align="center">output</td><td align="center">[32]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">cycle_output</td><td align="center">[32]</td><td align="center">uint64_t</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">bank_conflict_kernel</td></tr>
</table>

## 样例实现

### UB Bank与Subbank规则

在本文中，SIMT编程模式下提到的UB bank冲突，主要指同一个Warp内多个线程在同一条UB访问指令中竞争同一个bank group内同编号subbank资源而产生的subbank冲突。

以Ascend 950PR/Ascend 950DT为例，下图为UB bank结构示意图。UB总大小为256KB，划分为8个bank group，每个bank group包含2个bank。16个bank的编号为bank0到bank15，其中bank i与bank i+8同属bank group i，即`bank_group_number = bank_number % 8`。每个bank大小为16KB，包含512行，每行32B。在SIMT编程模式下，每个bank的32B行进一步按8B粒度划分为4个subbank。

SIMT场景下，同一个Warp内的多个线程可能在同一条UB访问指令中同时访问UB；当这些访问数据属于同一个bank group的相同编号的subbank资源时，例如同时访问bank0和bank8的subbank2，硬件无法在一个周期内处理全部请求，需要排队等待，从而形成subbank冲突并增加访问延迟；如果这些访问数据属于同一个subbank的同一行内的8B地址范围，硬件会将这些请求合并处理，不会形成subbank冲突。

**图 1**  bank结构示意图
![](./figures/bank结构示意图.png)

### 地址编码规则

UB地址采用低位交织，如下图所示，连续地址按32B粒度映射到bank0到bank15：第1个32B地址段映射到bank0，第2个映射到bank1，依次类推；第16个映射到bank15，第17个映射回bank0的下一行。

**图 2**  bank内存排布示意图
![](./figures/bank内存排布示意图.png)

SIMT编程方式下subbank冲突主要有以下几种：

- **写写冲突**：多个写操作同时尝试访问同一个bank group的相同编号的subbank，例如多个线程同时写bank0中的subbank2和bank8中的subbank2。
- **读读冲突**：多个读操作同时尝试访问同一个bank group的相同编号的subbank，例如多个线程同时读bank0中的subbank2和bank8中的subbank2。

SIMT编程模式下同一个Warp内的线程同一时刻执行的是相同指令，无读写冲突场景，本样例聚焦读读冲突，通过多个线程在同一条UB读指令中访问不同UB地址模式，观察可合并访问、无冲突访问和subbank冲突访问的cycles差异。

### Case实现说明

本样例使用`float`类型数组作为UB访问对象，每个线程根据自己的`thread_id`计算一个UB读索引。每个case只是在`get_ub_index_device()`中改变`ub_index`的计算方式，使32个线程在同一条UB读指令中形成不同的访问分布：完全相同地址、按8B粒度分散到不同subbank、连续地址、按512B访问间隔回到同一bank同一subbank的不同行，或按256B访问间隔落到同一bank group的同编号subbank资源。

通过这种方式，样例可以在数据类型和线程组织保持不变的前提下，只对比UB地址模式变化带来的访问合并和subbank冲突差异。

kernel先将输入数据从GM搬入`__ubuf__`数组`ub`，再通过`threadIdx.x`获取当前线程编号，并调用`get_ub_index_device()`得到当前case对应的UB读索引。`ub_ptr`指向同一块UB数组，增加`volatile`修饰保证循环中的每次读取都保留为实际UB读访问，避免编译器将重复读取优化成一次读取。

```cpp
for (uint32_t i = tid; i < UB_ELEMENT_COUNT; i += blockDim.x) {
    ub[i] = input[i];
}

uint32_t ub_index = get_ub_index_device(tid);
__ubuf__ volatile float* ub_ptr;
ub_ptr = ub;

#pragma unroll
for (uint32_t i = 0; i < REPEAT_TIMES; ++i) {
    acc += ub_ptr[ub_index];
}
```

**表 1**  Case实现说明

| Case   | 场景描述                            | 预期访问特征                                                       |
| ------ | ----------------------------------- | ------------------------------------------------------------------ |
| Case 0 | 同一Warp的线程读同一个地址          | 同地址请求合并                                                     |
| Case 1 | 跨subbank访问                       | 32个线程访问8个bank group的不同subbank，作为无冲突基线             |
| Case 2 | 同一Warp的线程连续访问相邻float元素 | 相邻两个线程访问同一8B范围，硬件可合并处理                         |
| Case 3 | 访问同一bank的同subbank不同行       | 32个线程访问同一bank、同一subbank的不同行，形成subbank冲突         |
| Case 4 | 访问同bank group的同编号subbank     | 32个线程访问同一bank group内相同编号的两个subbank，形成subbank冲突 |

### 数据切分与线程块布局

本样例的kernel入口使用`__global__`修饰，并通过`__launch_bounds__`指定最大线程数。样例固定启动1个Thread Block，每个Thread Block包含32个线程，对应同一个Warp。样例不做跨block切分，而是让同一个Warp内的32个线程构造不同UB访问模式，用于观察同一条UB访问指令下的访问合并和subbank冲突差异。

- `gridDim`：`(1, 1, 1)`。
- `blockDim`：`(32, 1, 1)`。
- `threadIdx.x`范围为0-31，对应同一个Warp内的32个线程。
- 每个线程先按`threadIdx.x + n * blockDim.x`的索引模式参与GM到UB搬运，其中`n`表示搬运循环轮次，使`input[4096]`完整搬入UB数组。
- 当前样例只启动32个线程，这32个线程位于同一个Warp内，GM到UB搬运结束后直接根据自己的`thread_id`和当前case计算UB读索引，并重复读取`REPEAT_TIMES`次。
- Host侧校验32个线程的累加输出应与CPU计算结果一致；不同case的cycles可用于对比UB访问模式带来的开销差异。

### 访问打点

由于subbank冲突属于更低级别的存储冲突，不被算子调优工具msOpProf所捕获，因此本样例使用`clock()`对访问UB的代码打点，将循环起始和结束时刻相减得到当前线程读取UB数据所消耗的cycles，并写入`cycle_output`返回Host侧。打点代码如下：

```cpp
uint64_t start = clock();
// 重复UB读循环。
uint64_t end = clock();
cycle_output[threadIdx.x] = end - start;
```

### 性能指标说明

kernel会将32个线程各自统计到的cycles写入`cycle_output[32]`。Host侧按线程顺序打印32个线程的cycles，对比不同case之间的访问开销。

输出格式如下：

```text
Cycles per thread: <thread0> <thread1> ... <thread31>
```

### Case 0：同一Warp的线程读同一个地址

**样例目标**：展示同一个Warp内多个线程读取完全相同UB地址时，硬件可以合并同地址请求。

**核心实现**：

```cpp
__aicore__ inline uint32_t get_ub_index_device(uint32_t thread_id)
{
...
#if SCENARIO_NUM == 0
    // 所有线程读取完全相同的UB地址。
    return 0;
...
}
```

32个线程都读取`ub[0]`。这些访问的起始地址完全相同，属于同地址读场景，硬件可合并处理，不产生subbank冲突。访问示意图如下：

![](./figures/case0.png)

**性能数据**：

```text
Cycles per thread: 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078 1078
```

**分析**：

Case 0中32个线程的读请求指向同一个UB地址，硬件可合并处理这些请求，因此不会产生subbank资源竞争。

### Case 1：跨bank/subbank分散访问

**样例目标**：构造无subbank冲突的UB访问基线。

**核心实现**：

```cpp
__aicore__ inline uint32_t get_ub_index_device(uint32_t thread_id)
{
...
#elif SCENARIO_NUM == 1
    // 8B访问间隔使32个线程分散到8个bank group的4个subbank。
    return thread_id * 2;
...
}
```

32个线程的访问覆盖连续256B空间，并按照8B粒度分散到8个bank group的4个subbank上。同一条UB访问指令下，每个bank group的每个subbank只被一个线程访问，因此该case可作为无冲突访问基线。访问示意图如下：

![](./figures/case1.png)

**性能数据**：

```text
Cycles per thread: 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107 1107
```

**分析**：

Case 1中32个线程按8B粒度分散访问8个bank group的4个subbank，同一条UB访问指令下每个bank group的每个subbank只被一个线程访问，不形成subbank资源竞争。该case作为无冲突基线，用于和Case 2、Case 3、Case 4对比。

### Case 2：连续float访问中的同一8B范围合并读

**样例目标**：32个线程访问32个不同地址。Case 2连续访问相邻float元素，相邻两个线程访问同一8B范围，硬件可合并处理。

**核心实现**：

```cpp
__aicore__ inline uint32_t get_ub_index_device(uint32_t thread_id)
{
...
#elif SCENARIO_NUM == 2
    // 连续float访问使相邻两个线程落在同一8B subbank范围内。
    return thread_id;
...
}
```

该case中32个线程分别读取`ub[0]`到`ub[31]`。对float类型而言，`ub[0]`和`ub[1]`位于同一个subbank的同一行8B范围内，`ub[2]`和`ub[3]`位于下一个subbank同一行8B范围内，依次类推。相邻两个线程的访问可按同一8B范围合并处理。访问示意图如下：

![](./figures/case2.png)

**性能数据**：

```text
Cycles per thread: 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081 1081
```

**分析**：

Case 2中32个线程访问连续`float`元素，相邻两个线程访问同一8B范围，可按同一8B范围合并处理。与Case 1相比，二者都访问32个不同地址，但Case 2存在相邻线程同一8B范围合并，cycles略低于Case 1。

### Case 3：同一bank的同subbank不同行冲突

**样例目标**：构造32个线程分别访问不同地址，但全部集中到同一subbank的不同行。

**核心实现**：

```cpp
__aicore__ inline uint32_t get_ub_index_device(uint32_t thread_id)
{
...
#elif SCENARIO_NUM == 3
    // 512B访问间隔使地址回到同一bank和同一subbank的不同行。
    return thread_id * 128;
...
}
```

由于UB低位交织规则，512B访问间隔会使地址回到相同bank group、相同bank和相同subbank，但depth行号递增。因此，同一个Warp内32个线程会同时访问同一bank的同编号subbank的不同行，形成subbank冲突。访问示意图如下：

![](./figures/case3.png)

**性能数据**：

```text
Cycles per thread: 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760
```

**分析**：

Case 3中32个线程访问同一bank、同一subbank的不同行，会竞争同一subbank资源。与Case 1的无冲突基线相比，cycles明显升高，说明该访问模式形成subbank冲突。

### Case 4：同bank group的同编号subbank冲突

**样例目标**：构造32个线程分别访问不同地址，但全部集中到同一个bank group的同编号subbank资源。

**核心实现**：

```cpp
__aicore__ inline uint32_t get_ub_index_device(uint32_t thread_id)
{
...
#elif SCENARIO_NUM == 4
    // 256B访问间隔切换bank group内的bank编号，但保持subbank编号不变。
    return thread_id * 64;
...
}
```

`thread_id * 64`表示相邻线程间隔64个`float`元素，即256B。256B访问间隔会切换bank group内的bank编号，但bank group编号和subbank编号保持一致。因此，同一个Warp内32个线程会集中访问同一个bank group的同编号subbank资源，形成subbank冲突。访问示意图如下：

![](./figures/case4.png)

**性能数据**：

```text
Cycles per thread: 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760 3760
```

**分析**：

Case 4中32个线程集中访问同一bank group内的同编号subbank资源，与Case 1相比，cycles明显升高，说明访问同bank group内同编号subbank也会产生subbank资源竞争。

### 调优建议

Case 0和Case 2用于说明可合并访问不会形成subbank冲突，Case 1作为无冲突基线；Case 3和Case 4的cycles明显升高，说明同一个Warp内集中访问同一个bank group的同编号subbank会产生subbank资源竞争。

编写SIMT代码时，应尽量避免同一个Warp内多个线程在同一条UB访问指令中集中访问同一个bank group的同编号subbank；如果访问模式不可避免，可通过调整数据布局、访问步长或在UB中增加padding等方式分散subbank访问，避免访问冲突的优化样例可参考[矩阵转置样例](../../../../03_best_practices/00_memory_optimizations/matrix_transpose_practice)。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```
  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。
  >
- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  SCENARIO_NUM=0                       # 选择执行场景，可选0-4
  mkdir -p build && cd build;          # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..; make -j;  # 编译工程
  ./demo                               # 执行样例
  ```
- 编译选项说明

  | 选项                        | 可选值       | 说明                                                       |
  | --------------------------- | ------------ | ---------------------------------------------------------- |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构：本样例仅支持dav-3510（Ascend 950PR/Ascend 950DT） |
  | `SCENARIO_NUM`            | `0`-`4`  | 样例类型，默认为0                                          |

  执行结果如下，说明精度对比成功。


  ```text
  [Success] Case accuracy verification passed.
  ```
