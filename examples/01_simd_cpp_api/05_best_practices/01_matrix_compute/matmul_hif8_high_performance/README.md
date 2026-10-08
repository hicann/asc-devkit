# 使用hifloat8_t实现Matmul性能优化样例

## 概述

本样例基于Matmul高阶API实现`C = A * B`，通过三个场景展示half输入、hifloat8_t输入和Fixpipe随路量化输出的数据通路。三个场景使用相同的矩阵shape和多核Tiling流程。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
| --- | --- |
| Ascend 950PR&950DT系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```text
├── matmul_hif8_high_performance
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   ├── hif8.py             // hifloat8_t数据类型转换脚本
│   │   └── verify_result.py    // 结果校验脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读写辅助函数
│   ├── matmul_hif8.asc         // Kernel与Host实现
│   ├── README.md               // 中文说明文档
│   └── README_en.md            // 英文说明文档
```

## 样例场景

样例矩阵shape为`M = N = K = 1024`，A、B、C均为ND格式且不转置。

| `SCENARIO_NUM` | A/B输入类型 | C输出类型 | 说明 |
| --- | --- | --- | --- |
| 0 | half | float | 未量化基线 |
| 1 | hifloat8_t | float | 使用HIF8输入，结果保持float |
| 2 | hifloat8_t | hifloat8_t | 使用HIF8输入，并通过Fixpipe随路量化输出 |

Case 2在Host Tiling侧配置`DequantType::SCALAR`，在Kernel侧调用`SetQuantScalar`设置值为1.0的量化系数。量化在L0C Buffer结果搬出时完成。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 样例执行

  在本样例目录下执行如下命令。`SCENARIO_NUM`需要同时传给编译工程、数据生成脚本和结果校验脚本。

  ```bash
  SCENARIO_NUM=2                                                                    # 选择执行场景
  mkdir -p build && cd build;                                                       # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py --scenario $SCENARIO_NUM                           # 生成测试输入数据和真值数据
  ./demo                                                                            # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py --scenario $SCENARIO_NUM                      # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式或Scenario前需清理cmake缓存，可在build目录下执行`rm CMakeCache.txt`后重新cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  | --- | --- | --- |
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | NPU运行或NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU硬件架构 |
  | `SCENARIO_NUM` | `0`、`1`、`2` | 选择数据类型和量化场景 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```

## 精度说明

数据生成脚本根据场景生成输入和真值数据。Case 0将half输入转换为float后计算矩阵乘真值，Case 1将hifloat8_t输入转换为float后计算矩阵乘真值，Case 2进一步采用Half to Away Round方式将float真值转换为hifloat8_t。

结果校验脚本对Case 0和Case 1使用相对误差、绝对误差均为`1e-3`的容差进行比较；对Case 2逐元素比较hifloat8_t输出数据和真值数据。
