# SoftmaxGrad样例

## 概述

本样例在神经网络反向传播场景下，基于 `SoftmaxGrad` 或 `SoftmaxGradFront` 高阶 API 实现 softmax 梯度计算。

- **SoftmaxGrad**：对输入 tensor `[m, n]` 按行做完整的 softmax 梯度反向计算，计算公式为 `(grad - sum(grad * src)) * src`。
- **SoftmaxGradFront**：仅计算 softmax 梯度的前半部分，计算公式为 `sum(grad * src)`，输出每行的求和结果。该接口常用于 FlashAttention 等需要中间梯度结果的场景。

两个接口的关系：`SoftmaxGradFront` 是 `SoftmaxGrad` 的子集，`SoftmaxGrad(isFront=true)`时推荐使用 `SoftmaxGradFront`。本样例通过编译宏 `USE_FRONT_MODE` 控制两种模式，使用 float 数据类型，输入 x 和 y 的 shape 均为 `[960, 960]`。

> **注意：** 输入 tensor 的 last 轴长度必须满足 32 字节对齐（float 类型下为 8 的倍数）。`SoftmaxGradFront` 模式下输出 tensor 的 last 轴固定为 1 个 datablock（float 类型下为 8 个元素），所有元素值相同。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── softmaxgrad
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本（支持两种模式）
│   ├── CMakeLists.txt          // 编译工程文件（支持 -DUSE_FRONT_MODE）
│   ├── data_utils.h            // 数据读入写出函数
│   ├── softmaxgrad.asc         // Ascend C 样例实现 & 调用示例（两种模式合并）
│   └── README.md               // 样例说明文档
```

## 样例规格

<div align="left">
<table>
<caption>表1：SoftmaxGrad 模式样例规格</caption>
<tr><td align="center">名称</td><td align="center">shape</td><td align="center">数据类型</td><td align="center">排布格式</td></tr>
<tr><td align="center">输入 x</td><td align="center">[960, 960]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">输入 y</td><td align="center">[960, 960]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">输出 z</td><td align="center">[960, 960]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">核函数名</td><td align="center" colspan="3">softmaxgrad_custom</td></tr>
</table>
</div>

- 样例实现:  
本样例实现的是固定 shape 的样例，输入 x `[960, 960]`，y `[960, 960]`，分核进行计算。

  - Tiling 实现  
    按照行数进行分核，使用平均分配法按核数向上对齐分配，确定主核和尾核的计算行数。根据 reduce 轴长度查询 SLICE_TABLE 确定每次循环处理的行数，调用 `GetSoftMaxGradMinTmpSize` 和 `SoftMaxGradTilingFunc` 获取 API 所需的 Tiling 参数。

  - Kernel 实现  
    核心计算步骤：将输入数据搬入后，调用对应的 SoftmaxGrad/SoftmaxGradFront API 完成梯度计算，再将结果搬出。

    SoftmaxGrad 模式调用方式：
    ```cpp
    AscendC::SoftmaxGrad<float, true>(yLocal, xLocal, yLocal, tmpBuffer, softmaxTiling, false, srcShape);
    ```

    SoftmaxGradFront 模式调用方式：
    ```cpp
    AscendC::SoftmaxGradFront<float>(zLocal, xLocal, yLocal, tmpBuffer, softmaxTiling, srcShape);
    ```

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

   **SoftmaxGrad 模式（默认）**：
   ```bash
   mkdir -p build && cd build;
   cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j;
   python3 ../scripts/gen_data.py
   ./demo
   ```

   **SoftmaxGradFront 模式**：
   ```bash
   mkdir -p build && cd build;
   cmake -DUSE_FRONT_MODE=ON -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j;
   python3 ../scripts/gen_data.py --front-mode
   ./demo
   ```

  使用 CPU 调试或 NPU 仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

   <div align="left">
   <table>
   <caption>表3：编译选项说明</caption>
   <tr><td align="center">选项</td><td align="center">可选值</td><td align="center">说明</td></tr>
   <tr><td align="center">CMAKE_ASC_RUN_MODE</td><td align="center">npu（默认）、cpu、sim</td><td align="center">运行模式：NPU 运行、CPU调试、NPU仿真</td></tr>
   <tr><td align="center">CMAKE_ASC_ARCHITECTURES</td><td align="center">dav-2201（默认）、dav-3510</td><td align="center">NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品</td></tr>
   <tr><td align="center">USE_FRONT_MODE</td><td align="center">OFF（默认）、ON</td><td align="center">样例模式：OFF 为 SoftmaxGrad，ON 为 SoftmaxGradFront</td></tr>
   </table>
   </div>

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
