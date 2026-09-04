# Mmad GEMV样例

## 概述

本样例介绍了GEMV（M=1）模式下的矩阵乘法。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.2.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```
├── mmad_gemv
│   ├── figures                     // 本文中的插图文件
│   ├── scripts
│   │   ├── gen_data.py             // 输入数据和真值数据生成脚本
│   │   └── verify_result.py        // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── data_utils.h                // 数据读入写出函数
│   ├── mmad_gemv.asc               // Ascend C样例实现 & 调用样例
│   └── README.md                   // 样例说明文档
```

## 样例描述

GEMV模式是指Mmad计算中M=1，形状为(1, K)的左矩阵A与形状为(K, N)的右矩阵B进行矩阵乘运算的场景。当M=1时，自动开启GEMV模式，仅在Ascend 950PR/Ascend 950DT上支持通过设置`mmadParams.disableGemv = true`关闭。本样例中通过编译参数`DISABLE_GEMV`选择是否关闭GEMV模式，0 代表开启GEMV，1 代表关闭。

以M=1，K=256，N=32，左右矩阵数据类型为half为具体示例，说明GEMV模式、非GEMV模式下Mmad计算的过程。

- GEMV模式

  将A矩阵从A1搬运到A2时，1 * 256的向量被当作16 * 16的矩阵进行处理，调用LoadData接口一次完成16 * 16分形大小的矩阵搬运。B矩阵的搬运以及矩阵乘计算跟基础场景相同，如下图1所示。
  <p align="center">
  <img src="figures/开启gemv.png" width="600">
  </p>
  <p align="center">
  图1：GEMV模式，Mmad计算示意图
  </p>

- 非GEMV模式

  将A矩阵从A1搬运到A2时，1 * 256的向量被当作非对齐矩阵数据进行处理，将M方向对齐到16后进行搬运。调用LoadData接口每次搬运16 * 16分形大小的矩阵，一共搬运CeilDiv(K, 16)=16次，导致搬运数据量增加，性能相较于GEMV模式差，如下图2所示。
  <p align="center">
  <img src="figures/关闭gemv.png" width="600">
  </p>
  <p align="center">
  图2：非GEMV模式，Mmad计算示意图
  </p>

## 约束说明

- 在Mmad计算中，若要开启GEMV模式，参数`mmadParams.m`必须等于1。
- GEMV场景下，左矩阵A在L1搬运到L0A时不支持转置。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行算子。
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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DDISABLE_GEMV=0 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```
  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DDISABLE_GEMV=0 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DDISABLE_GEMV=0 ..;make -j; # NPU仿真模式
  ```
  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品/Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `DISABLE_GEMV` | `0`（默认）、`1` | 是否关闭GEMV模式，`仅在CMAKE_ASC_ARCHITECTURES==dav-3510时支持设为1` |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
