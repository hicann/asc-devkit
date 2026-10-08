# Copy接口样例

## 概述

本样例基于Copy接口实现UB（Unified Buffer）内部数据搬运功能，适用于需要在VECIN、VECCALC、VECOUT等不同TPosition之间搬运数据的场景。样例支持通过编译参数切换不同场景，便于开发者理解Copy接口的使用方法。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── copy_ub2ub
│   ├── scripts
│   │   ├── gen_data.py             // 输入数据和真值数据生成脚本
│   │   └── verify_result.py        // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── data_utils.h                // 数据读入写出函数
│   ├── copy.asc                    // Ascend C样例实现 & 调用样例
│   └── README.md                   // 样例说明文档
```

## 场景说明

本样例通过编译参数 `SCENARIO_NUM` 选择不同场景，所有场景数据格式为 ND，核函数名为 `copy_custom`。

<table border="2">
<caption>表1：场景配置对照表</caption>
<tr><th>scenarioNum</th><th>输入Shape</th><th>输出Shape</th><th>计算模式</th><th>说明</th></tr>
<tr><td>1</td><td>[1, 512]</td><td>[1, 512]</td><td>tensor高维切分计算</td><td>源操作数和目的操作数空间一样大</td></tr>
<tr><td>2</td><td>[18, 64]</td><td>[18, 8]</td><td>tensor高维切分计算</td><td>源操作数和目的操作数空间不同</td></tr>
<tr><td>3</td><td>[18, 64]</td><td>[18, 8]</td><td>Counter模式</td><td>源操作数和目的操作数空间不同</td></tr>
</table>

### 场景参数说明

**tensor高维切分计算**：通过mask参数控制每次迭代内参与计算的元素个数，每个DataBlock大小为32B，包含8个元素（int32类型下）。通过repeatTime参数控制迭代次数，stride参数控制源操作数和目的操作数的地址步长。

**Counter模式**：mask参数表示每次Repeat处理的元素个数，参与计算的元素个数为repeatTime * mask。通过SetMaskCount设置计算模式，通过SetVectorMask设置mask。

**stride参数**：{dstStride, srcStride, dstRepeatSize, srcRepeatSize}控制源操作数和目的操作数在同一迭代内和相邻迭代间的地址步长。

- **场景1**：tensor高维切分计算，mask=64，repeatTime=8，stride={1, 1, 8, 8}。源操作数和目的操作数空间一样大，每次迭代处理64个元素，迭代8次，共搬运512个元素。

- **场景2**：tensor高维切分计算，mask=8，repeatTime=18，stride={1, 1, 1, 8}。从[18, 64]搬运[18, 8]，srcRepeatSize=8表示源操作数每次Repeat跳过64个元素（跳到下一行），dstRepeatSize=1表示目的操作数紧凑排列，共搬运144个元素。

- **场景3**：Counter模式，mask=144，repeatTime=1，stride={1, 8, 8, 8}。从[18, 64]搬运[18, 8]，srcStride=8表示源操作数每个DataBlock地址步长为8（取每一行的前8个元素），共搬运144个元素。

## 样例描述

- 样例规格
  <table border="2">
  <caption>表2：样例规格</caption>
  <tr>
    <td align="center">类别</td>
    <td align="center">name</td>
    <td align="center">shape</td>
    <td align="center">data type</td>
    <td align="center">format</td>
  </tr>
  <tr>
    <td align="center">样例输入</td>
    <td align="center">x</td>
    <td align="center">[1, 512]/[18, 64]</td>
    <td align="center">int32</td>
    <td align="center">ND</td>
  </tr>
  <tr>
    <td align="center">样例输出</td>
    <td align="center">z</td>
    <td align="center">[1, 512]/[18, 8]</td>
    <td align="center">int32</td>
    <td align="center">ND</td>
  </tr>
  <tr>
    <td align="center">核函数名</td>
    <td colspan="4" align="center">copy_custom</td>
  </tr>
  </table>

- 样例实现
  - Kernel实现
    - 调用DataCopy基础API，将数据从GM（Global Memory）搬运到UB（Unified Buffer）
    - 调用Copy接口，将数据从UB（Unified Buffer）搬运到UB（Unified Buffer），支持tensor高维切分计算和Counter模式两种计算模式
    - 调用DataCopy基础API，将数据从UB（Unified Buffer）搬运到GM（Global Memory）

- 调用实现  
  使用内核调用符<<<>>>调用核函数。

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
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin  # 验证输出结果是否正确
  ```

  使用CPU调试或NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=cpu`或`-DCMAKE_ASC_RUN_MODE=sim`参数即可。
  
  示例如：

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |
  | `SCENARIO_NUM` | `1`（默认）、`2`、`3` | 场景编号 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
