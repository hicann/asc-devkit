# Swish样例

## 概述

本样例在深度学习模型激活函数场景下，基于Swish/Silu高阶API实现激活函数计算。两个API具有紧密的数学关系：

- **Swish（默认）**：`y = x / (1 + exp(-beta * x))`，beta为可调节参数，本样例beta=1.702（GELU近似）
- **Silu**：Swish在beta=1时的特例，`y = x / (1 + exp(-x))`，比ReLU更平滑，梯度不会完全消失

本样例通过编译宏 `USE_SILU_MODE` 控制两种模式，使用float数据类型，输入Tensor元素个数为32。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── swish
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本（支持Swish/Silu两种模式）
│   ├── CMakeLists.txt          // 编译工程文件（支持 -DUSE_SILU_MODE）
│   ├── data_utils.h            // 数据读入写出函数
│   ├── swish.asc               // Ascend C算子实现 & 调用样例（两种模式合并）
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：  
  本样例对输入Tensor按元素做Swish/Silu激活计算，将计算结果写入输出Tensor。Swish和Silu具有紧密的数学关系，Silu是Swish在beta=1时的特例。

  计算公式如下：
  $$dstLocal_i = Swish(srcLocal_i) = \frac{srcLocal_i}{1 + e^{-\beta \cdot srcLocal_i}}$$
  $$dstLocal_i = Silu(srcLocal_i) = \frac{srcLocal_i}{1 + e^{-srcLocal_i}}$$

  本样例中Swish模式使用beta=1.702（GELU近似值），通过编译宏 `USE_SILU_MODE` 控制两种模式切换。

- 样例规格：

<div align="left">
<table>
<caption>表1：样例规格表</caption>
<tr><td align="center" rowspan="1">样例类型(OpType)</td><td align="center" colspan="4"> swish / silu </td></tr>

<tr><td align="center" rowspan="3">样例输入</td></tr>
<tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">src</td><td align="center">[1, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center" rowspan="2">样例输出</td></tr>
<tr><td align="center">dst</td><td align="center">[1, 32]</td><td align="center">float</td><td align="center">ND</td></tr>

<tr><td align="center" rowspan="1">核函数名</td><td align="center" colspan="4">swish_custom</td></tr>
</table>
</div>

- 样例实现：  
  本样例中实现的是固定shape为输入src[1, 32]，输出dst[1, 32]的swish_custom样例。

  - Kernel实现  
    核心计算步骤：将输入数据搬入后，调用对应的高阶API完成计算，再将结果搬出。

    Swish模式调用方式：
    ```cpp
    AscendC::Swish(dstLocal, srcLocal, dataSize, scalarValue);
    ```

    Silu模式调用方式：
    ```cpp
    AscendC::Silu<T, false>(dstLocal, srcLocal, dataSize);
    ```

  - Tiling实现  
    本样例为单核逐元素计算场景，无复杂分核逻辑。Host端通过 `AscendC::GetSwishTmpSize`（Swish模式）或 `AscendC::GetSiluTmpSize`（Silu模式）获取API所需临时缓冲区大小，直接传递给Kernel使用。

  - 调用实现  
    使用内核调用符 `<<<>>>` 调用核函数。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  **Swish模式（默认）**：
  ```bash
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j;
  python3 ../scripts/gen_data.py
  ./demo
  ```

  **Silu模式**：
  ```bash
  mkdir -p build && cd build;
  cmake -DUSE_SILU_MODE=ON -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j;
  python3 ../scripts/gen_data.py --silu-mode
  ./demo
  ```

  使用CPU调试或NPU仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行 `rm CMakeCache.txt` 后重新cmake。

- 编译选项说明

  <div align="left">
  <table>
  <caption>表2：编译选项说明</caption>
  <tr><td align="center">选项</td><td align="center">可选值</td><td align="center">说明</td></tr>
  <tr><td align="center">CMAKE_ASC_RUN_MODE</td><td align="center">npu（默认）、cpu、sim</td><td align="center">运行模式：NPU运行、CPU调试、NPU仿真</td></tr>
  <tr><td align="center">CMAKE_ASC_ARCHITECTURES</td><td align="center">dav-2201（默认）、dav-3510</td><td align="center">NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品</td></tr>
  <tr><td align="center">USE_SILU_MODE</td><td align="center">OFF（默认）、ON</td><td align="center">样例模式：OFF为Swish，ON为Silu</td></tr>
  </table>
  </div>

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
