# SoftmaxFlashV2样例

## 概述

本样例在大语言模型注意力机制场景下，基于SoftmaxFlashV2高阶API实现softmaxflashv2单算子。该API是SoftmaxFlash增强版本，对应FlashAttention-2算法，支持update模式（增量计算），常用于大语言模型中的注意力机制计算和长序列分块处理场景。本样例使用float数据类型，输入Tensor shape为[960, 960]，完成SoftmaxFlashV2注意力计算。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── softmaxflashv2
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── softmaxflashv2.asc      // Ascend C算子实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：
  softmaxflashv2单算子，将输入tensor[m0, m1, ...mt, n]（t大于等于0）的非尾轴长度相乘的结果看作m，则输入tensor的shape看作[m, n]。对输入tensor[m, n]按行做如下计算，不同的update值对应不同的计算公式，其中x、inmax和insum为输入，M、S、E均为输出。
  update为false：

  $$M = rowmax(x_i)$$

  $$SoftmaxFlashV2(z_i) = exp(x_i - M)$$

  $$S=\sum_{i}^{n} \exp(x_i - M)$$

  update为true：

  $$M = max(rowmax(x_i), inmax)$$

  $$SoftmaxFlashV2(z_i) = exp(x_i - M)$$

  $$E = exp(inmax_i - M)$$

  $$S = sum_{i}^{n} exp(x_i - M) + E \cdot insum$$

- 样例规格：

<div align="left">
<table>
<caption>表1：样例规格表</caption>
<tr><td align="center" rowspan="1">样例类型(OpType)</td><td align="center" colspan="4">Softmaxflash</td></tr>

<tr><td align="center" rowspan="3">样例输入</td></tr>
<tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center"> [960, 960] </td><td align="center">float</td><td align="center">ND</td></tr>

<tr><td align="center" rowspan="3">样例输出</td></tr>
<tr><td align="center">max</td><td align="center"> [960, 8] </td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">sum</td><td align="center"> [960, 8] </td><td align="center">float</td><td align="center">ND</td></tr>

<tr><td align="center" rowspan="1">核函数名</td><td align="center" colspan="4">softmaxflashv2_custom</td></tr>
</table>
</div>

- 样例实现：  
  本样例中实现的是固定shape为输入x [960, 960]，输出max[960, 8]、sum[960, 8]的softmaxflashv2样例。

  - Kernel实现  
    核心计算步骤：将输入数据搬入后，调用 `AscendC::SoftmaxFlashV2(xLocal, sumLocal, maxLocal, xLocal, expmaxLocal, sharedTmpBuffer, softmaxTiling)` 完成 SoftmaxFlashV2 计算，再将结果搬出。

  - Tiling实现  
    softmaxflashv2样例的tiling实现流程如下：首先对shape按照行数进行分核，使用平均分配法先按照核数向上对齐分配，确定主核的计算行数，再确定尾核计算行数，对主核计算的shape调用SoftmaxFlashV2高阶API的tiling函数获取API所需tiling参数，尾核计算所需的高阶API的tiling由kernel侧自行计算。

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