# SIMT编程模式实现Histogram样例

## 概述

本样例展示了如何利用 Ascend C 的SIMT的[`asc_atomic_add()`](../../../../../../docs/zh/api/SIMT-API/atomic_operations/asc_atomic_add.md)接口，高效统计输入字节序列中每个字节值的出现频率。功能示意如下图：<br>
<img src="figures/introduction.png" alt="intro" style="width: 50%; height: auto;">

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.1.0

## 目录结构介绍

```text
├── histogram
│   ├── figures                // README中的图片资源
│   ├── CMakeLists.txt         // 样例构建脚本
│   ├── histogram.asc          // Histogram样例功能实现
│   └── README.md
```

## 样例描述

- 样例功能：  
  Histogram样例对固定大小的输入字节流进行统计，统计各个字节数值（0-255）出现的频率。输入数据的shape为[294912]。

- 样例规格：
  <table>
  <tr><td align="center">样例类型(OpType)</td><td colspan="4" align="center">histogram</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[294912]</td><td align="center">uint8</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td></tr>
  <tr><td align="center">actual_histogram</td><td align="center">[256]</td><td align="center">uint32</td><td align="center">ND</td></tr>
  </table>

- 数据切分：
  * 第一阶段Kernel `accumulate_block_local_histogram`：
    * ThreadBlock数：采用最大物理核数`72`。
    * 每个ThreadBlock线程数：采用默认值1024。
    * 单线程处理：按grid-stride遍历`uint32`输入，每次解析4个字节并更新所属Warp的局部直方图。
  * 第二阶段Kernel `merge_block_local_histogram`：
    * ThreadBlock数：`256`，每个ThreadBlock负责一个bin（桶），这里的桶代表直方图数组中单个计数单元。
    * 每个ThreadBlock线程数：为了使用规约算法，采用高于72的第一个2的n次幂即`128`。
    * 单线程处理：每个线程读取若干份局部直方图中同一个桶的计数并累加，再在ThreadBlock内归约得到最终计数。
  * 结果预期：NPU输出的256个桶计数应与CPU基于同一输入计算得到的结果一致。

## 样例实现
整体流程分为两个 Kernel。第一阶段先将输入切分到ThreadBlock中处理，每个ThreadBlock生成一份局部直方图；第二阶段每个ThreadBlock按桶合并所有局部直方图，得到最终256桶的结果。

### 阶段一

整体流程：从GM中获取输入，然后将输入数据切分分配给对应的ThreadBlock，每个ThreadBlock会继续对输入数据进行切分，并分配给对应的Warp。每个Warp都会负责计算一个局部直方图，
Warp对局部直方图中每个桶的计数是在UB中通过`asc_atomic_add`进行原子累加得到的。最后对每个Warp计算得到的局部直方图进行合并，得到当前ThreadBlock的局部直方图，并将结果写回GM。流程图如下：

<img src="figures/local_his.png" alt="local_his" style="width: 50%; height: auto;">

说明：
  1. 由于UB读写延迟更低，所以直方图计数的写操作在UB上进行。
  2. UB的最大可用空间为216KB，当前启动的核数为72核，每个核采用默认1024个线程的配置（即32个Warp）。由于每个Warp需要占用1KB空间，32个Warp共需32KB，可以一次性放置在UB中。
  3. 如果一个ThreadBlock中的Warp都维护一个局部直方图，会导致严重的线程冲突，影响性能，所以每个Warp都会维护一份局部直方图，当前Warp内的线程只会针对对应的局部直方图进行竞争写入，Warp内的线程按grid-stride读取输入并通过`asc_atomic_add`将每个字节对应桶的计数加1，然后在ThreadBlock内合并所有Warp的局部直方图。 其中`asc_atomic_add`用于对同一Warp的局部直方图中的桶计数做原子累加，保证多个线程同时命中同一个bin时更新结果正确。<br>

### 阶段二

整体流程：从GM中获取到局部直方图的数据，每个ThreadBlock负责对所有的局部直方图中的一个桶进行求和，每个线程会将局部直方图中对应桶的计数写入UB中，然后在UB中完成归约求和，最后结果写入GM。
流程图如下：下图以Block0为例，Block0负责计算索引为0的桶的计数，即bin[0]。该Block中的每个线程将对应局部直方图的bin[0]，写入UB。例如，线程0会把局部直方图0中bin[0]写入到UB的索引为0的位置，线程1会把局部直方图1中bin[0]写入到UB的索引为1的位置，以此类推。最后UB中存储每个局部直方图的bin[0]的计数，然后对UB上的数据进行规约求和，得到总的bin[0]，最后将结果写回GM。

<img src="figures/merge_his.png" alt="merge_his" style="width: 50%; height: auto;">

## 编译运行
在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  mkdir -p build && cd build      # 创建并进入 build 目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..   # 配置工程
  make -j                         # 编译样例
  ./histogram                     # 执行样例
  ```

  编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT） |

  执行结果如下，说明精度对比成功。
  
  ```text
  Running histogram256 on Ascend C SIMT for fixed xxx bytes
  Validation passed
  ```
