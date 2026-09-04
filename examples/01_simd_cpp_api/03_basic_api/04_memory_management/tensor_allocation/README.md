# tensor_allocation样例

## 概述

本样例介绍GlobalTensor、LocalTensor、RegTensor的申请与使用方式。

<table>
  <tr>
    <td align="center">Tensor类型</td>
    <td align="center">用法</td>
    <td align="center">申请方式</td>
  </tr>
  <tr>
    <td align="center">GlobalTensor</td>
    <td>用来存放Global Memory（外部存储）的全局数据</td>
    <td>构造函数创建</td>
  </tr>
  <tr>
    <td align="center">LocalTensor</td>
    <td>用于存放AI Core中Local Memory（内部数据）的数据</td>
    <td>TQue队列管理、LocalMemAllocator分配器管理、构造函数创建</td>
  </tr>
  <tr>
    <td align="center">RegTensor</td>
    <td>Reg编程的计算基本单元，用于矢量计算</td>
    <td>构造函数创建</td>
  </tr>
</table>

> **注意：** RegTensor仅适用于Ascend 950PR/Ascend 950DT产品。

本样例包含4个场景，场景1~3为LocalTensor的三种申请方式，场景4为RegTensor的申请与使用，具体描述如下：

<table>
  <tr>
    <td align="center">scenarioNum</td>
    <td align="center">场景名称</td>
    <td align="center">特点</td>
  </tr>
  <tr>
    <td align="center">1</td>
    <td align="center">使用TQue队列管理LocalTensor</td>
    <td>使用难度低，无需手动设置同步事件，编程界面更简洁</td>
  </tr>
  <tr>
    <td align="center">2</td>
    <td align="center">使用LocalMemAllocator分配器管理LocalTensor</td>
    <td>使用难度中等，需要手动设置硬件事件同步，同步更灵活<br>当LocalTensor的大小为常量时，可以在申请时使用模板参数设置大小，以获得更优的性能</td>
  </tr>
  <tr>
    <td align="center">3</td>
    <td align="center">使用LocalTensor构造函数创建LocalTensor</td>
    <td>使用难度高，开发者完全手动控制内存布局，需要精确计算避免地址冲突，也需要手动设置硬件事件同步</td>
  </tr>
  <tr>
    <td align="center">4</td>
    <td align="center">在LocalTensor的基础上使用RegTensor</td>
    <td>通过Reg编程接口直接对芯片的vector寄存器进行操作，实现更大的灵活性和更好的性能<br>Reg编程中LocalTensor的部分，场景1~3都适用<br>仅适用于Ascend 950PR/Ascend 950DT产品</td>
  </tr>
</table>

**图 1**  数据在各Tensor间的流转图

<img src="figures/Tensor流转图.png">

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.2.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.2.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.2.0 |

## 目录结构介绍

```
├── tensor_allocation
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── tensor_allocation.asc   // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能：
  本样例使用Ascend C实现向量加法操作（z = x + y）。介绍了三种不同的方式管理UB（Unified Buffer）上的LocalTensor，以及RegBase编程实现RegTensor加法操作，通过设置`SCENARIO_NUM`环境变量选择不同的场景。

- 样例规格：
  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">ElementWise</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x</td><td align="center">[1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y</td><td align="center">[1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[1024]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">tensor_allocate_custom</td></tr>
  </table>

- 样例实现：
  - Kernel实现

    **场景1（Process1）：使用TQue队列机制**
    - 通过`TPipe::InitBuffer`初始化TQue队列，指定队列深度和buffer大小
    - 使用`TQue::AllocTensor`申请LocalTensor，使用`TQue::FreeTensor`释放LocalTensor
    - 通过队列的EnQue/DeQue机制管理数据的流入流出
    
    **场景2（Process2）：使用LocalMemAllocator分配器**
    - 创建`LocalMemAllocator<Hardware::UB>`分配器对象
    - 调用`Alloc<T>`方法直接分配指定大小的LocalTensor
    - 需要手动调用`SetFlag/WaitFlag`进行硬件事件同步
    - 当buffer的大小tileSize为常量时，建议使用tileSize模板参数，以获得更优的性能
    
    **场景3（Process3）：使用地址偏移手动管理**
    - 手动计算各tensor在UB中的起始地址偏移
    - 通过LocalTensor构造函数指定`TPosition`和起始地址
    - 完全手动控制内存布局，需要开发者精确计算地址避免冲突
    - 需要手动调用`SetFlag/WaitFlag`进行硬件事件同步

    **场景4（Process4）：基于Reg编程实现RegTensor的加法**
    - LocalTensor的部分同场景2
    - 在__simd_vf__中申请RegTensor，使用RegTensor完成加法计算

  - 调用实现
    使用内核调用符<<<>>>调用核函数，启动1个核执行计算。

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
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;    # 编译工程，默认npu模式
  ./demo                           # 执行样例
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # CPU调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |
  | `SCENARIO_NUM` | `1`、`2`、`3`、`4` | 场景编号：1（使用TQue队列机制）、2（使用LocalMemAllocator分配器）、3（使用地址偏移手动管理）、4（基于Reg编程实现RegTensor的加法）|

  > **注意：** 当`SCENARIO_NUM`设置为`4`时，仅支持`CMAKE_ASC_ARCHITECTURES`设置为`dav-3510`。

- 执行结果
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```