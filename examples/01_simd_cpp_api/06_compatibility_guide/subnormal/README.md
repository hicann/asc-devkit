# Subnormal兼容性样例


## 概述

本样例以Ln接口为例，演示矢量计算接口的Subnormal计算模式兼容性适配，通过编译时宏隔离不同硬件实现。

SubNormal浮点数指指数位全为0、尾数不为0的浮点数，用于表示比最小正常数更小的值。3510架构版本默认不支持Subnormal，Subnormal浮点数在计算中被视为0（FTZ，Flush To Zero）。

- Atlas A2/A3 训练/推理系列产品：硬件默认支持Subnormal，调用`Ln`接口时无需额外配置。
- Ascend 950PR/950DT：默认不支持Subnormal，需通过`LnConfig`将参数`algo`配置为`LnAlgo::PRECISION_1ULP_FTZ_FALSE`，由软件仿真保留Subnormal数据的计算结果。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── subnormal
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── subnormal_custom.asc    // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例规格

| 类别 | name | shape | data type | format |
|------|------|-------|-----------|--------|
| 样例输入 | x | [1, 1024] | half | ND |
| 样例输出 | z | [1, 1024] | half | ND |
| 核函数名 | subnormal_custom | | | |

> **说明：** 输入数据`x`中包含Subnormal half值（指数位全0、尾数非0，取值范围为`(0, 2^-14)`即`(0, 6.10e-5)`）。真值`z`为输入数据以高精度计算自然对数后的结果，保留了Subnormal输入的真实对数值，而非被近似为0后得到的`-inf`。

## 样例实现

数据流为：GM -> UB（Unified Buffer）-> GM。

1. 将输入数据从GM搬运到UB。
2. 调用`Ln`接口计算自然对数，根据架构采用不同配置以保留Subnormal数据的计算结果：
   - Atlas A2/A3 训练/推理系列产品：直接调用`Ln`接口，硬件默认支持Subnormal。
   - Ascend 950PR/950DT：通过`LnConfig`将`algo`配置为`LnAlgo::PRECISION_1ULP_FTZ_FALSE`后调用`Ln`接口，由软件仿真保留Subnormal计算结果。
3. 将计算结果从UB搬运回GM。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。
  ```bash
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin
  ```

  使用CPU调试或NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=cpu`或`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  针对Ascend 950PR/950DT编译：

  ```bash
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;                      # 编译工程（Ascend 950PR/950DT）
  ```

  > **注意：** 切换编译模式前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果

  执行结果如下，说明精度对比成功：

  ```bash
  test pass!
  ```
