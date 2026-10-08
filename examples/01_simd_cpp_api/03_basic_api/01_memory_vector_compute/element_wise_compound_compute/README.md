# ElementWiseCompoundCompute样例

## 概述

本样例展示了复合计算类接口的使用方法。复合计算接口将多个计算操作融合在一条指令中完成，相比分开调用多个基础接口，可有效减少指令数量、降低中间存储开销、提升计算效率。接口资料参考AddRelu/Axpy。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── element_wise_compound_compute
│   ├── scripts
│   │   ├── gen_data.py                      // 输入数据和真值数据生成脚本
│   │   └── verify_result.py                 // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt                       // 编译工程文件
│   ├── data_utils.h                         // 数据读入写出函数
│   ├── element_wise_compound_compute.asc    // Ascend C样例实现 & 调用样例
│   └── README.md                            // 样例说明文档
```

## 样例描述

- 样例规格：
  <table border="2">
  <caption>表1：样例规格对照表</caption>
  <tr>
    <th align="left">场景编号(SCENARIO_NUM)</th>
    <th align="left">接口名称</th>
    <th align="left">功能说明</th>
    <th align="left">计算公式</th>
    <th align="left">输入类型</th>
    <th align="left">输出类型</th>
  </tr>
  <tr>
    <td align="left">1</td>
    <td align="left">AddRelu</td>
    <td align="left">向量加法与ReLU激活融合</td>
    <td align="left">dst = max(src0 + src1, 0)</td>
    <td align="left">half</td>
    <td align="left">half</td>
  </tr>
  <tr>
    <td align="left">2</td>
    <td align="left">Axpy</td>
    <td align="left">标量乘法与向量加法融合</td>
    <td align="left">dst = dst + src * scalar</td>
    <td align="left">half</td>
    <td align="left">half</td>
  </tr>
  </table>

  输入输出shape均为[1, 512]，format为ND，核函数名为`element_wise_compound_compute_custom`。

- 样例实现：
  - Kernel实现
    - 调用DataCopy基础API，将数据从GM（Global Memory）搬运到UB（Unified Buffer）
    - 根据场景调用不同的复合计算接口：场景1调用AddRelu实现向量加法与ReLU激活融合、场景2调用Axpy实现标量乘法与向量加法融合
    - 调用DataCopy基础API，将结果从UB（Unified Buffer）搬运至GM（Global Memory）

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
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM  # 生成测试输入数据
  ./demo                        # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin  # 验证输出结果是否正确
  ```
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU调试模式
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |
  | `SCENARIO_NUM` | `1`（默认）、`2` | 场景编号：1(AddRelu)、2(Axpy) |
