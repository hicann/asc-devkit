# C语言编程概述

C语言编程是Ascend C SIMD编程路径的底层核心接口，可直接映射NPU硬件指令，支持开发者对向量处理单元、矩阵运算单元、本地存储等硬件资源进行精细化控制。开发者可手动规划本地内存布局、管控数据流转、插入硬件流水同步指令、调整数据搬运与计算的流水并行策略，围绕计算单元、存储层次和流水同步机制来组织代码实现。该编程方式适用于对性能和可控性要求较高的算子开发场景，也是习惯C语言编程、追求极致性能的开发者充分释放NPU硬件潜能的核心技术路径。基于以上定位，下面从核函数、内存架构、同步机制、算子开发四大核心模块，逐一讲解Ascend C语言编程的核心规则与使用方法。

## Kernel函数

[核函数](../核函数.md)在前序章节已作介绍。Ascend C语言编程完全沿用SIMD编程的核函数定义与调用规范，未新增额外语法约束。核函数是运行在Device侧的执行入口，所有内存读写、数据搬运、计算逻辑均在核函数内部实现。下文将首先介绍C语言编程的内存层级，这也是核函数开发的基础。

## 内存层级

AI Core采用分级存储架构，不同计算单元的编程视角有所差异：

**Cube矩阵计算**：采用「Global Memory → L1/L0系列Buffer」两级层级，其中L1 Buffer、L0A/L0B Buffer、L0C Buffer均服务于矩阵计算。

**Vector矢量计算**：传统架构采用「Global Memory → UB」两级层级；[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)引入寄存器后构建「Global Memory → Unified Buffer → Register」三级层级。Global Memory存放输入输出数据，Unified Buffer作为矢量计算数据中间缓存，Register位于Vector计算单元最内层，直接与矢量计算执行单元交互。

<img src="../../../../figures/AI_Core存储层次结构.png" alt="AI Core存储层次结构" width="800" />


### 外部存储（Global Memory）

Global Memory是昇腾NPU的设备内存，位于AI Core外部，容量大、带宽高但访问延迟相对较长。用于存储算子的输入数据、输出结果和中间计算数据。运行在Device侧的核函数可直接访问Global Memory。Global Memory具备数据持久性：通过全局内存分配的空间，其存储的数据会一直保留，直至该内存空间被释放或应用程序终止。用户通过Runtime API完成Device侧全局内存的管理。

结合下述示例说明：Host侧通过aclrtMalloc接口分配Device侧全局内存，通过aclrtMemcpy完成Host与Device全局内存间的数据互相拷贝（Host→Device或Device→Host）；随后通过<<<>>>语法触发Device侧核函数执行运算；此外，通过aclrtMalloc分配的Device全局内存，需调用aclrtFree接口释放。有关Runtime API的更多信息与细节，可以参考[《Runtime运行时API》](https://hiascend.com/document/redirect/CannCommunityRuntimeApi)。

此外，核函数需使用__global__函数前缀进行声明。该前缀用于标识函数在Host侧调用、Device侧执行，是Ascend C核函数的标准定义方式，确保编译器正确识别核函数入口并生成相应的硬件执行代码。完整的调用流程为：Host分配GM内存 → 数据拷贝 → 启动核函数 → 释放内存，示例代码如下：

```c
// Kernel implementation, decorated with __global__ to mark kernel entry point
__global__ __aicore__ void add_custom(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z)
{
    // Kernel internal logic implementation, customized based on operator scenario
}

// Host function: allocate Device memory, copy data via aclrtMemcpy, launch kernel via <<<>>>
std::vector<float> kernel_add(std::vector<float> &x, std::vector<float> &y) {
    // Get host-side buffer pointers
    uint8_t *xHost = reinterpret_cast<uint8_t *>(x.data());
    uint8_t *yHost = reinterpret_cast<uint8_t *>(y.data());
    uint8_t *zHost = nullptr;
    // Allocate Device memory
    aclrtMalloc((void **)&xDevice, totalByteSize, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc((void **)&yDevice, totalByteSize, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc((void **)&zDevice, totalByteSize, ACL_MEM_MALLOC_HUGE_FIRST);
    // Copy data from Host memory to Device memory via aclrtMemcpy
    aclrtMemcpy((uint8_t*)xDevice, totalByteSize, xHost, totalByteSize, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy((uint8_t*)yDevice, totalByteSize, yHost, totalByteSize, ACL_MEMCPY_HOST_TO_DEVICE);
    // Launch kernel via <<<>>>, passing Device memory pointers xDevice, yDevice, zDevice
    add_custom<<<numBlocks, 0, stream>>>(xDevice, yDevice, zDevice);
    aclrtSynchronizeStream(stream);
    // Release Device memory
    aclrtFree(xDevice);
    aclrtFree(yDevice);
    aclrtFree(zDevice);
}
```

### 内部存储（Local Memory）

上文介绍了外部全局内存（Global Memory），接下来说明AI Core内部高速存储。AI Core包含Cube、Vector、Scalar三类计算单元，数据必须从外部存储加载至内部存储后才能执行计算。内部存储按服务对象分为两大体系：服务矢量计算的AIV内部存储、服务矩阵计算的AIC内部存储。

#### AIV内部存储之Unified Buffer

Unified Buffer是AI Core中服务于矢量计算单元的核心内部存储单元，部署在每个Vector Core（AIV）内部。相较于外部存储空间，Unified Buffer容量更小，但具备更高的带宽和更低的访问延迟，可作为内核执行期间由用户自主管理的高速缓存资源。

在C指针编程中，用户可通过数组定义的方式动态或静态申请Unified Buffer。

**静态申请**：分配一段指定大小的内存空间，空间大小在编译时确定，不可动态修改。

```c
__global__ __vector__ void add_custom(...)
{
    constexpr uint32_t ub_size = 256;
    __ubuf__ half static_buffer[ub_size];  // Compiler automatically aligns to 32 bytes
}
```

**动态申请**：除静态分配外，也可在运行阶段动态指定UB大小，即动态申请方式，用户需要通过<<<>>>中参数dynamic_ub_size指定动态内存的空间大小，其大小在运行期确定。

```c
// Device-side: declare dynamic shared memory
__global__ __vector__ void add_custom(...)
{
    extern __ubuf__ char dynamic_buf[];
    // ...
}

// Host-side: specify dynamic shared memory size via third parameter in <<<>>>
int32_t main(int argc, char const *argv[])
{
    // ...
    uint32_t dyn_ubuf_size = 1024 * sizeof(char);
    add_custom<<<block_nums, dyn_ubuf_size, stream>>>(...);
    // ...
}
```

除基础数组定义外，还可通过结构体整合多块内存，简化复杂算子的内存管理：

```c
// Define src0 and src1 memory blocks unified via struct
typedef struct asc_src_buffer_s {
    half src0_buf[128];
    half src1_buf[128];
} asc_src_buffer_t;

__global__ __vector__ void add_custom(...) {
    __ubuf__ asc_src_buffer_t local_buf{};
    __ubuf__ asc_src_buffer_t* src_buffer = (__ubuf__ asc_src_buffer_t*)&local_buf;
    ...
    asc_add(dst, src_buffer.src0_buf, src_buffer.src1_buf, 128);
}
```

> 📌 Unified Buffer的内存地址必须按32字节对齐，目的是匹配硬件总线的数据传输粒度和SIMD计算单元的并行处理能力；若地址非对齐访问，会直接导致程序运行时错误。

#### AIV内部存储之Register File

[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)引入寄存器可编程能力，Vector计算单元的所有计算均需经「GM → UB → Register」三级数据流转，在寄存器上完成矢量计算。C语言编程通过直接定义寄存器变量方式进行寄存器编程。可参考[Reg矢量计算编程](./Reg矢量计算编程.md)中，有关寄存器类型等描述。


#### AIC内部存储L1 Buffer & L0 Buffer

Cube计算单元是AI Core中专用于执行矩阵运算的单元，Cube计算单元直接访问的专用缓存为：L0A Buffer用于存储左矩阵，L0B Buffer用于存储右矩阵，L0C Buffer用于存储初始化累加值和矩阵计算结果。

在C语言编程中，用户可通过数组定义方式申请`L1 Buffer`和`L0 A Buffer`等，不同Cube专用存储对应专属地址限定符，具体说明如下表：

**存储单元及地址限定符说明**：

| 存储单元 | 作用 | 地址限定符 | 示例 |
|---------|------|-----------|------|
| L1 Buffer | 缓存矩阵计算输入数据 | `__cbuf__` | `__cbuf__ half l1_buffer[512];` |
| L0A Buffer | 存储左矩阵 | `__ca__` | `__ca__ half l0a_buffer[256];` |
| L0B Buffer | 存储右矩阵 | `__cb__` | `__cb__ half l0b_buffer[256];` |
| L0C Buffer | 存储计算结果 | `__cc__` | `__cc__ half l0c_buffer[256];` |
| Fixpipe Buffer | 存储量化参数 | `__fbuf__` | `__fbuf__ half fixpipe_buffer[128];` |
| BiasTable Buffer | 存储Bias数据 | `__biasbuf__` | `__biasbuf__ half bias_buffer[64];` |

结合地址限定符，开发者可以直接通过数组定义的方式申请一块内存块，示例如下：

```c
constexpr uint32_t m_size = 256;
constexpr uint32_t n_size = 128;
constexpr uint32_t k_size = 256;

__cbuf__ half l1a_buffer[m_size * k_size];  // L1 Buffer, 32 bytes aligned
__cbuf__ half l1b_buffer[n_size * k_size];
__ca__ half l0a_buffer[m_size * k_size];    // L0A Buffer, 512 bytes aligned
__cb__ half l0b_buffer[n_size * k_size];    // L0B Buffer, 512 bytes aligned
__cc__ half l0c_buffer[m_size * n_size];    // L0C Buffer, 64 bytes aligned
```

同时为优化矩阵乘法等计算密集型任务、提升Cube计算效率，AI Core的L1、L0 Buffer均采用分形内存排布格式（硬件定制化内存布局，适配矩阵计算访存特征）。不同硬件产品的分形格式存在差异，对比如下表：

**表 1** 不同产品型号的分形格式对比

| 存储单元 | [NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md) | [NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md) |
|---------|----------------------------|---------------------------|
| L1 Buffer | Nz | Nz |
| L0A Buffer | Zz | Nz |
| L0B Buffer | Zn | Zn |
| L0C Buffer | Nz | Nz |

以`Nz`为例，内存排布如下图所示，整块内存通过大N外部column major（列主序）+ 小z内部row major（行主序）存放。

![Nz内存排布示意图](../../../../figures/FRACTAL_NZ内存排布示意图.png)

#### Bank冲突

上文介绍了各类内部存储的定义、排布格式。由于AI Core内部存储采用Bank分组架构，不当的内存访问会引发Bank冲突，进而降低算子性能。下面以Unified Buffer为例，介绍Bank冲突的相关原理，在[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)产品上，Unified Buffer总大小为256KB（256 × 1024字节），包含8个bank group，每个bank group包含2个bank。每个bank大小为16KB，由512行组成，每行长度为32B，采用低位地址交织。[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)的UB容量规格请参考相应硬件架构文档。

<img src="../../../../figures/UB_Bank冲突示意图.png" alt="UB Bank冲突示意图" width="800" />

该架构地址使用低位编址，地址排布如下图所示，每个bank可以独立地进行数据的读写操作，允许多个数据请求同时进行。然而，当多个读写操作试图同时访问同一个bank，由于硬件资源的限制，这些操作必须排队等待，会导致bank冲突，引起性能下降。

![低位交织地址排布](../../../../figures/低位交织地址排布.png)

每个bank一拍只能完成一读或者一写，每个bank group最多只允许2读0写或者1读1写。根据内存结构，bank冲突主要可以分为以下三种类型：
- 读写冲突：读操作和写操作同时尝试访问同一个bank，或者两个读操作和一个写操作同时尝试访问同一个bank group。
- 写写冲突：多个写操作同时尝试访问同一个bank group。
- 读读冲突：两个读操作访问同一个bank，或两个以上操作同时尝试访问同一个bank group。

以读写冲突为例，当进行一组`dst = src0 + src1`的矢量加法计算时，如果`src0`的起始地址在0x0000，`dst`的起始地址为0x4000，结合Bank的排布，可以看到当前`src0`和`dst`的地址分别属于同一个bank和同一个bank group，则存在读写冲突。

当遇到Bank冲突的时候，会引起算子性能的下降，通常可以通过优化计算逻辑和优化分配地址方式来避免冲突的产生。详情请参考[避免Bank冲突](../../../../算子实践参考/SIMD算子性能优化/内存访问/避免UB的bank冲突/避免bank冲突（NPU架构版本3510）.md)。

## 同步机制

前文介绍了AI Core的多级存储与计算单元架构。由于DMA搬运、矢量计算、矩阵计算等单元均为异步并行执行，数据读写、计算操作存在强依赖，因此必须依靠同步机制管控执行时序，保证程序逻辑正确。在编程场景中，同步是指协调多个执行单元（如线程、进程等）对共享资源的访问顺序和时机，以保障程序执行结果的正确性。若未通过同步机制严格控制执行顺序，会导致计算结果不一致、程序行为不可预测等问题。

AI Core的同步主要分为**核内同步**和**核间同步**。

### 核内同步

AI Core内部的执行单元（如MTE2数据搬运单元、Vector计算单元等）采用异步并行方式运行，当多个单元读写同一存储资源时，必然存在数据依赖关系。为保障数据一致性和计算结果的正确性，必须通过同步控制来协调各单元的操作时序。

下图示例描述了一个常见的Vector计算数据流：

1. 通过DMA将数据从Global Memory搬入到Local Memory
2. 进行矢量计算
3. 通过DMA将数据从Local Memory搬出到Global Memory

上述不同执行单元对应AI Core内部不同的硬件流水线，各流水线分工明确，具体分类如下表：

<img src="../../../../figures/Vector计算数据流.png" alt="Vector计算数据流" width="800" />

四个执行单元Scalar、Vector、DMA（MTE2）、DMA(MTE3)并行执行，若访问同一片Local Memory，需要同步机制来控制它们的访问时序：保证先搬入Local Memory后再计算，计算完成后再搬出。

<img src="../../../../figures/同步顺序.png" alt="同步顺序示意图" width="800" />

> 📌 提示：上例描述为主要路径，当`Scalar`执行单元读写GM或者UB时，也需要考虑`Scalar`流水与其他流水的同步。

AI Core上的执行单元分别属于不同的执行流水，同步即是保证不同执行流水间能够正确执行，如下为AI Core内的主要执行流水。
| 流水类型 | 含义 |
|---------|------|
| **PIPE_S** | 标量流水线 |
| **PIPE_V** | 矢量计算流水及部分硬件架构下的L0C Buffer→Unified Buffer数据搬运流水 |
| **PIPE_M** | 矩阵计算流水 |
| **PIPE_MTE1** | L1 Buffer→L0A Buffer、L1 Buffer→L0B Buffer数据搬运流水 |
| **PIPE_MTE2** | GM→L1 Buffer、GM→UB等数据搬运流水 |
| **PIPE_MTE3** | UB→GM等数据搬运流水 |
| **PIPE_FIX** | L0C Buffer→GM、L0C Buffer→L1等数据搬运流水 |

为确保流水线间正确执行，AI Core针对不同应用场景提供了多种同步机制，开发者可根据实际需求灵活选择合适的同步方式来实现算子功能。

| 同步类型 | 核心作用及场景说明 | 关键接口 | 实现代码示例 |
|---------|-------------------|---------|-------------|
| 多流水同步（机制一） | 通过"锁定-释放"指定流水的方式，解决核内异步流水间的同步依赖问题；该机制功能类似于传统CPU的锁机制，是[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)新引入的同步方式，相较于notify/wait更易使用。 | `asc_lock` & `asc_unlock` | `uint8_t mutex_id = 1;`<br>`asc_lock(PIPE_MTE2, mutex_id);`<br>`asc_copy_gm2ub((__ubuf__ void*)src0, ...);`<br>`asc_unlock(PIPE_MTE2, mutex_id);`<br>`asc_lock(PIPE_V, mutex_id);`<br>`asc_add(shared_buffer, x_local, y_local);`<br>`asc_unlock(PIPE_V, mutex_id);` |
| 多流水同步（机制二） | 通过notify与wait机制实现两条流水间的同步等待，用于核内异步流水的同步协调。[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)引入mutex机制后，仍保持对该接口的兼容支持，但优先推荐使用mutex机制进行多流水同步。 | `asc_sync_notify` & `asc_sync_wait` | `asc_copy_gm2ub((__ubuf__ void*)src0, ...);`<br>`asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);`<br>`asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);`<br>`asc_add(dst, src1, src0, total_length);` |
| 单流水同步 | 等待同一流水内前序指令完成，用于同一流水内存在前后数据依赖。 | `asc_sync_pipe` & `asc_sync_mte2等` | `asc_add(dst, src1, src0, total_length);`<br>`asc_sync_pipe(PIPE_V);`<br>`asc_sub(dst, dst, src0, total_length);` |
| 内存访问顺序控制 | 等待所有前序内存访问完成，用于存在数据依赖或内存依赖的场景。 | `asc_sync_data_barrier` | `asc_sync_data_barrier(mem_dsb_t::DSB_ALL);` |

以上为单个AI Core内部各流水线、执行单元的同步方案。当业务逻辑需要跨多个AI Core协作时，则需要使用核间同步机制。

### 核间同步

若某个核的计算依赖其他相关核的全部计算结果，则需通过核间同步机制保障多核执行的时序正确性。以下图为例，AIC需要依赖于AIV的ReduceSum计算结果。由于整体矢量较大，必须拆分为多个部分，由每个AIV分别完成部分计算。每个AIV将其部分计算结果通过原子累加写入GM。AIC需要读取的必须是所有AIV均完成累加后的最终结果。

<img src="../../../../figures/核间同步业务场景示例.png" alt="核间同步业务场景示例" width="800" />

从上图可以看到，当前AI Core是由AIC核和AIV核组成，其中AIC核主要用于Cube计算，AIV核负责Vector计算。AIC/AIV按group进行划分。一个group内细分为block（主核）和subblock（从核），二者比例为1:N（N≥1）；其中，block代表主核的数量，subblock代表单个主核关联的从核数量。

<img src="../../../../figures/block和subblock之间关系.png" alt="block和subblock之间关系" width="800" />

算子按计算特征可划分为三类：Cube算子（矩阵计算）、Vector算子（矢量计算）和Mix算子（矩阵与矢量混合计算）。算子类型决定了其核间同步方式与group配置模式的选择。针对不同算子场景，C语言编程提供了相应的编程接口，以满足多样化的算子开发需求。

<table>
  <thead>
    <tr>
      <th>核间同步类型</th>
      <th>场景说明</th>
      <th>主要接口</th>
      <th>实现代码示例</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><strong>所有AIC核同步</strong></td>
      <td><p>同步所有AIC核，通常用于对Cube算子计算结果进行多核累加操作。</p></td>
      <td><code>asc_sync_inter_arrive</code><br><code>asc_sync_inter_wait</code></td>
      <td>
        <code>if ASCEND_IS_AIC {</code><br>
        <code>    int64_t flagID = 1;</code><br>
        <code>    asc_sync_inter_arrive(PIPE_S, flagID);</code><br>
        <code>    asc_sync_inter_wait(PIPE_S, flagID);</code><br>
        <code>}</code>
      </td>
    </tr>
    <tr>
      <td><strong>所有AIV核同步</strong></td>
      <td><p>同步所有AIV核，通常用于对Vector算子计算结果进行多核累加操作。</p></td>
      <td><code>asc_sync_inter_arrive</code><br><code>asc_sync_inter_wait</code></td>
      <td>
        <code>if ASCEND_IS_AIV {</code><br>
        <code>    int64_t flagID = 1;</code><br>
        <code>    asc_sync_inter_arrive(PIPE_S, flagID);</code><br>
        <code>    asc_sync_inter_wait(PIPE_S, flagID);</code><br>
        <code>}</code>
      </td>
    </tr>
    <tr>
      <td><strong>单个AI Core内AIC与AIV同步</strong></td>
      <td><p>同步单个AI Core内AIC与AIV核，通常用于Mix算子中Cube与Vector间的交互操作。</p></td>
      <td><code>asc_sync_intra_arrive</code><br><code>asc_sync_intra_wait</code></td>
      <td>
        <code>if ASCEND_IS_AIC {</code><br>
        <code>    // Atlas A3 training/inference products: AIC sends one arrive, two AIVs wait simultaneously</code><br>
        <code>    asc_sync_intra_arrive(PIPE_FIX, 0);</code><br>
        <code>    // [NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md) products: AIC sends two arrives, two AIVs wait separately</code><br>
        <code>    asc_sync_intra_arrive(PIPE_FIX, 0);</code><br>
        <code>    asc_sync_intra_arrive(PIPE_FIX, 16);</code><br>
        <code>}</code><br>
        <code>if ASCEND_IS_AIV {</code><br>
        <code>    asc_sync_intra_wait(PIPE_MTE3, 0);</code><br>
        <code>}</code>
      </td>
    </tr>
    <tr>
      <td><strong>单个AI Core内两个AIV间同步</strong></td>
      <td><p>同步单个AI Core内两个AIV子核，通常用于Vector算子内AIV核间的协调操作。</p></td>
      <td><code>asc_sync_subblock_arrive</code><br><code>asc_sync_subblock_wait</code></td>
      <td>
        <code>int64_t flag_id = 1;</code><br>
        <code>asc_sync_subblock_arrive(PIPE_S, flag_id);</code><br>
        <code>asc_sync_subblock_wait(PIPE_S, flag_id);</code>
      </td>
    </tr>
  </tbody>
</table>

> 📌 基于分离模式（AIC和AIV分离）开发融合算子时，算子逻辑中通常同时包含AIV和AIC的处理逻辑，此时需要使用Ascend C提供的宏ASCEND_IS_AIV/ASCEND_IS_AIC来对AIV和AIC的代码进行隔离。

## 算子类型

结合前文介绍的AI Core计算单元、多级内存架构与同步机制，基于硬件能力可开发三类算子：矢量算子、矩阵算子、融合算子。前序章节对硬件架构的介绍中已说明：AI Core包含Vector（矢量计算）和Cube（矩阵计算）两类核心计算单元，分别对应矢量计算和矩阵计算场景。Cube计算单元部署于AIC核，Vector计算单元部署于AIV核，每个核均配备独立的Scalar单元，可独立加载各自的代码段。基于AI Core的硬件能力，开发者可实现Vector矢量算子、Cube矩阵算子及Mix CV融合算子三类算子。

### Vector矢量计算

Vector矢量计算主要包含基础算术、归约计算、数据类型转换等矢量计算。

在[NPU架构版本2201](../../../语言扩展层/SIMD-BuiltIn关键字.md)上，Vector矢量计算的数据来自于Unified Buffer（要求32B对齐），C语言编程在Vector计算单元上提供了通过UB指针直接操作计算的接口能力。此类直接操作UB的编程方式称为Memory矢量计算编程。

以Add计算为例，当开发者需要计算4096长度的向量加法，可以通过`asc_add(dst, src0, src1, count)`的接口直接进行计算。更多内容可参考[Memory矢量计算编程](./Memory矢量计算编程.md)中有关Memory矢量计算编程的描述。

```c
// Define Vector operator, marked with __vector__ to execute on Device Vector core, suitable for operators with only Vector computation
__global__ __vector__ void add_kernel(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    __ubuf__ float x[4096];
    __ubuf__ float y[4096];
    __ubuf__ float z[4096];

    // 1. Data transfer in: transfer GM data to UB, this implementation is omitted
    // ...
    // 2. Perform calculation for the first 4096 elements via asc_add
    asc_add(z, x, y, 4096);
}

```

以上为传统架构的矢量计算编程方式；[NPU架构版本3510](../../../语言扩展层/SIMD-BuiltIn关键字.md)新增寄存器能力，编程模式有所变化：在传统UB缓存体系的基础上，开放寄存器（Register）可编程能力，构建出「Global Memory → UB → Register」的三级内存层级。新一代架构下的矢量计算都需要从Unified Buffer加载到Register寄存器后，再利用Register寄存器进行计算操作。可参考[Reg矢量计算编程](./Reg矢量计算编程.md)中有关Reg矢量计算编程的描述。

### Cube矩阵计算

除矢量计算外，Cube单元专门实现矩阵运算，对应矩阵类算子的开发，Cube矩阵计算主要涵盖矩阵乘计算与卷积计算，其数据主要来源于L1 Buffer与L0A/B Buffer。L1 Buffer用于矩阵计算数据缓存，L0A Buffer用于存储左矩阵，L0B Buffer用于存储右矩阵，L0C Buffer用于存储初始累加值及矩阵计算结果。此类基于Cube计算单元直接操作L1/L0A/L0B/L0C Buffer的编程范式称为[Cube矩阵计算编程](./Cube矩阵计算编程.md)。

以Matmul矩阵乘计算为例，开发者通过操作Cube矩阵计算能力可以完成如下图所示的`c = a * b + bias`的能力。
- A、B为源操作数，A为左矩阵，形状为[M, K]；B为右矩阵，形状为[K, N]。
- C为目的操作数，存放矩阵乘结果的矩阵，形状为[M, N]。
- bias为矩阵乘偏置，形状为[1, N]。对A*B结果矩阵的每一行都采用该bias进行偏置。

![矩阵乘示意图](../../../../figures/矩阵乘示意图.png)

```c
// Define Cube matrix operator, marked with __cube__ to execute on Device Cube core, suitable for operators with only Cube computation
__global__ __cube__ void add_kernel(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    __cbuf__ half a_l1_buf[BLOCK_M * BLOCK_K];   // L1 staging area
    __cbuf__ half b_l1_buf[BLOCK_K * BLOCK_N];   // L1 staging area
    __ca__ half l0a_buf[BLOCK_M * BLOCK_K];    // L0A input
    __cb__ half l0b_buf[BLOCK_K * BLOCK_N];    // L0B input
    __cc__ float l0c_buf[BLOCK_M * BLOCK_N];   // L0C output

    // GM2L1 and L12L0 data transfer operations
    // ...
    // Execute Mmad, computation size is (BLOCK_M, BLOCK_K, BLOCK_N)
    asc_mmad(l0c_buf, l0a_buf, l0b_buf, BLOCK_M, BLOCK_K, BLOCK_N, 0, false, false, false);
    // Matrix data transfer out
}
```

### Mix CV融合计算

将矢量计算与矩阵计算融合在同一个算子中，即为CV融合算子，可进一步提升整体性能。开发者可根据业务场景，自由融合Vector、Cube算子以优化性能 —— 融合了Cube计算和Vector计算的算子统称为CV融合算子。例如LLM大模型核心的Flash Attention算子，就是将Matmul（Cube）、Scale（Vector）、Mask（Vector）、SoftMax（Vector）等操作融合为一个算子，核心实现如下：


![Flash Attention核心实现](../../../../figures/Flash-Attention核心实现.png)

```c
// Define CV fusion operator, marked with __mix__ to execute on Device, suitable for operators combining Cube and Vector computation
__global__ __mix__ void add_kernel(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    if ASCEND_IS_AIV {
        // ...
        asc_add(z, x, y, 4096);
        // ...
    }

    if ASCEND_IS_AIC {
        // ...
        asc_mmad(l0c_buf, l0a_buf, l0b_buf, BLOCK_M, BLOCK_K, BLOCK_N, 0, false, false, false);
        // ...
    }
}

```

融合算子可合并多个独立小算子，减少整体计算量与内存占用、优化数据流传输路径、简化代码实现逻辑，最终实现计算效率、硬件资源利用率与代码可维护性的同步提升。

## 总结

本章从技术定位出发，依次讲解了C语言编程的核函数规范、内存层级、内外核同步机制以及三类算子开发范式。作为底层硬件编程接口，该路径可充分挖掘NPU算力。如需深入实践，可参考以下专项文档：[Memory矢量计算编程](./Memory矢量计算编程.md)、[Cube矩阵计算编程](./Cube矩阵计算编程.md)和[Reg矢量计算编程](./Reg矢量计算编程.md)。
