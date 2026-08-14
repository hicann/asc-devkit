# Histogram原子操作性能调优样例

## 概述

直方图（Histogram）是一种统计方法，用于统计输入数据中各值的出现频率。直方图中每个统计值对应一个桶（bin），各桶均设有独立的计数器。当输入数据中出现该值时，对应桶的计数器执行递增操作。

本样例以Histogram（直方图统计）为载体，展示原子操作性能优化方法。

<img src="figures/introduction.png" alt="intro" style="width: 60%; height: auto;">

## 支持的产品

- Ascend 950PR/Ascend 950DT

## 支持的CANN软件版本

- \>= CANN 9.1.0

## 目录结构介绍

```text
├── atomic_histogram
│   ├── figures                // README中的图片资源
│   ├── CMakeLists.txt         // 样例构建脚本
│   ├── histogram.asc          // Ascend C SIMT核函数实现 & Host调用样例
│   ├── README.md
│   └── README_en.md
```

## 样例描述
本样例对输入字节流进行统计，统计各个字节数值（0-255）出现的频率，输出256个桶的计数结果。
- 计算公式：

  ```text
  histogram[input[i]] += 1,  i = 0, 1, ..., byte_count - 1
  ```

  - input为输入字节流，取值范围为0-255
  - histogram为直方图，每个元素对应该字节值的出现次数
- 样例规格：

  <table>
  <tr><td align="center">样例类型（OpType）</td><td colspan="4" align="center">Atomic Optimization</td></tr>
  <tr><td rowspan="2" align="center">样例输入</td><td align="center">name</td><td align="center">shape</td><td align="center">data type</td><td align="center">format</td></tr>
  <tr><td align="center">input</td><td align="center">[262144]</td><td align="center">uint8</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">样例输出</td><td align="center">histogram</td><td align="center">[256]</td><td align="center">uint32</td><td align="center">ND</td></tr>
  <tr><td rowspan="1" align="center">核函数名</td><td colspan="4" align="center">histogram_gm_global / histogram_ub_block_local</td></tr>
  </table>

## 样例实现

### Case实现说明

本样例通过2个Case构成优化对比，各Case的核函数、计数位置如下表所示。

| Case | 核函数 | 计数位置 |
|---|---|---|
| 0 | `histogram_gm_global` | GM |
| 1 | `histogram_ub_block_local` | UB |

两个Case采用相同的线程块数量和线程数，输入读取方式与原子更新总次数完全一致，仅原子操作的目标地址所在的内存层级不同。

### 性能指标说明

| 指标 | 说明 |
| --- | --- |
| Task Duration（μs） | Task整体耗时，包含调度到加速器的时间、加速器上的执行时间以及响应结束时间 |

---

### Case 0：GM上的全局直方图计数

**样例目标**：所有线程直接对GM上的全局直方图执行原子累加，完成输入字节流的频率统计。

**场景配置**：

| Case | 计数位置 | 数据类型 | 规模（Block×Thread） |
|:---:|:---:|:---:|:---:|
| 0 | GM | uint32_t | 64×2048 |

**核心实现**：启动64个线程块，每个线程块2048个线程。各线程按固定步长遍历输入字节，对GM直方图对应桶执行[asc_atomic_add()](../../../../../docs/zh/api/SIMT-API/atomic_operations/asc_atomic_add.md)进行原子加1操作。262144次原子更新全部集中在256个GM地址上。

Case 0中各线程直接对GM上的全局直方图执行原子累加，执行流程如下图所示。

<img src="figures/case0_gm_global.png" alt="intro" style="width: 60%; height: auto;">

对应的核函数实现如下。

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void histogram_gm_global(
    uint32_t* histogram, const uint8_t* input, uint32_t byte_count)
{
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < byte_count; index += stride) {
        asc_atomic_add(&histogram[input[index]], 1U);
    }
}
```

**性能数据**：

| Case | 计数位置 | Task Duration（μs） |
|:---:|:---:|:---:|
| 0 | GM | 424.22 |

Case 0的耗时约424.22μs，由以下两个因素共同导致：

- **同一地址上的原子操作只能串行执行。** 原子加需完整完成“读出旧值—计算新值—写回新值”三个步骤。若多个线程读到相同旧值，后写回的结果将覆盖先写回的结果，导致部分累加丢失。原子操作为保证三个步骤不可分割，同一地址上的多次原子操作无法并行执行。本Case中262144次原子更新集中在256个GM地址上，平均每个地址需串行处理1024次原子加。
- **GM上单次原子操作的处理开销较高。** GM位于AI Core外部，访问路径较长、延迟较高。单次处理开销在串行执行过程中不断叠加，最终反映为较高的总耗时。

上述两个原理详细见[atomic_add_perf样例](../../../02_features/01_api_features/02_atomic_operation/atomic_add_perf/README.md)。

---

### Case 1：分块累加再合并的两级原子计数

**样例目标**：将直方图计数拆分为“线程块内UB原子累加”和“块间GM原子合并”两个层级，验证分块累加策略对高竞争原子场景的性能收益。

**场景配置**：

| Case | 计数位置 | 数据类型 | 规模（Block×Thread） |
|:---:|:---:|:---:|:---:|
| 1 | UB块内累加 + GM合并 | uint32_t | 64×2048 |

**核心实现**：启动64个线程块，每个线程块2048个线程。每个线程块在UB上维护一份256桶直方图，先将UB直方图各桶计数初始化为0，并通过`asc_syncthreads()`同步保证所有线程完成清零后再开始累加；随后各线程按固定步长遍历输入字节，对本线程块私有的UB直方图对应桶执行[asc_atomic_add()](../../../../../docs/zh/api/SIMT-API/atomic_operations/asc_atomic_add.md)进行原子加1操作，累加完成后再次通过`asc_syncthreads()`同步确保UB计数结果完整再合并至GM；最后由前256个线程将UB直方图结果原子累加至最终GM直方图。

Case 1在每个线程块的UB上维护独立直方图，最后再合并至GM，执行流程如下图所示。

<img src="figures/case1_ub_local.png" alt="intro" style="width: 60%; height: auto;">

对应的核函数实现如下。

```cpp
__global__ __launch_bounds__(THREADS_PER_BLOCK) void histogram_ub_block_local(
    uint32_t* histogram, const uint8_t* input, uint32_t byte_count)
{
    __ubuf__ uint32_t block_histogram[HISTOGRAM_BIN_COUNT];

    // Block内线程协作清零UB直方图
    for (uint32_t bin_index = threadIdx.x; bin_index < HISTOGRAM_BIN_COUNT; bin_index += blockDim.x) {
        block_histogram[bin_index] = 0;
    }
    asc_syncthreads();

    // grid-stride遍历，对本Block私有的UB直方图做原子累加
    uint32_t index = blockIdx.x * blockDim.x + threadIdx.x;
    const uint32_t stride = gridDim.x * blockDim.x;
    for (; index < byte_count; index += stride) {
        asc_atomic_add(&block_histogram[input[index]], 1U);
    }
    asc_syncthreads();

    // 前256个线程各负责一个桶，把Block局部结果原子累加到最终GM直方图
    for (uint32_t bin_index = threadIdx.x; bin_index < HISTOGRAM_BIN_COUNT; bin_index += blockDim.x) {
        asc_atomic_add(histogram + bin_index, block_histogram[bin_index]);
    }
}
```

**性能数据**：

| Case | 计数位置 | Task Duration（μs） |
|:---:|:---:|:---:|
| 1 | UB块内累加 + GM合并 | 7.95 |

**分析**：

Case 1的Task Duration为7.95μs，相比Case 0的424.22μs下降至约1/53。两个Case的原子更新总次数一致（均为262144次累加），输入读取方式也相同，唯一变化的是原子目标所在的内存层级与竞争范围。下面对这一现象进行分析。

收益来自两个叠加因素：

- **原子目标由GM迁移至UB，单次处理开销降低。** GM位于AI Core外部，访问路径较长；UB位于AI Core内部，访问路径较短、延迟较低。同一地址上的原子操作只能串行执行，单次处理开销的差异随串行次数不断叠加。
- **分块布局将竞争范围由全局收窄至Block内部，同一地址上的排队等待显著减少。** Case 0中256个GM地址由全部线程共享，平均每个地址执行1024次操作；Case 1中每个Block持有独立的256桶UB直方图，竞争范围限于本Block的2048个线程，平均每个地址执行16次操作，且64个Block之间的UB原子操作互不竞争，可并行执行。落在GM上的原子操作仅剩最后`64 × 256 = 16384`次合并，相比Case 0的262144次显著减少。

两个因素共同作用：绝大部分原子操作既迁移至开销更低的UB，又被拆分至各Block并行执行，所得收益远大于UB初始化、`asc_syncthreads`同步与GM合并引入的开销。

**结论**：将高竞争的原子操作目标由GM迁移至UB，既降低单次原子处理开销，又使各线程块的UB原子操作互不竞争、可并行执行，显著缩短串行开销。

---

## 性能对比总结

| Case | 计数位置 | Task Duration（μs） | 相对基线 |
|:---:|:---:|:---:|:---:|
| 0 | GM | 424.22 | 1× |
| 1 | UB块内累加 + GM合并 | 7.95 | **~53×** |

## 调优建议

**优先使用UB完成原子累加，再合并至GM**：UB位于AI Core内部，单次原子处理开销更低；各线程块的UB相互独立、原子操作互不竞争可并行执行，仅需最后将各Block结果合并至GM。

## 编译运行

在本样例根目录下执行如下步骤，编译并执行样例。

- 配置环境变量

  请根据当前环境上CANN开发套件包的[安装方式](../../../../../docs/zh/quick_start.md#prepare&install)，配置环境变量。

  ```bash
  source ${install_path}/cann/set_env.sh
  ```

  > **说明：** `${install_path}`为CANN包安装目录，未指定安装目录时默认安装至`/usr/local/Ascend`下。

- 样例执行

  在本样例目录下执行如下命令。

  ```bash
  mkdir -p build && cd build                                # 创建并进入build目录
  cmake -DCMAKE_ASC_ARCHITECTURES=dav-3510 ..               # 配置工程
  make -j                                                   # 编译样例
  ./histogram 0                                             # 执行Case 0（基线）
  ./histogram 1                                             # 执行Case 1（优化）
  ```

  编译选项说明：

  | 选项 | 可选值 | 说明 |
  |------|--------|------|
  | `CMAKE_ASC_ARCHITECTURES` | `dav-3510` | NPU架构：本样例仅支持dav-3510（Ascend 950PR/Ascend 950DT） |
  | `SKIP_VALIDATION` | `ON`/`OFF` | 是否跳过结果校验，默认`OFF`。使用`msopprof`采集性能时建议设为`ON` |

  执行结果如下，说明精度对比成功。

  ```text
  Case 0 (GM global): 262144 bytes, 64 blocks, 2048 threads per block
  [Success] Case accuracy is verification passed.
  ```

## 性能分析

使用`msOpProf`工具获取详细性能数据：

```bash
msopprof ./histogram 0    # 采集Case 0性能
msopprof ./histogram 1    # 采集Case 1性能
```

> **关于性能采集时的Validation failed：** 样例的直方图结果写入GM，该GM计数器仅在host侧分配时清零一次，kernel仅累加、不在入口清零。`msopprof`的warmup+replay会在同一块GM内存上重复执行kernel，计数器被累加多次，因此严格校验模式下会报`Validation failed`（计数为期望值的整数倍）。该现象是replay机制与校验逻辑的固有冲突。采集性能时建议先以`-DSKIP_VALIDATION=ON`重新编译再执行`msopprof`，跳过校验。

命令完成后，会在默认目录下生成以"OPPROF_{timestamp}_XXX"命名的文件夹，性能数据文件夹结构示例如下：

```text
├──dump                       # 原始的性能数据，用户无需关注
├──ArithmeticUtilization.csv  # cube/vector指令cycle占比
├──L2Cache.csv                # L2 Cache命中率
├──Memory.csv                 # UB，L1和主存储器读写带宽速率
├──MemoryL0.csv               # L0A，L0B，和L0C读写带宽速率
├──MemoryUB.csv               # Vector和Scalar到UB的读写带宽速率
├──OpBasicInfo.csv            # 算子基础信息
├──PipeUtilization.csv        # 采集计算单元和搬运单元耗时和占比
├──ResourceConflictRatio.csv  # UB上的bank group、bank conflict和资源冲突率在所有指令中的占比
└──visualize_data.bin         # MindStudio Insight呈现文件
```
