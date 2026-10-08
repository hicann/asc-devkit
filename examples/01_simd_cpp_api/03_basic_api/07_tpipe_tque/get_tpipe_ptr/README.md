# GetTPipePtr样例

## 概述

本样例基于GetTPipePtr接口获取全局TPipe指针，并通过该指针进行TPipe相关操作。

> **注意：** 本样例仅适用于基于TPipe和TQue的编程模式。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── get_tpipe_ptr
│   ├── CMakeLists.txt          // 编译工程文件
│   ├── get_tpipe_ptr.asc       // Ascend C样例实现 & 调用样例
│   └── README.md               // 样例说明文档
```

## 样例描述

- 样例功能

  创建TPipe对象时，对象初始化会设置全局唯一的TPipe指针。本样例调用GetTPipePtr接口获取该指针，核函数无需显式传入TPipe指针即可进行TPipe相关操作。下面将展示调用GetTPipePtr接口和不调用的代码片段示例。

  **调用GetTPipePtr接口**

  ```cpp
  template <uint32_t totalLength, uint32_t tileNum>
  __aicore__ inline void Process(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z)
  {
      // 没有TPipe变量
      // ...
      // 调用GetTPipePtr获取TPipe指针并使用
      GetTPipePtr()->InitBuffer(inQueueX, BUFFER_NUM, tileLength * sizeof(float));
  }

  template <uint32_t totalLength, uint32_t tileNum>
  __global__ __vector__ void get_tpipe_ptr_custom(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z)
  {
      AscendC::TPipe pipe;
      // 不用显式传入TPipe指针
      Process<totalLength, tileNum>(x, y, z);
  }
  ```

  **不调用GetTPipePtr接口（核函数显式传入TPipe指针）**

  ```cpp
  template <uint32_t totalLength, uint32_t tileNum>
  __aicore__ inline void Process(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z, AscendC::TPipe* pipe)
  {
      // 需要TPipe指针入参
      // ...
      // 使用传入的TPipe指针
      pipe->InitBuffer(inQueueX, BUFFER_NUM, tileLength * sizeof(float));
  }

  template <uint32_t totalLength, uint32_t tileNum>
  __global__ __vector__ void get_tpipe_ptr_custom(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z)
  {
      AscendC::TPipe pipe;
      // 需要显式传入TPipe指针
      Process<totalLength, tileNum>(x, y, z, &pipe);
  }
  ```

- 样例规格

  <table>
    <tr>
      <td align="center">类别</td>
      <td align="center">name</td>
      <td align="center">shape</td>
      <td align="center">data type</td>
      <td align="center">format</td>
    </tr>
    <tr>
      <td rowspan="2" align="center">样例输入</td>
      <td align="center">x</td>
      <td align="center">[8, 2048]</td>
      <td align="center">float</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">y</td>
      <td align="center">[8, 2048]</td>
      <td align="center">float</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">样例输出</td>
      <td align="center">z</td>
      <td align="center">[8, 2048]</td>
      <td align="center">float</td>
      <td align="center">ND</td>
    </tr>
    <tr>
      <td align="center">核函数名</td>
      <td colspan="4" align="center">get_tpipe_ptr_custom</td>
    </tr>
  </table>

- 样例实现

  - Kernel实现

    - 调用GetTPipePtr接口获取全局TPipe指针。

    - 调用TPipe::InitBuffer接口，为TQue分配内存空间。

    - 调用DataCopy基础API，将数据从GM（Global Memory）搬运到UB（Unified Buffer）。

    - 调用Add接口，对两个输入tensor执行加法操作。

    - 调用DataCopy基础API，将计算结果从UB（Unified Buffer）搬运至GM（Global Memory）。

  - 调用实现

    使用内核调用符<<<>>>调用核函数。

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
  ./demo                           # 执行编译生成的可执行程序，执行样例
  ```

  使用 CPU调试 或 NPU仿真 模式时，添加 `-DCMAKE_ASC_RUN_MODE=cpu` 或 `-DCMAKE_ASC_RUN_MODE=sim` 参数即可。
  
  示例如下：
  ```bash
  cmake -DCMAKE_ASC_RUN_MODE=cpu -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # cpu调试模式
  cmake -DCMAKE_ASC_RUN_MODE=sim -DCMAKE_ASC_ARCHITECTURES=dav-2201 ..;make -j; # NPU仿真模式
  ```

  > **注意：** 切换编译模式前需清理 cmake 缓存，可在 build 目录下执行 `rm CMakeCache.txt` 后重新 cmake。

- 编译选项说明

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_RUN_MODE` | `npu`（默认）、`cpu`、`sim` | 运行模式：NPU 运行、CPU调试、NPU仿真 |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-2201`（默认）、`dav-3510` | NPU 架构：dav-2201 对应 Atlas A2系列产品和 Atlas A3系列产品，dav-3510 对应 Ascend 950PR&950DT系列产品 |

- 执行结果

  执行结果如下，说明精度对比成功：
  ```bash
  [Success] Case accuracy verification passed.
  ```
