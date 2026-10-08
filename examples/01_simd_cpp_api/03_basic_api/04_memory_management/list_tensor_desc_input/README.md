# list_tensor_desc_input样例

## 概述

本样例基于静态Tensor编程模型实现AddN样例，采用ListTensorDesc结构处理动态输入参数，结合静态内存分配与事件同步机制实现数据搬运与计算任务的协同调度。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR&950DT系列产品 | >= CANN 9.1.0 |
| Atlas A3系列产品 | >= CANN 9.0.0 |
| Atlas A2系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── list_tensor_desc_input
│   ├── CMakeLists.txt              // 编译工程文件
│   ├── list_tensor_desc_input.asc  // Ascend C样例实现 & 调用样例
│   └── README.md                   // 样例说明文档
```

## 样例描述

- 样例功能

  本样例以Add计算为例展示静态Tensor编程模型中ListTensorDesc动态输入参数的使用方法，适用于以下场景：
  1. 多输入参数动态处理：支持模型中多个输入张量的动态组合运算（如多分支网络结构）。
  2. 内存流水线优化：通过静态双缓冲与事件同步机制实现数据搬运与计算的流水线并行，降低内存访问延迟。
  3. 多核并行计算：适配AI处理器的多核架构，支持大规模张量运算的高效分发。

- 样例规格

  <table>
  <tr><td rowspan="1" align="center">样例类型(OpType)</td><td colspan="4" align="center">AddN</td></tr>
  <tr><td rowspan="3" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">x（动态输入参数srcList[0]）</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td align="center">y（动态输入参数srcList[1]）</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">z</td><td align="center">[8, 2048]</td><td align="center">float</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">list_tensor_desc_input_custom</td></tr>
  </table>

- 样例实现

  - Kernel实现

    动态输入特性是指，核函数的入参采用ListTensorDesc的结构存储输入数据信息。构造ListTensorDesc数据结构，示例如下。
    ```cpp
    constexpr uint32_t SHAPE_DIM = 2;
    struct TensorDesc {
      uint32_t dim{SHAPE_DIM};
      uint32_t index;
      uint64_t shape[SHAPE_DIM] = {8, 2048};
    };

    constexpr uint32_t TENSOR_DESC_NUM = 2;
    struct ListTensorDesc {
      uint64_t ptrOffset;
      TensorDesc tensorDesc[TENSOR_DESC_NUM];
      uintptr_t dataPtr[TENSOR_DESC_NUM];
    } inputDesc;
    ```
    将申请分配的Tensor入参组合成ListTensorDesc的数据结构，示例如下。
    ```cpp
    inputDesc = {(1 + (1 + SHAPE_DIM) * TENSOR_DESC_NUM) * sizeof(uint64_t),
                {xDesc, yDesc},
                {(uintptr_t)xDevice, (uintptr_t)yDevice}};
    ``` 
    按照传入的数据格式，解析出对应的各入参，示例如下。

    ```cpp
    AscendC::ListTensorDesc keyListTensorDescInit((__gm__ void*)srcList);
    __gm__ uint8_t* x = (__gm__ uint8_t*)keyListTensorDescInit.GetDataPtr<__gm__ uint8_t>(0);
    __gm__ uint8_t* y = (__gm__ uint8_t*)keyListTensorDescInit.GetDataPtr<__gm__ uint8_t>(1);
    ```

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
