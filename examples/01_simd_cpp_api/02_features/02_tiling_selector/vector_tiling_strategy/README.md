# 矢量计算多核Tiling切分策略样例

## 概述

本样例以两个一维Tensor相加为例，介绍矢量计算多核Tiling切分策略。该策略根据输入数据量计算不同的核间和核内切分参数，使数据在多个AI Core之间尽量均衡分配，从而充分利用多核计算能力实现高效计算。

样例固定使用8个核，核内主块数据量为3200个`half`元素（即200个DataBlock），通过CMake编译参数`SCENARIO_NUM`选择不同数据量场景。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|--------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```text
├── vector_tiling_strategy
│   ├── scripts
│   │   ├── gen_data.py            // 输入数据和真值数据生成脚本
│   │   └── verify_result.py       // 输出结果验证脚本
│   ├── CMakeLists.txt             // 编译工程文件
│   ├── vector_tiling_strategy.asc // Ascend C样例实现与调用样例
│   ├── data_utils.h               // 数据读入写出函数
│   └── README.md                  // 样例说明文档
```

## 场景详细说明

本样例实现两个一维Tensor相加，计算逻辑为`z = x + y`。样例通过CMake编译参数`SCENARIO_NUM`选择不同数据量场景，对应不同的Tiling切分场景，所有场景数据格式为ND，输入输出均为`half`类型，核函数名为`add_custom`。

样例固定使用8个核，主块数据量为3200个`half`元素，其中“主块”（`mainTileLength`）是用户基于UB可用空间确定的一次可以处理的最大数据量，且满足32字节对齐。

<a name="scenario0-main-tile-only"></a>

### 场景0：主块均分

- 输入：`x`、`y`均为[1, 256000]个`half`元素
- 输出：`z`为[1, 256000]个`half`元素
- Tiling切分：8个核各处理32000个元素；每个核包含10个主块，每个主块3200个元素
- 说明：所有核处理数据量相同，核内只包含主块

<a name="scenario1-main-tile-tail-block"></a>

### 场景1：尾块均分

- 输入：`x`、`y`均为[1, 260096]个`half`元素
- 输出：`z`为[1, 260096]个`half`元素
- Tiling切分：8个核各处理32512个元素；每个核包含10个主块和512个元素的尾块
- 说明：所有核处理数据量相同，每个核在主块后都有等长尾块

<a name="scenario2-main-tile-tail-core"></a>

### 场景2：尾核切分

- 输入：`x`、`y`均为[1, 256064]个`half`元素
- 输出：`z`为[1, 256064]个`half`元素
- Tiling切分：前4个整核各处理32016个元素，后4个尾核各处理32000个元素；整核包含10个主块和16个元素的尾块，尾核包含10个主块
- 说明：前4个整核比尾核多处理1个DataBlock，尾核无尾块

<a name="scenario3-tail-block-tail-core"></a>

### 场景3：尾核尾块切分

- 输入：`x`、`y`均为[1, 258112]个`half`元素
- 输出：`z`为[1, 258112]个`half`元素
- Tiling切分：前4个整核各处理32272个元素，后4个尾核各处理32256个元素；整核包含10个主块和272个元素的尾块，尾核包含10个主块和256个元素的尾块
- 说明：所有核都有尾块，前4个整核再多处理1个DataBlock

## 样例规格

<table border="2">
<caption>表1：样例输入输出规格（场景0）</caption>
<tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 256000]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[1, 256000]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">z</td><td align="center">[1, 256000]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">add_custom</td></tr>
</table>

<table border="2">
<caption>表2：样例输入输出规格（场景1）</caption>
<tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 260096]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[1, 260096]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">z</td><td align="center">[1, 260096]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">add_custom</td></tr>
</table>

<table border="2">
<caption>表3：样例输入输出规格（场景2）</caption>
<tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 256064]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[1, 256064]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">z</td><td align="center">[1, 256064]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">add_custom</td></tr>
</table>

<table border="2">
<caption>表4：样例输入输出规格（场景3）</caption>
<tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 258112]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[1, 258112]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">z</td><td align="center">[1, 258112]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">add_custom</td></tr>
</table>

## Host侧Tiling计算过程

本样例的Tiling计算过程如下所示，Host侧输入为`totalLength`，逐级计算主块数量、尾块数量、尾核数量，输出为Tiling切分结果。

1. 将总数据量按DataBlock对齐。

   ```cpp
   totalLengthAligned = AlignUp(totalLength, alignNum);
   ```

2. 先在所有核上分配相同数量的主块`mainTileLength`。

   ```cpp
   mainTileNum = totalLengthAligned / (numBlocks * mainTileLength);
   mainTileRemainder = totalLengthAligned % (numBlocks * mainTileLength);
   ```

   此时每个核至少处理`mainTileNum * mainTileLength`个元素，`mainTileRemainder`表示完成主块分配后的剩余数据量。当`mainTileRemainder`为0时，进入主块均分场景并结束Tiling计算。

   ```cpp
   if (mainTileRemainder == 0) {
       // 使用MainTileOnlyTiling。
       return;
   }
   ```

3. `mainTileRemainder`不为0时，再按DataBlock给所有核补齐等长尾块。

   ```cpp
   tailBlockNumEachCore = mainTileRemainder / (numBlocks * alignNum);
   remainingTailLength = mainTileRemainder % (numBlocks * alignNum);
   baseLength = mainTileNum * mainTileLength + tailBlockNumEachCore * alignNum;
   ```

   `baseLength`表示每个核处理的基础数据量。`remainingTailLength`表示每核补齐等长DataBlock尾块后尚未分配的元素数量，其值为`alignNum`的整数倍。

4. 判断尾块均分场景。当`remainingTailLength`为0时，表示所有核均分到等长尾块，进入尾块均分场景。

   ```cpp
   if (remainingTailLength == 0) {
       // 使用MainTileWithTailBlockTiling。
       return;
   }
   ```

5. `remainingTailLength`不为0时，将最后剩余的DataBlock分给前若干个整核，并计算整核和尾核的数据长度。

   ```cpp
   formerNum = remainingTailLength / alignNum;
   formerLength = baseLength + alignNum;
   tailLength = baseLength;
   ```

6. 判断尾核切分场景。`tailBlockNumEachCore`为0时，尾核内只包含主块，进入尾核切分场景；否则，进入尾核尾块切分场景。

   ```cpp
   if (tailBlockNumEachCore == 0) {
       // 使用MainTileWithTailCoreTiling。
   } else {
       // 使用TailBlockAndTailCoreTiling。
   }
   ```

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
  SCENARIO_NUM=0  # 设置场景编号
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
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `0`（默认）、`1`、`2`、`3` | 场景编号：0（主块均分）、1（尾块均分）、2（尾核切分）、3（尾核尾块切分） |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
