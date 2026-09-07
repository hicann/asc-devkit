# Fixpipe参数结构体切换兼容性样例


## 概述

本样例演示Fixpipe接口在2201和3510两种架构下使用不同参数结构体的切换方案，通过编译时宏隔离不同硬件实现。样例包含NZ2ND和NZ2NZ两个场景，重点展示dstStride单位差异。

Fixpipe接口用于将矩阵乘结果从L0C Buffer搬运到GM。3510架构引入了新的原生参数结构体`FixpipeParamsArch3510`，与2201架构使用的`FixpipeParamsV220`存在关键字段差异。

- Atlas A2/A3 训练/推理系列产品：使用`FixpipeParamsV220`参数结构体。
- Ascend 950PR/950DT：使用`FixpipeParamsArch3510`原生参数结构体。3510也兼容`FixpipeParamsV220`（内部自动转换），但推荐使用原生结构体以获得完整能力。

### 两种参数结构体的关键差异

| 对比项 | FixpipeParamsV220（2201） | FixpipeParamsArch3510（3510） |
|--------|--------------------------|------------------------------|
| NZ2ND参数 | `ndNum`/`srcNdStride`/`dstNdStride`为顶层字段 | 收纳于`params`子结构（由`format`模板选择类型） |
| dstStride单位（NZ2NZ场景） | datablock（32字节） | element（元素个数） |
| dstStride单位（NZ2ND场景） | element | element（与V220一致） |
| 高级ReLU | 不支持 | 支持`preReluMode`/`preClipReluMode`/`reluScalar`/`vectorRelu` |
| NZ2DN格式转换 | 不支持 | 支持`CO2Layout::COLUMN_MAJOR` |
| UB通路控制 | 不支持 | 支持`dualDstCtl`/`subBlockId` |
| 量化模式 | 基础量化模式 | 扩展FP8/HIF8/BF16等量化模式 |

> **迁移注意**：在NZ2NZ（非NZ2ND）场景下，`dstStride`的单位从datablock变为element，直接复用V220的值会导致3510上地址计算错误。在NZ2ND（CFG_ROW_MAJOR）场景下，两种结构体的dstStride单位均为element，可直接统一赋值。

### 场景说明

| 场景编号 | 场景 | 输出格式 | dstStride单位差异 | 说明 |
|---------|------|---------|------------------|------|
| 1 | NZ2ND | ND（CFG_ROW_MAJOR） | 无差异（均为element） | 公共参数可直接统一赋值 |
| 2 | NZ2NZ | NZ（CFG_NZ） | 有差异（V220=datablock, Arch3510=element） | dstStride需分别赋值 |

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── fixpipe_params_switch
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── fixpipe_params_switch.asc // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例规格

| 类别 | name | shape | data type | format |
|------|------|-------|-----------|--------|
| 样例输入 | x | [128, 128] | half | ND |
| 样例输入 | y | [128, 256] | half | ND |
| 样例输出（场景1） | z | [128, 256] | float | ND |
| 样例输出（场景2） | z | [128, 256] | float | NZ |
| 核函数名 | fixpipe_params_switch | | | |

## 样例实现

数据流为：GM -> L1 Buffer -> L0A/L0B Buffer -> L0C Buffer -> GM。

1. 将矩阵A和B从GM搬运到L1 Buffer（NZ格式）。
2. 将矩阵A从L1搬运到L0A Buffer：2201需要NZ→ZZ分形转换（旧版`LoadData2DParams`循环搬运），3510直接按NZ搬运（`LoadData2DParamsV2`）。
3. 将矩阵B从L1搬运到L0B Buffer（`LoadData2DParams`循环搬运）。
4. 调用`Mmad`接口进行矩阵乘计算，结果写入L0C Buffer。
5. 调用`Fixpipe`接口将结果从L0C搬运到GM，根据场景和架构选用不同参数结构体：
   - **场景1（NZ2ND）**：2201使用`FixpipeParamsV220`（设置顶层字段`ndNum`/`srcNdStride`/`dstNdStride`），3510使用`FixpipeParamsArch3510<ROW_MAJOR>`（设置`params.ndNum`/`params.srcNdStride`/`params.dstNdStride`）。公共参数字段名一致，dstStride单位均为element，可直接统一赋值。
   - **场景2（NZ2NZ）**：2201使用`FixpipeParamsV220`，dstStride单位为datablock（32字节）；3510使用`FixpipeParamsArch3510<NZ>`，dstStride单位为element。dstStride需分别计算赋值。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。通过`-DSCENARIO_NUM=1`或`-DSCENARIO_NUM=2`选择场景。

  ```bash
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=1 ..;make -j;   # 场景1：NZ2ND
  python3 ../scripts/gen_data.py
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin

  # 切换场景前需清理cmake缓存
  rm CMakeCache.txt
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=2 ..;make -j;   # 场景2：NZ2NZ
  python3 ../scripts/gen_data.py
  SCENARIO_NUM=2 ./demo
  SCENARIO_NUM=2 python3 ../scripts/verify_result.py output/output.bin output/golden.bin
  ```

  使用CPU调试或NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=cpu`或`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  针对Ascend 950PR/950DT编译：

  ```bash
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=1 ..;make -j;
  ```

  > **注意：** 切换编译模式或场景前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2/A3 训练/推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`（默认）、`2` | 场景编号：1=NZ2ND（CFG_ROW_MAJOR），2=NZ2NZ（CFG_NZ） |

- 执行结果

  执行结果如下，说明精度对比成功：

  ```bash
  test pass!
  ```
