# 类型对齐优化搬运效率最佳实践样例

## 概述

  本样例量化展示**结构体类型对齐对Global Memory访存的影响**：结合Ascend 950PR/Ascend 950DT支持1B、2B、4B、8B、16B位宽的访存指令，构造多个大小不同的结构体做逐元素数据搬运，对比类型对齐带来的搬运效率差异。

## 支持的产品

  - Ascend 950PR/Ascend 950DT

## 支持的 CANN 软件版本

  - \>= CANN 9.1.0

## 目录结构

  ```
  ├── aligned_types
  │   ├── aligned_types.asc      // SIMT实现&调用样例
  │   ├── CMakeLists.txt         // cmake编译文件
  │   └── README.md
  ```

## 样例描述

本样例选取14个结构体，按`sizeof`分为4B、8B、16B、32B四组，每组做类型对齐与非对齐的对照；其中4B组覆盖`alignof=1`与`alignof=2`两档窄对齐，并补充2组字段类型不一致的混合结构体对照。测试时依次将长度为`num_elements`的输入数组`d_idata`整体复制到`d_odata`，对比搬运效率。

- 基础概念

  **1、alignof**

  编译期运算符，返回对象的对齐要求，即该对象的起始地址必须是多少字节的倍数。结构体的默认`alignof`等于其所有字段`alignof`的最大值，可通过关键字`alignas`强制加大。例如下面的`struct T`和`struct T2`，`T`的`alignof`是4B，`T2`的`alignof`是8B。

  ```cpp
  struct T { uint8_t a; uint32_t b; };  // alignof = max(1, 4) = 4
  struct alignas(8) T2 { uint8_t a; uint32_t b; };  // alignof = 8 (alignas 放大)
  ```

  **2、alignas**

  C++11引入的对齐说明符，在声明对象时强制加大对齐。

  `alignas`的几条限制：

  - **只能放大，不能缩小**：`alignas(N)`中的`N`必须大于等于类型的默认`alignof`，即所有字段`alignof`的最大值，否则编译器会忽略或报错。
  - **取值必须是2的幂**：`alignas(N)`中的`N`必须是2的幂。
  - **会同步抬升sizeof**：`sizeof`必须是`alignof`的整数倍，所以放大`alignof`时，`sizeof`会向上进位。

  ```cpp
  struct alignas(4) Align3 { unsigned char r, g, b; }; // alignof=4 sizeof=4 (随alignas抬升)
  struct Misalign3 { unsigned char r, g, b; }; // alignof=1 sizeof=3 (默认)
  ```

- 关键参数

  | 参数 | 值 | 说明 |
  |------|-----|------|
  | MEM_BYTES      | **512MiB** | 输入/输出buffer字节数 |
  | THREAD_COUNT   | 2048   | 单核2048线程，grid固定`<<<1, 2048>>>` |
  | num_elements   | MEM_BYTES / sizeof(TData) | 每个用例的元素数，随结构体大小自动伸缩 |

- 样例规格

  | 名称              | 字段                    | sizeof | alignof | num_elements  |
  |------------------|--------------------------|-------|--------|-----------|
  | Align4           | 4 × u8                   |  4B   |   4B   | 134217728 |
  | Misalign4        | 4 × u8                   |  4B   | **1B** | 134217728 |
  | AlignU8U16       | 2 × u8 + u16             |  4B   |   4B   | 134217728 |
  | MisalignU8U16    | 2 × u8 + u16             |  4B   | **2B** | 134217728 |
  | AlignU8PadU16    | u8 (1B padding) + u16    |  4B   |   4B   | 134217728 |
  | MisalignU8PadU16 | u8 (1B padding) + u16    |  4B   | **2B** | 134217728 |
  | Align2U16        | 2 × u16                  |  4B   |   4B   | 134217728 |
  | Misalign2U16     | 2 × u16                  |  4B   | **2B** | 134217728 |
  | Align8           | 2 × u32                  |  8B   |   8B   | 67108864  |
  | Misalign8        | 2 × u32                  |  8B   | **4B** | 67108864  |
  | Align16          | 4 × u32                  |  16B  |  16B   | 33554432  |
  | Misalign16       | 4 × u32                  |  16B  | **4B** | 33554432  |
  | Align32          | 8 × u32                  |  32B  |  32B   | 16777216  |
  | Misalign32       | 8 × u32                  |  32B  | **4B** | 16777216  |

- 样例实现

  - Kernel实现  

    本样例总数据量固定为512MiB，采用单核2048线程完成数据搬运。核函数以模板方式实现，支持不同类型与对齐方式的结构体测试，执行逐元素搬运操作，具体实现如下：

    ```
    constexpr uint32_t THREAD_COUNT = 2048;                  // threads per block
    constexpr size_t MEM_BYTES = 512ULL * 1024ULL * 1024ULL; // 总数据量 512 MiB
    uint32_t num_elements = static_cast<uint32_t>(MEM_BYTES / sizeof(TData));

    template <class TData>
    __global__ __launch_bounds__(THREAD_COUNT) void aligned_type_kernel(
        TData* d_odata, TData* d_idata, uint32_t num_elements)
    {
        const uint32_t tid = blockIdx.x * blockDim.x + threadIdx.x;
        const uint32_t num_threads = blockDim.x * gridDim.x;
        for (uint32_t pos = tid; pos < num_elements; pos += num_threads) {
            d_odata[pos] = d_idata[pos];
        }
    }
    ```

  - 调用实现  

    使用内核调用符<<<>>>调用核函数。

## 性能分析

- 性能指标说明

  |             字段名          | 字段含义                                             |
  |:---------------------------:|:-------------------------------------------------|
  |      Task Duration(μs)      | Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间。          |
  |      aiv_total_cycles       | Task在 Vector Core上执行所消耗的CPU周期（Cycle）总数。          |

  除 Task Duration 外，本例中其余指标均展示的是所有 block 上性能指标的平均值。

- 性能数据

  |       TData       | Task Duration(μs) | aiv_total_cycles |
  | :---------------: | :---------------: | :--------------: |
  |      Align4       |     27889.33      |   46005213.32    |
  |     Misalign4     |     81223.02      |   134028903.60   |
  |     Align2U16     |     27833.56      |   45918496.16    |
  |   Misalign2U16    |     46656.61      |   76990974.19    |
  |    AlignU8U16     |     27854.56      |   45939796.81    |
  |   MisalignU8U16   |     46666.11      |   76970946.68    |
  |   AlignU8PadU16   |     27809.72      |   45872788.00    |
  | MisalignU8PadU16  |     46664.42      |   76969440.94    |
  |      Align8       |     17417.23      |   28735088.65    |
  |     Misalign8     |     21287.91      |   35112582.91    |
  |      Align16      |     27931.29      |   46090787.94    |
  |    Misalign16     |     38353.60      |   63284236.16    |
  |      Align32      |     52357.78      |   86396567.61    |
  |    Misalign32     |     77790.93      |   128352303.30   |

- 综合分析

  本次所有测试用例的总搬运数据量统一为512MiB，更宽的访存指令，总体耗时更少，搬运效率更高。将各对照组对齐前后的耗时差异汇总如下：

  | 结构体大小                  | 对齐Task Duration(μs) | 非对齐Task Duration(μs)  | 非对齐Task Duration/对齐Task Duration |
  | :------------------------: | :-------------------: | :---------------------: | :---------: |
  | 4B (4×u8)                  | 27889.33 (alignof=4)  | 81223.02 (alignof=1)    | **2.91x**   |
  | 4B (2×u16)                 | 27833.56 (alignof=4)  | 46656.61 (alignof=2)    | **1.68x**   |
  | 4B (2×u8 + u16)            | 27854.56 (alignof=4)  | 46666.11 (alignof=2)    | **1.68x**   |
  | 4B (u8 + 1B padding + u16) | 27809.72 (alignof=4)  | 46664.42 (alignof=2)    | **1.68x**   |
  | 8B                         | 17417.23 (alignof=8)  | 21287.91 (alignof=4)    | **1.22x**   |
  | 16B                        | 27931.29 (alignof=16) | 38353.60 (alignof=4)    | **1.37x**   |
  | 32B                        | 52357.78 (alignof=32) | 77790.93 (alignof=4)    | **1.49x**   |

  从上表可以看出：

  （1）数据总量相同时，`sizeof`相同的结构体，类型对齐的搬运效率明显高于非对齐。其中4B组对照最明显，随`alignof`由`1B→2B→4B`依次变大，`非对齐Task Duration/对齐Task Duration`的耗时倍数依次减小，即搬运耗时依次减少。这是因为`alignof`影响了访存指令的位宽，位宽越小，搬运耗时越长。
  
  > 需要注意的是，**指令宽度翻倍并不等于耗时减半**：kernel函数使用for循环逐元素load/store数据，不同元素间的load/store指令可以流水并行。以元素`x1`、`x2`为例，`load x1`搬运完成后，`store x1`与`load x2`可同步进入访存流水，多条访存的执行时间被流水部分掩盖，因此4B组实测加速比小于按指令条数推算的理论上界。

  （2）三组4B结构体2×u16、2×u8 + u16、u8 + 1B padding + u16的`Task Duration`纵向对比没有差异，说明字段是同类型还是混合类型、有无padding均不影响搬运效率，编译器只关心结构体整体的`sizeof`与`alignof`。

  综上说明，在数据总量不变时，相同大小的结构体，类型对齐可以提升搬运效率，且`alignof`越大效率越高，上限为16B。

- 调优建议

  Ascend 950PR/Ascend 950DT支持1B、2B、4B、8B、16B五种位宽的访存指令，因此建议使用时，在不超过结构体大小的情况下，尽量选择更高的位宽。

## 编译运行

- 配置环境变量  
  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。
  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}` 为CANN包安装目录，未指定安装目录时默认安装至 `/usr/local/Ascend` 下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  mkdir -p build && cd build           # 创建并进入 build 目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 .. && make -j   # 编译工程
  ./demo                                # 执行样例
  ```

- 编译选项说明

  | 选项                      | 可选值     | 说明                                                         |
  | ------------------------- | ---------- | ----------------------------------------------------------- |
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU 架构：本样例仅支持 dav-3510（Ascend 950PR/Ascend 950DT）  |

- 执行结果

  执行结果如下，说明样例运行成功。

  ```bash
  [Success] Case accuracy is verification passed.
  ```

## 性能数据获取

  使用 `msOpProf` 工具获取单个组件上的性能数据：

  ```bash
  msopprof ./demo   # 分析样例的性能
  ```

  命令完成后，会在默认目录下生成以"OPPROF_{timestamp}_XXX"命名的文件夹,性能数据文件夹结构示例如下：

  ```text
  ├──dump                       # 原始的性能数据，用户无需关注
  ├──ArithmeticUtilization.csv  # cube/vector指令cycle占比
  ├──L2Cache.csv                # L2 Cache命中率
  ├──Memory.csv                 # UB，L1和主存储器读写带宽速率
  ├──MemoryL0.csv               # L0A，L0B，和L0C读写带宽速率
  ├──MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
  ├──OpBasicInfo.csv            # 算子基础信息
  ├──PipeUtilization.csv        # 采集计算单元和搬运单元耗时和占比
  ├──ResourceConflictRatio.csv  # UB上的 bank group、bank conflict和资源冲突率在所有指令中的占比
  └──visualize_data.bin         # MindStudio Insight呈现文件
  ```

  查看具体的性能分析结果：

  ```
  # 如查看Task Duration 相关数据
  cat ./OPPROF_*/OpBasicInfo.csv
  ```
