# 交叉编译

本文是扩展内容，介绍当编译平台与运行平台架构不同时（例如在x86_64开发机上编译、再部署到aarch64边缘设备），如何对算子工程进行交叉编译。


## 概述

完成算子代码实现后，如果编译平台和运行平台架构一致，直接参考[基本流程](basic_process.md)编译即可。如需交叉编译（编译平台架构与运行平台架构不同），参考本文档。

交叉编译的典型场景：
- 在x86_64开发机上编译，部署到aarch64边缘设备。
- 在aarch64服务器上编译，部署到x86_64服务器。

## 交叉编译工具下载

以Ubuntu系列操作系统为例，交叉编译工具下载命令如下表所示。其他操作系统请替换为实际的下载命令。

| 当前平台架构 | 运行环境平台架构 | 编译工具下载命令 |
|---|---|---|
| x86_64 | aarch64 | `sudo apt-get install -y g++-aarch64-linux-gnu` |
| aarch64 | x86_64 | `sudo apt-get install -y g++-x86-64-linux-gnu` |

安装完成后，可通过以下命令验证工具是否可用：

```bash
# x86_64 → aarch64
aarch64-linux-gnu-g++ --version

# aarch64 → x86_64
x86_64-linux-gnu-g++ --version
```

## CMakePresets.json配置

交叉编译需修改算子工程目录下CMakePresets.json的两个配置项：

### 使能交叉编译

将`ENABLE_CROSS_COMPILE`设置为`True`：

```json
"ENABLE_CROSS_COMPILE": {
    "type": "BOOL",
    "value": "True"
}
```

### 指定交叉编译工具路径

使用`which`命令查找交叉编译工具的安装路径：

```bash
which aarch64-linux-gnu-g++
```

假设安装在 /usr/bin/ 下，将`CMAKE_CROSS_PLATFORM_COMPILER`设置为交叉编译工具的安装路径：

```json
"CMAKE_CROSS_PLATFORM_COMPILER": {
    "type": "PATH",
    "value": "/usr/bin/aarch64-linux-gnu-g++"
}
```

## 执行交叉编译

交叉编译Kernel二进制时，编译框架需要先加载本机架构可执行的Tiling动态库来获取编译参数。因此设置完`ENABLE_CROSS_COMPILE=True`且`ENABLE_BINARY_PACKAGE=True`之后，需要执行两步才能完成交叉编译：

- 要先在本机模式下生成`libcust_opmaster_rt2.0.so`；
- 再通过`HOST_NATIVE_TILING_LIB`传给交叉编译阶段。


下面以快速入门里的AddCustom、在aarch64编译机上生成x86_64目标算子包为例演示：

建议本机Tiling库和交叉编译产物使用两个不同的构建目录，避免本机架构产物和目标架构产物混在一起：

- `build_native_tiling`：只用于生成本机架构可加载的Tiling动态库。
- `build_out`：用于生成目标架构的算子包。

**步骤1：本机模式生成Tiling库**

```bash
source /usr/local/Ascend/cann/set_env.sh

VENDOR_NAME=customize

cmake -S . -B build_native_tiling --preset=default \
    -DENABLE_CROSS_COMPILE=False \
    -DCMAKE_INSTALL_PREFIX=${PWD}/build_native_tiling

cmake --build build_native_tiling \
    --target ${VENDOR_NAME}_ascendc_cust_optiling \
    -j$(nproc)

HOST_NATIVE_TILING_LIB=$(realpath build_native_tiling/op_host/libcust_opmaster_rt2.0.so)
```

`HOST_NATIVE_TILING_LIB`指向的是本机架构动态库。例如当前编译机是aarch64时，该库应为aarch64动态库。可以用以下命令确认：

```bash
file ${HOST_NATIVE_TILING_LIB}
```

**步骤2：执行交叉编译**

确保`CMakePresets.json`中已将`ENABLE_CROSS_COMPILE`设置为`True`，且`CMAKE_CROSS_PLATFORM_COMPILER`指向目标平台对应的交叉编译器，然后执行：

```bash
cmake -S . -B build_out --preset=default \
    -DENABLE_CROSS_COMPILE=True \
    -DCMAKE_CROSS_PLATFORM_COMPILER=/usr/bin/x86_64-linux-gnu-g++ \
    -DHOST_NATIVE_TILING_LIB=${HOST_NATIVE_TILING_LIB} \
    -DCMAKE_INSTALL_PREFIX=${PWD}/build_out

cmake --build build_out --target binary package -j$(nproc)
```

编译成功后，会在`build_out`目录下生成自定义算子安装包，可通过以下命令做基本检查：

```bash
ls build_out/custom_opp_ubuntu_*.run
file build_out/op_host/libcust_opapi.so
```

例如在aarch64编译机上交叉编译x86_64目标时，`libcust_opapi.so`应显示为`x86-64`动态库。

## 部署交叉编译的算子包

将生成的算子包传输到目标平台并部署：

```bash
# 1. 用户自行传输run包到目标平台，并source /usr/local/Ascend/cann/set_env.sh

# 2. 在目标平台的run包文件夹下执行安装
./custom_opp_ubuntu_x86_64.run 
```
安装后，按照安装日志提示给LD_LIBRARY_PATH设置环境变量，之后调用算子的使用方式与普通编译部署一致，参考[算子包调用](../aclnn_quick_start.md#部署)。

## 相关文档

- [基本流程](basic_process.md) — 标准编译与部署流程。
- [编译过程调试](compilation_debug.md) — 编译问题排查方法。
- [单算子API调用](../invocation/single_operator_api_call.md) — 调用编译部署后的算子。