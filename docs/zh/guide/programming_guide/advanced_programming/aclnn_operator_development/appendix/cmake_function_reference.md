# CMake函数参考

本文是参考内容，整理算子工程编译过程涉及的CMake函数接口，供需要深入了解编译机制的高级开发者查阅。

## 概述

自定义算子工程的CMakeLists.txt由开发者直接编写。为便于开发者组织编译脚本，CANN软件包中将算子工程编译所需的CMake脚本打包为CMake modules，开发者可通过`find_package(ASC REQUIRED)`查找并使用这些函数。

本文说明算子工程编译过程中使用的8个CMake函数，按用途分为**Package类**、**Library类**、**其他**三类。

## 函数一览

| 分类 | 函数 | 功能 |
|------|------|------|
| Package | `npu_op_package` | 创建一个package。 |
| Package | `npu_op_package_add` | 将目标或文件添加到package中。 |
| Library | `npu_op_library` | 创建Host侧库。 |
| Library | `npu_op_kernel_library` | 创建核函数（Kernel）侧库。 |
| Library | `npu_op_kernel_options` | 添加核函数（Kernel）目标编译选项。 |
| Library | `npu_op_kernel_sources` | 描述核函数（Kernel）目标的源码信息。 |
| Library | `npu_op_device_tiling_library` | 创建Device侧Tiling库。 |
| 其他 | `npu_op_code_gen` | 执行代码生成过程，生成aclnn单算子调用代码和入图所需的原型定义代码。 |

---

## Package类函数

### npu_op_package

创建一个package。

```cmake
npu_op_package(<package_name>
    TYPE <RUN|SHARED|STATIC>
    [CONFIG <key> <value> ...]
)
```

**参数说明：**

- `<package_name>`（必选）：package的名称。
- `TYPE <type>`（必选）：package的类型，取值为`RUN`、`SHARED`、`STATIC`。分别对应算子run包形式、算子动态库形式与算子静态库形式。
- `CONFIG <key> <value> ...`（可选）：用于配置package的内容和输出位置。各配置项以“参数名 参数值”的形式写在`CONFIG`之后，支持以下选项：
  - `ENABLE_SOURCE_PACKAGE <value>`：是否将源码打包到package中，默认使用工程中`ENABLE_SOURCE_PACKAGE`的值（未设置时为`True`）。
  - `ENABLE_BINARY_PACKAGE <value>`：是否将二进制文件打包到package中，默认使用工程中`ENABLE_BINARY_PACKAGE`的值（未设置时为`True`）。`SHARED`和`STATIC`模式下，必须开启此选项。
  - `INSTALL_PATH <path>`：指定构建产物的输出路径，例如生成的run包存放位置。默认使用`ASCENDC_INSTALL_PREFIX`：通常为构建目录；设置了`CMAKE_INSTALL_PREFIX`时，则使用该路径。
  - `ENABLE_CPACK <value>`：是否生成run包，仅用于`RUN`模式，默认为`True`。设置为`False`后，执行`install`目标会将目录和文件安装到`INSTALL_PATH`下，不再生成run包。
  - `ENABLE_DEFAULT_PACKAGE_NAME_RULE <value>`：默认为`True`，`RUN`模式下安装包默认命名为`custom_opp_<os>_<arch>.run`；设置为`False`时命名为`<package_name>.run`。
  - `VENDOR_NAME <name>`：算子包的厂商名称，默认为`<package_name>`，用于确定包内的`vendors/<name>`目录名。

**示例：**

```cmake
set(package_name ${vendor_name})
npu_op_package(${package_name}                     # package name
    TYPE RUN                                       #指定编译产物形态，[RUN|STATIC|SHARED]
    CONFIG
        ENABLE_SOURCE_PACKAGE True                 #是否将源码打包到package中
        ENABLE_BINARY_PACKAGE True                 #是否编译核函数（Kernel）二进制
        INSTALL_PATH ${CMAKE_BINARY_DIR}/          # package的安装位置
)
```

### npu_op_package_add

将目标或文件添加到package中。

```cmake
# 添加目标
npu_op_package_add(<package_name> LIBRARY <target_name1> [<target_name2>...]
    [PACKAGE_PATH <pkg_path>])
# 添加文件，仅给run包模式使用
npu_op_package_add(<package_name> FILES <file_name1> [<file_name2>...] TYPE <ACLNN|GRAPH>)
npu_op_package_add(<package_name> FILES <file_name1> [<file_name2>...] PACKAGE_PATH <pkg_path>)
```

**参数说明：**

- `<package_name>`（必选）：package的名称。

`LIBRARY`和`FILES`必须选择一个，不能同时使用。

- `LIBRARY <target_name1> [<target_name2>...]`：指定需要添加到package中的库目标。添加前，需先通过对应的`npu_op_*_library`函数定义该目标。
- `FILES <file_name1> [<file_name2>...]`：指定需要添加到package中的文件，仅支持`RUN`模式。添加文件时，还需通过`TYPE`或`PACKAGE_PATH`指定包内位置，否则文件不会被安装。
- `[TYPE <target_type>]`（可选）：指定文件类型，将文件安装到对应的目录中，取值为`ACLNN`、`GRAPH`。配置为`ACLNN`，会将文件打包至run包目录下aclnn单算子调用头文件所在目录；配置为`GRAPH`，会将文件打包至run包目录下入图原型定义头文件目录下。
- `[PACKAGE_PATH <pkg_path>]`（可选）：指定文件或Host侧库在run包中的存放位置。与`npu_op_package`的`INSTALL_PATH`不同，此参数设置的是包内目录。
  - 添加文件（`FILES`）时：填写相对于`packages/vendors/<vendor_name>/`的路径，例如`include`。`PACKAGE_PATH`与`TYPE`只能选择一个。
  - 添加Host侧库（`LIBRARY`）时：填写相对于包根目录的路径，例如`packages/vendors/customize/op_api`，用于替换该类库的默认目录。

同一次调用中，每种库类型只处理最后一个目标。例如，同时传入两个`TILING`目标时，只处理后一个。因此，多个同类库应分次添加；同时传入多个核函数库还会产生警告。

**示例：**

```cmake
# 添加Host侧相关library
npu_op_package_add(${package_name}
    LIBRARY
        cust_optiling
        cust_opapi
        cust_op_proto
)

# 添加核函数（Kernel）侧相关library
npu_op_package_add(${package_name}
    LIBRARY ascendc_kernels
)
```

---

## Library类函数

> [!NOTE]说明
> 本节`npu_op_kernel_options`/`npu_op_kernel_sources`的`SOC_SERIES`/`COMPUTE_UNIT`参数，与[编译配置](../compilation_and_deployment/basic_process.md#compile-configuration)中工程级的`ASCEND_SOC_SERIES`/`ASCEND_COMPUTE_UNIT`变量是两个不同层次的概念，参数名相同但含义不同，容易混淆：

| 层次 | 配置项 | 作用 |
|---|---|---|
| 全局 | `ASCEND_SOC_SERIES`（或`ASCEND_COMPUTE_UNIT`），工程级CMake变量 | 决定整个工程为哪些AI处理器系列构建产物 |
| 细粒度 | 本节函数的`SOC_SERIES`/`COMPUTE_UNIT`参数 | 在全局已配置的范围内，这一条编译选项或核函数（Kernel）源码具体用在哪些型号上 |

### npu_op_library

创建Host侧库。

```cmake
npu_op_library(<library_name> <library_type> <files>...
    [CONFIG <key> <value> ...])
```

**参数说明：**

- `<library_name>`（必选）：Host侧库的名称。
- `<library_type>`（必选）：Host库的类型，可选值为`TILING`、`ACLNN`、`GRAPH`、`TF_PLUGIN`、`ONNX_PLUGIN`。
  - `TILING`：Tiling相关库。
  - `ACLNN`：aclnn单算子调用库。
  - `GRAPH`：算子入图所需的算子原型库。
  - `TF_PLUGIN`：TensorFlow框架适配相关库。
  - `ONNX_PLUGIN`：ONNX框架适配相关库。
- `<files>...`（必选）：设置参与编译的源文件，相对路径以当前源码目录为基准。
- `CONFIG <key> <value> ...`（可选）：用于配置run包中Host侧动态库的名称，支持以下开关，参数值可填写`True`或`False`。开启后，动态库使用`<library_name>`作为库名。
  - `SET_OPAPI_NAME`：用于`ACLNN`库，默认库名为`cust_opapi`。
  - `SET_TILING_NAME`：用于`TILING`库，默认库名为`cust_opmaster_rt2.0`。
  - `SET_OP_PROTO_NAME`：用于`GRAPH`库，默认库名为`cust_opsproto_rt2.0`。

修改Tiling库或原型库的名称时，CMake会提示兼容性警告。通常保留默认名称即可。

`<library_type>`需使用上述大写形式。`ACLNN`、`TILING`和`GRAPH`先配置目标文件的编译，再由`npu_op_package_add`配置最终库的链接；`TF_PLUGIN`和`ONNX_PLUGIN`直接配置动态库的编译和链接，默认库名分别为`cust_tf_parsers`和`cust_onnx_parsers`。上述函数只配置构建任务，实际编译和链接在执行构建命令时完成。

**示例：**

下面的代码放在Host侧CMakeLists.txt中。调用前，需已通过`npu_op_package`创建`${package_name}`算子包（`TYPE RUN`），并通过`npu_op_code_gen`在`${ASCEND_AUTOGEN_PATH}`中生成代码；`${ops_srcs}`为参与代码生成的Host侧源码列表。

```cmake
# 配置aclnn单算子调用库，使用默认库名cust_opapi
file(GLOB autogen_aclnn_src ${ASCEND_AUTOGEN_PATH}/aclnn_*.cpp)
set_source_files_properties(${autogen_aclnn_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_opapi ACLNN
    ${autogen_aclnn_src}
)

# 配置算子原型库，入图场景需要，使用默认库名cust_opsproto_rt2.0
file(GLOB proto_src ${ASCEND_AUTOGEN_PATH}/op_proto.cc)
set_source_files_properties(${proto_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_op_proto GRAPH
    ${ops_srcs}
    ${proto_src}
)

# 配置Tiling库，使用默认库名cust_opmaster_rt2.0
file(GLOB fallback_src ${ASCEND_AUTOGEN_PATH}/fallback_*.cpp)
set_source_files_properties(${fallback_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_optiling TILING
    ${ops_srcs}
    ${fallback_src}
)

# 将上述目标加入算子包，并配置最终动态库的链接和安装
npu_op_package_add(${package_name}
    LIBRARY cust_opapi cust_op_proto cust_optiling
)
```

如需将aclnn动态库命名为`libmy_opapi.so`，可将上例中的`npu_op_library(cust_opapi ACLNN ...)`替换为：

```cmake
npu_op_library(my_opapi ACLNN
    ${autogen_aclnn_src}
    CONFIG SET_OPAPI_NAME True
)
```

同时，将`npu_op_package_add`中的`cust_opapi`改为`my_opapi`。`CONFIG`中的值是是否启用自定义库名的开关，实际库名由第一个参数`my_opapi`决定。

### npu_op_kernel_library

配置核函数（Kernel）侧库，指定源码目录和依赖的Tiling库。调用后，还需通过`npu_op_package_add`将其加入package，才能生成相应的编译和打包任务。

```cmake
npu_op_kernel_library(<target_name> SRC_BASE <path> TILING_LIBRARY <tiling_target>)
```

**参数说明：**

- `<target_name>`（必选）：目标的名称。
- `SRC_BASE <path>`（必选）：指定核函数（Kernel）源码的base目录，要求配置绝对路径。例如示例中的`op_kernel`目录的绝对路径。
- `TILING_LIBRARY <tiling_target>`（必选）：指定通过`npu_op_library(... TILING ...)`声明的Tiling目标。

**示例：**

```cmake
npu_op_kernel_library(ascendc_kernels
    SRC_BASE ${CMAKE_SOURCE_DIR}/op_kernel/
    TILING_LIBRARY cust_optiling
)
```

### npu_op_kernel_options

添加核函数（Kernel）目标编译选项。

```cmake
npu_op_kernel_options(<target_name> <op_type> [COMPUTE_UNIT <soc_version>... | SOC_SERIES <series>...] OPTIONS …)
```

**参数说明：**

- `<target_name>`（必选）：目标的名称。
- `<op_type>`（必选）：指定编译选项的生效范围。填写`ALL`表示对所有算子生效；填写具体的算子类型（如`AddCustom`）表示仅对该算子生效。
- `[COMPUTE_UNIT <soc_version>...]`（可选）：用于设置算子在具体AI处理器型号上的编译选项，不填写该选项时默认对所有型号生效。与`SOC_SERIES`不能同时使用。
- `[SOC_SERIES <series>...]`（可选）：用于按AI处理器系列设置编译选项。输入不区分大小写；与`COMPUTE_UNIT`不能同时使用。`SOC_SERIES`关键字后必须提供至少一个值；同一次调用同时提供`SOC_SERIES`和`COMPUTE_UNIT`，或只提供`SOC_SERIES`关键字不提供值，CMake配置阶段会报错。
>[!NOTE]说明 
>其中，AI处理器型号`soc_version`请通过以下方式获取：
><!-- npu="910b,910,310p,310b" id1 -->
>- 针对如下产品：在安装AI处理器的服务器执行`npu-smi info`命令进行查询，获取Name信息。实际配置值为AscendName，例如Name取值为xxxyy，实际配置值为Ascendxxxyy。<br><br>
>    <!-- npu="910b" id2 -->
>    Atlas A2系列产品  
>    <!-- end id2 -->
>
>    <!-- npu="310b" id3 -->
>    Atlas 200I/500 A2推理产品  
>    <!-- end id3 -->
>
>    <!-- npu="310p" id4 -->
>    Atlas推理系列产品  
>    <!-- end id4 -->
>
>    <!-- npu="910" id5 -->
>    Atlas训练系列产品<br><br> 
>    <!-- end id5 -->
><!-- end id1 -->
><!-- npu="A3" id6 -->
>- 针对Atlas A3系列产品，在安装AI处理器的服务器执行`npu-smi info -t board -i <id> -c <chip_id>`命令进行查询，获取Chip Name和NPU Name信息，实际配置值为`Chip Name_NPU Name`。例如Chip Name取值为Ascendxxx，NPU Name取值为1234，实际配置值为Ascendxxx_1234。其中：<br>`id`：设备id，通过`npu-smi info -l`命令查出的NPU ID即为设备id，命令中表示为`<id>`。<br>`chip_id`：芯片id，通过`npu-smi info -m`命令查出的Chip ID即为芯片id，命令中表示为`<chip_id>`。<br><br>
><!-- end id6 -->
><!-- npu="950" id7 -->
>- 针对Ascend 950PR&950DT系列产品，在安装AI处理器的服务器执行`npu-smi info -t board -i <id>`命令进行查询，获取Chip Name和NPU Name信息，实际配置值为`Chip Name_NPU Name`。例如Chip Name取值为Ascendxxx，NPU Name取值为1234，实际配置值为Ascendxxx_1234。<br>其中：`id`为设备id，通过`npu-smi info -l`命令查出的NPU ID即为设备id，命令中表示为`<id>`。
><!-- end id7 -->
- `OPTIONS …`（必选）：核函数编译选项，至少提供一个值。普通选项传给编译器；`--save-temp-files`、`--tiling_key`、`--kernel-template-input`等选项由构建脚本转换为对应的编译配置。
  - `--cce-auto-sync`：开启毕昇编译器自动同步，由编译器自动插入部分同步指令。自定义算子开发工程默认开启auto-sync（`--cce-auto-sync=on`），无需重复设置；可通过`--cce-auto-sync=off`关闭。
  - `--cce-auto-sync-log=<file>`：将编译器自动插入同步指令的信息输出到指定文件。需同时设置`-g`，以获取对应的源码行号。

**示例：**

```cmake
npu_op_kernel_options(ascendc_kernels ALL OPTIONS --save-temp-files -g)   #为算子添加编译选项

npu_op_kernel_options(ascendc_kernels AddCustom SOC_SERIES Ascendxxx Ascendyyy OPTIONS -DASCENDC_DEBUG)
```

下面以AddCustom为例，将自动同步日志输出到构建目录下的`add_custom_auto_sync.log`。工程默认已开启auto-sync，因此只需添加日志选项和`-g`：

```cmake
npu_op_kernel_options(ascendc_kernels AddCustom
    OPTIONS -g "--cce-auto-sync-log=${CMAKE_CURRENT_BINARY_DIR}/add_custom_auto_sync.log"
)
```

### npu_op_kernel_sources

描述核函数（Kernel）目标的源码信息，包括设置算子的核函数（Kernel）实现文件和源码路径等。

```cmake
npu_op_kernel_sources(<target_name> [OP_TYPE <op_type>] [KERNEL_DIR <path>] [COMPUTE_UNIT <soc_version>... | SOC_SERIES <series>...] [KERNEL_FILE <file>])
```

**参数说明：**

- `<target_name>`（必选）：目标的名称。
- `[OP_TYPE <op_type>]`（可选）：算子类型，必须与`KERNEL_FILE`同时存在。不指定时，配置对所有算子生效。
- `[KERNEL_DIR <path>]`（可选）：指定核函数（Kernel）源码相对于`SRC_BASE`的相对路径。若算子的源码文件没有平铺在`SRC_BASE`目录（通过`npu_op_kernel_library`设置）下，可以通过`KERNEL_DIR`指定特定目录。
- `[COMPUTE_UNIT <soc_version>...]`（可选）：设置`KERNEL_FILE`在指定型号生效。默认`KERNEL_FILE`对所有型号生效。与`SOC_SERIES`不能同时使用。
- `[SOC_SERIES <series>...]`（可选）：设置`KERNEL_FILE`在指定系列生效。输入不区分大小写；与`COMPUTE_UNIT`不能同时使用，`SOC_SERIES`关键字后必须提供至少一个值；同一次调用同时提供`SOC_SERIES`和`COMPUTE_UNIT`，或只提供`SOC_SERIES`关键字不提供值，CMake配置阶段会报错。
- `[KERNEL_FILE <file>]`（可选）：指定算子入口的核函数（Kernel）实现文件名。若算子的核函数（Kernel）实现cpp文件需要自定义命名，需同时指定`OP_TYPE`（算子类型）和`KERNEL_FILE`（核函数（Kernel）实现cpp文件名），以配置两者之间的对应关系。不配置时，核函数（Kernel）实现cpp文件名和OpType之间需满足转换规则，参考[命名转换规则对照表](./naming_conversion_table.md)。

**示例：**

```cmake
npu_op_kernel_sources(ascendc_kernels
    OP_TYPE AddCustom
    KERNEL_DIR ./Add
    COMPUTE_UNIT Ascendxxxyy
    KERNEL_FILE add_custom.cpp
)
```

也可以使用`SOC_SERIES`按系列指定：

```cmake
npu_op_kernel_sources(ascendc_kernels
    OP_TYPE AddCustom
    KERNEL_DIR ./Add
    SOC_SERIES Ascendxxx
    KERNEL_FILE add_custom.cpp
)
```

如果同一算子在不同AI处理器系列上需要不同的核函数（Kernel）实现，可多次调用`npu_op_kernel_sources`，分别用`SOC_SERIES`指定各自生效的范围：

以下`ascendxxx1`、`ascendxxx2`和`ascendxxx3`为不同AI处理器系列的构建目标占位符，请替换为实际取值。

```cmake
npu_op_kernel_sources(ascendc_kernels
    OP_TYPE AddCustom
    SOC_SERIES ascendxxx1 ascendxxx2
    KERNEL_FILE add_custom_v220.cpp
)
npu_op_kernel_sources(ascendc_kernels
    OP_TYPE AddCustom
    SOC_SERIES ascendxxx3
    KERNEL_FILE add_custom_v310.cpp
)
```

### npu_op_device_tiling_library

创建用于Tiling下沉的Device侧Tiling库。创建后，需通过`npu_op_package_add`将其加入package，package的类型仅支持`RUN`（run包模式）。

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
npu_op_package_add(${package_name} LIBRARY device_tiling)
```

---

## 其他

### npu_op_code_gen

执行代码生成过程，生成aclnn单算子调用代码和入图所需的原型定义代码。

```cmake
npu_op_code_gen(SRC <src_files>...
    OUT_DIR <output_dir>
    PACKAGE <pkg_name>
    [COMPILE_OPTIONS <options>...]
    [LINK_OPTIONS <options>...]
    [JOBS <count>]
    [JOIN_OP_DEF <True|False>]
    [OPTIONS <key> <value> ...]
)
```

**参数说明：**

- `SRC <src_files>`（必选）：参与代码生成的源文件范围。
- `OUT_DIR <output_dir>`（必选）：生成代码的输出路径。
- `PACKAGE <pkg_name>`（必选）：指定生成代码的package名称。
- `COMPILE_OPTIONS <options>...`（可选）：设置Host侧原型代码的编译选项，用于生成`libascend_all_ops.so`。核函数编译选项请通过`npu_op_kernel_options`设置。
- `LINK_OPTIONS <options>...`（可选）：设置生成`libascend_all_ops.so`时的链接选项。
- `JOBS <count>`（可选）：设置Host侧原型代码的编译并行数，必须为正整数。
- `JOIN_OP_DEF <True|False>`（可选）：设置生成型号配置的方式，默认为`False`。参数值区分大小写，仅支持`True`和`False`；其他值会产生警告，并按`False`处理。
  - `False`：根据工程中`ASCEND_SOC_SERIES`或`ASCEND_COMPUTE_UNIT`指定的构建目标生成型号配置。
  - `True`：根据算子原型中`AddConfig`注册的型号生成配置。后续仍按工程指定的目标编译核函数，因此`AddConfig`需覆盖这些目标。

  该参数与是否使用`CMakePresets.json`无关，也不会改变工程的构建目标。
- `OPTIONS <key> <value> ...`（可选）：设置opbuild的代码生成选项，各选项以“参数名 参数值”的形式提供：
  - `OPS_PROJECT_NAME`：设置生成代码的项目名称。
  - `OPS_DIRECT_ACCESS_PREFIX`：设置直接调用接口的名称前缀。
  - `OPS_ACLNN_GEN`：控制是否生成aclnn代码。
  - `OPS_ENABLE_SOURCE_PACKAGE`：设置源码发布配置。
  - `OPS_VENDOR_NAME`：设置代码生成使用的厂商名称。
  - `OPS_PROTO_SEPARATE`：控制是否按算子分别生成原型文件。

该函数在执行`cmake`配置工程时生成代码，包括aclnn调用代码、原型定义、Tiling辅助代码和型号配置等。请先创建package，再调用本函数；代码生成完成后，才能使用生成的文件，以及`npu_op_kernel_options`、`npu_op_kernel_sources`和`npu_op_kernel_library`。

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

- [算子工程编译拓展](../compilation_and_deployment/basic_process.md#advanced-build-organization) — 完整的CMakeLists.txt编写方法与编译命令说明。
- [算子原型定义](../design_and_implementation/operator_prototype_definition.md) — 核函数（Kernel）实现文件名与OpType的转换规则。
- [如何使用workspace](../../../appendix/common_operations/how_to_use_workspace.md) — Tiling函数中workspace的设置方法。
