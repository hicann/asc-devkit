# SwiGLU样例

## 概述

本样例在大语言模型和混合专家(MoE)场景下，基于SwiGLU高阶API对两个输入Tensor按元素做SwiGLU计算。SwiGLU是采用Swish作为激活函数的GLU变体，计算公式为 dst_i = src0_i ⊗ Swish(src1_i)，其中 Swish(x) = x/(1+e^(-βx))。该API常用于LLM中的门控前馈网络(FFN)，支持float/half/bfloat16_t等数据类型。本样例使用float数据类型，输入Tensor元素个数为32，beta值为1，完成SwiGLU激活计算。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── swiglu
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt          // 编译工程文件（支持 -DCMAKE_ASC_RUN_MODE、-DCMAKE_ASC_ARCHITECTURES）
│   ├── data_utils.h            // 数据读入写出函数
│   ├── swiglu.asc              // Ascend C样例实现 & 调用（含Tiling机制）
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：  
  SwiGLU是采用Swish作为激活函数的GLU变体。

  计算公式如下：
  $$dstTensor_i=(srcTensor0_i)\bigotimes Swish(srcTensor1_i)$$
  其中Swish激活函数的计算公式如下（β为常量）：
  $$Swish(x)=x/(1 + e^{(-\beta x)})$$

- 样例规格：  

  **表1：样例规格表**

  | 类别 | name | shape | data type | format |
  | --- | --- | --- | --- | --- |
  | 样例类型 | swiglu | - | - | - |
  | 样例输入 | src0 | [1, 32] | float | ND |
  | 样例输入 | src1 | [1, 32] | float | ND |
  | 样例输出 | dst | [1, 32] | float | ND |
  | 核函数名 | swiglu_custom | - | - | - |

- 样例实现：  
  本样例实现的是固定shape的样例，输入元素个数为32。通过Tiling机制将计算参数（dataLength、sharedTmpBufferSize）从Host端传递到Device端，支持灵活配置计算规模。

  - Kernel实现  
    核心计算步骤：将输入数据从GM搬运到UB后，调用 `AscendC::SwiGLU` 完成SwiGLU计算，再将结果搬回Global Memory。

  - Tiling实现  
    临时空间处理：通过 `AscendC::GetSwiGLUMaxMinTmpSize` 获取所需临时空间大小，由Tiling传递到Kernel端。当临时空间大于0时，使用开发者提供的buffer；否则由框架自动申请。

  - 调用实现  
    使用内核调用符 `<<<>>>` 调用核函数，传入src0、src1、dst、workspace和tiling参数。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  **默认模式（dav-2201架构）**：
  ```bash
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j;
  python3 ../scripts/gen_data.py
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
  <tr><td align="center">CMAKE_ASC_ARCHITECTURES</td><td align="center">dav-2201（默认）、dav-3510</td><td align="center">NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT</td></tr>
  </table>
  </div>

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
