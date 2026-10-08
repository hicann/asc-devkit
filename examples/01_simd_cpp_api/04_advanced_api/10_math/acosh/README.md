# Acosh样例

## 概述

本样例基于Acosh高阶API计算反双曲余弦函数。

> **涉及样例：** 除本样例使用的 `Acosh` 接口外，Ascend C 还提供了以下三角函数相关高阶API接口，除sincos外实现方式基本一致，如需调用替换接口名即可：
>
> - **acos**：反余弦函数。
> - **asin**：反正弦函数。
> - **asinh**：反双曲正弦函数。
> - **atanh**：反双曲正切函数。
> - **cos**：余弦函数。
> - **cosh**：双曲余弦函数。
> - **sinh**：双曲正弦函数。
> - **tan**：正切函数。
> - **sincos**：正弦余弦函数，分别计算正弦和余弦，调用时需要两个输出Tensor。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```text
├── acosh
│   ├── CMakeLists.txt     // 编译工程文件
│   ├── acosh.asc          // Ascend C样例实现 & 调用样例
│   └── README.md          // 样例说明文档
```

## 样例描述

- 样例功能：
  按元素做双曲反余弦函数计算，计算公式如下：
  $$dstTensor_i = Acosh(srcTensor_i)$$
  $$Acosh(x)=\begin{cases}Nan, & x < 1 \\ \ln(x+\sqrt{x^{2}-1}), & x > 1\end{cases}$$
- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center"> acosh </td></tr>

  <tr><td rowspan="3" align="center">样例输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">src</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">样例输出</td></tr>
  <tr><td align="center">dst</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">acosh_custom</td></tr>
  </table>

- 样例实现：  
  本样例中实现的是固定shape为输入src[1, 16]，输出dst[1, 16]的acosh_custom。

  - Kernel实现

    使用Acosh高阶API接口完成反双曲余弦计算。

  - Tiling实现

    Host侧通过GetAcoshMaxMinTmpSize获取Acosh接口计算所需的最大和最小临时空间。

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
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # 编译工程，默认npu模式
  ./demo                           # 执行编译生成的可执行程序，执行样例
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
