
# 矩阵乘法中LoadData（卷积数据搬运）v2数据搬运示例


## 概述

本样例介绍LoadData（卷积数据搬运）v2指令在矩阵乘法中的使用场景和方法。LoadData（卷积数据搬运）v2可以将二维的A、B矩阵从L1搬运到L0A/L0B，其中 A 和 B 分别表示矩阵乘法的左右输入矩阵。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.2.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```
├── mmad_load3dv2
│   ├── scripts
│   │   ├── gen_data.py                      // 输入数据和真值数据生成脚本
│   │   └── verify_result.py                 // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                       // 编译工程文件
│   ├── data_utils.h                         // 数据读入写出函数
│   ├── load3d.py                            // LoadData3D辅助脚本
│   ├── mmad_load3dv2.asc                    // Ascend C算子实现 & 调用样例
│   └── README.md                            // 样例说明文档
```
## 算子描述

LoadData（卷积数据搬运）v2指令以下简称load3dv2，该指令支持的数据类型：
- Ascend 950PR/Ascend 950DT，支持数据类型为：int8_t、uint8_t、hifloat8_t、fp8_e5m2_t、fp8_e4m3fn_t、half、bfloat16_t、int32_t、uint32_t、float。

- Atlas A3 训练系列产品/Atlas A3 推理系列产品和Atlas A2 训练系列产品/Atlas A2 推理系列产品：
    - 目的地址位于A2时，支持数据类型为：int4b_t、int8_t、uint8_t、half、bfloat16_t、int32_t、uint32_t、float。
    - 目的地址位于B2时，支持数据类型为：half、bfloat16_t、int32_t、uint32_t、float。

load3dv2指令对于二维矩阵的转置能力：
- Ascend 950PR/Ascend 950DT，目的地址位于A2，并且类型为b8/b16/b32时，enTranspose参数有效。
- Atlas A3 训练系列产品/Atlas A3 推理系列产品和Atlas A2 训练系列产品/Atlas A2 推理系列，目的地址位于A2，并且类型为b16/b32时，enTranspose参数有效。

由于本样例暂不支持输入数据类型为int4b_t，因此本样例展示了以下五种load3dv2在矩阵乘法的使用：

### LoadData（卷积数据搬运）v2接口场景对照表

| scenarioNum | 输入数据类型 | A矩阵转置 | B矩阵转置 |
| --- | --- | --- | --- |
| 1 | half | 不转置 | 不转置 |
| 2 | half | 转置 | 不转置 |
| 3 | float | 不转置 | 不转置 |
| 4 | float | 转置 | 不转置 |
| 5 | int8_t | 不转置 | 转置 |

注：Atlas A3 训练系列产品/Atlas A3 推理系列产品和Atlas A2 训练系列产品/Atlas A2 推理系列产品，输入数据类型为B8时且目的地址位于L0B时，不支持load3dv2指令，为保种兼容性，当scenarioNum=5时，SplitB中调用的是load2d指令。

  本样例中scenarioNum=3和4中A矩阵分别与样例[load_data_l12l0](../load_data_l12l0/README.md)中scenarioNum=12和13场景一致，B矩阵与该样例中scenarioNum=13场景一致，因此load3dv2指令具体的参数配置和示意图可以参考该样例readme的"6. L1到L0（LoadData）"小节中对LoadData（卷积数据搬运）v2接口的介绍。

  由于输入数据类型不同，对于load3dv2指令配置参数的影响不大，因此本样例中其余场景可以参考scenarioNum=3和4。

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
  SCENARIO_NUM=4
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：

  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # CPU调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim`、`cpu` | 运行模式：NPU运行、NPU仿真、CPU调试 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU架构：dav-2201对应Atlas A2训练系列产品/Atlas A2推理系列产品/Atlas A3训练系列产品/Atlas A3推理系列产品，dav-3510对应Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`、`2`、`3`、`4`（默认）、`5` | 场景编号 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
