# 单独编译样例

## 概述

本样例基于Gather算子，演示了将Host + Device混合.asc文件中的Device代码拆分到**多个.asc文件**的单独编译流程。单独编译的代码结构与调用关系如下图1所示，其中 `gather_compute.asc` 为**Device侧执行函数**（定义`gather_element()`函数并通过 extern 暴露供跨文件调用），`gather_kernel.asc` 为**Host + Device混合文件**（含Kernel定义、`<<<>>>`内核调用，通过extern调用gather_compute.asc中的Device侧执行函数），`main.cpp` 为纯Host侧代码（通过extern调用gather_kernel.asc暴露的 `<<<>>>` 内核启动函数）。

<p align="center">
  <img src="./figures/separate_compile.png" width="50%">
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
├── 01_separate_compile
│   ├── CMakeLists.txt       // 编译工程文件
│   ├── gather_compute.asc   // 计算逻辑函数定义
│   ├── gather_kernel.asc    // 混合文件（含Kernel定义、<<<>>>内核调用）
│   ├── main.cpp             // 纯Host侧代码（通过extern调用.asc暴露的函数）
│   └── figures              // README中的图片资源
```

## 算子描述

gather算子实现了从长度为100000的一维输入向量中获取指定索引的12288个数据的功能。

## 单独编译介绍

bisheng默认采用**全程序编译**模式，该模式要求单个源文件 `X.asc` 中编译的设备程序没有任何未解析的外部设备函数和变量引用。当需要在多个 `.asc` 文件间进行跨文件的设备代码调用时，需使用 **`-dc` 选项**启用**单独编译**模式。

单独编译有以下注意事项：
- 在一个编译单元中定义的非常量设备变量，在其他编译单元中引用时，必须使用 `extern` 关键字声明；
- 所有常量设备变量，定义和跨单元引用时都必须使用 `extern` 关键字；
- 所有 Ascend C 源文件（`.asc`）都必须使用 `-dc` 选项编译。

### bisheng命令行编译

本样例存在跨文件的设备函数调用，需要使用 `-dc` 选项分别将各 `.asc` 文件与Host侧 `.cpp` 编译为 `.o` 目标文件，再链接生成可执行二进制。具体编译命令如下：

```shell
bisheng -dc gather_compute.asc -o gather_compute.o --npu-arch=dav-3510 --enable-simt
bisheng -dc gather_kernel.asc -o gather_kernel.o --npu-arch=dav-3510 --enable-simt
bisheng -c main.cpp -o main.o -I${ASCEND_HOME_PATH}/include
bisheng gather_compute.o gather_kernel.o main.o -o demo
```

其中，`--npu-arch` 为编译时指定的AI处理器架构，本样例仅支持 `dav-3510`，各产品型号对应的架构版本号请通过[对应关系表](../../../../../docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)进行查询。`--enable-simt` 用于指定 SIMT 方式编译。

更多bisheng编译选项及用法说明，请参考[AI-Core算子编译基本用法](../../../../../docs/zh/guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md)。

### CMake方式编译

项目中可以使用 CMake 来更简便地使用毕昇编译器编译Ascend C SIMT算子，生成可执行二进制，核心步骤如下：

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
    gather_compute.asc
    gather_kernel.asc
    main.cpp)

# 5、设置链接器语言为ASC
set_target_properties(demo PROPERTIES LINKER_LANGUAGE ASC)

# 6、配置编译选项：-dc启用单独编译，--npu-arch指定架构版本，--enable-simt启用SIMT模式
target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:-dc --npu-arch=${CMAKE_ASC_ARCHITECTURES} --enable-simt>)
```

在后续版本，可通过设置 `CMAKE_ASC_ENABLE_SIMT` 为ON启用SIMT模式替代上述步骤6的 `--enable-simt` 编译选项：

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
    bisheng -dc gather_compute.asc -o gather_compute.o --npu-arch=dav-3510 --enable-simt   # 编译计算函数.asc为目标文件
    bisheng -dc gather_kernel.asc -o gather_kernel.o --npu-arch=dav-3510 --enable-simt     # 编译Kernel混合.asc为目标文件
    bisheng -c main.cpp -o main.o -I${ASCEND_HOME_PATH}/include                            # 编译Host侧.cpp为目标文件
    bisheng gather_compute.o gather_kernel.o main.o -o demo                                # 链接生成可执行文件
    ./demo                                                                                 # 执行样例
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
