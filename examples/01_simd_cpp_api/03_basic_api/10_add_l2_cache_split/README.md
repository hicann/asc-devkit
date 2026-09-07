# L2 Cache切分样例

## 概述

本样例通过一个原地加法固定负载，展示L2 Cache切分对跨轮数据复用的影响。

样例使用一份384MB的GM数据，连续执行两轮`data = data + 1`计算。两种实现的逻辑GM->UB搬运量、UB->GM搬运量和Adds计算量相同，区别在于数据访问顺序：

- 反例`no-split`：每轮直接处理完整384MB数据。
- 正例`l2-split`：将384MB数据切成4个96MB的L2 Batch，同一Batch连续完成两轮计算后再处理下一Batch。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```text
├── 10_add_l2_cache_split
│   ├── scripts
│   │   ├── gen_data.py               // 输入数据和真值数据生成脚本
│   │   └── verify_result.py          // 验证输出数据和真值数据是否一致的验证脚本
│   ├── add_l2cache_nosplit.asc       // 反例Kernel入口和Host main
│   ├── add_l2cache_split.asc         // 正例Kernel入口和Host main
│   ├── CMakeLists.txt                // 编译工程文件
│   ├── data_utils.h                  // 数据读入写出函数
│   ├── l2_cache_kernel.h             // 固定负载参数和公共Device处理逻辑
│   ├── l2_cache_runner.h             // 公共Host运行逻辑
│   ├── README.md                     // 中文样例说明文档
│   └── README_en.md                  // 英文样例说明文档
```

## 样例描述

- 样例功能：

  本样例对输入`data`执行两轮原地加法计算，每轮对每个`half`元素加1，最终结果为`data + 2`。样例通过`SCENARIO_NUM`区分不切分和L2 Cache切分两个编译场景，用于对比相同计算量下不同访问顺序的性能差异。

- 样例规格：

  <table>
  <tr><th align="center">样例类型(OpType)</th><th colspan="4" align="center">AddL2CacheSplit</th></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">data</td><td align="center">[201326592]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">data</td><td align="center">[201326592]</td><td align="center">half</td><td align="center">ND</td></tr>
  <tr><td rowspan="2" align="center">核函数名</td><td colspan="4" align="center">l2_cache_nosplit</td></tr>
  <tr><td colspan="4" align="center">l2_cache_split</td></tr>
  </table>

- 样例参数：

  | 参数 | 取值 | 说明 |
  |------|-----:|------|
  | `totalByteSize` | 384MB | 输入数据总大小 |
  | `L2_TILE_THRESHOLD` | 100MB | 单批数据的L2 Cache有效容量阈值 |
  | `l2BatchCount` | 4 | 正例L2 Batch数量 |
  | `l2BatchSizeBytes` | 96MB | 正例单个L2 Batch大小 |
  | `vectorCoreCount` | 20 | Vector Core数量 |
  | `computeRoundCount` | 2 | 计算轮数 |

- 样例场景：

  | `SCENARIO_NUM` | 场景 | 源文件 | Kernel入口 | 输出文件 |
  |------|------|------|------|------|
  | `1` | 不做L2 Cache切分 | `add_l2cache_nosplit.asc` | `l2_cache_nosplit` | `output/output.bin` |
  | `2` | 开启L2 Cache切分 | `add_l2cache_split.asc` | `l2_cache_split` | `output/output.bin` |

- 样例实现：

  - Kernel实现

    公共Device逻辑按32字节数据块切分数据范围，并分配给20个Vector Core。每个核内部按UB Tile循环执行`DataCopy`、`Adds`和写回。

    反例每轮处理完整`data[0, 384MB)`范围：

    ```text
    Round 0: data[0, 384MB)
    Round 1: data[0, 384MB)
    ```

    正例按L2 Batch处理，每个Batch连续完成两轮计算：

    ```text
    B0: Round 0 -> Round 1
    B1: Round 0 -> Round 1
    B2: Round 0 -> Round 1
    B3: Round 0 -> Round 1
    ```

    384MB超过样例设置的100MB有效容量阈值，不切分时Round 0早期访问的数据可能在后续扫描中被替换出L2 Cache。正例单个Batch为96MB，同一批数据连续复用时更容易命中L2 Cache。

  - 同步实现

    反例每处理完一轮完整数据后调用`AscendC::SyncAll()`。正例每个L2 Batch与Round组合结束后调用`AscendC::SyncAll()`，确保所有核完成当前阶段的UB->GM搬运后，再进入下一阶段。

  - 调用实现

    使用内核调用符`<<<>>>`调用核函数。编译时通过`SCENARIO_NUM`选择源文件，生成的可执行程序统一为`demo`。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量
  请根据当前环境上CANN开发套件包的[安装方式](../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  SCENARIO_NUM=1
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # 编译工程，默认NPU模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据和真值数据
  ./demo                           # 执行样例
  python3 ../scripts/verify_result.py output/output.bin ./output/golden.bin -scenarioNum=$SCENARIO_NUM  # 验证输出结果
  ```

  本样例主要面向NPU模式运行和性能采集。需要排查编译或功能问题时，可添加`-DCMAKE_ASC_RUN_MODE=cpu`或`-DCMAKE_ASC_RUN_MODE=sim`参数进行对应模式的工程配置和运行验证。

  示例如下：

  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU调试模式配置
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式配置
  ```

  > **注意：** 切换编译模式、架构或场景前需清理CMake缓存，可在`build`目录下执行`rm CMakeCache.txt`后重新执行CMake配置。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 工程配置模式：NPU运行、CPU调试、NPU仿真。其中`sim`模式支持完整执行验证，但384MB负载仿真耗时较长，仿真耗时不作为L2 Cache性能收益评估依据 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU架构：dav-2201对应Atlas A2 训练系列产品/Atlas A2 推理系列产品和Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510对应Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`、`2` | 场景编号：1=不做L2 Cache切分，2=开启L2 Cache切分 |

- 执行结果

  执行结果如下，说明精度对比成功。

  ```bash
  test pass!
  ```

  程序运行阶段校验失败时会打印`test failed!`，并输出首个错误元素的索引、输入值、实际输出值和期望输出值。`verify_result.py`验证失败时会打印首个错误元素的索引、实际输出值和期望输出值。

  运行后输出文件如下：

  ```text
  output/output.bin
  output/golden.bin
  ```

## 性能采集与分析

本样例用于观察L2 Cache切分对数据搬入路径的影响。建议分别启动独立进程采集正反例，避免同一进程中前一个Kernel残留的Cache状态影响后一个Kernel。

```bash
msopprof ./demo
```

分析时，先确认`Memory.csv`中的`GM_to_UB_datas(KB)`总量接近，保证正反例逻辑搬运量公平；再结合`L2Cache.csv`中的L2 Cache读命中率、`PipeUtilization.csv`中的MTE2耗时和`OpBasicInfo.csv`中的`Task Duration(us)`进行判断。

常用指标如下：

| 字段名 | 含义 | 来源文件 |
|------|------|---------|
| `Task Duration(us)` | Kernel任务整体耗时 | `OpBasicInfo.csv` |
| `aiv_mte2_time(us)` | MTE2搬入耗时 | `PipeUtilization.csv` |
| `GM_to_UB_datas(KB)` | 逻辑GM->UB搬运量 | `Memory.csv` |
| `read_main_memory_datas(KB)` | 主存储器读取计数，统计口径与架构和工具版本相关 | `Memory.csv` |
| `aiv_read_hit_rate(%)` | L2 Cache读命中率 | `L2Cache.csv` |

`aiv_gm_to_ub_bw(GB/s)`是有效带宽，不是GM读取量。优化后带宽可能升高，不能将“带宽降低”作为优化预期。分析性能收益时，应以逻辑搬运量公平性为前提，结合L2 Cache读命中率、MTE2耗时和整体任务耗时综合判断。
