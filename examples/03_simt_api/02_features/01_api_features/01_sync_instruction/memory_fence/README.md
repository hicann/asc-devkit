# SIMT编程实现归约求和样例

## 概述

本样例基于 Ascend C SIMT 编程方式实现一维 `float` 输入的归约求和，通过 2 个递进场景展示线程块内同步与线程块间内存顺序控制的典型用法。

2 个场景分别对应小 shape 输入、大 shape 输入下的归约求和，重点说明[`asc_syncthreads()`](../../../../../../docs/zh/api/SIMT-API/sync_and_memory_fence/sync_interface/asc_syncthreads.md)和[`asc_threadfence()`](../../../../../../docs/zh/api/SIMT-API/sync_and_memory_fence/memory_fence/asc_threadfence.md)在不同归约规模下的使用方法。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本
- \>= CANN 9.1.0

## 目录结构介绍

```text
├── memory_fence
│   ├── figures               // README中的图片资源
│   ├── sync_barrier.asc      // 场景1：小shape归约样例，展示 asc_syncthreads() 的线程块内同步用法
│   ├── memory_fence.asc      // 场景2：大shape归约样例，展示 asc_threadfence() 与原子计数的线程块间协作用法
│   ├── CMakeLists.txt        // 编译工程文件
│   ├── README.md
│   └── README_en.md      // 英文版样例说明
```

## 样例描述

### 样例功能

<table border="1" align="center">
  <tr>
    <td align="center">SCENARIO_NUM 取值</td>
    <td align="center">功能场景</td>
    <td align="center">场景说明</td>
    <td align="center">对应文件</td>
  </tr>
  <tr>
    <td align="center">1</td>
    <td align="center">小shape归约场景</td>
    <td align="center">使用 asc_syncthreads() 完成线程块内归约同步</td>
    <td align="center">sync_barrier.asc</td>
  </tr>
  <tr>
    <td align="center">2</td>
    <td align="center">大shape归约场景</td>
    <td align="center">使用 asc_threadfence() 完成多线程块间局部和的合并求和</td>
    <td align="center">memory_fence.asc</td>
  </tr>
</table>

本样例通过`SCENARIO_NUM`控制构建分支，2 个场景按输入规模大小分类：场景1演示小shape输入下的归约求和与`asc_syncthreads()`同步用法。当输入元素较少时，仅需1个block 即可覆盖全部数据，无需多block间同步；场景2扩展到大 shape 输入的多 block 分段归约，引入`asc_threadfence()`完成 block 间合并。

### 样例规格

#### SCENARIO_NUM=1（小shape归约场景）

- 样例功能：

  对 1024 个 `float` 输入元素在单个线程块内执行**归约求和**。

- 样例规格：

  <table border="1" align="center">
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">SyncBarrierSingleBlock</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[1]</td><td align="center">float</td><td align="center">ND</td></tr>
  </table>

- 数据切分：

  - gridDim：(1, 1, 1)
  - blockDim：(1024, 1, 1)
  - 单线程处理：1 个输入元素

#### SCENARIO_NUM=2（大shape归约场景）

- 样例功能：

  对 `1024 * 1024` 个 `float` 输入元素执行**分段归约求和**，每个线程块先完成本线程块负责片段的**局部求和**，再由单个线程块对所有线程块的局部和做第二次归约求和，得到最终总和。

- 样例规格：

  <table border="1" align="center">
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">ThreadFenceMultiBlock</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[1024 * 1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[1]</td><td align="center">float</td><td align="center">ND</td></tr>
  </table>

- 数据切分：

  - gridDim：(1024, 1, 1)
  - blockDim：(1024, 1, 1)
  - 单线程处理：1 个输入元素

### 样例实现

#### 1：小shape归约场景

单个线程块内的每个线程先读取一个输入元素，通过**两阶段归约求和**得到最终结果：

- **阶段 1（warp 内归约）**：调用[`asc_reduce_add()`](../../../../../../docs/zh/api/SIMT-API/Warp_functions/Warp_reduce_functions/asc_reduce_add.md)接口对当前warp内所有线程的值做归约，得到warp内数据的和，写入共享内存。
- **阶段 2（block 级顺序累加）**：跨warp同步后，0号线程按顺序将各warp的部分和累加，得到最终结果。

以 128 个元素（4 个 warp）为例，两阶段归约过程如图 1 所示：

<p align="center">
  <img src="./figures/两阶段归约求和.png" width="50%">
   </p>
<p align="center">
图1：两阶段归约过程示意图
</p>

阶段1中，各warp通过`asc_reduce_add()`完成warp内求和并将结果写入共享内存；阶段2中，0号线程依次读取各warp的部分和进行累加，得到最终结果。两个阶段之间需调用`asc_syncthreads()`同步。

`asc_syncthreads()`用于阻塞当前线程块的所有线程，直到所有线程都执行到该同步点位置。在本场景中，`asc_syncthreads()`保证所有warp的首线程已将warp内归约结果写入共享内存，后续block级顺序累加才能读取完整的共享内存数据。

#### 2：大shape归约场景

当输入元素总数较大时，需要**多个线程块**分段处理。

每个线程块先在块内完成上述两阶段局部归约，再由`tid = 0`的线程将本线程块的局部和写入`block_sums[blockIdx.x]`。写入后执行`asc_threadfence()`，再将全局计数器加 1。最后一个对计数器加 1 的线程块（即`ticket = gridDim.x - 1`的线程块）会读取`block_sums`并执行第二次两阶段归约，输出最终结果。

以 8 个线程块为例，跨线程块协作过程如图2所示：

<p align="center">
  <img src="./figures/跨线程块协作.png" width="75%">
   </p>
<p align="center">
图2：跨线程块协作过程示意图
</p>

上述流程中，多个线程块需要读写同一块全局内存`block_sums`，可能造成数据竞争。

`asc_threadfence()`作为核间内存屏障，确保调用线程在`asc_threadfence()`之前的所有全局内存和共享内存写操作对其他线程可见，且这些写操作不会被重排序到内存屏障之后。因此各线程块必须按以下顺序执行：将局部和写入`block_sums` → 递增原子计数器。这一顺序确保了最后一个线程块在读取`block_sums`执行全局归约时，一定能看到所有其他线程块已写入的部分和；若省略`asc_threadfence()`，则可能读到脏数据导致结果错误。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。
- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：**`${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  SCENARIO_NUM=1                # 取值为1、2
  mkdir -p build && cd build;   # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM ..; make -j;   # 编译工程
  ./demo                        # 执行样例
  ```

  编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT） |
  | `SCENARIO_NUM` | `1`、`2` | 场景编号：1表示小shape归约场景，2表示大shape归约场景 |

  执行结果如下，说明精度对比成功。
  ```
  [Success] Case accuracy verification passed.
  ```
