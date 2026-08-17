# Dynamic Library Compile 样例

## 概述

本样例基于Gather算子，演示了将Host + Device混合.asc文件（含Kernel定义与<<<>>>内核调用）编译为**动态共享库（.so）**，再由Host侧.cpp链接调用的动态库编译流程。其中.asc文件的<<<>>>调用函数通过extern暴露给纯Host侧的.cpp。代码结构与调用关系如图 1 所示：

<p align="center">
  <img src="./figures/dynamic_library_compile.png" width="50%">
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
├── 02_dynamic_library_compile
│   ├── CMakeLists.txt      // 编译工程文件
│   ├── gather_kernel.asc   // 混合文件（含Kernel定义、<<<>>>内核调用）
│   ├── main.cpp            // 纯Host侧代码（通过extern调用.asc暴露的函数）
│   └── figures             // README中的图片资源
```

## 算子描述
gather算子实现了从长度为100000的一维输入向量中获取指定索引的12288个数据的功能。

## 动态库编译介绍

通过动态库编译，`.asc` 混合文件被编译为独立的 **动态共享库（.so）**，Host侧 `.cpp` 通过链接该 `.so` 完成调用。

### bisheng命令行编译

使用 `-shared` 选项将 `.asc` 混合文件编译为动态共享库（.so）。具体编译命令如下：

```shell
bisheng -shared kernel.asc -o libkernel.so -fPIC --npu-arch=dav-3510 --enable-simt
```

其中，`--npu-arch` 为编译时指定的AI处理器架构，本样例仅支持 `dav-3510`，各产品型号对应的架构版本号请通过[对应关系表](../../../../../docs/zh/guide/programming_guide/language_extension/simd_builtin_keywords.md#npu-arch)进行查询。`--enable-simt` 用于指定SIMT方式编译。

更多 bisheng 编译选项及用法说明，请参考[AI-Core算子编译基本用法](../../../../../docs/zh/guide/programming_guide/compilation_and_execution/operator_compilation/ai_core_operator_compilation.md)。

### CMake方式编译

在 CMake 工程中，使用 `add_library` 配置 `SHARED` 关键字即可将 `.asc` 文件编译为动态共享库：

```cmake
# 将.asc文件编译为动态共享库
add_library(gather_kernel SHARED
    gather_kernel.asc
)
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
    bisheng -shared gather_kernel.asc -o libgather_kernel.so -fPIC --npu-arch=dav-3510 --enable-simt    # 编译 .asc 为动态共享库
    bisheng main.cpp -o demo -L./ -L${ASCEND_HOME_PATH}/lib64 -I${ASCEND_HOME_PATH}/include \
      -lgather_kernel -lascendcl -lstdc++                                                              # 链接生成可执行文件
    export LD_LIBRARY_PATH=.:$LD_LIBRARY_PATH                                                          # 设置动态库搜索路径
    ./demo                                                                                             # 执行样例
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
