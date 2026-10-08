# DataCopy多维数据搬运接口示例

## 概述

本样例介绍如何使用多维数据搬运接口实现GM（Global Memory）到UB（Unified Buffer）通路的数据搬运，通过自由配置搬入的维度信息以及对应的Stride，可以用于Padding、Transpose、BroadCast、Slice等多种数据变换操作。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |

## 目录结构介绍

```
├── data_copy_gm2ub_nddma
│   ├── scripts
│   │   ├── gen_data.py                         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py                    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                          // 编译工程文件
│   ├── data_utils.h                            // 数据读入写出函数
│   ├── multidimensional_data_movement.asc      // Ascend C样例实现 & 调用样例
│   └── README.md                               // 样例说明文档
```

## 场景说明

本样例通过编译参数 `SCENARIO_NUM` 选择不同场景，所有场景数据格式为二维ND格式，核函数名为 `datacopy_custom`。

<table border="2">
<caption>表1：场景配置对照表</caption>
<tr><th>scenarioNum</th><th>输入Shape</th><th>输出Shape</th><th>数据类型</th><th>说明</th></tr>
<tr><td>1</td><td>[16, 32]</td><td>[32, 64]</td><td>float</td><td>Padding场景</td></tr>
<tr><td>2</td><td>[28, 15]</td><td>[32, 32]</td><td>float</td><td>Padding场景，使能最近值填充方式</td></tr>
<tr><td>3</td><td>[16, 64]</td><td>[64, 16]</td><td>float</td><td>Transpose场景</td></tr>
<tr><td>4</td><td>[1, 16]</td><td>[3, 16]</td><td>float</td><td>BroadCast场景</td></tr>
<tr><td>5</td><td>[32, 64]</td><td>[16, 16]</td><td>float</td><td>Slice场景</td></tr>
</table>

### 场景详细说明

**场景1：Padding场景**
- 输入：[16, 32]个float元素
- 输出：[32, 64]个float元素
- 参数配置：NdDmaLoopInfo={{1, 32}, {1, 64}, {32, 16}, {15, 13}, {17, 3}}，paddingValue=0
- 说明：将GM上[16, 32]的数据搬运至UB并Padding为[32, 64]，左Padding 15，上Padding 13，右Padding 17，下Padding 3，Padding值填充为0

**场景2：Padding场景，使能最近值填充方式**
- 输入：[28, 15]个float元素
- 输出：[32, 32]个float元素
- 参数配置：NdDmaLoopInfo={{1, 15}, {1, 32}, {15, 28}, {11, 3}, {6, 1}}，isNearestValueMode=true
- 说明：将GM上[28, 15]的数据搬运至UB并Padding为[32, 32]，使能最近值填充模式，Padding位置填充边界数据而非0

**场景3：Transpose场景**
- 输入：[16, 64]个float元素
- 输出：[64, 16]个float元素
- 参数配置：NdDmaLoopInfo={{1, 64}, {16, 1}, {64, 16}, {0, 0}, {0, 0}}
- 说明：将GM上[16, 64]的数据搬运至UB并转置为[64, 16]，通过配置stride实现行列互换

**场景4：BroadCast场景**
- 输入：[1, 16]个float元素
- 输出：[3, 16]个float元素
- 参数配置：NdDmaLoopInfo={{1, 0}, {1, 16}, {16, 3}, {0, 0}, {0, 0}}
- 说明：将GM上[1, 16]的数据搬运至UB并广播为[3, 16]，通过配置stride为0实现行数据复制

**场景5：Slice场景**
- 输入：[32, 64]个float元素
- 输出：[16, 16]个float元素
- 参数配置：NdDmaLoopInfo={{1, 64}, {1, 16}, {16, 16}, {0, 0}, {0, 0}}
- 说明：将GM上[32, 64]的数据搬运至UB并截取为[16, 16]，通过配置搬运数据量实现最终数据切片

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
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin  # 验证输出结果是否正确
  ```

  使用CPU调试或NPU仿真模式时，添加`-DCMAKE_ASC_RUN_MODE=cpu`或`-DCMAKE_ASC_RUN_MODE=sim`参数即可。
  
  示例如：
  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # cpu调试模式
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu` | 运行模式：NPU 运行、CPU调试 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（默认） | NPU 架构：dav-3510 对应 Ascend 950PR&950DT系列产品 |
  | `SCENARIO_NUM` | `1`（默认）、`2`、`3`、`4`、`5` | 场景编号 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```
