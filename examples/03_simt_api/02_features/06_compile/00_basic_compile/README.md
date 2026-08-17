# Basic Compile 样例

## 概述

本样例基于Gather算子，演示了Host侧.cpp文件调用Host + Device混合.asc文件（含Kernel定义与<<<>>>内核调用）的基本编译流程。其中.asc文件中包含<<<>>>核函数调用的Host侧入口函数通过extern暴露给纯Host侧的.cpp。代码结构与调用关系如图 1 所示：

<p align="center">
  <img src="./figures/basic_compile.png" width="50%">
   </p>
<p align="center">
图1：代码结构与调用关系
</p>

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.1.0

## 目录结构介绍

```
├── 00_basic_compile
│   ├── CMakeLists.txt      // 编译工程文件
│   ├── gather_kernel.asc   // 混合文件（含Kernel定义、<<<>>>内核调用）
│   ├── main.cpp            // 纯Host侧代码（通过extern调用.asc暴露的函数）
│   └── figures             // README中的图片资源
```

## 算子描述
gather算子实现了从长度为100000的一维输入向量中获取指定索引的12288个数据的功能。

## 异构编译介绍

本样例采用**异构编译**方式，Host侧 `.cpp` 与Device侧 `.asc` 分开编译后再链接。AI Core SIMT的基本编译流程如下：Host代码使用Host编译器编译成Host二进制，AI Core SIMT代码由Device编译器编译成SIMT二进制，先将SIMT二进制链接成Fatbin文件，再与Host二进制合并生成可执行二进制。

### bisheng命令行编译

基本流程：先分别将 `.asc` 混合文件与Host侧 `.cpp` 编译为 `.o` 目标文件，再链接生成可执行二进制。具体编译命令如下：

```shell
# 分步编译
bisheng -c kernel.asc -o kernel.o --npu-arch=dav-3510 --enable-simt
bisheng -c host.cpp -o host.o -I${ASCEND_HOME_PATH}/include
bisheng kernel.o host.o -o demo

# 或一条编译命令
bisheng host.cpp kernel.asc -o demo --npu-arch=dav-3510 --enable-simt -I${ASCEND_HOME_PATH}/include
```

其中，`--npu-arch`为编译时指定的AI处理器架构，本样例仅支持`dav-3510`，各产品型号对应的架构版本号请通过[对应关系表](../../../../../docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)进行查询。`--enable-simt`用于指定SIMT方式编译。

更多 bisheng 编译选项及用法说明，请参考[AI-Core算子编译基本用法](../../../../../docs/zh/guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md)。

### CMake方式编译

项目中可以使用CMake来更简便地使用毕昇编译器编译Ascend C SIMT算子，生成可执行二进制，核心步骤如下：

```cmake
# 1、设置编译配置变量
# CMAKE_ASC_ARCHITECTURES：指定NPU架构版本，本样例仅支持 dav-3510，可通过-DCMAKE_ASC_ARCHITECTURES=dav-xxxx配置
set(CMAKE_ASC_ARCHITECTURES "dav-3510" CACHE STRING "NPU architecture")

# 2、find_package(ASC)是CMake中用于查找和配置Ascend C编译工具链的命令
find_package(ASC)

# 3、指定项目支持的语言包括ASC和CXX
project(kernel LANGUAGES ASC CXX)

# 4、使用CMake接口编译可执行文件
add_executable(demo
    kernel.asc
    host.cpp)

# 5、设置链接器语言为ASC
set_target_properties(demo PROPERTIES LINKER_LANGUAGE ASC)

# 6、配置编译选项：--npu-arch指定架构版本，--enable-simt启用SIMT模式
target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES} --enable-simt>)
```

在后续版本，可通过设置 `CMAKE_ASC_ENABLE_SIMT` 为ON启用SIMT模式替代上述步骤6的 `--enable-simt`编译选项：

```cmake
# 启用SIMT编译模式
set(CMAKE_ASC_ENABLE_SIMT ON)
```

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

  - 命令行方式

    ```bash
    bisheng -c gather_kernel.asc -o gather_kernel.o --npu-arch=dav-3510 --enable-simt  # 编译.asc混合文件为目标文件
    bisheng -c main.cpp -o main.o -I${ASCEND_HOME_PATH}/include                        # 编译Host侧.cpp为目标文件
    bisheng gather_kernel.o main.o -o demo                                             # 链接生成可执行文件
    ./demo                                                                             # 执行样例
    ```

  - CMake方式

    ```bash
    mkdir -p build && cd build;                                     # 创建并进入build目录
    cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j;            # 编译工程
    ./demo                                                          # 执行样例
    ```

    使用 NPU 仿真模式时，添加 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。示例如下：

    ```bash
    cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..;make -j; # NPU仿真模式
    ```

    > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 执行结果

  执行结果如下，说明精度对比成功。
  ```
  [Success] Case accuracy is verification passed.
  ```
