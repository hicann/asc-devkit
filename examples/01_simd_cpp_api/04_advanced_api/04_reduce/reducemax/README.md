# ReduceMax样例

## 概述

本样例在归约场景下，基于ReduceMax高阶API实现对矩阵的每一列求最大值。

> **接口提示：** 除本样例使用的 `ReduceMax` 接口外，Ascend C 还提供了以下归约类接口，调用方式与 `ReduceMax` 一致，只需替换函数名即可切换：
> - **ReduceMin**：求最小值。将 `reducemax.asc` 中 `AscendC::ReduceMax` 替换为 `AscendC::ReduceMin`，`GetReduceMaxMaxMinTmpSize` 替换为 `GetReduceMinMaxMinTmpSize`。
> - **ReduceMean**：求平均值。将 `reducemax.asc` 中 `AscendC::ReduceMax` 替换为 `AscendC::ReduceMean`，`GetReduceMaxMaxMinTmpSize` 替换为 `GetReduceMeanMaxMinTmpSize`。
> - **ReduceSum**：求和。将 `reducemax.asc` 中 `AscendC::ReduceMax` 替换为 `AscendC::ReduceSum`，`GetReduceMaxMaxMinTmpSize` 替换为 `GetReduceSumMaxMinTmpSize`。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── reducemax
│   ├── scripts
│   │   └── gen_data.py             // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── data_utils.h                // 数据读入写出函数
│   ├── reducemax.asc               // Ascend C样例实现 & 调用样例
│   └── README.md                   // 样例说明文档
```

## 样例描述  

- 样例功能：  
  本样例的功能是对输入矩阵的每一列求最大值。

- 样例规格：  
<table>
<tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center"> reducemax </td></tr>

<tr><td rowspan="3" align="center">样例输入</td></tr>
<tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[32, 136]</td><td align="center">float</td><td align="center">ND</td></tr>


<tr><td rowspan="2" align="center">样例输出</td></tr>
<tr><td align="center">y</td><td align="center">[1, 136]</td><td align="center">float</td><td align="center">ND</td></tr>


<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">reduce_custom</td></tr>
</table>

- 样例实现：  
  本样例将输入[32, 136]矩阵按列求最大值，输出[1，136]。

  - Kernel实现:  
    计算逻辑是：输入数据需要先搬运到片上存储，然后使用ReduceMax高阶API接口完成计算，再将结果搬出。

  - Tiling实现:  
    该样例的tiling实现流程如下：使用GetReduceMaxMaxMinTmpSize接口计算所需最大/最小临时空间大小，使用最小临时空间，然后根据输入长度确定所需tiling参数。

  - 调用实现:  
    使用内核调用符<<<>>>调用核函数。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。
- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build;
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # 默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
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

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
