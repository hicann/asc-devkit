# 自定义算子工程多 Vendor 并行编译、打包和部署样例

## 概述

本样例展示如何在一个顶层 CMake 工程中，使用 `ExternalProject_Add` 并行编译两个独立自定义算子工程：

- `add_custom`（AddCustom）
- `leaky_relu_custom`（LeakyReluCustom）

其中 `add_custom` 采用扁平目录组织（host/kernel/tiling 源码位于同一层级），`leaky_relu_custom` 保持分层目录结构。

每个子工程会分别完成自定义算子的编译、打包，并生成独立的 `custom_opp_*.run` 安装包。

## 本样例支持的产品及CANN软件版本

| 产品                                        | CANN软件版本  |
| ------------------------------------------- | ------------- |
| Ascend 950PR&950DT系列产品                   | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |
| Atlas 200I/500 A2推理产品                  | >= CANN 9.0.0 |
| Atlas推理系列产品                          | >= CANN 9.0.0 |

> 注意: 本样例中涉及多个算子示例，请以各个算子示例实际支持的产品型号为准。

## 目录结构介绍

```
├── parallel_ops_package
│   ├── CMakeLists.txt              // 顶层编译工程文件（并行构建多个算子包）
│   ├── add_custom                  // Add算子工程目录
│   ├── leaky_relu_custom           // LeakyRelu算子工程目录
│   │   ├── CMakeLists.txt          // LeakyRelu算子编译工程文件
│   │   ├── framework               // 框架适配插件目录
│   │   ├── op_host                 // Host侧算子实现目录
│   │   └── op_kernel               // Kernel侧算子实现目录
│   └── README.md                   // 样例说明文档
```

## 样例描述

`parallel_ops_package` 与 `custom_op` 使用相同的 Add/LeakyRelu 样例描述，请参考：

- [custom_op/README.md 的“样例描述”章节](../custom_op/README.md#样例描述)

## 样例规格描述

`parallel_ops_package` 与 `custom_op` 使用相同的 Add/LeakyRelu 规格描述，请参考：

- [custom_op/README.md 的“样例规格描述”章节](../custom_op/README.md#样例规格描述)

## 代码实现介绍

`parallel_ops_package` 的 Add/LeakyRelu 代码实现可参考 `custom_op` 文档说明。

- [custom_op/README.md 的“代码实现介绍”章节](../custom_op/README.md#代码实现介绍)

## 编译运行

在本样例根目录下执行如下步骤，编译、打包并部署自定义算子包。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 编译、打包并部署两个算子包

  ```bash
  cmake -S . -B build
  cmake --build build -j
  # add_custom 包
  ./build/add_custom/custom_opp_*.run

  # leaky_relu_custom 包
  ./build/leaky_relu_custom/custom_opp_*.run
  ```

  执行结果如下，说明执行成功。

  ```text
  SUCCESS
  ```

## 构建结果说明

顶层工程会在 `build/` 下生成两个子目录：

- `build/add_custom/`：AddCustom 的中间产物与安装包
- `build/leaky_relu_custom/`：LeakyReluCustom 的中间产物与安装包

这两个目录互相独立，便于多 Vendor 场景下并行开发与发布。
