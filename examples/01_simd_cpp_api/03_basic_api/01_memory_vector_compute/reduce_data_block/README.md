# ReduceDataBlock类接口多场景示例

## 概述

本样例在归约场景下，基于`ReduceDataBlock`实现类接口的多场景归约功能，对输入Tensor的每个DataBlock（32字节）内所有元素进行归约运算（求最大值、最小值或求和）。

注：`ReduceDataBlock`为 CANN 9.1.0 重命名后的 API。CANN 9.0.0 及之前版本请使用`BlockReduceMax`，`BlockReduceMin`，`BlockReduceSum`。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.1.0 |
| Atlas A2系列产品 | >= CANN 9.1.0 |

## 目录结构介绍

``` text
├── reduce_data_block
│   ├── scripts
│   │   ├── gen_data.py                    // 输入数据和真值数据生成脚本
│   │   └── verify_result.py               // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                     // 编译工程文件
│   ├── data_utils.h                       // 数据读入写出函数
│   ├── reduce_data_block.asc              // Ascend C样例实现 & 调用样例
│   └── README.md                          // 样例说明文档
```

## 场景说明

本样例通过编译参数 `SCENARIO_NUM` 选择不同的归约场景，所有场景数据格式为 ND，核函数名为 `reduce_data_block_custom`。

**场景1：`ReduceDataBlock<MAX, half>`**

- 输入：[1, 256]个half元素，mask=128（256/sizeof(half)），repeat=2
- 输出：[1, 16]个half元素（对应16个datablock各自的最大值）
- 实现：`ReduceDataBlock<ReduceType::MAX, half>(dstLocal, srcLocal, mask=128, repeat=2, dstRepStride=1, srcBlkStride=1, srcRepStride=8)`
- 说明：对每个datablock内所有元素求最大值，一个datablock处理32字节即16个half元素，256个元素共16个datablock，输出16个最大值

**场景2：`ReduceDataBlock<MIN, half>`**

- 输入：[4, 128]个half元素，mask=128（256/sizeof(half)），repeat=4
- 输出：[4, 8]个half元素（对应32个datablock各自的最小值）
- 实现：`ReduceDataBlock<ReduceType::MIN, half>(dstLocal, srcLocal, mask=128, repeat=4, dstRepStride=1, srcBlkStride=1, srcRepStride=8)`
- 说明：对每个datablock内所有元素求最小值，一个datablock处理32字节即16个half元素，512个元素共32个datablock，输出32个最小值

**场景3：`ReduceDataBlock<SUM, float>`**

- 输入：[1, 128]个float元素，mask=64（256/sizeof(float)），repeat=2
- 输出：[1, 16]个float元素（对应16个datablock各自的求和结果）
- 实现：`ReduceDataBlock<ReduceType::SUM, float>(dstLocal, srcLocal, mask=64, repeat=2, dstRepStride=1, srcBlkStride=1, srcRepStride=8)`
- 说明：对每个datablock内所有元素求和，源操作数相加采用二叉树方式两两相加，一个datablock处理32字节即8个float元素，128个元素共16个datablock，输出16个求和结果

## 样例规格

<table border="2">
<caption>表1：样例输入输出规格（场景1：ReduceDataBlock&lt;MAX&gt;）</caption>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 256]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">y</td><td align="center">[1, 16]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">reduce_data_block_custom</td></tr>
</table>

<table border="2">
<caption>表2：样例输入输出规格（场景2：ReduceDataBlock&lt;MIN&gt;）</caption>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[4, 128]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">y</td><td align="center">[4, 8]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">reduce_data_block_custom</td></tr>
</table>

<table border="2">
<caption>表3：样例输入输出规格（场景3：ReduceDataBlock&lt;SUM&gt;）</caption>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">y</td><td align="center">[1, 16]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">reduce_data_block_custom</td></tr>
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
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM ./output/output.bin ./output/golden.bin  # 验证输出结果是否正确
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
