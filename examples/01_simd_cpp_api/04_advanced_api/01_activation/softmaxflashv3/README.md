# SoftmaxFlashV3样例

## 概述

本样例在大模型训练注意力优化场景下，基于SoftmaxFlashV3高阶API实现softmaxflashv3单算子。该API是SoftmaxFlash增强版本，对应Softmax PASA算法，在V2基础上增加了均值（mean）计算，通过alpha参数进行数值稳定性优化，适用于需要更高数值精度的softmax计算场景。本样例使用half（输入/输出）和float（统计值）数据类型，输入Tensor shape为[8, 2048]，完成SoftmaxFlashV3注意力计算。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── softmaxflashv3
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── softmaxflashv3.asc      // Ascend C算子实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：
  将输入tensor[m0, m1, ..., mt, n]（t大于或等于0）的非尾轴长度m0, m1, ..., mt相乘的结果看作m，则输入tensor的shape看作[m, n]。对输入tensor x的尾轴进行切分，分块个数为splitMeanCnt，切分后的tensor为x_cnti。按如下公式进行计算，其中x、inmax、insum、inmean为输入，M、S、E、A均为输出。
  update为false：

  $$
  A_1 = \text{rowmean}(x_{cnt})_i, i \in [0, \text{splitMeanCnt}]\\
  A_2 = \text{rowmean}(x_i), i \in [0, n]\\
  x_i = x_i - (A_2 - A_1) * (\alpha / (1 - \alpha))\\
  A = A_2\\
  M_1 = \text{rowmax}(x_i), i \in [0, n]\\
  M = M_1\\
  M_2 = M\\
  \text{SoftmaxFlashV3}(z_i) = \exp(x_i - M_2), i \in [0, n]\\
  S = \sum_{i}^{n} \exp(x_i - M_2)\\
  $$

  update为true：

  $$
  A_1 = \text{rowmean}(x_{cnt})_i, i \in [0, \text{splitMeanCnt}]\\
  A_2 = \text{rowmean}(x_i), i \in [0, n]\\
  x_i = x_i - (A_2 - A_1) * (\alpha / (1 - \alpha))\\
  A = (A_2 + \text{inmean} * (\text{loopCnt} - 1)) / \text{loopCnt}\\
  M_1 = \text{rowmax}(x_i), i \in [0, n]\\
  C = (A_2 - A) * (\alpha / (1 - \alpha))\\
  P = (\text{inmean} - A) * (\alpha / (1 - \alpha))\\
  M = \max(C + M_1, P + \text{inmax})\\
  M_2 = M - C\\
  \text{SoftmaxFlashV3}(z_i) = \exp(x_i - M_2), i \in [0, n]\\
  E = \exp(\text{inmax}_i - M_2 + P)\\
  S = \sum_{i}^{n} \exp(x_i - M_2) + E * \text{insum}\\
  $$

- 样例规格：

<div align="left">
<table>
<caption>表1：样例规格表</caption>
<tr><td align="center" rowspan="1">样例类型(OpType)</td><td align="center" colspan="4"> softmaxflashv3 </td></tr>

<tr><td align="center" rowspan="6">样例输入</td></tr>
<tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">src</td><td align="center"> [8, 2048] </td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">inMax</td><td align="center"> [8, 8] </td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">inSum</td><td align="center"> [8, 8] </td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">inMean</td><td align="center"> [8, 8] </td><td align="center">float</td><td align="center">ND</td></tr>

<tr><td align="center" rowspan="2">样例输出</td></tr>
<tr><td align="center">dst</td><td align="center"> [8, 2048] </td><td align="center">half</td><td align="center">ND</td></tr>

<tr><td align="center" rowspan="1">核函数名</td><td align="center" colspan="4">softmaxflashv3_custom</td></tr>
</table>
</div>

- 样例实现：  
  本样例中实现的是固定shape为输入src[8, 2048]，inMax[8, 8]，inSum[8, 8]，inMean[8, 8]， 输出dst[8, 2048]的softmaxflashv3样例。

  - Kernel实现  
    核心计算步骤：将输入数据搬入后，调用 `AscendC::SoftmaxFlashV3` 完成 SoftmaxFlashV3 计算，再将结果搬出。

  - Tiling实现  
    softmaxflashv3样例的tiling实现流程如下：首先对shape按照行数进行分核，使用平均分配法先按照核数向上对齐分配，确定主核的计算行数，再确定尾核计算行数，Kernel侧根据baseM自行计算SoftMaxTiling参数。

  - 调用实现  
    使用内核调用符<<<>>>调用核函数。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # 默认npu模式
  python3 ../scripts/gen_data.py
  ./demo
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
