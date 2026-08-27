# SIMT编程模式实现一维Gather算子样例

## 概述

样例基于Ascend C SIMT编程方式实现简单场景（固定shape的）一维Gather算子，从一维输入中采集指定索引的元素，展示简化场景离散内存访问类算子的开发方法。

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.0.0

## 目录结构

```text
├── gather_1d
│   ├── CMakeLists.txt         # cmake编译文件
│   ├── gather_1d.asc          # SIMT实现一维gather调用样例
│   └── README.md
```

## 算子描述

- 算子功能：  
  gather_1d算子实现了从shape为[100000]的一维输入张量中，根据index中每个元素获取1个数据的功能。算子输出output第i个元素计算公式为：

  ```text
  output[i] = input[index[i]]
  ```

- 算子规格：  
  <table>
  <tr><td rowspan="1" align="center">算子类型(OpType)</td><td colspan="4" align="center">gather_1d</td></tr>
  <tr><td rowspan="3" align="center">算子输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[100000]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">index</td><td align="center">[12288]</td><td align="center">int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">算子输出</td><td align="center">output</td><td align="center">[12288]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">gather_1d_custom</td></tr>
  </table>

- 数据切分：  
  * 核数：48核
  * 每核线程数：256线程
  * 单线程处理：1个元素
  * 总处理能力：48 * 256 = 12288个元素
  * 结果预期：输出第i个元素等于输入的第index[i]个元素

- 算子实现：  
  gather_1d算子的实现流程为从输入input（Global Memory）中获取指定索引的数据。基于上述数据切分，首先计算线程应处理数据的索引，然后根据index[i]读取1个元素，并写入output[i]。

- 调用实现：  
  使用内核调用符`<<<>>>`启动Kernel。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  mkdir -p build && cd build;   # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..; make -j;   # 编译工程
  ./demo                        # 执行样例
  ```

  编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT） |

  执行结果如下，说明精度对比成功。

  ```text
  [Success] Case accuracy is verification passed.
  ```
