# Add样例

## 概述

本样例展示了Ascend C向量加法的基本用法。

## 本样例支持的产品及CANN软件版本

| 产品 | CANN软件版本 |
|------|-------------|
| Ascend 950PR/Ascend 950DT | >= CANN 9.1.0 |
| Atlas A3 训练系列产品/Atlas A3 推理系列产品 | >= CANN 9.0.0 |
| Atlas A2 训练系列产品/Atlas A2 推理系列产品 | >= CANN 9.0.0 |

## 目录结构介绍

```
├── add
│   ├── CMakeLists.txt      // 编译工程文件
│   ├── add.asc             // Ascend C样例实现 & 调用样例
│   └── README.md           // 样例说明文档
```

## 算子概述

Add算子实现两个向量的逐元素加法运算，计算公式为：

$$
z_i = x_i + y_i
$$

- x：输入，形状为[8, 2048]，数据类型为float，数据排布格式为ND；
- y：输入，形状为[8, 2048]，数据类型为float，数据排布格式为ND；
- z：输出，形状为[8, 2048]，数据类型为float，数据排布格式为ND；

样例运行参数：本样例使用8个核完成计算，每个核处理2048个元素（`blockLength = 2048`），数据总量为8×2048=16384个float元素。

## 算子实现

Add算子的计算逻辑遵循"搬入-计算-搬出"三段式流水结构：

1. 将输入数据x和y从[GM](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/GlobalTensor/GlobalTensor简介.md)（Global Memory，芯片外部全局内存）搬运到[UB](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md)（Unified Buffer，向量计算专用片上缓存）；
2. 在UB上对xLocal、yLocal执行向量加法操作，计算结果存储在zLocal中；
3. 将计算结果从UB搬运回GM。

**前置说明**：

- [GM（Global Memory）](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/GlobalTensor/GlobalTensor简介.md)：AI Core外部的全局存储，数据通过[GlobalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/GlobalTensor/GlobalTensor简介.md)访问，容量大但访问速度较慢。
- [UB（Unified Buffer）](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md)：AI Core内部的向量计算专用缓存，数据通过[LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md)访问，容量有限但访问速度快。
- [DataCopy](../../../../../docs/zh/api/SIMD-API/基础API/Memory矢量计算/数据搬运/DataCopy（GM与UB连续数据搬运）.md)：用于在GM和UB之间搬运数据的API，搬运方向由参数顺序决定。
- [PipeBarrier](../../../../../docs/zh/api/SIMD-API/基础API/同步控制/核内同步/PipeBarrier(ISASI).md)：流水线同步屏障，确保数据搬运完成后再执行后续操作，避免读写冲突。
- `block_idx`：通过[GetBlockIdx()](../../../../../docs/zh/api/SIMD-API/基础API/工具接口/系统资源与变量/GetBlockIdx.md)获取当前核的编号，用于多核并行时的数据分片计算。

核心代码如下：

```cpp
template <uint32_t blockLength>
__vector__ __global__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    AscendC::InitSocState();

    // Global Tensor：在GM上分配输入/输出缓冲区
    AscendC::GlobalTensor<float> xGm, yGm, zGm;
    xGm.SetGlobalBuffer(x + block_idx * blockLength, blockLength);  // 每个核按block_idx偏移处理各自的数据段
    yGm.SetGlobalBuffer(y + block_idx * blockLength, blockLength);
    zGm.SetGlobalBuffer(z + block_idx * blockLength, blockLength);

    // Local Tensor：在UB上分配计算缓冲区
    AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    AscendC::LocalTensor<float> xLocal = ubAllocator.Alloc<float, blockLength>();
    AscendC::LocalTensor<float> yLocal = ubAllocator.Alloc<float, blockLength>();
    AscendC::LocalTensor<float> zLocal = ubAllocator.Alloc<float, blockLength>();

    // GM -> UB: 搬入输入数据
    AscendC::DataCopy(xLocal, xGm, blockLength);
    AscendC::DataCopy(yLocal, yGm, blockLength);
    AscendC::PipeBarrier<PIPE_ALL>();  // 确保搬入完成后才进行计算

    // 向量计算: z = x + y
    AscendC::Add(zLocal, xLocal, yLocal, blockLength);
    AscendC::PipeBarrier<PIPE_ALL>();  // 确保计算完成后才搬出

    // UB -> GM: 搬出计算结果
    AscendC::DataCopy(zGm, zLocal, blockLength);
    AscendC::PipeBarrier<PIPE_ALL>();  // 确保搬出完成
}
```

调用方式：使用内核调用符`<<<numBlocks, 0, stream>>>`调用核函数，`numBlocks=8`指定使用8个核并行执行。

## 实现流程解析

| 阶段 | 数据流动/行为 | 实现目的/原因 |
|:---:|:---|:---|
| 初始化 | `InitSocState()` | 初始化AI Core硬件状态，为后续操作做准备 |
| GM地址分配 | `SetGlobalBuffer(x + block_idx * blockLength, blockLength)` | 每个核根据`block_idx`计算偏移量，处理不同的数据段，实现多核并行 |
| UB空间分配 | `ubAllocator.Alloc<float, blockLength>()` | 在UB上为x、y、z各分配一块连续内存，供向量计算使用 |
| 搬入（Stage 1） | GM → UB：`DataCopy(xLocal, xGm)`、`DataCopy(yLocal, yGm)` | 将输入数据从GM搬运到UB，因为向量计算单元只能访问UB上的数据 |
| 流水同步 | `PipeBarrier<PIPE_ALL>()` | 确保搬入完成后再开始计算，避免计算单元读取到未就绪的数据 |
| 计算（Stage 2） | UB上计算：`Add(zLocal, xLocal, yLocal)` | 在UB上执行向量加法，利用向量单元并行处理多个元素 |
| 流水同步 | `PipeBarrier<PIPE_ALL>()` | 确保计算完成后再开始搬出，避免搬出未完成的结果 |
| 搬出（Stage 3） | UB → GM：`DataCopy(zGm, zLocal)` | 将计算结果从UB搬运回GM，供后续使用或输出 |
| 流水同步 | `PipeBarrier<PIPE_ALL>()` | 确保搬出完成，保证数据一致性 |

## 可优化方向分析

| 序号 | 可优化方向 | 当前实现的问题 | 预期优化收益 |
|------|-----------|--------------|------------|
| 1 | 多核动态分配 | 固定使用8个核，未根据实际可用核数动态分配 | 动态获取可用核数，充分利用多核并行能力，减少端到端耗时 |
| 2 | 增大搬运粒度 | 每次搬运2048个float元素（8KB），搬运粒度较小 | 增大单次搬运数据量，减少搬运次数，摊薄启动开销，提升带宽利用率 |
| 3 | 双缓冲流水线并行 | 搬入、计算、搬出三个阶段严格串行执行，各硬件单元（MTE2/V/MTE3）无法同时工作 | 采用Ping-Pong双缓冲机制，使搬入、计算、搬出可并行执行，隐藏搬运延迟 |
| 4 | L2 Cache bypass | Add输入数据只读取一次，但默认经过L2 Cache，增加了Cache污染 | 对流式访问数据设置L2 Cache bypass，减少不必要的Cache开销，提升搬运效率 |

完整性能调优过程请参见[Add性能调优样例](https://gitcode.com/cann/asc-devkit/tree/master/examples/01_simd_cpp_api/05_best_practices/00_vector_compute/add_high_performance)。

## 功能调试

### printf

该接口提供CPU域/NPU域调试场景下的格式化输出功能。

在算子kernel侧实现代码中需要输出日志信息的地方调用[printf](../../../../../docs/zh/api/SIMD-API/基础API/调试接口/上板打印/printf.md)接口打印相关内容。

示例如下：

```cpp
AscendC::printf("add blockIdx=%d\n", AscendC::GetBlockIdx());
```

> **注意：** printf（PRINTF）接口打印功能会对算子实际运行的性能带来一定影响，通常在调测阶段使用。开发者可以按需通过设置`ASCENDC_DUMP=0`的方式关闭打印功能。

### DumpTensor

基于算子工程开发的算子，可以使用该接口Dump指定[LocalTensor](../../../../../docs/zh/api/SIMD-API/基础API/数据结构/LocalTensor/LocalTensor简介.md)的内容。同时支持打印自定义的附加信息（仅支持uint32\_t数据类型的信息），比如打印当前行号等。

在算子kernel侧实现代码中需要打印Tensor数据的地方调用[DumpTensor](../../../../../docs/zh/api/SIMD-API/基础API/调试接口/上板打印/DumpTensor.md)接口打印相关内容。样例如下：

```cpp
// 向量计算: z = x + y
AscendC::Add(zLocal, xLocal, yLocal, blockLength);
AscendC::DumpTensor(zLocal, 1, 32);
```

> **注意：** DumpTensor接口打印功能会对算子实际运行的性能带来一定影响，通常在调测阶段使用。开发者可以按需通过设置`ASCENDC_DUMP=0`来关闭打印功能。

## 性能调试

### msOpProf工具介绍
msOpProf工具是单算子性能分析工具。包含msopprof和msopprof simulator两种使用方式。该工具协助用户定位算子内存、算子代码以及算子指令的异常，实现全方位的算子调优。当前支持基于不同运行模式（上板或仿真）和不同文件形式（可执行文件或算子二进制.o文件）进行性能数据的采集和自动解析。

- 上板性能采集

    通过上板性能采集，可以直接测定算子昇腾AI处理器上的运行时间。该方式适合在板环境中快速定位算子性能问题。

    基于可执行文件demo通过msopprof执行算子调优：
    ```
    msopprof ./demo
    ```

    - 性能数据说明  
      命令完成后，会在默认目录下生成以“OPPROF_{timestamp}_XXX”命名的文件夹,性能数据文件夹结构示例如下：

      ```bash
      ├──dump                       # 原始的性能数据，用户无需关注
      ├──ArithmeticUtilization.csv  # cube/vector指令cycle占比
      ├──L2Cache.csv                # L2 Cache命中率，影响MTE2，建议合理规划数据搬运逻辑，增加命中率
      ├──Memory.csv                 # UB，L1和主存储器读写带宽速率
      ├──MemoryL0.csv               # L0A，L0B，和L0C读写带宽速率
      ├──MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
      ├──OpBasicInfo.csv            # 算子基础信息
      ├──PipeUtilization.csv        # 采集计算单元和搬运单元耗时和占比
      ├──ResourceConflictRatio.csv  # UB上的bank group、bank conflict和资源冲突率在所有指令中的占比
      └──visualize_data.bin         # MindStudio Insight呈现文件
      ```

查看具体的性能分析结果：

```bash
# 查看Task Duration 以及各项数据
cat ./OPPROF_*/PipeUtilization.csv
```

请参考 [《msOpProf用户指南》](https://gitcode.com/Ascend/msopprof/blob/master/docs/zh/user_guide/msopprof_user_guide.md)算子调优（msOpProf）中的内容。

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
