# 编译与部署基本流程

本文是基础内容，介绍算子核函数（Kernel）侧和Host侧实现开发完成后，如何对算子工程进行编译、生成自定义算子安装包\*.run。详细的编译操作包括：

-   编译Ascend C算子核函数（Kernel）侧代码实现文件\*.cpp，分为源码发布和二进制发布两种方式。
    -   **源码发布**<a id="source-release"></a>：不对算子核函数（Kernel）侧实现进行编译，保留算子核函数（Kernel）源码文件\*.cpp。该方式可以支持算子的在线编译、通过ATC模型转换的方式编译算子的场景。
    -   **二进制发布**：对算子核函数（Kernel）侧实现进行编译，生成描述算子相关信息的json文件\*.json和算子二进制文件\*.o。算子调用时，如果需要直接调用算子二进制，则使用该编译方式，比如通过[单算子API调用](../invocation/single_operator_api_call.md)的方式完成单算子的调用，PyTorch框架中单算子调用的场景，动态网络中调用算子的场景。

-   编译Ascend C算子Host侧代码实现文件\*.cpp、\*.h。
    -   将原型定义和shape推导实现编译成算子原型定义动态库libcust\_opsproto\_\*.so，并生成算子原型对外接口op\_proto.h。
    -   将Tiling实现编译成Tiling动态库liboptiling.so等。
    -   基于算子原型定义，自动生成单算子API调用代码和头文件aclnn\_\*.h，并编译生成单算子API调用的动态库libcust\_opapi.so。

上述编译过程示意图如下：


![](../../../../figures/operator_project_compilation_diagram.png "算子工程编译示意图")

### 基本流程

使用msOpGen生成的工程，已经包含完整CMake结构，不需要单独编写CMakeLists.txt处理；如果需要自行编写CMakeLists.txt，请参考[编译组织](#编译组织)。以[快速入门的AddCustom](../aclnn_quick_start.md)为例，msOpGen生成的算子工程编译部署只需三步：

1. 修改`CMakePresets.json`，填写`ASCEND_SOC_SERIES`或`ASCEND_COMPUTE_UNIT`（二选一）以及`vendor_name`（厂商名称）等编译配置选项；
2. 执行`./build.sh`，生成`custom_opp_*.run`安装包；
3. 执行`./custom_opp_*.run`，安装到运行环境。

以下各节详细介绍每一步的选项和细节。

## 编译配置<a id="compile-configuration"></a>

编译配置选项的必改参数有两个，其中芯片系列配置在`ASCEND_SOC_SERIES`和`ASCEND_COMPUTE_UNIT`中二选一：

| 参数名称 | 参数描述 | 默认值 |
|---------|---------|--------|
| ASCEND_SOC_SERIES | AI处理器系列，支持配置一个或多个值。输入不区分大小写，配置值会先归一为小写。 | 无 |
| ASCEND_COMPUTE_UNIT | AI处理器型号，按实际运行芯片配置。 | "ascendxxx" |
| vendor_name | 自定义算子所属厂商名称，用于形成`vendors/<vendor_name>`安装目录，避免与其他厂商算子包冲突；同一安装路径下`vendor_name`相同的算子包会安装到同一目录，同名文件会覆盖。 | "customize" |

`ASCEND_SOC_SERIES`与`ASCEND_COMPUTE_UNIT`不能同时配置，且`ASCEND_SOC_SERIES`不能为空。

除上述必改参数外，其他可选编译配置选项详情如下：

| 类型 | 选项名 | 默认值 | 说明 |
|------|--------|--------|------|
| STRING | CMAKE_BUILD_TYPE | Release | 编译类型，支持Release和Debug，Release版本：不包含调试信息，编译最终发布的版本；Debug版本：包含调试信息，用于开发调试。 |
| PATH | ASCEND_AUTOGEN_PATH | <CMAKE_BINARY_DIR>/autogen | 自动生成代码的存放路径。 |
| PATH | ASCEND_CANN_PACKAGE_PATH | - | CANN软件包路径，默认路径示例如下：/usr/local/Ascend/cann。 |
| BOOL | ENABLE_SOURCE_PACKAGE | TRUE | 是否开启源码编译，如果使用npu_op_package配置了源码和二进制编译相关配置，npu_op_package配置的优先级更高。 |
| BOOL | ENABLE_BINARY_PACKAGE | TRUE | 是否开启二进制编译，package的类型配置为SHARED或STATIC时，必须指定为TRUE。如果使用npu_op_package配置了源码和二进制编译相关配置，npu_op_package配置的优先级更高。 |
| BOOL | ENABLE_CROSS_COMPILE | FALSE | 是否启用交叉编译，参考[交叉编译](./cross_compilation.md)。 |
| PATH | CMAKE_CROSS_PLATFORM_COMPILER | /your/cross/compiler/path | 交叉编译工具路径。 |
| BOOL | ASCEND_PACK_SHARED_LIBRARY | FALSE | 是否开启动态库编译。 |
| BOOL | ASCEND_SKIP_FAILED_COMPUTE_UNIT | FALSE | 该参数用于控制部分AI处理器型号对应的算子编译失败时，是否跳过失败并继续编译其他型号的算子，设置成True为跳过失败，跳过失败不影响其他AI处理器型号对应算子的编译流程；设置成False，遇到失败会影响其他AI处理器型号对应算子的编译流程。 |

修改编译配置选项的方式有两种：

### 方式一：修改CMakePresets.json

工程目录下有`CMakePresets.json`时，修改其中的`cacheVariables`即可。msOpGen生成的工程默认使用此方式。

> [!NOTE]说明
>
> 直接执行`cmake --preset=default`要求CMake >= 3.19。msOpGen生成的`build.sh`会在低版本CMake环境下解析`CMakePresets.json`并转成普通`cmake -D`参数，因此优先建议使用工程自带的`build.sh`。

**最小配置示例**：

```json
{
    "version": 1,
    "cmakeMinimumRequired": {
        "major": 3,
        "minor": 19,
        "patch": 0
    },
    "configurePresets": [
        {
            "name": "default",
            "cacheVariables": {
                "ASCEND_COMPUTE_UNIT": {
                    "type": "STRING",
                    "value": "ascendxxyy"
                },
                "vendor_name": {
                    "type": "STRING",
                    "value": "customize"
                }
            }
        }
    ]
}
```

### 方式二：在CMakeLists.txt中设置

直接在顶层CMakeLists.txt中设置变量，并通过`npu_op_*`接口配置编译行为，无需CMakePresets.json文件。

```cmake
# 设置必要变量。也可以使用变量ASCEND_SOC_SERIES，二者不能同时设置。
set(ASCEND_COMPUTE_UNIT "ascendxxxyy") | set(ASCEND_SOC_SERIES "Ascendxxx")
set(vendor_name "customize")

# 配置编译产物包（通过CONFIG参数替代ENABLE_SOURCE_PACKAGE / ENABLE_BINARY_PACKAGE等变量）
npu_op_package(${vendor_name}
    TYPE RUN
    CONFIG
        ENABLE_SOURCE_PACKAGE True
        ENABLE_BINARY_PACKAGE True
        INSTALL_PATH ${CMAKE_BINARY_DIR}/
)
```

其他配置（如动态库编译、交叉编译等）也可在CMakeLists.txt中通过变量或`npu_op_package` CONFIG参数设置。完整参数说明请参考[cmake函数参考](../appendix/cmake_function_reference.md)。

需要注意如果`npu_op_package(... CONFIG ...)`中硬编码了`ENABLE_SOURCE_PACKAGE`或`ENABLE_BINARY_PACKAGE`的值（如本节样例直接写True），命令行或`CMakePresets.json`中的同名变量将无法覆盖。要允许外部变量生效，CONFIG中应使用`${ENABLE_SOURCE_PACKAGE}`、`${ENABLE_BINARY_PACKAGE}`变量形式。

## 执行编译

有两种编译方法：

**方法一：使用build.sh脚本**

如果工程目录下有`build.sh`脚本（msOpGen生成的工程默认提供），直接执行：

```bash
./build.sh
```

**方法二：执行cmake命令**

```bash
cmake -S . -B build_out --preset=default
cmake --build build_out --target binary package -j$(nproc)
```

- `cmake -S . -B build_out --preset=default`是CMake的配置和生成阶段：从当前目录读取源码、`CMakeLists.txt`和`CMakePresets.json`中的`default`配置，在`build_out`目录下生成后续编译需要的构建文件。

- `cmake --build build_out --target binary package -j$(nproc)`是实际编译和打包阶段：使用`build_out`中已经生成的构建文件，执行`binary`和`package`目标；`binary`负责编译算子二进制相关产物，`package`负责生成算子安装包，`-j$(nproc)`表示按当前机器CPU核数并行编译。

编译成功后，会在编译产物目录下生成自定义算子安装包`custom_opp_<target os>_<target architecture>.run`。该run包包含算子原型库、Tiling库、aclnn API库和核函数（Kernel）二进制，部署后框架会自动扫描并加载。

**编译日志存储**（可选）：

用户如果需要编译过程日志存盘，可以使用环境变量ASCENDC\_BUILD\_LOG\_DIR来控制存储路径。用户设置该选项之后，如果编译过程中无错误产生，则对应的log文件后缀会添加"\_success"，若编译过程有错误产生，则会在屏幕打印对应的报错信息，以及指示用户log文件的具体路径与文件名，同时，对应log文件后缀会添加“\_error”。

```
# 如希望编译日志存储在/home/build_log/，则可以按照如下设置，默认不打开日志存储
export ASCENDC_BUILD_LOG_DIR=/home/build_log/
```

<a id="算子包部署"></a>
## 算子包部署<a id="operator-package-deployment"></a>

编译生成的算子安装包需要部署到运行环境。

### 安装算子包

在自定义算子包所在路径下，执行安装命令：

```bash
# source CANN包的环境变量，以默认安装路径为例
source /usr/local/Ascend/cann/set_env.sh
# 安装自定义算子run包
./custom_opp_<target os>_<target architecture>.run --install-path=<path>
```

`--install-path`是可选参数，指定安装目录。支持绝对路径，要求对目标路径有读写权限。

#### 默认安装（不指定--install-path）

安装到`$ASCEND_OPP_PATH/vendors/<vendor_name>`目录（`$ASCEND_OPP_PATH`通常等于`${CANN安装路径}/opp`，以默认安装路径为例：`/usr/local/Ascend/cann/opp`）。

> **路径关系说明**：默认安装场景下，算子包部署到`$ASCEND_OPP_PATH/vendors/<vendor_name>`。安装脚本会将当前vendor写入`$ASCEND_OPP_PATH/vendors/config.ini`的`load_priority`配置项，框架按照该配置加载默认路径中的算子包。

**表3** 默认安装与指定目录安装对比。

| 安装方式 | 安装命令 | 安装路径 | 自定义算子包安装后是否需要用户自行source自定义算子包的环境变量 | 优先级 |
|---------|---------|---------|----------------|--------|
| 默认安装 | `./custom_opp_*.run` | `${ASCEND_OPP_PATH}/vendors/<vendor_name>` | 否，自动生效 | 较低 |
| 指定目录安装 | `./custom_opp_*.run --install-path=<path>` | `<path>/vendors/<vendor_name>` | 是，需要source环境变量 | 较高 |

> [!NOTE]说明
> - 默认安装路径权限与CANN软件包安装用户和配置有关，如果因权限不足导致安装失败，可使用指定目录安装或联系CANN安装用户修改权限。
> - 指定目录安装后，在调用算子前需要执行`source <path>/vendors/<vendor_name>/bin/set_env.bash`。该脚本会将算子包目录`<path>/vendors/<vendor_name>`添加到`ASCEND_CUSTOM_OPP_PATH`环境变量的首部，供框架在运行时查找自定义算子包；同时将`<path>/vendors/<vendor_name>/op_api/lib/`添加到`LD_LIBRARY_PATH`环境变量的首部，供系统动态链接器查找算子动态库。
> - 算子包要求架构与系统架构一致，且满足glibc版本依赖，否则可使用`--force`强制安装。

<a id="部署后的目录结构"></a>
### 部署后的目录结构

以默认安装场景为例：

```
├── opp    //算子库目录
│   ├── vendors     //自定义算子所在目录
│       ├── config.ini
│       └── vendor_name1   // 存储对应厂商部署的自定义算子
│           ├── framework     //自定义算子插件库
│           ├── op_api
│           │   ├── include
│           │   │   └── aclnn_xx.h      //算子调用API声明文件
│           │   └── lib
│           │       └── libcust_opapi.so
│           ├── op_impl
│           │   └── ai_core
│           │       └── tbe
│           │           ├── config
│           │           ├── vendor_name1_impl    //自定义算子实现代码文件
│           │           │   └── dynamic
│           │           │       ├── xx.cpp
│           │           │       ├── xx.py
│           │           │       └── xx_tiling.h
│           │           ├── kernel     //自定义算子二进制文件
│           │           │   └── ${soc_version}     //AI处理器类型
│           │           │   └── config
│           │           └── op_tiling
│           │               ├── lib
│           │               └── liboptiling.so
│           ├── op_proto     //自定义算子原型库所在目录
│                ├── inc
│                │   └── op_proto.h
│                └── lib
│           ├── scripts      //算子包维护脚本，如卸载脚本
│           └── version.info //版本信息
```

<a id="配置自定义算子优先级"></a>
### 配置自定义算子优先级

多个算子包共存时，如果包含相同OpType的算子，优先级高的算子包中的版本生效。

#### 默认安装场景

多个算子包包含同名OpType时，可通过配置`opp/vendors/config.ini`文件调整各自定义算子的加载优先级；其他场景不建议手动修改该文件。`opp/vendors/config.ini`文件内容如下：

```ini
load_priority=vendor_name1,vendor_name2,vendor_name3
```

有效的`load_priority`配置需要满足以下要求：

- 配置项名称必须为`load_priority`，不可修改。
- 等号右侧至少包含一个vendor名称，不能为空。vendor名称应与`opp/vendors`目录下已安装算子包的目录名一致。
- 配置多个vendor时，使用英文逗号分隔。vendor按从左到右的顺序确定优先级，最左侧的vendor优先级最高。

例如，`load_priority=vendor_name1,vendor_name2,vendor_name3`是有效配置；`load_priority=`、缺少`load_priority`配置项或修改了配置项名称均为无效配置。

默认安装时，如果`config.ini`不存在，安装脚本会创建该文件并写入当前vendor；如果文件已存在，安装脚本会基于有效的`load_priority`配置将当前vendor调整到首位。安装或升级过程中，如果配置无效、文件无法读写或配置更新结果不符合预期，脚本将报错并停止安装或升级，不会自动补全或修正异常配置。

#### 指定目录安装场景<a id="custom-install-path"></a>

分别执行各算子包安装路径下的`set_env.bash`脚本，脚本执行顺序越靠后，优先级越高。

```bash
source <path>/vendors/vendor_name1/bin/set_env.bash  #先执行，优先级较低
source <path>/vendors/vendor_name2/bin/set_env.bash  #后执行，优先级较高
```

执行后`ASCEND_CUSTOM_OPP_PATH`示例：

```
ASCEND_CUSTOM_OPP_PATH=<path>/vendors/vendor_name2:<path>/vendors/vendor_name1:
```

### 卸载与更新算子包

#### 卸载算子包

算子包安装目录中包含`uninstall.sh`脚本，执行该脚本即可完成卸载：

**默认安装场景**：

```bash
bash ${INSTALL_DIR}/opp/vendors/<vendor_name>/scripts/uninstall.sh
```

**指定目录安装场景**：

```bash
bash <path>/vendors/<vendor_name>/scripts/uninstall.sh
```

#### 更新算子包

**默认安装场景**：重新执行安装命令即可覆盖旧版本。

```bash
# source CANN包的环境变量，以默认路径为例
source /usr/local/Ascend/cann/set_env.sh
# 安装自定义算子run包
./custom_opp_<target os>_<target architecture>.run
```

**指定目录安装场景**：先卸载旧版本，再安装新版本。

```bash
# 1. 卸载旧版本
bash <path>/vendors/<vendor_name>/scripts/uninstall.sh

# 2. 安装新版本
# source CANN包的环境变量，以默认路径为例
source /usr/local/Ascend/cann/set_env.sh
# 安装自定义算子run包
./custom_opp_<target os>_<target architecture>.run --install-path=<path>
```

> [!NOTE]说明
> - 更新算子包后，需要重新source环境变量（指定目录安装场景）。
> - 如果需要回退到旧版本，建议先备份旧版本算子包。

---

> 以上为编译部署的基本流程。以下内容涉及CMake组织和自定义编译选项，适合需要定制编译流程的开发者参考。常规场景下msOpGen生成的工程已包含完整的CMake结构，直接修改`CMakePresets.json`并执行`./build.sh`即可。

---

<a id="编译组织"></a>
## 编译组织（进阶）<a id="advanced-build-organization"></a>

算子工程通过CMake组织编译。如需自行组织工程或用CANN提供的CMake modules定制编译流程，可参考以下内容。

算子工程的目录组织方式不固定，以下仅示意两种常见方式；也可以将Host、核函数（Kernel）、Tiling文件完全平铺在同一目录，只要CMakeLists.txt正确传入源文件和`npu_op_*`接口参数即可。

| 方式 | 特点 | 适用场景 |
|------|------|----------|
| 按Host/核函数（Kernel）划分 | Host和核函数（Kernel）代码分目录，三层CMakeLists。 | msOpGen默认生成，单算子或少量算子。 |
| 按算子划分 | 每个算子一个独立目录，算子目录维护自己的源文件清单和核函数（Kernel）声明。 | 多算子工程，算子间独立性高。 |

msOpGen工具默认生成"按Host/核函数（Kernel）划分"的结构。

### 按Host/核函数（Kernel）划分的目录结构

所有算子的Host侧代码集中在`op_host/`目录下，核函数（Kernel）侧代码集中在`op_kernel/`目录下。工程包含三层CMakeLists.txt：顶层定义算子包并添加子目录，`op_host/`下编译Host侧库，`op_kernel/`下编译核函数（Kernel）侧库。

**1. 顶层CMakeLists.txt**

顶层CMakeLists.txt设置编译环境、定义package形态，并添加子目录：

1.  使用**find\_package**找到对应的编译库。
2.  使用**npu\_op\_package**设置算子工程的编译产物形态，支持RUN/SHARED/STATIC，分别对应算子run包形式、算子动态库形式与算子静态库形式，同时，该接口还可配置package（即编译产物）的内容和package的安装位置。
3.  添加需要进行编译的子目录。

```cmake
cmake_minimum_required(VERSION 3.19.0)
project(opp)

# 1. 使用find_package查找CANN软件包提供的ASC CMake modules
find_package(ASC REQUIRED HINTS ${ASCEND_CANN_PACKAGE_PATH}/compiler/tikcpp/ascendc_kernel_cmake)

# 2. 设置算子工程的编译产物形态
# vendor_name在CMakePresets.json中配置，用于区分不同厂商的算子包
set(package_name ${vendor_name})
npu_op_package(${package_name}
    TYPE RUN                                       #编译产物形态：RUN/STATIC/SHARED
    CONFIG
        ENABLE_SOURCE_PACKAGE True
        ENABLE_BINARY_PACKAGE True
        INSTALL_PATH ${CMAKE_BINARY_DIR}/
)

# 3. 添加Host和核函数（Kernel）子目录进行编译
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/op_host)
    add_subdirectory(op_host)
endif()
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/op_kernel)
    add_subdirectory(op_kernel)
endif()
```

**TYPE参数说明**：
- `RUN`：生成run包，用于算子部署。
- `SHARED`：生成动态库，用于集成到应用。
- `STATIC`：生成静态库，用于静态链接。

**2. Host侧CMakeLists.txt**

`op_host/`下的CMakeLists.txt生成aclnn调用代码，编译Host侧各类库：

1.  使用 **npu\_op\_code\_gen** 生成aclnn单算子调用代码、入图所需的原型定义代码等。
2.  单算子调用场景，使用 **npu\_op\_library** 编译aclnn单算子调用库。
3.  算子入图场景，使用**npu\_op\_library**编译算子入图所需的算子原型库。
4.  使用 **npu\_op\_library** 编译Tiling相关库。
5.  使用**npu\_op\_package\_add**添加上述Host侧库至对应package中。

```cmake
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} ops_srcs)

# 1. 生成aclnn调用代码和原型定义代码
npu_op_code_gen(
    SRC ${ops_srcs}
    PACKAGE ${package_name}
    OUT_DIR ${ASCEND_AUTOGEN_PATH}
)

# 2. 编译aclnn单算子调用库
file(GLOB autogen_aclnn_src ${ASCEND_AUTOGEN_PATH}/aclnn_*.cpp)
set_source_files_properties(${autogen_aclnn_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_opapi ACLNN
    ${autogen_aclnn_src}
)
target_compile_options(cust_opapi PRIVATE -fvisibility=hidden)

# 3. 编译算子原型库（用于入图）
file(GLOB proto_src ${ASCEND_AUTOGEN_PATH}/op_proto.cc)
set_source_files_properties(${proto_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_op_proto GRAPH
    ${ops_srcs}
    ${proto_src}
)
target_compile_options(cust_op_proto PRIVATE -fvisibility=hidden)

# 4. 编译Tiling库
file(GLOB fallback_src ${ASCEND_AUTOGEN_PATH}/fallback_*.cpp)
set_source_files_properties(${fallback_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_optiling TILING
    ${ops_srcs}
    ${fallback_src}
)
target_compile_options(cust_optiling PRIVATE -fvisibility=hidden)

# 5. 添加Host侧库到package
npu_op_package_add(${package_name}
    LIBRARY
        cust_optiling
        cust_opapi
        cust_op_proto
)
```

**3. 核函数（Kernel）侧CMakeLists.txt**

`op_kernel/`下的CMakeLists.txt配置核函数（Kernel）编译选项、指定源码，并编译核函数（Kernel）库：

1.  使用**npu\_op\_kernel\_options**添加[算子编译选项](#自定义编译选项进阶)。
2.  使用**npu\_op\_kernel\_sources**指定算子特定目录与编译源文件。
    -   若算子的源码文件没有平铺在`SRC_BASE`目录（通过`npu_op_kernel_library`设置）下，可以通过`KERNEL_DIR`指定特定目录。
    -   若算子的核函数（Kernel）实现cpp文件需要自定义命名，需同时指定`OP_TYPE`（算子类型）和`KERNEL_FILE`（核函数（Kernel）实现cpp文件名），以配置两者之间的对应关系。不配置时，核函数（Kernel）实现cpp文件名和OpType之间需满足[转换规则](../appendix/naming_conversion_table.md)。

3.  使用**npu\_op\_kernel\_library**编译核函数（Kernel）库。
4.  使用**npu\_op\_package\_add**添加上述核函数（Kernel）侧库到对应package中。

```cmake
# 1. 设置核函数（Kernel）编译选项
npu_op_kernel_options(ascendc_kernels ALL OPTIONS --save-temp-files -g)

# 2. 指定核函数（Kernel）源码
npu_op_kernel_sources(ascendc_kernels
    OP_TYPE AddCustom                              #算子类型
    COMPUTE_UNIT ascendxxxyy                       # AI处理器型号
    KERNEL_FILE add_custom.cpp                     # kernel实现文件名
)
或者
npu_op_kernel_sources(ascendc_kernels
    OP_TYPE AddCustom                              #算子类型
    SOC_SERIES Ascendxxx                           # AI处理器型号
    KERNEL_FILE add_custom.cpp                     # kernel实现文件名
)

# 3. 编译核函数（Kernel）库
npu_op_kernel_library(ascendc_kernels
    SRC_BASE ${CMAKE_SOURCE_DIR}/op_kernel/        # 核函数（Kernel）源码根目录
    TILING_LIBRARY cust_optiling                   #依赖的Tiling库
)

# 4. 添加核函数（Kernel）库到package
npu_op_package_add(${package_name}
    LIBRARY ascendc_kernels
)
```

示例中核函数（Kernel）实现文件名为`add_custom.cpp`，所以在`npu_op_kernel_sources`中同时指定了`OP_TYPE`和`KERNEL_FILE`。如果想按目录扫描核函数（Kernel）源码，可改用`KERNEL_DIR ./`，此时`KERNEL_DIR`相对于后续`npu_op_kernel_library`中的`SRC_BASE`。

### 按算子划分的目录结构

工程包含多个算子且各算子独立性较高时，按算子划分更方便维护——每个算子的Host、核函数（Kernel）、Tiling文件放在同一目录，新增算子只需添加一个独立子目录。

下面示例在顶层CMakeLists.txt中集中列出各算子的Host源文件，并通过`npu_op_kernel_sources`声明各算子的核函数（Kernel）源码信息，最终仍由`npu_op_code_gen`、`npu_op_library`、`npu_op_kernel_library`和`npu_op_package_add`完成代码生成、编译和打包。

**1. 目录结构**

以AddCustom、AddCustom2、AddCustom3三个算子为例：

```text
add_custom/
├── add_custom.cpp           # Host侧原型定义、Shape推导、Tiling实现
├── add_custom_kernel.cpp    # 核函数（Kernel）实现
└── add_custom_tiling.h      # Tiling数据结构定义
add_custom2/
├── add_custom2.cpp
├── add_custom2_kernel.cpp
└── add_custom2_tiling.h
add_custom3/
├── add_custom3.cpp
├── add_custom3_kernel.cpp
└── add_custom3_tiling.h
```

**2. 顶层CMakeLists.txt**

顶层CMakeLists.txt设置编译环境、定义package形态，声明各算子的Host/核函数（Kernel）文件，并完成Host侧库、核函数（Kernel）侧库和算子包打包。

```cmake
cmake_minimum_required(VERSION 3.19.0)
project(opp)

# 查找CANN提供的ASC CMake modules。ASCEND_CANN_PACKAGE_PATH通常在CMakePresets.json中配置。
find_package(ASC REQUIRED HINTS ${ASCEND_CANN_PACKAGE_PATH}/compiler/tikcpp/ascendc_kernel_cmake)

# vendor_name在CMakePresets.json中配置，用作算子包的厂商标识。npu_op_package定义一个算子包目标
set(package_name ${vendor_name})
npu_op_package(${package_name}
    TYPE RUN
    CONFIG
        INSTALL_PATH ${CMAKE_BINARY_DIR}/
)

# 1. 声明所有算子的Host源文件。
set(ops_srcs
    ${CMAKE_CURRENT_SOURCE_DIR}/add_custom/add_custom.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/add_custom2/add_custom2.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/add_custom3/add_custom3.cpp
)

# 2. 统一生成aclnn调用代码和原型定义代码。
# npu_op_code_gen必须先于npu_op_kernel_sources调用，
# 因为npu_op_kernel_sources依赖这里设置的自动生成文件路径。
npu_op_code_gen(
    SRC ${ops_srcs}
    PACKAGE ${package_name}
    OUT_DIR ${ASCEND_AUTOGEN_PATH}
)

# 3. 编译aclnn单算子调用库。
# npu_op_code_gen执行后，ASCEND_AUTOGEN_PATH下会生成aclnn_*.cpp/aclnn_*.h。
file(GLOB autogen_aclnn_src ${ASCEND_AUTOGEN_PATH}/aclnn_*.cpp)
set_source_files_properties(${autogen_aclnn_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_opapi ACLNN
    ${autogen_aclnn_src}
)
target_compile_options(cust_opapi PRIVATE -fvisibility=hidden)

# 4. 编译算子原型库（用于入图）。
# 原型库同时依赖各算子的Host源文件和自动生成的op_proto代码。
file(GLOB group_proto_src ${ASCEND_AUTOGEN_PATH}/group_op_proto/*.cc)
file(GLOB proto_src ${ASCEND_AUTOGEN_PATH}/op_proto.cc)
set_source_files_properties(${group_proto_src} PROPERTIES GENERATED TRUE)
set_source_files_properties(${proto_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_op_proto GRAPH
    ${ops_srcs}
    ${group_proto_src}
    ${proto_src}
)
target_compile_options(cust_op_proto PRIVATE -fvisibility=hidden)

# 5. 编译Tiling库。
# Tiling实现位于各算子的Host源文件中；fallback_*.cpp为npu_op_code_gen生成的辅助代码。
file(GLOB fallback_src ${ASCEND_AUTOGEN_PATH}/fallback_*.cpp)
set_source_files_properties(${fallback_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_optiling TILING
    ${ops_srcs}
    ${fallback_src}
)
target_compile_options(cust_optiling PRIVATE -fvisibility=hidden)

# 6. 声明各算子的核函数（Kernel）源码信息。
# KERNEL_DIR相对于下面npu_op_kernel_library的SRC_BASE。
npu_op_kernel_sources(ascendc_kernels
    OP_TYPE AddCustom
    KERNEL_DIR add_custom
    KERNEL_FILE add_custom_kernel.cpp
)
npu_op_kernel_sources(ascendc_kernels
    OP_TYPE AddCustom2
    KERNEL_DIR add_custom2
    KERNEL_FILE add_custom2_kernel.cpp
)
npu_op_kernel_sources(ascendc_kernels
    OP_TYPE AddCustom3
    KERNEL_DIR add_custom3
    KERNEL_FILE add_custom3_kernel.cpp
)

# 7. 编译核函数（Kernel）库。
# SRC_BASE是所有算子目录共同的源码根目录；TILING_LIBRARY指向上面编译出的Tiling库。
npu_op_kernel_library(ascendc_kernels
    SRC_BASE ${CMAKE_CURRENT_SOURCE_DIR}/
    TILING_LIBRARY cust_optiling
)

# 8. 将Host侧库和核函数（Kernel）库加入算子包，最终生成run包。
npu_op_package_add(${package_name}
    LIBRARY
        cust_optiling
        cust_opapi
        cust_op_proto
        ascendc_kernels
)
```


> [!NOTE]说明
> - 示例在顶层集中列出各算子的Host源文件和核函数（Kernel）源码信息，是为了展示`npu_op_*`接口的调用关系；如果工程有自己的目录封装，只要最终传给`npu_op_code_gen`的Host源文件、传给`npu_op_kernel_sources`的`OP_TYPE`/`KERNEL_DIR`/`KERNEL_FILE`，以及传给`npu_op_kernel_library`的`SRC_BASE`匹配即可。
> - `npu_op_kernel_sources`依赖`npu_op_code_gen`设置的自动生成文件路径，因此需要在`npu_op_code_gen`之后调用。
> - `OP_TYPE`需要与算子原型注册中的OpType一致；当核函数（Kernel）文件名不是OpType默认转换后的文件名时，需要显式指定`KERNEL_FILE`。
> - 新增算子时，只需要新增算子子目录，在`ops_srcs`中追加Host源文件，并新增一组`npu_op_kernel_sources`配置。

**与"按Host/核函数（Kernel）划分"的关键差异**：
- Host侧源文件按算子目录列出，顶层统一调用`npu_op_code_gen`。
- 核函数（Kernel）源码信息按算子目录声明，顶层在`npu_op_code_gen`后统一调用`npu_op_kernel_sources`。
- `SRC_BASE`指向所有算子目录共同的源码根目录，`KERNEL_DIR`指向具体算子子目录。
- 各算子的Host、核函数（Kernel）、Tiling文件集中在同一目录下。

> 详细cmake函数参数请参考[cmake函数参考](../appendix/cmake_function_reference.md)。

完成编译组织之后，可按照[基本流程](#基本流程)的三个步骤完成算子编译与部署。

<a id="自定义编译选项"></a>
## 自定义编译选项（进阶）<a id="custom-compile-options"></a>

在CMakeLists.txt中，用`npu_op_kernel_options`为核函数（Kernel）侧代码添加编译选项：

```cmake
npu_op_kernel_options(<target_name> <op_type> [COMPUTE_UNIT <soc_version>... | SOC_SERIES <series>...] OPTIONS ...)
```

**表4** 参数说明。

| 参数名称 | 可选/必选 | 参数描述 |
|---------|----------|---------|
| target_name | 必选 | 核函数（Kernel）库的目标名称（如`ascendc_kernels`）。 |
| op_type | 必选 | 算子类型。如需对所有算子生效，配置为`ALL`。 |
| COMPUTE_UNIT | 可选 | AI处理器型号，不配置时对所有型号生效。与`SOC_SERIES`不能同时使用。 |
| SOC_SERIES | 可选 | AI处理器系列，输入不区分大小写。与`COMPUTE_UNIT`不能同时使用。 |
| OPTIONS | 必选 | 自定义编译选项，多个选项用空格间隔。支持`--save-temp-files`、`-g`、`-DASCENDC_DEBUG`等。 |

**常见示例**：

```cmake
# 为所有算子保留核函数（Kernel）编译中间产物
npu_op_kernel_options(ascendc_kernels ALL OPTIONS --save-temp-files)

# 为特定算子在特定型号上开启AscendC调试宏
npu_op_kernel_options(ascendc_kernels AddCustom COMPUTE_UNIT ascendxxxyy OPTIONS -DASCENDC_DEBUG)

# 使用SOC_SERIES按系列配置，多个值用空格分隔
npu_op_kernel_options(ascendc_kernels AddCustom SOC_SERIES Ascendxxx OPTIONS -DASCENDC_DEBUG)
```

> [!NOTE]说明
> - 编译选项是基于“算子类型+AI处理器型号系列”进行配置的，也就是说不同的“算子类型+AI处理器型号系列”可以配置不同的编译选项。
> - 对相同算子类型+AI处理器型号系列，做多次编译选项配置，以后配置的为准。
> - 对ALL生效的编译选项和对单一算子生效的编译选项如果没有冲突，同时生效，如果有冲突，以单一算子的编译选项为准。

Ascend C框架提供的编译选项介绍如下：

-   `--tiling_key`，设置该选项后，只编译指定的[TilingKey](../design_and_implementation/multi_branch_strategy.md#compile-selected-tiling-keys)相关的核函数（Kernel）代码，用于加速编译过程。若不指定TilingKey编译，则默认编译所有的TilingKey。配置多个TilingKey时，TilingKey之间不能有空格。示例如下，其中1、2为TilingKey取值。

    ```
    --tiling_key=1,2
    ```

-   编译宏开关请参考[内置编译宏开关](../../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md)。
-   `--op_relocatable_kernel_binary`，设置该选项为true时，会额外编译一份可被重新链接的二进制文件；不配置或设置为false时该选项均不生效。该选项用于自定义Tiling下沉算子开启SuperKernel的场景，配置该选项所生成的二进制文件，可以使算子在SuperKernel编译时直接复用二进制文件，降低编译耗时。
-   `--kernel-template-input`，编译指定的模板参数组合相关的核函数（Kernel）代码，用于加速编译过程。更多信息参考[编译加速 — 选择性编译](./compilation_acceleration.md#选择性编译)。
-   `-DFORCE_TILING_CONST_PROPAGATION`，该选项用于静态shape场景Tiling数据的常量化优化。对于复杂算子，该编译选项可以提升算子静态shape执行性能。该编译选项仅支持自定义算子工程，且仅在算子使用`BEGIN_TILING_DATA_DEF`注册Tiling结构体的情形下生效，对于算子使用标准C++语法定义Tiling结构体的情形下不生效。使用示例如下：
    ```
    -DFORCE_TILING_CONST_PROPAGATION
    ```
    该编译选项的原理是，在算子使用`BEGIN_TILING_DATA_DEF`注册Tiling结构体的场景下，自定义算子工程在定义该Tiling结构体时，会将其成员变量都定义为static constexpr类型常量，即将Tiling结构体变为静态常量类。由于在前端标记了常量，编译器会根据Tiling数据进行更彻底的常量折叠和优化。
    
    该选项对算子写法有约束，以下情形不支持该选项：

    1. 算子代码中不允许有用Tiling结构体定义非const对象，也不允许对Tiling对象的成员进行赋值操作。否则会有编译错误。原因是静态常量无法被重新赋值。

    2. 算子代码中不允许有`reinterpret_cast<OtherType*>(tiling_data)`的行为，否则会得到非预期的结果。这是因为Tiling结构体所有成员都是常量，该类型的对象没有实体，size恒为1字节，对`tiling_data`的`reinterpret_cast`会产生未定义的行为。

    对于算子使用标准C++语法定义Tiling结构体的情形下不生效的原因是，算子定义Tiling结构体为static constexpr时，无法获得Tiling数据，无法给Tiling结构体成员在定义时初始化。

-   --save-temp-files，该选项用于保留核函数（Kernel）侧编译过程中生成的临时文件。在编译过程中会根据芯片类型、输入输出的dtype/format等因素排列组合触发不同的`kernel.o`编译，开启该选项，可查看每个`kernel.o`的编译过程，用于辅助定位问题。

    添加该编译选项并执行工程编译，可在`build/op_kernel/<OpType>_<soc>/kernel_*/kernel_meta_*/kernel_meta`目录查看实际参与编译的核函数（Kernel）源码、编译命令等中间文件，其中`<OpType>`表示算子类型，`<soc>`表示编译目标芯片类型。

## 相关文档

- [单算子API调用](../invocation/single_operator_api_call.md) — 调用编译部署后的算子。
- [多算子包组织](./multi_operator_package.md) — 是否需要算子拆包建议。
- [算子动态库和静态库编译](./dynamic_static_lib_compilation.md) — 动态库/静态库编译方式。
- [编译过程调试](./compilation_debug.md) — 查看实际编译命令、保留中间产物和常见错误诊断。
- [编译加速](./compilation_acceleration.md) — 构建加速策略（选择性编译、缓存、增量编译等）。
- [simplified key二进制配置JSON](./simplified_key_json_configuration.md) — 按自定义simplified key编译多份binary的配置方法。
- [交叉编译](./cross_compilation.md) — 跨平台架构编译。
