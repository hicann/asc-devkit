# GeGLU类样例

## 概述

本样例在GLU（Gated Linear Unit）门控激活函数场景下，基于GeGLU高阶API实现GLU变体的激活计算。GeGLU采用GELU作为激活函数，计算模式为 `dst = src0 * GELU(src1)`。本样例使用float数据类型，输入Tensor元素个数为1024，完成Geglu样例计算。

> **接口提示：** 除本样例使用的 `GeGLU` 接口外，Ascend C 还提供了以下 GLU 变体接口，调用方式与 `GeGLU` 一致，只需替换函数名即可切换：
> - **ReGlu**：采用 Relu 作为激活函数的 GLU 变体。将 `geglu.asc` 中 `AscendC::GeGLU` 替换为 `AscendC::ReGlu`。


## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── geglu
│   ├── scripts
│   │   └── gen_data.py         // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── README.md               // 样例说明文档
│   └── geglu.asc               // Ascend C样例实现 & 调用样例
```

## 样例描述

- 样例功能：
  本样例对两个输入Tensor执行GeGLU门控激活计算，将计算结果写入输出Tensor。

  GeGLU是采用GELU作为激活函数的GLU变体。

  计算公式如下：
  $$dstTensor_i = srcTensor0_i \otimes GELU(srcTensor1_i)$$
  其中GELU激活函数的计算公式如下：
  $$GELU(x)=0.5 * x * (1 + tanh(\sqrt{\frac{2}{\pi}} * (x + 0.044715 * x^3)))$$
  化简后可得：
  $$GELU(x)=\frac{x}{1 + e^{-1.59576912 * (x + 0.044715 * x^3)}}$$

- 样例规格：

<div align="left">
<table>
<tr><td align="center" rowspan="1">样例类型(OpType)</td><td align="center" colspan="4"> geglu </td></tr>

<tr><td align="center" rowspan="4">样例输入</td></tr>
<tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">src0</td><td align="center">[1,1024]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">src1</td><td align="center">[1,1024]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center" rowspan="2">样例输出</td></tr>
<tr><td align="center">dst</td><td align="center">[1,1024]</td><td align="center">float</td><td align="center">ND</td></tr>

<tr><td align="center" rowspan="1">核函数名</td><td align="center" colspan="4">geglu_custom</td></tr>
</table>
</div>

- 样例实现：  
  本样例中实现的是固定shape为输入src[1,1024]、src1[1,1024]，输出dst[1,1024]的geglu_custom样例。

  - Kernel实现  
    核心计算步骤：将输入数据搬入后，调用 `AscendC::GeGLU` 完成GeGLU门控激活计算，再将结果搬出。

  - Tiling实现  
    本样例为单核逐元素计算场景，无复杂分核逻辑。Host端通过 `AscendC::GetGeGLUMaxMinTmpSize` 获取API所需临时缓冲区大小，直接传递给Kernel使用。

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
