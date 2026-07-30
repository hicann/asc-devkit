# CMake函数参考

本文是参考内容，整理算子工程编译过程涉及的CMake函数接口，供需要深入了解编译机制的高级开发者查阅。

## 概述

使用msOpGen工具创建算子工程时，相关编译脚本被固化在本地。为便于开发者直接使用算子工程对外提供的cmake函数接口而无需频繁重建工程，CANN软件包中将算子工程的cmake脚本打包，开发者可通过`find_package`查找对应的cmake modules包来使用这些函数。

本文说明算子工程编译过程中使用的8个CMake函数，按用途分为**Package类**、**Library类**、**其他**三类。

## 函数一览

| 分类 | 函数 | 功能 |
|------|------|------|
| Package | `npu_op_package` | 创建一个package。 |
| Package | `npu_op_package_add` | 将目标或文件添加到package中。 |
| Library | `npu_op_library` | 创建Host侧库。 |
| Library | `npu_op_kernel_library` | 创建Kernel侧库。 |
| Library | `npu_op_kernel_options` | 添加Kernel目标编译选项。 |
| Library | `npu_op_kernel_sources` | 描述Kernel目标的源码信息。 |
| Library | `npu_op_device_tiling_library` | 创建Device侧Tiling库。 |
| 其他 | `npu_op_code_gen` | 执行代码生成过程，生成aclnn单算子调用代码和入图所需的原型定义代码。 |

---

## Package类函数

### npu_op_package

创建一个package。

```cmake
npu_op_package(<package_name> TYPE <type> [CONFIG] [ENABLE_SOURCE_PKG <value>] [ENABLE_BINARY_PACKAGE <value>] [INSTALL_PATH <path>])
```

**参数说明：**

- `<package_name>`（必选）：package的名称。
- `TYPE <type>`（必选）：package的类型，取值为`RUN`、`SHARED`、`STATIC`。分别对应算子run包形式、算子动态库形式与算子静态库形式。
- `[CONFIG]`（可选）：用于配置package的内容和安装位置。
  - `[ENABLE_SOURCE_PKG <value>]`（可选）：是否将源码打包到package中，默认为True。
  - `[ENABLE_BINARY_PACKAGE <value>]`（可选）：是否将二进制文件打包到package中，默认为True。
  - `[INSTALL_PATH <path>]`（可选）：指定包的安装路径，默认为`CMAKE_BINARY_DIR`。
  - `[ENABLE_CPACK <value>]`（可选）：是否打包，默认为True。对于用户需要定制打包的场景，可以设置为False，配置为False的情况下直接在`<path>`路径下生成编译产物，不会打包为run包。

**示例：**

```cmake
set(package_name ${vendor_name})
npu_op_package(${package_name}                     # package name
    TYPE RUN                                       #指定编译产物形态，[RUN|STATIC|SHARED]
    CONFIG
        ENABLE_SOURCE_PACKAGE True                 #是否将源码打包到run包中
        ENABLE_BINARY_PACKAGE True                 #是否编译Kernel二进制
        INSTALL_PATH ${CMAKE_BINARY_DIR}/          # package的安装位置
)
```

### npu_op_package_add

将目标或文件添加到package中。

```cmake
# 添加目标
npu_op_package_add(<package_name> LIBRARY <target_name1> [<target_name2>...] )
# 添加文件，仅给run包模式使用
npu_op_package_add(<package_name> FILES <file_name1> [<file_name2>...] [TYPE <target_type>] [PACKAGE_PATH <pkg_path>])
```

**参数说明：**

- `<package_name>`（必选）：package的名称。
- `LIBRARY`（必选）：指定需要添加到package中的目标名称。
  - `<target_name1> [<target_name2>...]`（必选）：目标名称列表。
- `FILES`（必选）：指定需要添加到package中的文件名称。
  - `<file_name1> [<file_name2>...]`（必选）：文件名称列表。
- `[TYPE <target_type>]`（可选）：指定文件类型，将文件安装到对应的目录中，取值为`ACLNN`、`GRAPH`。配置为`ACLNN`，会将文件打包至run包目录下aclnn单算子调用头文件所在目录；配置为`GRAPH`，会将文件打包至run包目录下入图原型定义头文件目录下。
- `[PACKAGE_PATH <pkg_path>]`（可选）：指定文件在包中的相对路径位置。`TYPE`和`PACKAGE_PATH`参数互斥，即只能选择其中一个进行配置。

**示例：**

```cmake
# 添加Host侧相关library
npu_op_package_add(${package_name}
    LIBRARY
        cust_optiling
        cust_opapi
        cust_op_proto
)

# 添加Kernel侧相关library
npu_op_package_add(${package_name}
    LIBRARY ascendc_kernels
)
```

---

## Library类函数

### npu_op_library

创建Host侧库。

```cmake
npu_op_library(<library_name> TYPE <library_type> <files>)
```

**参数说明：**

- `<library_name>`（必选）：Host侧库的名称。
- `TYPE <library_type>`（必选）：Host库的类型，可选值为`TILING`、`ACLNN`、`GRAPH`、`TF_PLUGIN`、`ONNX_PLUGIN`。
  - `TILING`：Tiling相关库。
  - `ACLNN`：aclnn单算子调用库。
  - `GRAPH`：算子入图所需的算子原型库。
  - `TF_PLUGIN`：TensorFlow框架适配相关库。
  - `ONNX_PLUGIN`：ONNX框架适配相关库。
- `<files>`（必选）：设置参与编译的源文件。

**示例：**

```cmake
# 单算子调用场景，编译aclnn单算子调用库
file(GLOB autogen_aclnn_src ${ASCEND_AUTOGEN_PATH}/aclnn_*.cpp)
set_source_files_properties(${autogen_aclnn_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_opapi ACLNN
    ${autogen_aclnn_src}
)

# 算子入图场景，编译算子入图所需的算子原型库
file(GLOB proto_src ${ASCEND_AUTOGEN_PATH}/op_proto.cc)
set_source_files_properties(${proto_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_op_proto GRAPH
    ${ops_srcs}
    ${proto_src}
)

# 编译Tiling相关库
file(GLOB fallback_src ${ASCEND_AUTOGEN_PATH}/fallback_*.cpp)
set_source_files_properties(${fallback_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_optiling TILING
    ${ops_srcs}
    ${fallback_src}
)
```

### npu_op_kernel_library

创建Kernel侧库。

```cmake
npu_op_kernel_library(<target_name> SRC_BASE <path> TILING_LIBRARY <tiling_target>)
```

**参数说明：**

- `<target_name>`（必选）：目标的名称。
- `SRC_BASE <path>`（必选）：指定Kernel源码的base目录，要求配置绝对路径。例如示例中的`op_kernel`目录的绝对路径。
- `TILING_LIBRARY <tiling_target>`（必选）：指定依赖的Tiling目标。

**示例：**

```cmake
npu_op_kernel_library(ascendc_kernels
    SRC_BASE ${CMAKE_SOURCE_DIR}/op_kernel/
    TILING_LIBRARY cust_optiling
)
```

### npu_op_kernel_options

添加Kernel目标编译选项。

```cmake
npu_op_kernel_options(<target_name> <op_type> [COMPUTE_UNIT <soc_version>] OPTIONS …)
```

**参数说明：**

- `<target_name>`（必选）：目标的名称。
- `<op_type>`（必选）：定义配置生效的范围，取值为`ALL`、`OP_TYPE`。`ALL`表示对所有算子生效，`OP_TYPE`表示对特定算子生效。
- `[COMPUTE_UNIT <soc_version>]`（可选）：用于设置算子在具体AI处理器型号上的编译选项，不填写该选项时默认对所有型号生效。

  > **说明**
  > - 针对如下产品：在安装AI处理器的服务器执行`npu-smi info`命令进行查询，获取Name信息。实际配置值为AscendName，例如Name取值为xxxyy，实际配置值为Ascendxxxyy。
  >   - <!-- npu="910b" id1 -->Atlas A2训练系列产品 / Atlas A2推理系列产品。<!-- end id1 -->
  >   - <!-- npu="310b" id2 -->Atlas 200I/500 A2推理产品。<!-- end id2 -->
  >   - Atlas推理系列产品。
  >   - Atlas训练系列产品。
  > - <!-- npu="A3,910b" id3 -->针对Atlas A3训练系列产品 / Atlas A3推理系列产品，在安装AI处理器的服务器执行`npu-smi info -t board -i id -c chip_id`命令进行查询，获取Chip Name和NPU Name信息，实际配置值为Chip Name_NPU Name。例如Chip Name取值为Ascendxxx，NPU Name取值为1234，实际配置值为Ascendxxx_1234。其中：id为设备id（通过`npu-smi info -l`查出的NPU ID即为设备id）；chip_id为芯片id（通过`npu-smi info -m`查出的Chip ID即为芯片id）。<!-- end id3 -->
  > - <!-- npu="950" id4 -->针对Ascend 950PR / Ascend 950DT，在安装AI处理器的服务器执行`npu-smi info -t board -i id`命令进行查询，获取Chip Name和NPU Name信息，实际配置值为Chip Name_NPU Name。例如Chip Name取值为Ascendxxx，NPU Name取值为1234，实际配置值为Ascendxxx_1234。其中id为设备id（通过`npu-smi info -l`查出的NPU ID即为设备id）。
<!-- end id4 -->

- `OPTIONS …`（必选）：传递给编译器的编译选项。

**示例：**

```cmake
npu_op_kernel_options(ascendc_kernels ALL OPTIONS --save-temp-files -g)   #为算子添加编译选项
```

### npu_op_kernel_sources

描述Kernel目标的源码信息，包括设置算子的Kernel实现文件和源码路径等。

```cmake
npu_op_kernel_sources(<target_name> [OP_TYPE <op_type>] [KERNEL_DIR <path>] [COMPUTE_UNIT <soc_version>] [KERNEL_FILE <file>])
```

**参数说明：**

- `<target_name>`（必选）：目标的名称。
- `[OP_TYPE <op_type>]`（可选）：算子类型，必须与`KERNEL_FILE`同时存在。
- `[KERNEL_DIR <path>]`（可选）：指定Kernel源码相对于`SRC_BASE`的相对路径。若算子的源码文件没有平铺在`SRC_BASE`目录（通过`npu_op_kernel_library`设置）下，可以通过`KERNEL_DIR`指定特定目录。
- `[COMPUTE_UNIT <soc_version>]`（可选）：设置`KERNEL_FILE`在`<soc_version>`型号生效。默认`KERNEL_FILE`对所有型号生效。
- `[KERNEL_FILE <file>]`（可选）：指定算子入口的Kernel实现文件名。若算子的Kernel实现cpp文件需要自定义命名，需同时指定`OP_TYPE`（算子类型）和`KERNEL_FILE`（Kernel实现cpp文件名），以配置两者之间的对应关系。不配置时，Kernel实现cpp文件名和OpType之间需满足转换规则，参考[命名转换规则对照表](./命名转换规则对照表.md)。

**示例：**

```cmake
npu_op_kernel_sources(ascendc_kernels
    OP_TYPE AddCustom
    KERNEL_DIR ./Add
    COMPUTE_UNIT Ascendxxxyy
    KERNEL_FILE add_custom.cpp
)
```

### npu_op_device_tiling_library

创建Device侧Tiling库。使用该选项时，package的类型仅支持配置为RUN（run包模式）。

```cmake
npu_op_device_tiling_library(<target_name> <type> <files>)
```

**参数说明：**

- `<target_name>`（必选）：目标的名称。
- `<type>`（必选）：指定Tiling产物的类型。支持取值为`SHARED`、`STATIC`。
- `<files>`（必选）：指定Tiling源码文件。

**示例：**

```cmake
npu_op_device_tiling_library(device_tiling SHARED
    device_tiling_impl.cpp
)
```

---

## 其他

### npu_op_code_gen

执行代码生成过程，生成aclnn单算子调用代码和入图所需的原型定义代码。

```cmake
npu_op_code_gen(SRC <src_files> OUT_DIR <output_dir> PACKAGE <pkg_name> [COMPILE_OPTIONS ...] [JOIN_OP_DEF ...])
```

**参数说明：**

- `SRC <src_files>`（必选）：参与代码生成的源文件范围。
- `OUT_DIR <output_dir>`（必选）：生成代码的输出路径。
- `PACKAGE <pkg_name>`（必选）：指定生成代码的package名称。
- `[COMPILE_OPTIONS ...]`（可选）：自定义编译过程中的编译选项。
- `[JOIN_OP_DEF ...]`（可选）：默认为False。设置为False，编译CMakePreset.json中设置的AI处理器型号对应的算子；设置成True，则编译CMakePreset.json和算子原型注册中共有的AI处理器型号对应的算子。

**示例：**

```cmake
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} ops_srcs)
npu_op_code_gen(
    SRC ${ops_srcs}
    PACKAGE ${package_name}
    COMPILE_OPTIONS -g
    OUT_DIR ${ASCEND_AUTOGEN_PATH}
    JOIN_OP_DEF False
)
```

---

## 相关文档

- [算子工程编译拓展](../编译与部署/基本流程.md#advanced-build-organization) — 完整的CMakeLists.txt编写方法与编译命令说明。
- [算子原型定义](../设计与实现/算子原型定义.md) — Kernel实现文件名与OpType的转换规则。
- [如何使用workspace](../../../附录/常用操作/如何使用workspace.md) — Tiling函数中workspace的设置方法。
