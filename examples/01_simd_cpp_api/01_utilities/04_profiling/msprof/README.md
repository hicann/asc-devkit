# 基于Add的msProf profiling样例说明

## 概述

本样例以Add计算为载体，演示通过`msopprof`采集上板性能数据的方法。样例在Host侧直接构造输入数据和golden数据，执行Ascend C核函数后在进程内完成结果校验，不依赖额外的数据生成或校验脚本。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── msprof
│   ├── CMakeLists.txt       // 编译工程文件
│   ├── msprof.asc           // Ascend C样例实现和Host侧调用样例
│   └── README.md            // 样例说明
```

## 样例描述

- Add计算介绍：

  本样例默认使用`float`类型，完成两个Tensor逐元素相加：

  $$
  z = x + y
  $$

输入`x`、`y`和输出`z`的Shape均为`[8, 2048]`。Host侧直接构造输入数据和golden数据，将输入数据拷贝到Device侧后启动`add_custom` Kernel，Kernel按`blockNum = 8`切分数据并完成Add计算，最后将结果拷贝回Host侧完成精度对比。

## msOpProf工具介绍

msOpProf工具是单算子性能分析工具，包含`msopprof`和`msopprof simulator`两种使用方式。该工具可采集算子执行时间、流水占比、存储访问等性能数据，辅助分析算子性能瓶颈。

- 上板性能采集

  通过上板性能采集，可以直接测定算子在昇腾AI处理器上的运行时间。基于可执行文件`demo`执行如下命令：

  ```bash
  msopprof ./demo
  ```

  命令完成后，会在默认目录下生成以`OPPROF_{timestamp}_XXX`命名的文件夹，性能数据文件夹结构示例如下：

  ```bash
  ├── dump                       # 原始性能数据
  ├── ArithmeticUtilization.csv  # cube/vector指令cycle占比
  ├── L2Cache.csv                # L2 Cache命中率，影响MTE2，建议合理规划数据搬运逻辑，增加命中率
  ├── Memory.csv                 # UB、L1和主存储器读写带宽速率
  ├── MemoryL0.csv               # L0A、L0B和L0C读写带宽速率
  ├── MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
  ├── OpBasicInfo.csv            # 算子基础信息
  ├── PipeUtilization.csv        # 计算单元和搬运单元耗时及占比
  ├── ResourceConflictRatio.csv  # UB bank group、bank conflict和资源冲突率占比
  └── visualize_data.bin         # MindStudio Insight呈现文件
  ```

更多msOpProf工具使用方式请参考[MindStudio工具](https://www.hiascend.com/document/redirect/CannCommercialToolOpDev)算子调优（msOpProf）中的内容。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  > 💡 使用 msProf 工具需安装 CANN 商用/社区版，详细信息可参考[msOpProf工具安装指南](https://www.hiascend.com/document/detail/zh/canncommercial/900/devaids/optool/docs/zh/install_guide/msopprof_install_guide.md)。


  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build;           # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;   # 编译工程
  msopprof ./demo                      # 基于可执行文件demo通过msopprof执行性能调优
  ```

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU架构：`dav-2201`对应Atlas A2系列产品和Atlas A3系列产品，`dav-3510`对应Ascend 950PR&950DT系列产品 |

- 执行结果

  精度对比成功时，输出如下：

  ```bash
  test pass!
  ```

  `msopprof`执行完成后，会在当前目录下生成以`OPPROF_{timestamp}_XXX`命名的性能数据目录，目录中包含执行时间、流水占比、存储访问等性能数据文件。
