# 归约类接口多场景示例

## 概述

本样例介绍归约类接口`ReduceRepeat`在多种场景下的使用方法。这些接口用于对LocalTensor中每个repeat内所有元素进行归约运算（求最大值、最小值或求和），归约结果存放到目的LocalTensor中。

注：`ReduceRepeat`为 CANN 9.1.0 重命名后的 API。CANN 9.0.0 及之前版本请使用`WholeReduceMax`，`WholeReduceMin`，`WholeReduceSum`。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.1.0 |
| Atlas A2系列产品 | >= CANN 9.1.0 |

## 目录结构介绍

``` text
├── reduce_repeat
│   ├── scripts
│   │   ├── gen_data.py                    // 输入数据和真值数据生成脚本
│   │   └── verify_result.py               // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                     // 编译工程文件
│   ├── data_utils.h                       // 数据读入写出函数
│   ├── reduce_repeat.asc                  // Ascend C样例实现 & 调用样例
│   └── README.md                          // 样例说明文档
```

## 场景详细说明

本样例通过编译参数 `SCENARIO_NUM` 选择不同的归约场景，所有场景数据格式为 ND，核函数名为 `reduce_repeat_custom`。

**场景1：`ReduceRepeat<MAX>`**

- 输入：[1, 1024]个half元素，mask=128，repeat=8（1024/128）
- 输出：[1, 8]个half元素（8个最大值，不返回索引）
- 实现：`ReduceRepeat<ReduceType::MAX, half>(dstLocal, srcLocal, mask=128, repeat=8, 1, 1, 8, AscendC::ReduceOrder::ORDER_ONLY_VALUE)`
- 说明：使用`ORDER_ONLY_VALUE`参数，只返回最大值不返回索引，输出存储顺序为[max0, max1, max2, ...]

**场景2：`ReduceRepeat<MIN>`**

- 输入：[1, 1024]个half元素，mask=128，repeat=8（1024/128）
- 输出：[1, 16]个half元素（8个最小值 + 8个索引，交错存储）
- 实现：`ReduceRepeat<ReduceType::MIN, half>(dstLocal, srcLocal, mask=128, repeat=8, 1, 1, 8)`
- 说明：使用默认`order=ORDER_VALUE_INDEX`，输出按 [min0, idx0, min1, idx1, ...] 交错存储

**场景3：`ReduceRepeat<SUM>`**

- 输入：[1, 2048]个float元素，mask=64（float类型32位，mask范围为[1,64]），repeat=32（2048/64）
- 输出：[1, 32]个float元素（每个repeat的求和结果）
- 实现：`ReduceRepeat<ReduceType::SUM, float>(dstLocal, srcLocal, mask=64, repeat=32, 1, 1, 8)`
- 说明：每个repeat独立求和，共输出32个求和结果

**场景4：`ReduceRepeat<SUM>`非对齐场景**

- 输入：[13, 57]个float元素（13行×57列，列数57×4字节=228字节，非32字节对齐）
- 输出：[1, 13]个float元素（每行的求和结果）
- 实现：使用 `DataCopyPad` 搬入非对齐数据，`ReduceRepeat<ReduceType::SUM, float>(dstLocal, srcLocal, mask=57, repeat=13, 1, 1, srcStride)` 对每行求和
- 说明：演示非对齐数据场景下的归约操作。每行57个float元素（228字节），使用`DataCopyPad`搬运时按32字节对齐填充至232字节（58个float元素），`ReduceRepeat<SUM>`的srcRepeatStride按对齐后的block数计算

## 样例规格

<table border="2">
<caption>表1：样例输入输出规格（场景1）</caption>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">z</td><td align="center">[1, 8]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">reduce_repeat_custom</td></tr>
</table>

<table border="2">
<caption>表2：样例输入输出规格（场景2）</caption>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">z</td><td align="center">[1, 16]</td><td align="center">half</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">reduce_repeat_custom</td></tr>
</table>

<table border="2">
<caption>表3：样例输入输出规格（场景3）</caption>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">z</td><td align="center">[1, 32]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">reduce_repeat_custom</td></tr>
</table>

<table border="2">
<caption>表4：样例输入输出规格（场景4）</caption>
<tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[13, 57]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="2" align="center">样例输出</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">z</td><td align="center">[1, 13]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">reduce_repeat_custom</td></tr>
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
  SCENARIO_NUM=1  # 设置场景编号
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake .. -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM;make -j;    # 编译工程
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM ./output/output.bin ./output/golden.bin  # 验证输出结果是否正确
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
  | `SCENARIO_NUM` | `1`（默认）、`2`、`3`、`4` | 场景编号：1（`ReduceRepeat<MAX>`）、2（`ReduceRepeat<MIN>`）、3（`ReduceRepeat<SUM>`）、4（非对齐`ReduceRepeat<SUM>`） |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
