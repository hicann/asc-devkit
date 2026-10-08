# MrgSort样例

## 概述

本样例在排序场景下，基于Sort32与MrgSort实现多路归并排序功能。首先调用Sort32将数据并行地预处理为多个有序子序列（每32个元素为一组进行降序排序，形成以(score, index)交替结构存储的有序队列）；随后调用MrgSort，将这些子序列合并为一个全局有序的结果。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── mrg_sort
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── mrg_sort.asc            // Ascend C 样例实现 & 调用
│   └── README.md               // 样例说明文档
```

## 场景说明

本样例通过编译参数 `SCENARIO_NUM` 选择不同场景，所有场景数据格式为 ND，核函数名为 `vec_mrgsort_kernel`。

**场景1：4条队列排序**
- 输入：128个float元素（score） + 128个uint32元素（index）
- 输出：[1, 256]个float元素（128*2个排序结果）
- 实现：Sort32 repeat=4，将128个元素分为4组分别降序排序；MrgSort validBit=0b1111，ifExhaustedSuspension=false，repeatTimes=1，将4条队列合并为1条有序队列
- 说明：演示4条队列完全合并为1条有序队列的场景

**场景2：3条队列非4对齐合并**
- 输入：96个float元素（score） + 96个uint32元素（index）
- 输出：[1, 192]个float元素（96*2个排序结果）
- 实现：Sort32 repeat=3，将96个元素分为3组分别降序排序；MrgSort validBit=0b0111，ifExhaustedSuspension=false，repeatTimes=1，将3条队列合并为1条有序队列
- 说明：演示非4对齐情况下的合并，validBit=0b0111表示前3条队列有效，第4条队列长度设为0

**场景3：32条队列多轮归并排序**
- 输入：1024个float元素（score） + 1024个uint32元素（index）
- 输出：[1, 2048]个float元素（1024*2个排序结果）
- 实现：Sort32 repeat=32，将1024个元素分为32组分别降序排序；第一轮MrgSort repeatTimes=8，每4条队列合并为1条，得到8条有序队列；第二轮MrgSort repeatTimes=2，每4条队列合并为1条，得到2条有序队列；第三轮MrgSort2将2条队列合并为1条全局有序队列
- 说明：演示多路归并排序场景，通过多轮归并将32条有序队列合并为1条全局有序队列

## 样例规格

<table border="2">
<caption>表1：样例输入输出规格（场景1）</caption>
<tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 128]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[1, 128]</td><td align="center">uint32</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[1, 256]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">vec_mrgsort_kernel</td></tr>
</table>

<table border="2">
<caption>表2：样例输入输出规格（场景2）</caption>
<tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 96]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[1, 96]</td><td align="center">uint32</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[1, 192]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">vec_mrgsort_kernel</td></tr>
</table>

<table border="2">
<caption>表3：样例输入输出规格（场景3）</caption>
<tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
<tr><td align="center">x</td><td align="center">[1, 1024]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td align="center">y</td><td align="center">[1, 1024]</td><td align="center">uint32</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">样例输出</td><td align="center">output</td><td align="center">[1, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
<tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">vec_mrgsort_kernel</td></tr>
</table>

## 编译运行

在本样例根目录下执行如下步骤，编译并运行样例。
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
  mkdir -p build && cd build;                             # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j;         # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                                                   # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py -scenarioNum=$SCENARIO_NUM output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
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
