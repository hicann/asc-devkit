# Transpose类样例

## 概述

本样例基于Transpose、TransDataTo5HD接口实现数据转置功能，详细介绍该功能在普通转置、增强转置和5HD格式转换三种场景下的使用方法，主要覆盖三类数据转换需求：（1）实现16*16的二维矩阵数据块转置，（2）完成[N,C,H,W]与[N,H,W,C]四维矩阵格式的互相转换，（3）NCHW格式转换成NC1HWC0格式。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── transpose
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── transpose.asc           // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：
  transpose类样例实现矩阵转置功能，支持普通转置、增强转置和5HD格式转换三种场景:

  1. 普通转置，支持16*16的二维矩阵数据块进行转置

  2. 增强转置，通过transposeParams指定转置类型，支持16*16的二维矩阵数据块转置，支持[N,C,H,W]与[N,H,W,C]四维矩阵互相转换

  3. 5HD格式转换，支持16*16的二维矩阵数据块转置，支持[N,C,H,W]四维格式转换成[N,C1,H,W,C0]五维格式

- 场景说明：

  样例可通过编译参数 `SCENARIO_NUM` 来切换不同的场景，参数详见下表：

  | 场景编号 | 场景名称 | 输入shape | 输出shape | 数据类型 | 输入格式 | 输出格式 | 转置类型 |
  |------|--------|------|------|--------|------|------|------|
  | 1 | 普通转置 | [16,16] | [16,16] | half | ND | ND | / |
  | 2 | 增强转置 | [3,3,2,8] | [3,2,8,3] | half | NCHW | NHWC | TRANSPOSE_NCHW2NHWC |
  | 3 | 5HD格式转换 | [2,32,16,16] | [2,2,16,16,16] | half | NCHW | NC1HWC0 | / |

- 数据格式说明：  
  卷积神经网络的特征图（Feature Map）通常用四维数组保存，即4D，4D格式解释如下：
    - N：Batch数量。
    - H：Height，特征图高度。
    - W：Width，特征图宽度。
    - C：Channels，特征图通道。

  由于数据只能线性存储，因此这四个维度有对应的顺序，不同深度学习框架会按照不同的顺序存储特征图数据，比如在TensorFlow中，排列顺序为[Batch, Height, Width, Channels]，即NHWC。

  5HD格式（NC1HWC0）是昇腾NPU特有的数据排布格式，其中：
    - N：Batch数量。
    - C1：C1 = ceil(C / C0)。如果结果不整除，向下取整。
    - H：Height，特征图高度。
    - W：Width，特征图宽度。
    - C0：等于AI Core中矩阵计算单元的大小，如果是数据类型的位宽为32位或者16位，则C0=16；如果数据类型的位宽为8位，则C0=32。本样例为16。

    NHWC/NCHW -> NC1HWC0的转换过程为：将数据在C维度进行分割，变成C1份NHWC0/NC0HW，再将C1份NHWC0/NC0HW在内存中连续排列成NC1HWC0。

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
  SCENARIO_NUM=1      # 设置场景编号（1=普通转置，2=增强转置，3=5HD格式转换）
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # 编译工程
  python3 ../scripts/gen_data.py -scenario_num=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin  # 验证输出结果是否正确
  ```
  使用CPU调试或NPU仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  
  示例如下：
  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # CPU调试模式
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |
  | `SCENARIO_NUM` | `1`（默认）、`2`、`3` | 场景编号：1（普通转置）、2（增强转置）、3（5HD格式转换） |

- 执行结果

  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```


