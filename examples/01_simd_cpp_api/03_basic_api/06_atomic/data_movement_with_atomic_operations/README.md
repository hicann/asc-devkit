# DataCopy伴随原子操作样例

## 概述

本样例基于 `SetAtomicAdd` 和 `SetAtomicMax` 原子操作接口，介绍UB本地张量向GM搬运时，原子累加和原子最大值比较的实现流程。需要注意，在调用原子操作接口完成相关操作后，要调用 `DisableDmaAtomic()` 关闭原子模式，防止后续其他计算受影响。

> **接口提示：** 除本样例使用的 `SetAtomicAdd`、`SetAtomicMax` 接口外，Ascend C还提供了 `SetAtomicMin`接口用于配置 VECOUT到GM的传输规则。`SetAtomicMin` 的调用方式与 `SetAtomicMax` 一致，只需替换函数名即可切换。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── data_movement_with_atomic_operations
│   ├── scripts
│   │   ├── gen_data.py                           // 输入数据和真值数据生成脚本
│   │   └── verify_result.py                      // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                            // 编译工程文件
│   ├── data_utils.h                              // 数据读入写出函数
│   ├── data_movement_with_atomic_operations.asc  // Ascend C样例实现 & 调用样例
│   └── README.md                                 // 样例说明文档
```

## 场景详细说明

本样例通过编译参数 `SCENARIO_NUM` 来切换不同的场景：

<table border="2">
<caption>表1：场景配置对照表</caption>
<tr><th>scenarioNum</th><th>原子操作接口</th><th>输入Shape</th><th>输出Shape</th><th>数据类型</th><th>说明</th></tr>
<tr><td>1</td><td>SetAtomicAdd</td><td>[1, 256]（三个核读取相同）</td><td>[1, 256]</td><td>half</td><td>三个核读取相同输入，原子累加操作</td></tr>
<tr><td>2</td><td>SetAtomicMax</td><td>[1, 256]×3（三个核读取不同）</td><td>[1, 256]</td><td>half</td><td>三个核读取不同输入，原子最大值比较操作</td></tr>
</table>

**场景1：SetAtomicAdd原子累加操作（三个核读取相同输入）**
- 输入shape：src=[1, 256]（三个核同时读取 input_x.bin），dst=[1, 256]（输入input_y.bin表示dst上已有的数据）
- 输出shape：dst=[1, 256]
- 数据类型：half
- 说明：三个核同时读取相同的输入数据（input_x.bin），通过 `SetAtomicAdd` 开启原子累加模式，将各自的数据累加到共享的输出缓冲区，结果为input_y + input_x*3


**场景2：SetAtomicMax原子最大值比较操作（三个核读取不同输入）**
- 输入shape：src0=[1, 256]、src1=[1, 256]、src2=[1, 256]（三个核分别读取 input_x0.bin、input_x1.bin、input_x2.bin）
- 输出shape：dst=[1, 256]
- 数据类型：half
- 说明：三个核通过 `GetBlockIdx()` 获取自身编号，分别读取不同的输入数据（input_x0.bin、input_x1.bin、input_x2.bin）。input_y.bin会先作为GM上已有dst初值拷贝到输出缓冲区，通过 `SetAtomicMax` 开启原子最大值比较模式后，每位输出为三个输入和dst初值中的最大值。

## 样例描述

- 样例规格：
  <table>
  <caption>表2：样例规格说明</caption>
  <tr><td rowspan="5" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td rowspan="1" align="center">input_x / input_x0</td><td align="center">[1, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">input_x1</td><td align="center">[1, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">input_x2</td><td align="center">[1, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">input_y</td><td align="center">[1, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">dst</td><td align="center">[1, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">data_movement_with_atomic_operations_custom</td></tr>
  <tr><td rowspan="1" align="center">并行block数</td><td colspan="4" align="center">3</td></tr>
  </table>

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
  SCENARIO_NUM=1  # 设置场景编号（取值为1、2）
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # 编译工程
  python3 ../scripts/gen_data.py -scenarioNum $SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin  # 验证输出结果是否正确
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
  | `SCENARIO_NUM` | `1`（默认）、`2` | 场景编号：1（SetAtomicAdd原子累加）、2（SetAtomicMax原子最大值比较） |

- 执行结果

  执行结果如下，说明精度对比成功：

  ```bash
  test pass!
  ```
