# LayerNorm样例

## 概述

本样例介绍了在一个核函数中连续调用LayerNorm、LayerNormGrad、LayerNormGradBeta三个高阶API实现LayerNorm的完整前向和反向传播。

> **相似接口提示：** Ascend C 还提供了`BatchNorm`、`GroupNorm`，调用方式与 `LayerNorm` 一致。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── layernorm
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── layernorm.asc           // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：  
  LayerNorm样例，在一个核函数中连续调用LayerNorm、LayerNormGrad、LayerNormGradBeta三个高阶API，完成LayerNorm的完整前向和反向传播计算。该样例在训练场景中可以一次性计算前向输出、输入梯度、参数梯度，提高计算效率。

- 样例规格：  
  <table border="2" align="center">
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center"> layernorm </td></tr>
  <tr><td rowspan="6" align="center">样例输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">inputXGm</td><td align="center">[2, 32, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">gammaGm</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">betaGm</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">dyGm</td><td align="center">[2, 32, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="7" align="center">样例输出</td></tr>
  <tr><td align="center">outputYGm</td><td align="center">[2, 32, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">outputMeanGm</td><td align="center">[2, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">outputVarGm</td><td align="center">[2, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">outputPdXGm</td><td align="center">[2, 32, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">outputPdGammaGm</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">outputPdBetaGm</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">layernorm_custom</td></tr>
  </table>

- 样例实现：  
  本样例实现固定shape(inputX[2, 32, 16]、gamma[1, 16]、beta[1, 16]、dy[2, 32, 16])的融合LayerNorm样例。

  - Kernel实现

    本样例在一个核函数内部实现了完整的数据流计算：
    1. LayerNorm前向计算：输入inputX、gamma、beta，计算得到输出y、均值mean、方差var。
    2. LayerNormGrad反向计算：输入dy、inputX、var、mean、gamma，计算得到输入梯度pd_x和中间结果resForGamma
    3. LayerNormGradBeta参数梯度计算：输入dy、resForGamma，计算得到gamma和beta的梯度pd_gamma、pd_beta。

  - Tiling实现

    layernorm样例的tiling实现流程如下：
    1. 分别获取并设置LayerNorm、LayerNormGrad、LayerNormGradBeta接口能完成计算所需的最小临时空间大小。
    2. 根据输入shape、剩余可供计算的空间大小等信息分别获取三个API kernel侧接口所需tiling参数。

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
  mkdir -p build && cd build;   # 创建并进入build目录
  cmake ..;make -j;             # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                        # 执行编译生成的可执行程序，执行样例
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

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
