# CMake Function Reference

<!-- md-trans-meta sourceCommit=c891f0c5d5b2ac25c5683ff40810fa381d654ece translatedAt=2026-10-23T13:33:30.590Z pushedAt=2026-11-03T07:05:13.394Z nl=2 -->

This document is a reference that organizes the CMake function interfaces involved in the compilation of operator projects, for advanced developers who need to understand the compilation mechanism in depth.

## Overview

When you use the msOpGen tool to create an operator project, the related compilation scripts are fixed locally. To make it easier for developers to directly use the CMake function interfaces provided by the operator project without frequently rebuilding the project, the CANN software package packs the CMake scripts of the operator project. Developers can use these functions by locating the corresponding CMake modules package through `find_package`.

This document describes the eight CMake functions used in the compilation of operator projects, which are classified into three categories by purpose: **Package**, **Library**, and **Other**.

## Function Overview

| Category | Function | Description |
|------|------|------|
| Package | `npu_op_package` | Creates a package. |
| Package | `npu_op_package_add` | Adds targets or files to the package. |
| Library | `npu_op_library` | Creates a host-side library. |
| Library | `npu_op_kernel_library` | Creates a kernel function side library. |
| Library | `npu_op_kernel_options` | Adds compilation options for the kernel function target. |
| Library | `npu_op_kernel_sources` | Describes the source information of the kernel function target. |
| Library | `npu_op_device_tiling_library` | Creates a device-side Tiling library. |
| Others | `npu_op_code_gen` | Executes the code generation process to generate the aclnn single-operator call code and the prototype definition code required for graph compilation. |

---

## Package Class Functions

### npu_op_package

Creates a package.

```cmake
npu_op_package(<package_name> TYPE <type> [CONFIG] [ENABLE_SOURCE_PACKAGE <value>] [ENABLE_BINARY_PACKAGE <value>] [INSTALL_PATH <path>])
```

**Parameter description:**

- `<package_name>` (required): name of the package.
- `TYPE <type>` (required): type of the package. The value is `RUN`, `SHARED`, or `STATIC`, corresponding to the operator run package form, operator dynamic library form, and operator static library form, respectively.
- `[CONFIG]` (optional): used to configure the content and installation location of the package.
  - `[ENABLE_SOURCE_PACKAGE <value>]` (optional): specifies whether to package the source code into the package. The default value is True.
  - `[ENABLE_BINARY_PACKAGE <value>]` (optional): specifies whether to package the binary files into the package. The default value is True.
  - `[INSTALL_PATH <path>]` (optional): specifies the installation path of the package. The default value is `CMAKE_BINARY_DIR`.
  - `[ENABLE_CPACK <value>]` (optional): specifies whether to package. The default value is True. For scenarios where users need to customize packaging, it can be set to False. When configured as False, compilation outputs are generated directly in the `<path>` directory and are not packaged into a run package.

**Example:**

```cmake
set(package_name ${vendor_name})
npu_op_package(${package_name}                     # package name
    TYPE RUN                                       #Specify the compilation output form, [RUN|STATIC|SHARED]
    CONFIG
        ENABLE_SOURCE_PACKAGE True                 # Whether to package the source code into the package.
        ENABLE_BINARY_PACKAGE True                 # Whether to compile the kernel function binary.
        INSTALL_PATH ${CMAKE_BINARY_DIR}/          # Installation location of the package.
)
```

### npu_op_package_add

Adds targets or files to the package.

```cmake
# Add a target
npu_op_package_add(<package_name> LIBRARY <target_name1> [<target_name2>...] )
# Add a file, used only in run package mode
npu_op_package_add(<package_name> FILES <file_name1> [<file_name2>...] [TYPE <target_type>] [PACKAGE_PATH <pkg_path>])
```

**Parameter description:**

- `<package_name>` (required): name of the package.
- `LIBRARY` (required): specifies the name of the target to be added to the package.
  - `<target_name1> [<target_name2>...]` (required): list of target names.
- `FILES` (required): specifies the name of the file to be added to the package.
  - `<file_name1> [<file_name2>...]` (required): list of file names.
- `[TYPE <target_type>]` (optional): specifies the file type and installs the file to the corresponding directory. The value is `ACLNN` or `GRAPH`. When configured as `ACLNN`, the file is packaged to the directory where the aclnn single-operator call header files are located under the run package directory. When configured as `GRAPH`, the file is packaged to the directory where the operator prototype definition header files required for graph compilation are located under the run package directory.
- `[PACKAGE_PATH <pkg_path>]` (optional): specifies the relative path of the file in the package. The `TYPE` and `PACKAGE_PATH` parameters are mutually exclusive, that is, only one of them can be configured.

**Example:**

```cmake
# Add the Host-side related library
npu_op_package_add(${package_name}
    LIBRARY
        cust_optiling
        cust_opapi
        cust_op_proto
)

# Add the kernel function side related library
npu_op_package_add(${package_name}
    LIBRARY ascendc_kernels
)
```

---

## Library Class Functions

### npu_op_library

Creates a host-side library.

```cmake
npu_op_library(<library_name> TYPE <library_type> <files>)
```

**Parameter description:**

- `<library_name>` (mandatory): name of the host-side library.
- `TYPE <library_type>` (mandatory): type of the host library. The value is `TILING`, `ACLNN`, `GRAPH`, `TF_PLUGIN`, or `ONNX_PLUGIN`.
  - `TILING`: tiling-related library.
  - `ACLNN`: aclnn single-operator call library.
  - `GRAPH`: operator prototype library required for graph compilation.
  - `TF_PLUGIN`: library related to TensorFlow framework adaptation.
  - `ONNX_PLUGIN`: library related to ONNX framework adaptation.
- `<files>` (mandatory): sets the source files to be compiled.

**Example:**

```cmake
# Single operator invocation scenario. Compile the aclnn single-operator call library.
file(GLOB autogen_aclnn_src ${ASCEND_AUTOGEN_PATH}/aclnn_*.cpp)
set_source_files_properties(${autogen_aclnn_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_opapi ACLNN
    ${autogen_aclnn_src}
)

# Operator graph integration scenario. Compile the operator prototype library required for operator graph integration.
file(GLOB proto_src ${ASCEND_AUTOGEN_PATH}/op_proto.cc)
set_source_files_properties(${proto_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_op_proto GRAPH
    ${ops_srcs}
    ${proto_src}
)

# Compile the Tiling-related library.
file(GLOB fallback_src ${ASCEND_AUTOGEN_PATH}/fallback_*.cpp)
set_source_files_properties(${fallback_src} PROPERTIES GENERATED TRUE)
npu_op_library(cust_optiling TILING
    ${ops_srcs}
    ${fallback_src}
)
```

### npu_op_kernel_library

Creates a kernel function side library.

```cmake
npu_op_kernel_library(<target_name> SRC_BASE <path> TILING_LIBRARY <tiling_target>)
```

**Parameter description:**

- `<target_name>` (required): target name.
- `SRC_BASE <path>` (required): specifies the base directory of the kernel function source code. An absolute path must be configured, for example, the absolute path of the `op_kernel` directory in the example.
- `TILING_LIBRARY <tiling_target>` (required): specifies the dependent tiling target.

**Example:**

```cmake
npu_op_kernel_library(ascendc_kernels
    SRC_BASE ${CMAKE_SOURCE_DIR}/op_kernel/
    TILING_LIBRARY cust_optiling
)
```

### npu_op_kernel_options

Adds compilation options for the kernel function target.

```cmake
npu_op_kernel_options(<target_name> <op_type> [COMPUTE_UNIT <soc_version>... | SOC_SERIES <series>...] OPTIONS …)
```

**Parameter description:**

- `<target_name>` (mandatory): target name.
- `<op_type>` (mandatory): defines the scope where the configuration takes effect. The value is `ALL` or `OP_TYPE`. `ALL` indicates that the configuration takes effect on all operators, and `OP_TYPE` indicates that it takes effect on specific operators.
- `[COMPUTE_UNIT <soc_version>...]` (optional): sets the compilation options of the operator on a specific AI processor model. If this option is not specified, the options take effect on all models by default. It cannot be used together with `SOC_SERIES`.
- `[SOC_SERIES <series>...]` (optional): sets the compilation options by AI processor series. The input is case-insensitive. It cannot be used together with `COMPUTE_UNIT`. At least one value must be provided after the `SOC_SERIES` keyword. If both `SOC_SERIES` and `COMPUTE_UNIT` are provided in the same call, or if only the `SOC_SERIES` keyword is provided without a value, an error is reported during the CMake configuration phase.
>[!NOTE] Note 
>Among them, obtain the AI processor model `soc_version` in the following ways:
><!-- npu="910b,910,310p,310b" id1 -->
>- For the following products: run the `npu-smi info` command on the server where the AI processor is installed to query and get the Name information. The actual configuration value is AscendName. For example, if the Name value is xxxyy, the actual configuration value is Ascendxxxyy.<br><br>
>    <!-- npu="910b" id2 -->
>    Atlas A2 products  
>    <!-- end id2 -->
>
>    <!-- npu="310b" id3 -->
>    Atlas 200I/500 A2 inference products  
>    <!-- end id3 -->
>
>    <!-- npu="310p" id4 -->
>    Atlas inference products  
>    <!-- end id4 -->
>
>    <!-- npu="910" id5 -->
>    Atlas training products<br><br> 
>    <!-- end id5 -->
><!-- end id1 -->
><!-- npu="A3" id6 -->
>- For <term>Atlas A3 products</term>, run the `npu-smi info -t board -i <id> -c <chip_id>` command on the server where the AI processor is installed to query and get the Chip Name and NPU Name information. The actual configuration value is `Chip Name_NPU Name`. For example, if the Chip Name value is Ascendxxx and the NPU Name value is 1234, the actual configuration value is Ascendxxx_1234. Among them:<br>`id`: device ID. The NPU ID obtained through the `npu-smi info -l` command is the device ID, which is represented as `<id>` in the command.<br>`chip_id`: chip ID. The Chip ID obtained through the `npu-smi info -m` command is the chip ID, which is represented as `<chip_id>` in the command.<br><br>
><!-- end id6 -->
><!-- npu="950" id7 -->
>- For Ascend 950PR&950DT products, run the `npu-smi info -t board -i <id>` command on the server where the AI processor is installed to query and get the Chip Name and NPU Name information. The actual configuration value is `Chip Name_NPU Name`. For example, if the Chip Name value is Ascendxxx and the NPU Name value is 1234, the actual configuration value is Ascendxxx_1234.<br>Among them: `id` is the device ID. The NPU ID obtained through the `npu-smi info -l` command is the device ID, which is represented as `<id>` in the command.
><!-- end id7 -->
- `OPTIONS …` (mandatory): compilation options passed to the compiler.

**Example:**

```cmake
npu_op_kernel_options(ascendc_kernels ALL OPTIONS --save-temp-files -g)   #Add compilation options for the operator
```
npu_op_kernel_options(ascendc_kernels AddCustom SOC_SERIES Ascendxxx Ascendyyy OPTIONS -DASCENDC_DEBUG)
### npu_op_kernel_sources

Describes the source information of the kernel function target, including setting the kernel function implementation file and source path of the operator.

```cmake
npu_op_kernel_sources(<target_name> [OP_TYPE <op_type>] [KERNEL_DIR <path>] [COMPUTE_UNIT <soc_version>... | SOC_SERIES <series>...] [KERNEL_FILE <file>])
```

**Parameter description:**

- `<target_name>` (mandatory): target name.
- `[OP_TYPE <op_type>]` (optional): operator type. It must coexist with `KERNEL_FILE`.
- `[KERNEL_DIR <path>]` (optional): specifies the relative path of the kernel function source code to `SRC_BASE`. If the source files of the operator are not placed flat in the `SRC_BASE` directory (set through `npu_op_kernel_library`), you can specify a specific directory through `KERNEL_DIR`.
- `[COMPUTE_UNIT <soc_version>...]` (optional): sets `KERNEL_FILE` to take effect on the specified models. By default, `KERNEL_FILE` takes effect on all models. It cannot be used together with `SOC_SERIES`.
- `[SOC_SERIES <series>...]` (optional): sets `KERNEL_FILE` to take effect on the specified series. The input is case-insensitive. It cannot be used together with `COMPUTE_UNIT`. At least one value must be provided after the `SOC_SERIES` keyword. If both `SOC_SERIES` and `COMPUTE_UNIT` are provided in the same call, or if only the `SOC_SERIES` keyword is provided without a value, an error is reported during the CMake configuration phase.
- `[KERNEL_FILE <file>]` (optional): specifies the kernel function implementation file name of the operator entry. If the kernel function implementation cpp file of the operator needs a custom name, you must specify both `OP_TYPE` (operator type) and `KERNEL_FILE` (kernel function implementation cpp file name) to configure the correspondence between them. If not configured, the kernel function implementation cpp file name and OpType must satisfy the conversion rule. For details, see [Naming Conversion Table](./naming_conversion_table.md).

**Example:**

```cmake
npu_op_kernel_sources(ascendc_kernels
    OP_TYPE AddCustom
    KERNEL_DIR ./Add
    COMPUTE_UNIT Ascendxxxyy
    KERNEL_FILE add_custom.cpp
)
```
You can also use `SOC_SERIES` to specify by series:
```cmake
npu_op_kernel_sources(ascendc_kernels
    OP_TYPE AddCustom
    KERNEL_DIR ./Add
    SOC_SERIES Ascendxxx
    KERNEL_FILE add_custom.cpp
)
```
### npu_op_device_tiling_library

Creates a device-side tiling library. When this option is used, the package type can only be configured as **RUN** (run package mode).

```cmake
npu_op_device_tiling_library(<target_name> <type> <files>)
```

**Parameter description:**

- `<target_name>` (required): target name.
- `<type>` (required): specifies the type of the tiling output. The supported values are `SHARED` and `STATIC`.
- `<files>` (required): specifies the tiling source files.

**Example:**

```cmake
npu_op_device_tiling_library(device_tiling SHARED
    device_tiling_impl.cpp
)
```

---

## Others

### npu_op_code_gen

Executes the code generation process to generate the aclnn single-operator call code and the prototype definition code required for graph compilation.

```cmake
npu_op_code_gen(SRC <src_files> OUT_DIR <output_dir> PACKAGE <pkg_name> [COMPILE_OPTIONS ...] [JOIN_OP_DEF ...])
```

**Parameter description:**

- `SRC <src_files>` (required): scope of source files involved in code generation.
- `OUT_DIR <output_dir>` (required): output path of the generated code.
- `PACKAGE <pkg_name>` (required): specifies the package name of the generated code.
- `[COMPILE_OPTIONS ...]` (optional): custom compilation options during compilation.
- `[JOIN_OP_DEF ...]` (optional): defaults to **False**. When set to **False**, compiles the operators corresponding to the AI processor type set in **CMakePreset.json**. When set to **True**, compiles the operators corresponding to the AI processor types common to both **CMakePreset.json** and the operator prototype registration.

**Example:**

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

## Related Documents

- [Operator Project Build Extension](../compilation_and_deployment/basic_process.md#advanced-build-organization) — Complete CMakeLists.txt authoring method and build command description.
- [Operator Prototype Definition](../design_and_implementation/operator_prototype_definition.md) — Conversion rules between the kernel function implementation file name and OpType.
- [How to Use Workspace](../../../appendix/common_operations/how_to_use_workspace.md) — Method for setting the workspace in the Tiling function.
