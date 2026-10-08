# ACLGraph样例

## 概述

当一段固定的数据拷贝和计算流程需要重复执行时，主机侧默认需要在每次执行前重新提交数据拷贝、核函数启动和同步等运行时任务；当重复次数较多时，这部分任务下发开销会影响整体执行效率。

ACLGraph 可以先捕获指定流上的一组运行时任务，再将这些任务作为一个整体重复执行。具体流程是：通过 `aclmdlRICaptureBegin` 和 `aclmdlRICaptureEnd` 生成 `aclmdlRI` 执行实例；后续通过 `aclmdlRIExecuteAsync` 执行该实例，无需再次逐条提交相同任务。关于ACLGraph的更多内容，可参考[ACLGraph文档](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta1/programug/acldevg/runtime_doc_dev_0045.html)。

本样例以Add算子为例，展示如何捕获Ascend C `<<<>>>` 核函数调用及其前后的数据拷贝任务，并分别演示单流线性任务捕获和双流事件依赖捕获。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.1.0 |
| Atlas A2系列产品 | >= CANN 9.1.0 |

## 目录结构介绍

```text
├── 04_aclgraph
│   ├── figures                        // 图示
│   │   ├── single_stream_capture.png  // 单流捕获流程图
│   │   └── double_stream_capture.png  // 双流捕获流程图
│   ├── CMakeLists.txt                 // 编译工程
│   ├── add.asc                        // Ascend C核函数实现和ACLGraph捕获调用样例
│   └── README.md                      // 样例说明文档
```

## 场景详细说明

本样例通过CMake编译参数 `SCENARIO_NUM` 选择不同的ACLGraph捕获场景，`SCENARIO_NUM` 不同取值对应的含义如下表所示。所有场景使用相同的Add算子规格：输入和输出形状均为 `[8, 2048]`，数据类型为 `float`，核函数名为 `add_custom`。

<table>
<caption style="font-weight: normal;">
         <span style="font-weight: bold; font-size: 1.2em;">📌 表1：SCENARIO_NUM不同取值的含义</span></caption>
<tr><td rowspan="1" align="center">SCENARIO_NUM</td><td align="center">场景</td><td align="center">任务组织方式</td><td align="center">说明</td></tr>
<tr><td align="center">1</td><td align="center">单流</td><td align="center">全部任务提交到 <code>mainStream</code></td><td align="center">展示最基础的线性任务捕获和重复执行流程</td></tr>
<tr><td align="center">2</td><td align="center">双流</td><td align="center"><code>mainStream</code> 与 <code>computeStream</code> 通过事件同步</td><td align="center">展示跨流任务和事件同步关系的捕获方式</td></tr>
</table>

**场景1：单流捕获**
- 输入：x [8, 2048] float类型；y [8, 2048] float类型
- 输出：z [8, 2048] float类型
- 捕获内容：在 `mainStream` 上依次提交x、y输入数据拷贝、`add_custom` 核函数启动和z输出数据拷贝。
- 说明：`aclmdlRICaptureEnd` 生成的 `aclmdlRI` 保存一条线性任务链，后续通过 `aclmdlRIExecuteAsync` 整体重放。该场景用于展示最基础的“捕获一次，多次执行”流程。

<p align="center">
  <img src="figures/single_stream_capture.png" width="800">
</p>

**场景2：双流捕获**
- 输入：x [8, 2048] float类型；y [8, 2048] float类型
- 输出：z [8, 2048] float类型
- 捕获内容：`mainStream` 负责x输入数据拷贝和z输出数据拷贝；`computeStream` 等待x拷贝完成后执行y输入数据拷贝和 `add_custom` 核函数。两个流之间通过 `aclrtRecordEvent` 和 `aclrtStreamWaitEvent` 建立依赖。
- 说明：`aclmdlRI` 不只记录单个流上的任务顺序，也记录跨流的事件依赖，适合展示多流任务关系的捕获方式。

<p align="center">
  <img src="figures/double_stream_capture.png" width="800">
</p>

## 样例实现

### 算子实现

算子实现的核心内容在 `add.asc` 中，Add的计算公式为：

$$
z = x + y
$$

- x：输入，形状为[8, 2048]，数据类型为float；
- y：输入，形状为[8, 2048]，数据类型为float；
- z：输出，形状为[8, 2048]，数据类型为float；

核函数中每个block处理2048个float元素，8个block共同完成全部输入数据的计算。每个block先将x和y从GM搬运到UB，调用 `AscendC::Add` 完成向量加法，再将结果写回GM。

### ACLGraph捕获流程

本样例中的ACLGraph可以理解为“捕获一次，重复执行”。主机侧首先调用 `aclmdlRICaptureBegin` 进入捕获状态，然后提交需要记录的运行时任务，最后调用 `aclmdlRICaptureEnd` 结束捕获并获取 `aclmdlRI`。

捕获流程如下：

1. 开始捕获：调用 `aclmdlRICaptureBegin`，开始捕获指定流上的运行时任务。
2. 任务提交：提交输入数据拷贝、核函数启动、输出数据拷贝，以及双流场景中的事件同步任务。
3. 结束捕获：调用 `aclmdlRICaptureEnd`，结束捕获并生成 `aclmdlRI`。
4. 重复执行：调用 `aclmdlRIExecuteAsync`，重复执行已经捕获的任务流程。
5. 资源释放：调用 `aclmdlRIDestroy`，释放不再使用的 `aclmdlRI` 执行实例。

```cpp
aclmdlRICaptureBegin(mainStream, ACL_MODEL_RI_CAPTURE_MODE_GLOBAL);

// 提交输入数据拷贝、核函数启动、输出数据拷贝、事件同步等运行时任务

aclmdlRICaptureEnd(mainStream, &modelRI);
```

生成 `modelRI` 后，通过 `aclmdlRIExecuteAsync` 重复执行捕获到的任务流程。

```cpp
for (int i = 0; i < 5; ++i) {
    aclmdlRIExecuteAsync(modelRI, mainStream);
    aclrtSynchronizeStream(mainStream);
}
```

样例还调用 `aclmdlRIDebugJsonPrint` 将捕获到的任务信息输出为JSON文件，单流场景输出 `modelRI_single_stream.json`，双流场景输出 `modelRI_double_stream.json`。

## 编译运行

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例根目录下执行如下步骤，运行该样例。

  ```bash
  SCENARIO_NUM=1  # 设置场景编号
  mkdir -p build && cd build;
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 -DSCENARIO_NUM=$SCENARIO_NUM ..;make -j;
  ./demo
  ```

  `SCENARIO_NUM=1` 表示编译单流捕获场景，`SCENARIO_NUM=2` 表示编译双流捕获场景。不指定 `SCENARIO_NUM` 时，默认编译单流捕获场景。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品 与 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |
  | `SCENARIO_NUM` | `1`（默认）、`2` | 捕获场景编号：1表示单流捕获，2表示双流捕获 |

- 执行结果

  成功时可观察到如下输出：

  ```bash
  Output: 0 0.3 0.6 0.9 1.2 ...
  Golden: 0 0.3 0.6 0.9 1.2 ...
  test pass!
  ```
