# ms_sanitizer样例说明

## 概述

msSanitizer是基于昇腾AI处理器的异常检测工具，用于检测Ascend C样例运行过程中的内存访问、内存泄漏、未使用内存、竞争和未初始化访问等异常。本样例以Add计算为载体，展示如何通过msSanitizer工具运行样例并定位异常。

本样例代码为正确实现。用户可以按照“异常场景检测”章节中的说明修改`ms_sanitizer.asc`，重新编译后通过msSanitizer工具体验对应的异常检测能力。

请参考[算子开发工具](https://www.hiascend.com/document/redirect/CannCommercialToolOpDev)中的“环境准备”章节，获取详细的安装指南和步骤。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── ms_sanitizer
│   ├── CMakeLists.txt       // 编译工程文件
│   ├── ms_sanitizer.asc     // Ascend C样例实现和Host侧调用样例
│   └── README.md            // 样例说明
```

## 样例描述

本样例默认使用`float`类型，完成两个Tensor逐元素相加：

```text
z = x + y
```

输入`x`、`y`和输出`z`的Shape均为`[8, 2048]`。Host侧直接构造输入数据和golden数据，将输入数据拷贝到Device侧后启动`add_custom` Kernel，Kernel按`blockNum = 8`切分数据并完成Add计算，最后将结果拷贝回Host侧完成精度对比。

## 异常场景检测

复现异常场景时，需要先按对应说明修改`ms_sanitizer.asc`中的代码，再在`build`目录下重新编译并运行对应检测命令。如果只修改源码但没有重新编译，`mssanitizer`仍会运行旧的`demo`，不会检测到新改入的错误。

- **内存检测**

  - 非法读写：由于访问了未分配的内存导致的异常。

    用户可以注释掉正确的`DataCopy`，使用错误的`DataCopy`复现该场景。`xLocal`分配的大小为`blockLength`，但是搬运长度错误地写成`blockLength * 2`，大于`xLocal`分配的大小，因此触发非法读写。

    ```cpp
    // 1. 正确的DataCopy。
    AscendC::DataCopy(xLocal, xGm, blockLength);
    // 2. 错误示例：搬运长度写成blockLength * 2，会触发非法读写。
    // AscendC::DataCopy(xLocal, xGm, blockLength * 2);
    ```

    重新编译并执行检测：

    ```bash
    make -B demo
    mssanitizer ./demo
    ```

  - 非对齐访问：内存访问未满足字节对齐要求。

    用户可以注释掉正确的`DataCopy`，使用错误的`DataCopy`复现该场景。`DataCopy` GM到UB的搬运中，UB侧地址需要满足32B对齐要求，`xLocal[5]`不满足32B对齐要求，因此触发非对齐访问。

    ```cpp
    // 1. 正确的DataCopy。
    AscendC::DataCopy(xLocal, xGm, blockLength);
    // 3. 错误示例：xLocal[5]不满足32B对齐要求，会触发非对齐访问。
    // AscendC::DataCopy(xLocal[5], xGm, blockLength);
    ```

    重新编译并执行检测：

    ```bash
    make -B demo
    mssanitizer ./demo
    ```

  - 内存泄漏：申请内存使用后未释放，导致程序在运行过程中内存占用持续增加的异常。

    用户可以注释掉`aclrtFree(zDevice)`来复现该场景。注释前`zDevice`被正常释放，注释后`zDevice`在使用后未释放，因此触发内存泄漏。

    注意：调用`mssanitizer`时需要传入`--leak-check=yes`来开启分配内存泄漏检查。

    ```cpp
    // 正确释放Device侧输出内存。删除该行后，可触发内存泄漏检测。
    aclrtFree(zDevice);
    ```

    重新编译并执行检测：

    ```bash
    make -B demo
    mssanitizer ./demo --leak-check=yes
    ```

  - 分配内存未使用：对内存分配后未使用导致的异常。

    用户可以注释掉正确的`aclrtMalloc`，使用错误的`aclrtMalloc`复现该场景。`xDevice`实际只使用`totalByteSize`大小，但是错误示例申请了5倍大小，其中多余部分未使用，因此触发分配内存未使用。

    注意：调用`mssanitizer`时需要传入`--check-unused-memory=yes`来开启分配内存未使用检查。

    ```cpp
    // 1. 正确的Device侧输入内存申请。
    aclrtMalloc((void**)&xDevice, totalByteSize, ACL_MEM_MALLOC_HUGE_FIRST);
    // 2. 错误示例：实际只使用totalByteSize，却申请5倍大小，会触发未使用内存检测。
    // aclrtMalloc((void**)&xDevice, totalByteSize * 5, ACL_MEM_MALLOC_HUGE_FIRST);
    ```

    重新编译并执行检测：

    ```bash
    make -B demo
    mssanitizer ./demo --check-unused-memory=yes
    ```

- **竞争检测**

  竞争检测用于定位并行执行中的内存访问竞争问题。用户可以注释掉`MTE2_V`对应的`SetFlag`和`WaitFlag`来复现该场景。这组同步用来保证MTE2完成GM到UB搬运后，Vector再读取`xLocal`和`yLocal`执行Add。删除后，Vector可能在搬入完成前读取UB数据，因此触发竞争检测。

  注意：调用`mssanitizer`时需要传入`--tool=racecheck`来开启竞争检测。

  ```cpp
  // Add读取xLocal/yLocal前，需要等待MTE2搬运完成；删除后会触发RAW竞争。
  AscendC::SetFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
  AscendC::WaitFlag<AscendC::HardEvent::MTE2_V>(EVENT_ID0);
  ```

  重新编译并执行检测：

  ```bash
  make -B demo
  mssanitizer ./demo --tool=racecheck
  ```

- **未初始化检测**

  未初始化检测用于定位内存申请后未初始化、直接读取未初始化值导致的异常。用户可以注释掉`zGm.SetGlobalBuffer`来复现该场景。对于Device侧的`zGm`在使用前未初始化，因此触发未初始化检测。

  注意：调用`mssanitizer`时需要传入`--tool=initcheck`来开启未初始化检测。

  ```cpp
  // 正确初始化zGm。删除该行后，可触发未初始化检测。
  zGm.SetGlobalBuffer(z + block_idx * blockLength, blockLength);
  ```

  重新编译并执行检测：

  ```bash
  make -B demo
  mssanitizer ./demo --tool=initcheck
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
  mkdir -p build && cd build
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..; make -j

  # 根据调测需求执行对应的mssanitizer命令。
  mssanitizer ./demo
  mssanitizer ./demo --leak-check=yes
  mssanitizer ./demo --check-unused-memory=yes
  mssanitizer ./demo --tool=racecheck
  mssanitizer ./demo --tool=initcheck
  ```

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201` | NPU架构：`dav-2201`对应Atlas A2系列产品和Atlas A3系列产品 |

- 执行结果

  精度对比成功时，输出如下：

  ```bash
  test pass!
  ```
