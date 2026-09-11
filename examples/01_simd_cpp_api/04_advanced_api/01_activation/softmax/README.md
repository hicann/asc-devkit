# Softmax样例

## 概述

本样例在激活函数场景下，基于SoftMax高阶API对输入Tensor按行做SoftMax计算，并使用AdjustSoftMaxRes对计算结果做后处理。SoftMax将输入tensor[m, n]按行执行reducemax、sub、exp、reducesum、div等步骤，得到归一化概率分布，常用于注意力机制和分类任务的输出层。

> **接口提示：** 除本样例使用的 `SoftMax` 接口外，Ascend C 还提供了 `SimpleSoftMax` 接口，使用预计算的sum和max数据对输入Tensor做SoftMax计算，内部无reduce过程。将 `softmax.asc` 中 `AscendC::SoftMax` 替换为 `AscendC::SimpleSoftMax` 即可切换。

AdjustSoftMaxRes用于对SoftMax计算结果做后处理，当输入的max中存在指定值时，按行调整对应softmaxres结果为自定义的值。本样例使用float数据类型，输入Tensor shape为[32, 32]，完成SoftMax计算及AdjustSoftMaxRes后处理。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── softmax
│   ├── scripts
│   │   └── gen_data.py         // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── README.md               // 样例说明文档
│   └── softmax.asc             // Ascend C样例实现 & 调用样例
```

## 样例描述

- 样例功能：

  本样例对输入 Tensor 按行做 SoftMax 计算，并使用 AdjustSoftMaxRes 对计算结果做后处理。`0xFF7FFFFF`是float类型最小有限值（约为-3.4028235e+38）的IEEE 754位模式。数据生成脚本将输入的第一行设置为该值，使对应的max命中指定值，并将该行的SoftMax输出调整为0.0。该机制常用于注意力掩码（attention mask）场景，将无效位置的SoftMax输出置零。

- 样例规格：

<div align="left">
<table>
<caption>表1：样例规格表</caption>
<tr><td align="center" rowspan="1">样例类型(OpType)</td><td align="center" colspan="4">softmax</td></tr>

<tr><td align="center" rowspan="3">样例输入</td></tr>
<tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[32, 32]</td><td align="center">float</td><td align="center">ND</td></tr>

<tr><td align="center" rowspan="4">样例输出</td></tr>
<tr><td align="center">y</td><td align="center">[32, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">max</td><td align="center">[32, 8]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">sum</td><td align="center">[32, 8]</td><td align="center">float</td><td align="center">ND</td></tr>

<tr><td align="center" rowspan="1">核函数名</td><td align="center" colspan="4">softmax_custom</td></tr>
</table>
</div>

- 样例实现：  
  本样例中实现的是固定shape为输入x[32, 32]，输出y[32, 32]、max[32, 8]、sum[32, 8]的softmax样例。

  - Kernel实现  
    核心计算步骤：将输入数据搬入后，调用 `AscendC::SoftMax` 完成 SoftMax 计算，再调用 `AscendC::AdjustSoftMaxRes` 对结果做后处理，最后将结果搬出。

  - Tiling实现  
    softmax样例的tiling实现流程如下：首先对shape按照行数进行分核，使用平均分配法先按照核数向上对齐分配，确定主核的计算行数，再确定尾核计算行数，对主核计算的shape调用GetSoftMaxMinTmpSize获取API所需临时缓冲区大小，再调用SoftMax高阶API的tiling函数获取API所需tiling参数，尾核计算所需的高阶API的tiling由kernel侧自行计算。

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
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
