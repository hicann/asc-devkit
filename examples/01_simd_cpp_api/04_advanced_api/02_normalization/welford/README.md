# Welford样例

## 概述

本样例基于Kernel直调样例工程，介绍了在一个核函数中连续调用WelfordUpdate和WelfordFinalize高阶API，实现完整的Welford在线算法。Welford是一种在线计算均值和方差的方法，可以在单次数据遍历中逐步计算均值和方差，减少访存次数，提高计算性能。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── welford
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── welford.asc             // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：  
  本样例在一个核函数中依次调用WelfordUpdate和WelfordFinalize高阶API，实现完整的Welford在线算法。Welford算法优势：
  - 可以在不存储所有样本的情况下，逐步计算所有样本的均值和方差
  - 只需对数据进行一次遍历，减少访存次数，提高计算性能
  - WelfordUpdate：在线更新均值和方差，计算公式为 $M_t = M_{t-1} + (x_t - M_{t-1}) / n$, $S_t = S_{t-1} + (x_t - M_{t-1}) \times (x_t - M_t)$
  - WelfordFinalize：汇总多个块的结果得到最终均值和方差，计算公式为 $Mean = \frac{\sum(M_i \cdot n_i)}{\sum n_i}$, $Var = \frac{\sum(S_i + (Mean_i - Mean)^2 \cdot n_i)}{\sum n_i}$

- 样例规格：  
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center"> welford </td></tr>

  <tr><td rowspan="5" align="center">样例输入</td></tr>
  <tr><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">srcGm</td><td align="center">[1, 64]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">inMeanGm</td><td align="center">[1, 64]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">inVarGm</td><td align="center">[1, 64]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="5" align="center">样例输出</td></tr>
  <tr><td align="center">outMeanGm</td><td align="center">[1, 64]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">outVarGm</td><td align="center">[1, 64]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">finalMeanGm</td><td align="center">[8]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">finalVarGm</td><td align="center">[8]</td><td align="center">float</td><td align="center">ND</td></tr>

  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">welford_coop_custom</td></tr>
  </table>

- 样例实现：  
  本样例实现固定shape的融合Welford样例（RN=1, AB=64）。

  本样例在单个核函数中调用两个API：
    1. WelfordUpdate：对每个数据块计算局部均值和方差
    2. WelfordFinalize：汇总所有块的均值和方差，得到全局均值和方差

  Welford算法公式已在样例功能中描述，详细说明请参考WelfordUpdate API文档和WelfordFinalize API文档。

  - Kernel实现

    计算逻辑是：  
    在一个核函数内部依次调用WelfordUpdate和WelfordFinalize高阶API，实现完整的Welford在线算法，并将计算结果搬出到外部存储上。

  - Tiling实现

    样例的tiling实现流程如下：
    1. 调用AscendC::GetWelfordUpdateMaxMinTmpSize 获取 WelfordUpdate 接口计算所需的最小临时空间大小，使用最小值作为工作空间大小确保功能正确。
    2. 通过 CopyTilingData 函数将 GM 中的 Tiling 数据拷贝到 kernel 侧。

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
  mkdir -p build && cd build;                              # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;     # 编译工程（默认npu模式）
  python3 ../scripts/gen_data.py                           # 生成测试输入数据
  ./demo                                                   # 执行编译生成的可执行程序，执行样例
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
