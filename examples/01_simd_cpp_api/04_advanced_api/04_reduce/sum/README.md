# Sum 样例

## 概述

本样例演示了Sum高阶API，对输入数据按照最后一个维度的元素求和。如果输入是一维向量，则对一维向量中各元素求和；如果输入是矩阵，则沿最后一个维度对每行中元素求和。

> **接口提示：** 除本样例使用的 `Sum` 接口外，Ascend C 还提供了 `Mean` 接口用于求平均值，调用方式与 `Sum` 一致，只需替换函数名即可切换：
> - **Mean**：求均值。将 `sum.asc` 中 `AscendC::Sum` 替换为 `AscendC::Mean`，`GetSumMaxMinTmpSize` 替换为 `GetMeanMaxMinTmpSize`。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构说明

```
├── sum
│   ├── scripts
│   │   └── gen_data.py               // 生成数据和真值数据的生成脚本
│   ├── CMakeLists.txt                // 编译工程文件
│   ├── data_utils.h                  // 数据读写工具函数
│   ├── sum.asc                       // Ascend C样例实例 & 主程序
│   └── README.md                     // 样例说明文档
```

## 样例规格

- 样例功能：  
  本样例是对矩阵的每行元素求和。

<table>
<caption>表1：样例规格</caption>
<tr><td rowspan="1" align="left">样例类型(OpType)</td><td colspan="4" align="center"> sum </td></tr>

<tr><td rowspan="3" align="left">样例输入</td></tr>
<tr><td align="left">name</td><td align="left">shape</td><td align="left">data type</td><td align="left">format</td></tr>
<tr><td align="left">src</td><td align="left">[3，32]</td><td align="left">float</td><td align="left">ND</td></tr>

<tr><td rowspan="2" align="left">样例输出</td></tr>
<tr><td align="left">dst</td><td align="left">[8，1]</td><td align="left">float</td><td align="left">ND</td></tr>
<tr><td rowspan="1" align="left">核函数名</td><td colspan="4" align="center">sum_custom</td></tr>
</table>

- 样例实现：  
  本样例中实现的是固定shape为输入x[3，32]，输出y[8，1]，其中y中的有效值数量为3，对输入x的每行元素求和后，输出y的有效数据为前3位，最后5位为padding填充的数据。
  - Kernel实现:  
    计算逻辑是：本样例将输入数据搬运到片上存储，然后使用Sum高阶API接口完成求和，再将结果搬出。

  - Tiling实现:  
    该样例的tiling实现流程如下：使用GetSumMaxMinTmpSize接口计算所需最大/最小临时空间大小，使用最小临时空间，然后根据输入长度确定所需tiling参数。

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
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j;    # 编译工程
  python3 ../scripts/gen_data.py   # 生成测试输入数据
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

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
