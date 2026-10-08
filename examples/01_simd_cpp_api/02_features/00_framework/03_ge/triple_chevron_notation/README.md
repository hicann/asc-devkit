# Ascend C算子接入GE样例

## 概述

本样例以AddCustom算子为例，介绍如何把Ascend C `<<<>>>`核函数调用接入GE（Graph Engine，图引擎）原生图执行链路。

> **什么是GE？** <br>
> GE是昇腾计算架构中的图执行引擎。与每个算子下发后立即执行的方式不同，GE图模式会先把算子组织成一张计算图，再以图为单位执行。图模式具备全局视角，便于图优化和统一调度。更多背景可参考：[图模式开发指南](https://hiascend.com/document/redirect/CannCommunityGraphguide)。

**接入路径**：
- 算子实现：在 `add_custom.asc` 中实现Ascend C核函数，完成一次 `z = x + y` 计算。
- GE接入：声明 `AddCustom` 算子原型，实现GE执行入口，并在 `graph.cpp` 中构造GE图。
- 图执行：`Session::RunGraph` 执行整张图，GE运行到 `AddCustom` 节点时调用算子侧执行实现。

<img src="figures/execution_flow.png" width="75%">

上图从左到右展示了完整接入链路。左侧是构图侧：先通过算子原型声明 `AddCustom`，再在 `graph.cpp` 中把 `Data` 节点和两个 `AddCustom` 节点连成GE图，最后通过 `Session::RunGraph` 触发整图执行。右侧是算子侧：GE运行到每个 `AddCustom` 节点时，会进入 `add_custom.asc` 中注册的执行实现，并由该执行入口通过 `<<<>>>` 启动Ascend C核函数。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.1.0 |
| Atlas A2系列产品 | >= CANN 9.1.0 |

## 目录结构介绍

```text
├── triple_chevron_notation
│   ├── CMakeLists.txt          // 编译工程：产出自定义算子动态库和GE在线构图执行程序
│   ├── add_custom.asc          // 算子侧：Ascend C核函数实现和GE算子执行实现
│   ├── add_custom_proto.h      // 构图侧：AddCustom算子原型声明
│   ├── figures                 // 图示
│   ├── graph.cpp               // 构图侧：构造GE图并调用Session::RunGraph执行
│   └── README.md               // 样例说明文档
```

## 样例实现

### 算子实现

算子实现的核心内容在 `add_custom.asc` 中，Add的计算公式为：

$$
z = x + y
$$

- x：输入，形状为[8, 2048]，数据类型为float；
- y：输入，形状为[8, 2048]，数据类型为float；
- z：输出，形状为[8, 2048]，数据类型为float；

### GE接入流程

要让Ascend C核函数作为GE图中的节点执行，需要完成算子原型声明、算子执行实现和GE在线构图三个步骤。

第一步是在 `add_custom_proto.h` 中声明 `AddCustom` 算子原型。算子原型描述的是GE构图侧能看到的算子接口，包括输入、输出和数据类型。声明完成后，GE API才能在构图代码中使用 `ge::op::AddCustom` 创建自定义算子节点。算子原型的概念可参考[什么是算子原型](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta1/programug/graphdevg/atlasag_25_0008.html)。

算子原型声明如下，GE构图侧会根据该原型生成`ge::op::AddCustom`类型：

```cpp
REG_OP(AddCustom)
    .INPUT(x, TensorType({DT_FLOAT}))
    .INPUT(y, TensorType({DT_FLOAT}))
    .OUTPUT(z, TensorType({DT_FLOAT}))
    .OP_END_FACTORY_REG(AddCustom);
```

第二步是在 `add_custom.asc` 中实现 `ge::AddCustom` 执行类。该类继承 `EagerExecuteOp` 和 `ShapeInferOp`：`InferShape()` 和 `InferDataType()` 用于图编译阶段推导输出Tensor信息，`Execute()` 用于图运行阶段真正启动Ascend C核函数。`REG_AUTO_MAPPING_OP(AddCustom)` 会把GE图中的 `AddCustom` 节点映射到这个执行类。

第三步是在 `graph.cpp` 中基于算子原型创建 `ge::op::AddCustom` 节点，并把 `Data` 输入节点和 `AddCustom` 计算节点连接成GE图。通过算子原型构建Graph可参考[通过算子原型构建Graph](https://www.hiascend.com/document/detail/zh/CANNCommunityEdition/910beta1/programug/graphdevg/atlasag_25_0009.html)。

运行时通过`Session::RunGraph`触发整张GE图执行。

```cpp
std::vector<ge::Tensor> inputs{BuildInputTensor(X_SCALE), BuildInputTensor(Y_SCALE)};
std::vector<ge::Tensor> outputs;
const auto runRet = session.RunGraph(GRAPH_ID, inputs, outputs);
```

## 编译运行

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例根目录下执行如下步骤，运行该样例。

  ```bash
  # 创建并进入build目录
  mkdir -p build && cd build;
  # 配置并编译样例
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;
  # 配置自定义算子包路径
  export ASCEND_CUSTOM_OPP_PATH="$(pwd)/output:${ASCEND_CUSTOM_OPP_PATH}"
  # 执行样例
  ./demo
  ```

- 编译选项说明

  | 选项　　　　　 | 可选值　　　　　　　　　　　| 说明　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　　 |
  | ----------------| -----------------------------| --------------------------------------------------------------------------------------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品 与 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |

- 执行结果

  成功时可观察到如下输出：

  ```bash
  Output: 0 0.5 1 1.5 ...
  Golden: 0 0.5 1 1.5 ...
  AddCustom GE triple-chevron sample success.
  ```
