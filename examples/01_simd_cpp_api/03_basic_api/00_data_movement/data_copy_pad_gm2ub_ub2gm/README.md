# DataCopyPad样例

## 概述

本样例在数据搬运场景下，基于DataCopyPad API实现非32字节对齐数据的搬运及填充功能。DataCopyPad API支持从Global Memory到Local Memory的非对齐数据搬运，并可在数据左侧或右侧填充指定数值。

数据搬运过程包括：Global Memory（GM）→Unified Buffer（UB）（使用DataCopyPad进行非对齐搬运并填充）→Global Memory（GM）。本样例使用静态Tensor方式分配UB内存，支持通过编译参数切换不同场景，演示DataCopyPad的不同使用方式。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── data_copy_pad_gm2ub_ub2gm
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── figures                 // 图示
│   ├── data_copy_pad.asc       // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 场景说明

本样例通过编译参数 `SCENARIO_NUM` 选择不同场景，所有场景数据格式为 ND，核函数名为 `data_copy_pad_custom`。

<table border="2">
<caption>表1：场景配置对照表</caption>
<tr><th>scenarioNum</th><th>输入Shape</th><th>输出Shape</th><th>数据类型</th><th>填充/搬运模式</th><th>说明</th></tr>
<tr><td>1</td><td>[1, 20]</td><td>[1, 32]</td><td>half</td><td>SetPadValue填充</td><td>右侧填充12个元素，需配合SetPadValue设置填充值为1</td></tr>
<tr><td>2</td><td>[32, 59]</td><td>[32, 64]</td><td>float</td><td>rightPadding填充</td><td>右侧填充5个元素，填充值默认为0，无需SetPadValue</td></tr>
<tr><td>3</td><td>[3, 24]</td><td>[1, 80]</td><td>half</td><td>Compact填充</td><td>Compact模式，最后一个数据块右侧填充16字节（仅支持dav-3510）</td></tr>
<tr><td>4</td><td>[1, 320]</td><td>[1, 576]</td><td>int8</td><td>LoopMode搬运（Compact）</td><td>SetLoopModePara使能loop mode，Compact模式，实现GM→UB非连续stride搬运（仅支持dav-3510）</td></tr>
<tr><td>5</td><td>[1, 320]</td><td>[1, 576]</td><td>int8</td><td>LoopMode搬运（Normal）</td><td>SetLoopModePara使能loop mode，Normal模式，实现GM→UB非连续stride搬运（仅支持dav-3510）</td></tr>
<tr><td>6</td><td>[2, 4, 3, 128, 126]</td><td>[512, 128]</td><td>int8</td><td>LoopMode搬运（Normal）</td><td>五维数据搬运，搬运[2, 2, 2, 64, 126]，每行补2字节padding到128字节（仅支持dav-3510）</td></tr>
</table>

### 场景详细说明

**场景1：使用SetPadValue进行自定义填充**
- 输入shape：[1, 20]
- 输出shape：[1, 32]
- 数据类型：half
- 参数配置：isPad=false, leftPadding=0, rightPadding=12
- 说明：使用SetPadValue设置填充值为1，右侧填充12个元素。**SetPadValue需要用户额外调用设置填充值**，配合isPad=false使用。

**场景2：使用rightPadding进行默认填充**
- 输入shape：[32, 59]
- 输出shape：[32, 64]
- 数据类型：float
- 参数配置：isPad=true, leftPadding=0, rightPadding=5
- 说明：**无需使用SetPadValue**，isPad=true时填充值默认为0，右侧填充5个元素。

**场景3：使用Compact模式进行数据搬运 ----此场景仅在 Ascend 950PR&950DT系列产品支持**
- 输入shape：[3, 24]
- 输出shape：[1, 80]
- 数据类型：half
- 参数配置：blockLen=48, blockCount=3, leftPadding=0, rightPadding=16, isPad=false
- 说明：紧凑模式，允许单次搬运不对齐，统一在整块数据末尾补齐至32字节对齐。此处示例中，leftPadding为0，rightPadding为16，在最后一个数据块右侧填充16字节。目的操作数的数据量为160字节。

**场景4：使用SetLoopModePara使能loop mode（Compact模式） ----此场景仅在 Ascend 950PR&950DT系列产品支持**
- 输入shape：[1, 320]，如图 1 所示
- 输出shape：[1, 576]，如图 2 所示
- 数据类型：int8
- 参数配置：
  - GM→UB：LoopModeParams{loop1Size=2, loop2Size=2, loop1SrcStride=80, loop1DstStride=128, loop2SrcStride=160, loop2DstStride=288}，DataCopyMVType::OUT_TO_UB
  - DataCopyExtParams：BLOCK_COUNT=2, BLOCK_LEN=40（使用constexpr常量）
  - DataCopyPadExtParams：isPad=true, leftPadding=0, rightPadding=0, padValue=-1
- 说明：通过SetLoopModePara使能loop mode，使用Compact模式实现GM→UB的非连续stride数据搬运。Compact模式下每次内层循环搬运80B后填充16B使其96B对齐，padding值设为-1。

**图 1**  源操作数搬运场景示例

<img src="figures/datacopypad1.png" width="80%">

**图 2**  目的操作数Compact模式搬运场景示例

<img src="figures/datacopypad2.png">

**场景5：使用SetLoopModePara使能loop mode（Normal模式） ----此场景仅在 Ascend 950PR&950DT系列产品支持**
- 输入shape：[1, 320]，如图 1 所示
- 输出shape：[1, 576]，如图 3 所示
- 数据类型：int8
- 参数配置：
  - GM→UB：LoopModeParams{loop1Size=2, loop2Size=2, loop1SrcStride=80, loop1DstStride=128, loop2SrcStride=160, loop2DstStride=288}，DataCopyMVType::OUT_TO_UB
  - DataCopyExtParams：BLOCK_COUNT=2, BLOCK_LEN=40（使用constexpr常量）
  - DataCopyPadExtParams：isPad=true, leftPadding=0, rightPadding=0, padValue=-1
- 说明：通过SetLoopModePara使能loop mode，使用Normal模式实现GM→UB的非连续stride数据搬运。Normal模式下每个block搬运后都会填充24B使其64B对齐，padding值设为-1。

**图 3**  目的操作数Normal模式搬运场景示例

<img src="figures/datacopypad3.png">

**场景6：使用SetLoopModePara使能loop mode（Normal模式）进行五维数据搬运 ----此场景仅在 Ascend 950PR&950DT系列产品支持**
- 输入shape：[2, 4, 3, 128, 126]，五维数据
- 输出shape：[512, 128]，UB中连续存放
- 数据类型：int8
- 搬运规格：[2, 2, 2, 64, 126]，每个126字节补2字节padding到128字节
- 参数配置：
  - GM→UB：LoopModeParams{loop1Size=2, loop2Size=2, loop1SrcStride=128\*126, loop1DstStride=64\*128, loop2SrcStride=3\*128\*126, loop2DstStride=2\*64\*128}，DataCopyMVType::OUT_TO_UB
  - DataCopyExtParams：blockCount=64, blockLen=126, srcStride=0, dstStride=0
  - DataCopyPadExtParams：isPad=true, leftPadding=0, rightPadding=0, padValue=0
  - 第0维使用for循环搬运2次

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
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin -scenarioNum=$SCENARIO_NUM  # 验证输出结果是否正确
  ```

  使用NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=sim`参数即可。

  示例如下：

  ```bash
  cmake .. -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`sim` | 运行模式：NPU 运行、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | dav-2201 对应 Atlas A2系列产品和Atlas A3系列产品、dav-3510 对应 Ascend 950PR&950DT系列产品 |
  | `SCENARIO_NUM` | `1`（默认）、`2`、`3`、`4`、`5`、`6` | 场景编号：1（SetPadValue填充）、2（rightPadding）、3（Compact模式）、4（SetLoopModePara loop mode Compact）、5（SetLoopModePara loop mode Normal）、6（五维LoopMode Normal） |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
