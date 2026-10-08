# TBufPool内存管理样例

## 概述

本样例基于TPipe::InitBufPool和TBufPool::InitBufPool接口实现TBufPool内存资源管理，展示TBufPool资源分配、内存划分、内存复用和自定义TBufPool等使用方式。本样例给出了3种不同的测试场景（scenario）。

> **注意：** 本样例仅适用于基于TPipe和TQue的编程模式。

<table>
  <tr>
    <td align="center">scenarioNum</td>
    <td align="center">场景名称</td>
    <td align="center">描述</td>
  </tr>
  <tr>
    <td align="center">1</td>
    <td align="center">TBufPool内存复用</td>
    <td align="center">使用TPipe::InitBufPool初始化两个TBufPool，指定第二个复用第一个的起始地址及长度，实现内存复用</td>
  </tr>
  <tr>
    <td align="center">2</td>
    <td align="center">TBufPool资源细分</td>
    <td align="center">使用TBufPool::InitBufPool将整块资源继续划分成小块资源，并指定子资源池之间的复用关系</td>
  </tr>
  <tr>
    <td align="center">3</td>
    <td align="center">自定义TBufPool</td>
    <td align="center">使用EXTERN_IMPL_BUFPOOL宏辅助用户自定义TBufPool类，实现非连续内存块分配和内存共享</td>
  </tr>
</table>

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── tbufpool_management
│   ├── scripts
│   │   ├── gen_data.py         // 输入数据和真值数据生成脚本
│   │   └── verify_result.py    // 验证输出数据和真值数据是否一致的验证脚本
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── data_utils.h            // 数据读入写出函数
│   ├── tbufpool_management.asc // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能

  本样例展示TBufPool内存资源管理的多种使用方式，包括内存复用、资源细分和自定义TBufPool。

- 样例规格

  **场景1：TBufPool内存复用**
  <table>
    <tr>
      <td align="center">类别</td>
      <td align="center">name</td>
      <td align="center">shape</td>
      <td align="center">data type</td>
      <td align="center">format</td>
    </tr>
    <tr>
      <td rowspan="2" align="center">样例输入</td>
      <td align="center">x</td>
      <td align="center">[3, 65536]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">y</td>
      <td align="center">[3, 65536]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">样例输出</td>
      <td align="center">z</td>
      <td align="center">[3, 65536]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">核函数名</td>
      <td colspan="4" align="center">tbufpool_management_custom</td>
    </tr>
  </table>

  **场景2：TBufPool资源细分**
  <table>
    <tr>
      <td align="center">类别</td>
      <td align="center">name</td>
      <td align="center">shape</td>
      <td align="center">data type</td>
      <td align="center">format</td>
    </tr>
    <tr>
      <td rowspan="2" align="center">样例输入</td>
      <td align="center">x</td>
      <td align="center">[4, 32768]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">y</td>
      <td align="center">[4, 32768]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">样例输出</td>
      <td align="center">z</td>
      <td align="center">[4, 32768]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">核函数名</td>
      <td colspan="4" align="center">tbufpool_management_custom</td>
    </tr>
  </table>

  **场景3：自定义TBufPool**
  <table>
    <tr>
      <td align="center">类别</td>
      <td align="center">name</td>
      <td align="center">shape</td>
      <td align="center">data type</td>
      <td align="center">format</td>
    </tr>
    <tr>
      <td rowspan="2" align="center">样例输入</td>
      <td align="center">x</td>
      <td align="center">[1, 65536]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">y</td>
      <td align="center">[1, 65536]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">样例输出</td>
      <td align="center">z</td>
      <td align="center">[1, 65536]</td>
      <td align="center">half</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">核函数名</td>
      <td colspan="4" align="center">tbufpool_management_custom</td>
    </tr>
  </table>

- 样例实现

  - Kernel实现

    **内存资源管理（场景差异）**

    <table>
      <tr>
        <td align="center">scenarioNum</td>
        <td align="center">内存管理方式</td>
        <td align="center">实现描述</td>
      </tr>
      <tr>
        <td align="center">1</td>
        <td align="center">TBufPool内存复用</td>
        <td align="center">调用TPipe::InitBufPool初始化tbufPool1和tbufPool2，指定tbufPool2复用tbufPool1的起始地址及长度；调用TBufPool::InitBuffer为TQue分配内存空间</td>
      </tr>
      <tr>
        <td align="center">2</td>
        <td align="center">TBufPool资源细分</td>
        <td align="center">调用TPipe::InitBufPool初始化tbufPool0，调用TBufPool::InitBufPool细分tbufPool1和tbufPool2，指定tbufPool2复用tbufPool1；调用TBufPool::InitBuffer为TQue分配内存空间</td>
      </tr>
      <tr>
        <td align="center">3</td>
        <td align="center">自定义TBufPool</td>
        <td align="center">使用EXTERN_IMPL_BUFPOOL宏实现自定义TBufPool类MyBufPool；调用TPipe::InitBufPool为MyBufPool划分内存，通过InitBuffer实现TQue和TBuf的内存分配</td>
      </tr>
    </table>

    **通用计算流程**

    - 调用DataCopy基础API，将数据从GM（Global Memory）搬运到UB（Unified Buffer）。
    - 调用Add接口，对两个输入tensor执行加法操作。
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
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # 编译工程，默认npu模式
  python3 ../scripts/gen_data.py -scenarioNum=$SCENARIO_NUM   # 生成测试输入数据
  ./demo                           # 执行编译生成的可执行程序，执行样例
  python3 ../scripts/verify_result.py ./output/output.bin ./output/golden.bin   # 验证输出结果是否正确
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  
  示例如下：
  ```bash
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DSCENARIO_NUM=$SCENARIO_NUM -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真　　　　　　　　　　　　　　　　　　　　　　　　 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品 与 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |
  | `SCENARIO_NUM` | `1`、`2`、`3`　　　　　| 场景编号：1=TBufPool内存复用，2=TBufPool资源细分，3=自定义TBufPool |

- 执行结果

  执行结果如下，说明精度对比成功：
  ```bash
  test pass!
  ```
