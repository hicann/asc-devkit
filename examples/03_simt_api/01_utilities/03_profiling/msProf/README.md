# 基于Gather算子的profiling样例


## 概述

本样例基于Gather算子，演示了通过msOpProf采集上板的性能数据的方法。用户可根据输出的性能数据，快速定位算子的软、硬件性能瓶颈，提升算子性能的分析效率。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本
- \>= CANN 9.0.0

## 目录结构介绍

```text
├── msProf
│   ├── CMakeLists.txt         // cmake编译文件
│   ├── gather.asc             // SIMT实现gather调用样例
│   └── README.md
```

## 算子描述
  gather算子实现了从shape为[100000,128]的二维向量中获取指定索引的12288行数据的功能，具体功能描述可参考[Gather算子详情](../../../00_introduction/01_gather/basic_gather/gather_2d/README.md)章节。

## msOpProf工具介绍
msOpProf工具是单算子性能分析工具。包含msopprof和msopprof simulator两种使用方式。该工具协助用户定位算子内存、算子代码以及算子指令的异常，实现全方位的算子调优。当前支持基于不同运行模式（上板或仿真）和不同文件形式（可执行文件或算子二进制.o文件）进行性能数据的采集和自动解析。

- 上板性能采集

    通过上板性能采集，可以直接测定算子在昇腾AI处理器上的运行时间。该方式适合在板环境中快速定位算子性能问题。

    基于可执行文件demo通过msopprof执行算子调优：
    ```
    msopprof ./demo
    ```

    - 性能数据说明


      命令完成后，会在默认目录下生成以“OPPROF_{timestamp}_XXX”命名的文件夹，性能数据文件夹结构示例如下：

      ```bash
      ├──dump                       # 原始的性能数据，用户无需关注
      ├──ArithmeticUtilization.csv  # cube/vector指令cycle占比
      ├──L2Cache.csv                # L2 Cache命中率
      ├──Memory.csv                 # UB，L1和主存储器读写带宽速率
      ├──MemoryL0.csv               # L0A，L0B，和L0C读写带宽速率
      ├──MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
      ├──OpBasicInfo.csv            # 算子基础信息
      ├──PipeUtilization.csv        # 采集计算单元和搬运单元耗时和占比
      ├──ResourceConflictRatio.csv  # UB上的 bank group、bank conflict和资源冲突率在所有指令中的占比
      └──visualize_data.bin         # MindStudio Insight呈现文件
      ```

用户可以使用 MindStudio Insight 打开 `visualize_data.bin` 文件可视化查看算子信息，包含算子基础信息、核间负载分析、计算工作负载分析、内存负载分析等。更多msOpProf工具使用方式请参考[《算子开发工具》](https://www.hiascend.com/document/redirect/CannCommercialToolOpDev)中“算子调优 msOpProf”的内容。


## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。
- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  mkdir -p build && cd build;           # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;   # 编译工程
  msopprof ./demo                      # 基于可执行文件demo通过msopprof执行算子调优
  ```

  编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT） |

  执行结果如下，说明精度对比成功。
  ```
  [Success] Case accuracy is verification passed.
  ```
