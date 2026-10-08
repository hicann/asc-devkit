# GetUBSize样例


## 概述

本样例展示GetUBSizeInBytes和GetRuntimeUBSize接口使用方法，用于获取用户最大可使用的UB（Unified Buffer）大小（单位为Byte）。本样例给出了2种不同的测试场景（scenario）。

<table>
  <tr>
    <td align="center">scenarioNum</td>
    <td align="center">API接口</td>
    <td align="center">描述</td>
  </tr>
  <tr>
    <td align="center">1</td>
    <td align="center">GetUBSizeInBytes</td>
    <td>返回编译时常量，表示用户最大可使用的UB（Unified Buffer）大小。例如Ascend 950PR&950DT系列产品场景，系统预留8KB，UB总共256KB，返回248KB</td>
  </tr>
  <tr>
    <td align="center">2</td>
    <td align="center">GetRuntimeUBSize</td>
    <td>返回运行时变量，表示用户最大可使用的UB（Unified Buffer）大小。在SIMT和SIMD混合编程场景下，SIMT场景会在UB预留一部分空间用于Dcache。例如Ascend 950PR&950DT系列产品场景，SIMT编程申请32KB空间用于Dcache，系统预留8KB，UB总共256KB，返回216KB。在非SIMT和SIMD混合编程场景下，返回固定值248KB</td>
  </tr>
</table>

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |

## 目录结构介绍

```
├── get_ub_size
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── get_ub_size.asc         // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能

  样例基于Abs取绝对值运算进行功能说明，计算公式：
  ```
  z = Abs(x)
  ```

- 样例规格

  **场景1：GetUBSizeInBytes**
  <table>
    <tr>
      <td align="center">类别</td>
      <td align="center">name</td>
      <td align="center">shape</td>
      <td align="center">data type</td>
      <td align="center">format</td>
    </tr>
    <tr>
      <td rowspan="1" align="center">样例输入</td>
      <td align="center">x</td>
      <td align="center">[1, 16384]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">样例输出</td>
      <td align="center">z</td>
      <td align="center">[1, 16384]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
  </table>

  **场景2：GetRuntimeUBSize**
  <table>
    <tr>
      <td align="center">类别</td>
      <td align="center">name</td>
      <td align="center">shape</td>
      <td align="center">data type</td>
      <td align="center">format</td>
    </tr>
    <tr>
      <td rowspan="1" align="center">样例输入</td>
      <td align="center">x</td>
      <td align="center">[1, 126976]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">样例输出</td>
      <td align="center">z</td>
      <td align="center">[1, 126976]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
  </table>

- 样例实现

  - Kernel实现

    - 调用GetUBSizeInBytes或GetRuntimeUBSize接口获取可使用的UB（Unified Buffer）大小，用于计算tileLength。
    - 调用DataCopy基础API，将数据从GM（Global Memory）搬运到UB（Unified Buffer）。
    - 调用Abs接口，对输入tensor执行取绝对值操作。
    - 调用DataCopy基础API，将计算结果从UB（Unified Buffer）搬运至GM（Global Memory）。

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
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py output/output.bin output/golden.bin   # 验证输出结果是否正确，确认算法逻辑正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  
  示例如下：
  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # cpu调试模式
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　　　　　　　　　　　　　　　　　　 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510`（默认） | NPU 架构：dav-3510 对应 Ascend 950PR&950DT系列产品 |
  | `SCENARIO_NUM` | `1`、`2`　　　　　| 场景编号：1=GetUBSizeInBytes，2=GetRuntimeUBSize |

- 执行结果

  执行结果如下，说明精度对比成功：
  ```bash
  test pass!
  ```
