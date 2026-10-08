# Element-wise逻辑运算样例

## 概述

本样例基于And、Ors、ShiftLeft、ShiftRight接口实现按位逻辑运算功能。And接口对两个源操作数进行按位与运算。Ors接口对矢量内每个元素和标量间做或操作。ShiftLeft接口对源操作数进行左移操作（tensor形式）。ShiftRight接口对源操作数进行右移操作（scalar形式）。样例支持通过编译参数切换不同场景，便于开发者理解这些接口的使用方法和实现差异。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── element_wise_logic
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── element_wise_logic.asc  // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 场景详细说明

本样例通过编译参数 `SCENARIO_NUM` 来切换不同的场景：

<table border="2">
<caption>表1：场景配置对照表</caption>
<tr><th>scenarioNum</th><th>接口</th><th>输入Shape</th><th>输出Shape</th><th>数据类型</th><th>说明</th></tr>
<tr><td>1</td><td>And</td><td>[1, 512], [1, 512]</td><td>[1, 512]</td><td>uint16</td><td>对两个源操作数进行按位与运算</td></tr>
<tr><td>2</td><td>Ors</td><td>[1, 512], [1, 512]</td><td>[1, 512]</td><td>uint16</td><td>标量在前，src0[0]作为标量与src1矢量做或操作（仅支持 dav-3510）</td></tr>
<tr><td>3</td><td>ShiftLeft</td><td>[1, 512], [1, 512]</td><td>[1, 512]</td><td>uint16</td><td>对源操作数进行左移操作，左移位数由tensor指定（仅支持 dav-3510）</td></tr>
<tr><td>4</td><td>ShiftRight</td><td>[1, 512]</td><td>[1, 512]</td><td>uint16</td><td>对源操作数进行右移操作，右移位数由常量SHIFT_BITS=2指定</td></tr>
</table>

**场景1：And按位与运算**
- 输入shape：src0=[1, 512], src1=[1, 512]
- 输出shape：dst=[1, 512]
- 数据类型：uint16
- 参数：count=512
- 实现：

    ```cpp
    AscendC::And(dstLocal, src0Local, src1Local, COUNT);
    ```

- 说明：对src0和src1中的每个元素进行按位与运算，结果存入dst
- 示例：
  - 输入src0: [1 2 3 ... 512]
  - 输入src1: [512 511 510 ... 1]
  - 输出dst: [1 0 3 ... 0]

**场景2：Ors矢量与标量或操作（标量在前）** ----此场景仅在 Ascend 950PR&950DT系列产品支持**
- 输入shape：src0=[1, 512]（取src0Local[0]作为标量），src1=[1, 512]（矢量）
- 输出shape：dst=[1, 512]
- 数据类型：uint16
- 参数：count=512
- 实现：

    ```cpp
    static constexpr AscendC::BinaryConfig config = { 0 };
    AscendC::Ors<AscendC::BinaryDefaultType, true, config>(dstLocal, src0Local[0], src1Local, COUNT);
    ```

- 说明：标量在前，src0Local[0]作为标量（左操作数），src1Local作为矢量（右操作数），对src1中的每个元素与src0Local[0]进行按位或运算，结果存入dst
- 示例：
  - 输入src0[0]: 1
  - 输入src1: [1 2 3 ... 512]
  - 输出dst: [1 3 3 5 5 .. 513]

**场景3：ShiftLeft左移操作（tensor形式）** ----此场景仅在 Ascend 950PR&950DT系列产品支持**
- 输入shape：src0=[1, 512]（待移位数据，uint16），src1=[1, 512]（左移位数，int16）
- 输出shape：dst=[1, 512]
- 数据类型：uint16
- 参数：count=512
- 实现：

    ```cpp
    AscendC::ShiftLeft(dstLocal, src0Local, src1Local, COUNT);
    ```

- 说明：对src0中的每个元素按照src1中对应的左移位数进行左移操作，结果存入dst。src1存放左移位数，不支持设置为负数
- 示例：
  - 输入src0: [1 2 3 ... 512]
  - 输入src1: [2 2 2 ... 2]（左移位数）
  - 输出dst: [4 8 12 ... 2048]

**场景4：ShiftRight右移操作（scalar形式）**
- 输入shape：src0=[1, 512]
- 输出shape：dst=[1, 512]
- 数据类型：uint16
- 参数：count=512, SHIFT_BITS=2
- 实现：

    ```cpp
    AscendC::ShiftRight(dstLocal, src0Local, SHIFT_BITS, COUNT);
    ```

- 说明：对src0中的每个元素右移SHIFT_BITS位，结果存入dst。无符号数据类型执行逻辑右移，有符号数据类型执行算术右移
- 示例：
  - 输入src0: [1 2 3 4 5 ... 512]
  - 输出dst: [0 0 0 1 1 1 1 2 2 2 ... 128]

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
  SCENARIO_NUM=1  # 设置场景编号
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # 编译工程
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
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
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品。注意：场景2和场景3仅支持 dav-3510，编译时会自动切换 |
  | `SCENARIO_NUM` | `1`（默认）、`2`、`3`、`4` | 场景编号：1（And按位与）、2（Ors矢量标量或）、3（ShiftLeft左移tensor形式）、4（ShiftRight右移scalar形式） |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
