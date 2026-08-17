# Basic Compile 样例

## 概述

本样例基于Add算子，演示了Host侧.cpp文件调用Host + Device混合.asc文件（含Kernel定义与<<<>>>内核调用）的基本编译流程。其中.asc文件的<<<>>>调用函数通过extern暴露给纯Host侧的.cpp。代码结构与调用关系如图 1 所示：

<p align="center">
  <img src="./figures/basic_compile.png" width="50%">
</p>
<p align="center">
图1：代码结构与调用关系
</p>

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── 00_basic_compile
│   ├── figures             // 图示
│   ├── add_kernel.asc      // 混合文件（含Kernel定义、<<<>>>内核调用）
│   ├── CMakeLists.txt      // 编译工程文件
│   ├── main.cpp            // 纯Host侧代码（通过extern调用.asc暴露的函数）
│   └── README.md           // 样例说明文档
```

## 算子描述
Add算子实现了两个向量按元素求和的功能，输入向量长度为16384，输出向量长度为16384。

## 异构编译介绍

本样例采用**异构编译**方式，Host侧 `.cpp` 与Device侧 `.asc` 分开编译后再链接。AI Core SIMD的基本编译流程如下：Host代码使用Host编译器编译成Host二进制，AI Core SIMD代码由Device编译器编译成SIMD二进制，先将SIMD二进制链接成Fatbin文件，再与Host二进制合并生成可执行二进制。

### bisheng命令行编译

基本流程：先分别将 `.asc` 混合文件与Host侧 `.cpp` 编译为 `.o` 目标文件，再链接生成可执行二进制。具体编译命令如下：

```shell
# 分步编译
bisheng -c add_kernel.asc -o add_kernel.o --npu-arch=dav-2201
bisheng -c main.cpp -o main.o -I${ASCEND_HOME_PATH}/include
bisheng add_kernel.o main.o -o demo

# 或一条编译命令
bisheng main.cpp add_kernel.asc -o demo --npu-arch=dav-2201 -I${ASCEND_HOME_PATH}/include
```

其中，`--npu-arch`为编译时指定的AI处理器架构，本样例支持`dav-2201`和`dav-3510`，各产品型号对应的架构版本号请通过[对应关系表](../../../../../docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)进行查询。

更多 bisheng 编译选项及用法说明，请参考[AI-Core算子编译基本用法](../../../../../docs/zh/guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md)。

### CMake方式编译

项目中可以使用CMake来更简便地使用毕昇编译器编译Ascend C SIMD算子，生成可执行二进制，核心步骤如下：

```cmake
# 1、设置编译配置变量
# CMAKE_ASC_ARCHITECTURES：指定NPU架构版本，本样例支持 dav-2201 和 dav-3510，可通过-DCMAKE_ASC_ARCHITECTURES=dav-xxxx配置
set(CMAKE_ASC_ARCHITECTURES "dav-2201" CACHE STRING "NPU architecture")

# 2、find_package(ASC)是CMake中用于查找和配置Ascend C编译工具链的命令
find_package(ASC)

# 3、指定项目支持的语言包括ASC和CXX
project(kernel LANGUAGES ASC CXX)

# 4、使用CMake接口编译可执行文件
add_executable(demo
    add_kernel.asc
    main.cpp)

# 5、设置链接器语言为ASC
set_target_properties(demo PROPERTIES LINKER_LANGUAGE ASC)

# 6、配置编译选项：--npu-arch指定架构版本
target_compile_options(demo PRIVATE
    $<$<COMPILE_LANGUAGE:ASC>:--npu-arch=${CMAKE_ASC_ARCHITECTURES}>)
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
  ```bash
  mkdir -p build && cd build;      # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j;    # 编译工程，默认npu模式
  ./demo                           # 执行样例
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。

  示例如：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # CPU调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU架构：dav-2201 对应 Atlas A2 训练系列产品/Atlas A2 推理系列产品和 Atlas A3 训练系列产品/Atlas A3 推理系列产品，dav-3510 对应 Ascend 950PR/Ascend 950DT |

- 执行结果
  执行结果如下，说明精度对比成功。
  ```bash
  test pass!
  ```
