# AddBroadcast算子直调样例

## 概述

本样例介绍Add算子的核函数直调方法，多核&tiling场景下增加输入Broadcast。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── add_broadcast
│   ├── scripts
│   │   ├── gen_data.py          // 输入数据和真值数据生成脚本
│   │   └── verify_result.py     // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt           // 编译工程文件
│   ├── data_utils.h             // 数据读入写出函数
│   ├── add_broadcast.asc        // Ascend C算子实现 & 调用样例
│   └── README.md                // 样例说明文档
```

## 算子描述

- 算子功能：  
  Add算子实现了两个数据相加，返回相加结果的功能。对应的数学表达式为：
  ```
  z = x + y
  ```
- 算子规格：
  <table>
  <tr><td rowspan="1" align="center">算子类型(OpType)</td><td colspan="4" align="center">Add</td></tr>
  <tr><td rowspan="3" align="center">算子输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">axis = 0: (8, 1024) / (8, 1022) / (17, 1024) / (17, 1022) axis = 1: (16, 1) / (16, 1) / (20, 1) / (20, 1)
  </td><td align="center">bfloat16_t/int8_t/float/half/int16_t/int32_t</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">axis = 0: (1, 1024) / (1, 1022) / (1, 1024) / (1, 1022) axis = 1: (16, 256) / (16, 255) / (20, 256) / (20, 255)</td><td align="center">bfloat16_t/int8_t/float/half/int16_t/int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">算子输出</td><td align="center">z</td><td align="center">axis = 0: (8, 1024) / (8, 1022) / (17, 1024) / (17, 1022) axis = 1: (16, 256) / (16, 255) / (20, 256) / (20, 255)</td><td align="center">bfloat16_t/int8_t/float/half/int16_t/int32_t</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">add_broadcast_custom</td></tr>
  </table>

- 算子实现：  
  
  计算逻辑是：Ascend C提供的矢量计算接口的操作元素都为LocalTensor，输入数据需要先搬运进片上存储，然后使用计算接口完成两个输入参数相加，得到最终结果，再搬出到外部存储上。

    Add算子的实现流程分为3个基本任务：CopyIn，Compute，CopyOut。
    CopyIn任务负责将Global Memory上的输入Tensor xGm和yGm搬运到Local Memory，分别存储在xLocal、yLocal；
    Compute任务负责对xLocal、yLocal进行Broadcast并存入临时内存，之后执行加法操作，计算结果存储在zLocal中；
    CopyOut任务负责将输出数据从zLocal搬运至Global Memory上的输出Tensor zGm中。

  - Tiling实现  
    TilingData参数设计，TilingData参数本质上是和并行数据切分相关的参数。

    本样例尝试将输入数据均分在给定数目的核上计算，如不能均分，则计算整核和尾核的个数，整核和尾核分别处理不同数据量的数据。定义UB_BLOCK_NUM为最大可用的UB数据块，对应UB的大小为UB_BLOCK_NUM * 32 Bytes。 并以UB_BLOCK_NUM为粒度进行tiling切分。

    本示例算子使用了20个tiling参数： xLen、yLen、dataType为x, y输入数据的总长度和数据类型；coef为需要Broadcast的输入扩维的倍数；axis为输入数据Broadcast的轴；blockLength、tileNum、 tileLength、lastTileLength为核均分场景下每个核上的计算量、tiling切分个数以及整块和尾块长度；former(tail)Num、former(tail)Length、former(tail)TileNum、former(tail)TileLength、former(tail)LastTileLength为核不均分的场景下整核和尾核相关的切分数据，与核均分场景下对应；isEvenCore用来判断当前是否为核均分场景。

  - 调用实现  
    使用内核调用符<<<>>>调用核函数。

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
  mkdir -p build && cd build;
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201;make -j; # 默认npu模式
  python3 ../scripts/gen_data.py   # 生成测试输入数据
  ./demo
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
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