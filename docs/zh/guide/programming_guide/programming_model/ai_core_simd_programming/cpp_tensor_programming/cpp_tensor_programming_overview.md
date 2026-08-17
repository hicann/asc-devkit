# C++ Tensor编程概述

C++ Tensor编程是Ascend C SIMD编程路径的基础编程接口，基于Tensor对象抽象封装内存地址，提供类型安全、编译期检查的高层次编程能力。开发者通过LocalTensor、GlobalTensor、RegTensor等Tensor对象管理内存，无需直接操作指针地址，搬运计算接口直接操作Tensor对象即可完成数据流转。该路径适用于追求开发效率、类型安全、代码可维护性的算子开发场景，是习惯C++面向对象编程的开发者快速上手NPU算子开发的关键技术路径。本章旨在帮助开发者建立对C++ Tensor编程的系统化认知，理解其Tensor抽象编程范式、核心编程模型与开发方法论。

## Kernel函数

[核函数](../kernel_function.md)在前序章节已作介绍，C++ Tensor编程在核函数定义与调用上沿用SIMD编程通用规范，无额外语法约束。

## 内存层级

AI Core采用分级存储架构，Cube矩阵计算采用「Global Memory（以下简称为GM） → L1/L0系列Buffer」两级层级服务于矩阵计算，Vector矢量计算传统架构为「Global Memory → Unified Buffer（以下简称为UB）」两级层级，而[NPU架构版本3510](../../../language_extension/simd_builtin_keywords.md)引入寄存器后构建「GM → UB → Register」三级层级，其中GM存放输入输出数据、UB作为矢量计算中间缓存、Register位于Vector计算单元最内层直接与执行单元交互。

![AI Core存储层次结构](../../../../figures/aicore_mem_2.png)

### Tensor内存抽象

Tensor本质是基于数组的编程抽象，而非简单的内存地址封装，通过数组方式管理内存并执行计算，更匹配SIMD硬件执行模型。其核心思想是将原始内存地址抽象为Tensor对象，实现从“地址操作”到“对象管理”的编程层次提升。

根据芯片分级存储架构，Tensor分为三类：**GlobalTensor**对应GM，存放算子输入/输出数据；**LocalTensor**对应片上内存（UB、L1 Buffer、L0A Buffer/L0B Buffer/L0C Buffer等缓冲区），承载计算过程数据；**RegTensor**对应寄存器，用于寄存器级矢量计算（[NPU架构版本3510](../../../language_extension/simd_builtin_keywords.md)引入）。Tensor体系持续演进：基础Tensor仅携带数据指针和大小信息，需手动传递操作参数；扩展Tensor（Tensor API）引入Layout概念，携带Shape和Stride信息，接口可自动推导搬运长度、计算元素数等参数，大幅降低编程复杂度。该能力引入的接口统一通过`AscendC::Te`命名空间调用（如`AscendC::Te::Mmad`）。

#### 基础Tensor与扩展Tensor抽象定义

- 基础Tensor（不带Layout）仅封装数据指针与大小信息，无法表达数据的物理排布方式。开发者在执行搬运和计算时必须手动传入size/count参数、自行计算偏移与步长，面对高维张量时易出错且代码冗长。

    ```cpp
    __global__ __vector__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z) {
      AscendC::GlobalTensor<float> xGm;
      xGm.SetGlobalBuffer(x + block_idx * blockLength, blockLength);

      AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
      AscendC::LocalTensor<float> xLocal = ubAllocator.Alloc<float, blockLength>();

      // Data copy and computation require manual size parameter
      AscendC::DataCopy(xLocal, xGm, blockLength);
      // yLocal, zLocal的定义、搬运等跟xLocal保持一致，此处省略
      ...
      AscendC::Add(zLocal, xLocal, yLocal, blockLength);
    }
    ```

- 扩展Tensor（Tensor API）封装Shape和Stride信息，接口自动推导操作参数，实现编译期类型安全。开发者无需关注内存布局细节，一行代码即可完成切片、搬运与计算。当前能力赋能于Cube矩阵计算，后续将逐步支持Vector矢量计算。

    ```cpp
    __global__ __cube__ void matmul_custom(__gm__ half* a, __gm__ half* b, __gm__ half* c) {
        // 实现代码示例，其中M, N, K为单核上GM上左右矩阵的大小, (baseM, K), (baseN, K)为搬运到L1上左右矩阵的大小
        // 扩展Tensor（Tensor API）携带Layout信息，搬运接口自动推导参数
        auto gmATensor = AscendC::Te::MakeTensor(AscendC::Te::MakeMemPtr(a), AscendC::Te::MakeFrameLayout<AscendC::Te::NDExtLayoutPtn>(M, K));
        auto gmBTensor = AscendC::Te::MakeTensor(AscendC::Te::MakeMemPtr(b), AscendC::Te::MakeFrameLayout<AscendC::Te::NDExtLayoutPtn>(K, N));
        ...
        // __cbuf__ 为L1 Buffer的声明修饰符
        __cbuf__ half l1ABuf[1024];
        __cbuf__ half l1BBuf[1024];
        auto l1ATensor = AscendC::Te::MakeTensor(AscendC::Te::MakeMemPtr(l1ABuf), AscendC::Te::MakeFrameLayout<AscendC::Te::NZLayoutPtn>(baseM, K));
        auto l1BTensor = AscendC::Te::MakeTensor(AscendC::Te::MakeMemPtr(l1BBuf), AscendC::Te::MakeFrameLayout<AscendC::Te::NZLayoutPtn>(baseN, K));
        ...
        // 一行代码完成搬运，无需手动传入size参数
        AscendC::Te::Copy(l1ATensor, gmATensor);
        AscendC::Te::Copy(l1BTensor, gmBTensor);
    }
    ```

### 外部存储

前文的Tensor抽象示例中已展示了GlobalTensor和LocalTensor的基本用法。下面从硬件存储层级的角度，进一步介绍外部存储和内部存储的特性及Tensor管理方式。
GM是昇腾NPU的设备内存，位于AI Core外部，容量大、带宽高但访问延迟较长。用于存储算子的输入数据、输出结果和中间计算数据。GM具有持久性：其中存储的数据持续保留，直到内存空间被释放或应用程序终止。
用户通过Runtime API管理Device侧全局内存。Host侧使用`aclrtMalloc`分配Device内存，通过`aclrtMemcpy`完成Host与Device间的数据拷贝，通过`<<<>>>`拉起核函数执行，最后使用`aclrtFree`释放内存。有关Runtime API的详细信息，可参考[《Runtime运行时API》](https://hiascend.com/document/redirect/CannCommunityRuntimeApi)。

如下示例展示了在Host侧通过Runtime API管理分配Device内存、在Host与Device之间数据拷贝以及核函数的调用过程：
```cpp
// Kernel function definition (using __global__ prefix to indicate Host call, Device execution)
__global__ __vector__ void add_custom(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z)
{
    AscendC::GlobalTensor<float> xGm;
    xGm.SetGlobalBuffer(x + block_idx * blockLength, blockLength);
    ...
}

// Host side: allocate Device memory, copy data, launch kernel function
std::vector<float> kernel_add(std::vector<float> &x, std::vector<float> &y) {
    uint32_t totalLength = x.size();
    uint32_t byteSize = totalLength * sizeof(float);
    uint8_t *xHost = reinterpret_cast<uint8_t *>(x.data());
    uint8_t *yHost = reinterpret_cast<uint8_t *>(y.data());

    void *xDevice = nullptr;
    void *yDevice = nullptr;
    void *zDevice = nullptr;
    aclrtStream stream = nullptr;

    // Allocate Device memory
    aclrtMalloc(&xDevice, byteSize, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(&yDevice, byteSize, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtMalloc(&zDevice, byteSize, ACL_MEM_MALLOC_HUGE_FIRST);
    aclrtCreateStream(&stream);

    // Host to Device data copy
    aclrtMemcpy(xDevice, byteSize, xHost, byteSize, ACL_MEMCPY_HOST_TO_DEVICE);
    aclrtMemcpy(yDevice, byteSize, yHost, byteSize, ACL_MEMCPY_HOST_TO_DEVICE);

    // Launch kernel function
    uint32_t numBlocks = 1;
    add_custom<<<numBlocks, 0, stream>>>((uint8_t*)xDevice, (uint8_t*)yDevice, (uint8_t*)zDevice);
    aclrtSynchronizeStream(stream);

    // Device to Host data copy
    std::vector<float> z(totalLength);
    aclrtMemcpy(z.data(), byteSize, zDevice, byteSize, ACL_MEMCPY_DEVICE_TO_HOST);

    // Free memory
    aclrtFree(xDevice);
    aclrtFree(yDevice);
    aclrtFree(zDevice);
    aclrtDestroyStream(stream);

    return z;
}
```
GM作为大容量全局内存，用于Host与Device交互及算子输入输出数据存储，但访问延迟较长，无法满足AI Core低延迟计算需求。所有Cube/Vector计算必须将数据预加载至片上内部存储，下面分UB、寄存器、L1/L0缓冲区介绍内部存储Tensor管理方式。

### 内部存储

AI Core包含三类计算单元：Cube负责矩阵计算、Vector负责矢量计算、Scalar负责标量计算。计算前需将数据从外部存储加载到内部存储，不同存储单元服务不同计算单元：UB支撑矢量计算，L1 Buffer和L0A/L0B Buffer支撑矩阵计算。

#### AIV内部存储之UB

UB是AI Core内主要用于矢量计算的内部存储单元，位于每个Vector Core（AIV）内部。容量较小但带宽高、延迟低，可视为内核执行期间由用户管理的高速缓存。

在C++ Tensor编程中，通过LocalTensor对象管理UB内存，其中对于基础Tensor（不带Layout）而言，主要通过`LocalMemAllocator`指定物理存储位置和内存大小来创建对应的Tensor。LocalMemAllocator是一种线性内存分配器，开发者可以调用Alloc方法进行内存分配，根据调用次序依次向后进行线性分配，但该分配器不提供内存释放以及其它内存管理的能力。

```cpp
__global__ __vector__ void add_custom(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z)
{
    // Basic Tensor: allocate through Allocator
    AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    AscendC::LocalTensor<half> ubTensor = ubAllocator.Alloc<half, 256>();
}
```

在初始功能开发时，可以使用LocalMemAllocator简化算子编写。如需更精细的内存控制（如规避Bank冲突、内存复用等），可通过LocalTensor直接指定地址进行管理。使用时需合理规划地址分配，确保不超过物理存储上限，并在保证功能正确的前提下进行内存复用。

```cpp
__global__ __vector__ void add_custom(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z)
{
  // 开发者手动指定x, y, z的地址
  uint64_t xAddrPing = 0;
  uint64_t yAddrPing = 1024;
  uint64_t zAddrPing = 2048;

  // 通过构造函数指定物理存储位置和地址、大小等构造LocalTensor
  AscendC::LocalTensor<float> xLocalPing(AscendC::TPosition::VECCALC, xAddrPing, 256);
  AscendC::LocalTensor<float> yLocalPing(AscendC::TPosition::VECCALC, yAddrPing, 256);
  AscendC::LocalTensor<float> zLocalPing(AscendC::TPosition::VECCALC, zAddrPing, 256);
}
```
> 📌 其中AscendC::TPosition::VECCALC为对物理位置的逻辑抽象定义，与物理位置的映射参考文件[逻辑位置和物理存储的映射](../../../../../api/SIMD-API/general_description_and_constraints.md)。

相比基础Tensor，[NPU架构版本3510](../../../language_extension/simd_builtin_keywords.md)引入Layout描述并将其作为Tensor的核心属性，开发者可在Tensor创建时直接关联Shape与Stride信息，简化多维数据的布局管理。通过`MakeTensor`，开发者可灵活指定内存地址和Layout布局，更便捷地描述ND、NZ等典型数据排布。

```cpp
// UB buffer declaration
__ubuf__ half ubBuffer[1024];
auto ubPointer = AscendC::Te::MakeMemPtr(ubBuffer);
// Extended Tensor: create through MakeTensor
auto ubTensor = AscendC::Te::MakeTensor(ubPointer, AscendC::Te::MakeLayout(AscendC::Te::MakeShape(128, 64)));
```
> 📌 UB地址必须32字节对齐，以匹配硬件总线粒度和SIMD计算单元并行度，非对齐访问将导致运行时错误。

#### AIV内部存储之Register File

[NPU架构版本3510](../../../language_extension/simd_builtin_keywords.md)引入寄存器可编程能力，Vector计算单元的所有计算均需经「GM → UB → Register」三级数据流转，在寄存器上完成矢量计算。C++ Tensor编程通过定义RegTensor方式进行寄存器编程。详见[Reg矢量计算编程](./reg_vector_computation.md)。

> 📌 寄存器可编程能力从[NPU架构版本3510](../../../language_extension/simd_builtin_keywords.md)引入。

#### AIC内部存储之L1 Buffer & L0 Buffer

Cube计算单元专用的缓存为：L0A Buffer存储左矩阵，L0B Buffer存储右矩阵，L0C Buffer存储初始累加值及矩阵计算结果。L1 Buffer用于缓存矩阵计算输入数据。

通过`LocalTensor`对象管理这些存储单元：

| 存储单元 | 作用 | 示例 |
|---------|------|------|
| L1 Buffer | 缓存矩阵计算输入数据 | `LocalTensor<half> l1_buffer;` |
| L0A Buffer | 存储左矩阵 | `LocalTensor<half> l0a_buffer;` |
| L0B Buffer | 存储右矩阵 | `LocalTensor<half> l0b_buffer;` |
| L0C Buffer | 存储计算结果 | `LocalTensor<float> l0c_buffer;` |
| Fixpipe Buffer | 存储量化参数 | `LocalTensor<half> fixpipe_buffer;` |
| BiasTable Buffer | 存储Bias数据 | `LocalTensor<half> bias_buffer;` |

同UB的申请方式，开发者可通过LocalMemAllocator或直接构造LocalTensor来申请L1/L0 Buffer内存。为提升Cube计算效率，L1 Buffer和L0 Buffer采用分形格式。不同产品型号的分形格式存在差异：

| 存储单元 | [NPU架构版本2201](../../../language_extension/simd_builtin_keywords.md) | [NPU架构版本3510](../../../language_extension/simd_builtin_keywords.md) |
|---------|----------------------------|---------------------------|
| L1 Buffer | Nz | Nz |
| L0A Buffer | Zz | Nz |
| L0B Buffer | Zn | Zn |
| L0C Buffer | Nz | Nz |

以`FRACTAL_Nz`为例，内存排布采用大N外部column major（列主序）+ 小z内部row major（行主序）存放：

![FRACTAL_Nz内存排布示意图](../../../../figures/nz_layout.png)

#### Bank冲突

为了提高数据访问的效率和吞吐量，内部存储采用了bank（大小相等的内存模块）结构设计。以UB为例，[NPU架构版本3510](../../../language_extension/simd_builtin_keywords.md)产品上，UB总大小为256KB（256 × 1024字节），包含8个bank group，每个bank group包含2个bank。每个bank大小为16KB，由512行组成，每行长度为32B，采用低位地址交织。[NPU架构版本2201](../../../language_extension/simd_builtin_keywords.md)的UB容量规格请参考[UB bank结构](../../../../算子实践参考/SIMD算子性能优化/内存访问/避免UB的bank冲突/avoid_bank_conflict_npu_arch_2201.md)。

![UB Bank冲突示意图](../../../../figures/ub_conflict.png)

该架构地址使用低位编址，地址排布如下图所示，每个bank可以独立地进行数据的读写操作，允许多个数据请求同时进行。然而，当多个读写操作试图同时访问同一个bank时，由于硬件资源的限制，这些操作必须排队等待，会导致bank冲突，引起性能下降。

![低位交织地址排布](../../../../figures/low_interleave.png)

每个bank一拍只能完成一读或者一写，每个bank group最多只允许2读0写或者1读1写。根据内存结构，bank冲突主要可以分为以下三种类型：
- 读写冲突：读操作和写操作同时尝试访问同一个bank，或者两个读操作和一个写操作同时尝试访问同一个bank group。
- 写写冲突：多个写操作同时尝试访问同一个bank group。
- 读读冲突：两个读操作访问同一个bank，或两个以上操作同时尝试访问同一个bank group。

以读写冲突为例，当进行一组`dst = src0 + src1`的矢量加法计算时，如果`src0`的起始地址在0x0000，`dst`的起始地址为0x4000，结合Bank的排布，可以看到当前`src0`和`dst`的地址分别属于同一个bank和同一个bank group，则存在读写冲突。

当遇到Bank冲突时，会引起算子性能的下降，通常可以通过优化计算逻辑和优化分配地址方式来避免冲突的产生，详情请参考[避免Bank冲突](../../../../算子实践参考/SIMD算子性能优化/内存访问/避免UB的bank冲突/avoid_bank_conflict_npu_arch_3510.md)。

合理规划内存地址可规避Bank冲突、提升访存吞吐，但AI Core内DMA、Vector、Cube多流水异步执行，数据读写存在强依赖。需要配套同步机制管控多流水线执行时序，避免脏读、计算错乱。

## 同步机制

前文介绍了AI Core的多级存储与计算单元架构。由于DMA搬运、矢量计算、矩阵计算等单元均为异步并行执行，数据读写、计算操作存在强依赖，因此必须依靠同步机制管控执行时序，保证程序逻辑正确。在编程场景中，同步是指协调多个执行单元（如线程、进程等）对共享资源的访问顺序和时机，以保障程序执行结果的正确性。若未通过同步机制严格控制执行顺序，会导致计算结果不一致、程序行为不可预测等问题。

AI Core的同步主要分为**核内同步**和**核间同步**。

### 核内同步

AI Core内部的执行单元（如MTE2搬运单元、Vector计算单元等）以异步并行的方式运行，在读写同一存储资源时可能存在数据依赖关系。为确保数据一致性及计算正确性，需通过同步控制协调操作时序。

下图示例描述了一个常见的Vector计算数据流：

1. 通过DMA将数据从GM搬入到Local Memory
2. 进行矢量计算
3. 通过DMA将数据从Local Memory搬出到GM

![Vector计算数据流](../../../../figures/vec_flow.png)

四个执行单元Scalar、Vector、DMA（MTE2）、DMA(MTE3)并行执行，若访问同一片Local Memory，需要同步机制来控制它们的访问时序：保证先搬入Local Memory后再计算，计算完成后再搬出。

![同步顺序示意图](../../../../figures/sync_order.png)

> 📌 提示：上例描述的是主要路径，当`Scalar`执行单元读写GM或者UB时，也需要考虑`Scalar`流水与其他流水的同步。

AI Core上的执行单元分别属于不同的执行流水，同步即是保证不同执行流水间能够正确执行，如下为AI Core内的主要执行流水。

| 流水类型 | 含义 |
|---------|------|
| **PIPE_S** | 标量流水线 |
| **PIPE_V** | 矢量计算流水及部分硬件架构下的L0C Buffer→UB数据搬运流水 |
| **PIPE_M** | 矩阵计算流水 |
| **PIPE_MTE1** | L1 Buffer→L0A Buffer、L1 Buffer→L0B Buffer数据搬运流水 |
| **PIPE_MTE2** | GM→L1 Buffer、GM→UB等数据搬运流水 |
| **PIPE_MTE3** | UB→GM等数据搬运流水 |
| **PIPE_FIX** | L0C Buffer→GM、L0C Buffer→L1等数据搬运流水 |

为确保流水线间正确执行，AI Core针对不同应用场景提供了多种同步机制，开发者可根据实际需求灵活选择合适的同步方式来实现算子功能。

| 同步类型 | 核心作用及场景说明 | 关键接口 | 实现代码示例 |
|---------|-------------------|----------|-------------|
| **多流水同步（机制一）** | Mutex互斥锁用于核内异步流水间的同步，功能类似传统CPU锁机制。[NPU架构版本3510](../../../language_extension/simd_builtin_keywords.md)新引入，比SetFlag/WaitFlag更简便。 | `Mutex::Lock` & `Mutex::Unlock` | `AscendC::Mutex::Lock<PIPE_MTE2>(mutexId);`<br>`AscendC::DataCopy(srcLocal, srcGlobal, dataSize);`<br>`AscendC::Mutex::Unlock<PIPE_MTE2>(mutexId);`<br>`AscendC::Mutex::Lock<PIPE_V>(mutexId);`<br>`AscendC::Add(dstLocal, src0Local, src1Local, dataSize);`<br>`AscendC::Mutex::Unlock<PIPE_V>(mutexId);` |
| **多流水同步（机制二）** | SetFlag与WaitFlag实现两条流水间同步等待。引入Mutex后仍保持兼容，但优先推荐Mutex机制。 | `SetFlag` & `WaitFlag` | `AscendC::DataCopy(srcLocal, srcGlobal, dataSize);`<br>`AscendC::SetFlag<HardEvent::MTE2_V>(eventId);`<br>`AscendC::WaitFlag<HardEvent::MTE2_V>(eventId);`<br>`AscendC::Add(dstLocal, src0Local, src1Local, count);` |
| **单流水同步** | PipeBarrier等待同一流水内前序指令完成，用于同一流水内的前后数据依赖。 | `PipeBarrier` | `AscendC::Add(dstLocal, src0Local, src1Local, count);`<br>`AscendC::PipeBarrier<PIPE_V>();`<br>`AscendC::Sub(dstLocal, dstLocal, src0Local, count);` |
| **内存访问顺序控制** | 等待所有前序内存访问完成，用于存在数据依赖或内存依赖的场景。 | `DataSyncBarrier` | `AscendC::DataSyncBarrier<AscendC::MemDsbT::DDR>();` |

### 核间同步

当一个核计算所依赖的数据，需要等待其他相关核的计算全部完成后才能执行时，则需要通过核间同步来确保多核间的时序正确性。

以下图为例，AIC（AI Cube，主要用于Cube计算）需要依赖于AIV（AI Vector，负责Vector计算）的ReduceSum计算结果。由于整体矢量较大，必须拆分为多个部分，由每个AIV分别完成部分计算。每个AIV将其部分计算结果通过原子累加写入GM。AIC需要读取的必须是所有AIV均完成累加后的最终结果。

![核间同步业务场景示例](../../../../figures/inter_core_sync_ex.png)

从上图可以看到，当前AI Core是由AIC核和AIV核组成，其中AIC核主要用于Cube计算，AIV核负责Vector计算。AIC/AIV按group进行划分。一个group内细分block及subblock，block与subblock比例为1:N（N>=1），block表示多少个“主核”，subblock表示一个“主核”带多少个“从核”。

![block和subblock之间关系](../../../../figures/block_rel.png)

算子按计算特征可划分为三类：Cube算子（矩阵计算）、Vector算子（矢量计算）和CV融合算子（矩阵与矢量混合计算）。算子类型决定了其核间同步方式与group配置模式的选择。针对不同算子场景，C++ Tensor编程通过[CrossCoreSetFlag](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md)和[CrossCoreWaitFlag](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md)两个接口组合实现核间同步，以满足多样化的算子开发需求。

| 核间同步类型 | 场景说明 | 实现代码示例 |
|---|---|---|
| **所有AIC核同步** | 同步所有AIC核，通常用于对Cube算子计算结果进行多核累加操作。 | `if ASCEND_IS_AIC {`<br>`    AscendC::CrossCoreSetFlag<0x0, PIPE_FIX>(flagID);`<br>`    AscendC::CrossCoreWaitFlag(flagID);`<br>`}` |
| **所有AIV核同步** | 同步所有AIV核，通常用于对Vector算子计算结果进行多核累加操作。 | `if ASCEND_IS_AIV {`<br>`    AscendC::CrossCoreSetFlag<0x0, PIPE_MTE3>(flagID);`<br>`    AscendC::CrossCoreWaitFlag(flagID);`<br>`}` |
| **单个AI Core内两个AIV子核之间同步** | 同步单个AI Core内两个AIV子核，通常用于Vector算子内需要分组进行同步的场景。 | `if ASCEND_IS_AIV {`<br>`    AscendC::CrossCoreSetFlag<0x1, PIPE_MTE3>(flagID);`<br>`    AscendC::CrossCoreWaitFlag(flagID);`<br>`}` |
| **单个AI Core内AIC与AIV同步** | 同步单个AI Core内AIC与AIV核，通常用于CV融合算子中Cube与Vector间的交互操作。AIC触发两个AIV同时等待。 | `if ASCEND_IS_AIC {`<br>`    AscendC::CrossCoreSetFlag<0x2, PIPE_FIX>(0);`<br>`}`<br>`if ASCEND_IS_AIV {`<br>`    AscendC::CrossCoreWaitFlag(0);`<br>`}` |
| **单个AI Core内AIC与AIV同步（[NPU架构版本3510](../../../language_extension/simd_builtin_keywords.md)新引入）** | 同步单个AI Core内AIC与AIV核，通常用于CV融合算子中Cube与Vector间的交互操作。AIV0与AIV1可单独触发AIC等待。 | `if ASCEND_IS_AIC {`<br>`    AscendC::CrossCoreSetFlag<0x4, PIPE_FIX>(0);`<br>`    AscendC::CrossCoreSetFlag<0x4, PIPE_FIX>(16);`<br>`}`<br>`if ASCEND_IS_AIV {`<br>`    AscendC::CrossCoreWaitFlag(0);`<br>`}` |

> 📌 基于分离模式（AIC和AIV分离）开发融合算子时，算子逻辑中通常同时包含AIV和AIC的处理逻辑，此时需要使用Ascend C提供的宏ASCEND_IS_AIV/ASCEND_IS_AIC来对AIV和AIC的代码进行隔离。
>
> 上表中`CrossCoreSetFlag`的第一个模板参数为同步目标标识，不同取值对应不同的同步范围：`0x0`表示所有核同步，`0x1`表示单个AI Core内AIV子核间同步，`0x2`表示单个AI Core内AIC与AIV同步（两个AIV同时触发），`0x4`表示单个AI Core内AIC与AIV同步（AIV0与AIV1可单独触发，[NPU架构版本3510](../../../language_extension/simd_builtin_keywords.md)新引入）。

## 算子类型

结合前文介绍的AI Core计算单元、多级内存架构与同步机制，基于硬件能力可开发三类算子：矢量算子、矩阵算子、融合算子。前序章节对硬件架构的介绍中已说明：AI Core包含Vector（矢量计算）和Cube（矩阵计算）两类核心计算单元，Cube计算单元部署于AIC核，Vector计算单元部署于AIV核，每个核均配备独立的Scalar单元，可独立加载各自的代码段。基于此，开发者可实现Vector矢量算子、Cube矩阵算子及CV融合算子。

### Vector矢量计算

Vector矢量计算主要包含基础算术、归约计算、数据类型转换等矢量计算。

在[NPU架构版本2201](../../../language_extension/simd_builtin_keywords.md)上，Vector矢量计算的数据来自于UB（要求32B对齐），C++ Tensor编程在Vector计算单元上提供了通过LocalTensor直接操作计算的接口能力。此类直接操作UB的编程方式称为Memory矢量计算编程。

以Add计算为例，当开发者需要计算4096长度的向量加法，可通过`Add(dstTensor, src0Tensor, src1Tensor, count)`接口直接进行计算。更多内容请参考[Memory矢量计算编程](./memory_vector_computation.md)。

```cpp
// Define Vector operator, marked with __vector__ to execute on Device, suitable for operators with only Vector computation
__global__ __vector__ void add_kernel(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    AscendC::LocalTensor<float> xLocal = ubAllocator.Alloc<float, 4096>();
    AscendC::LocalTensor<float> yLocal = ubAllocator.Alloc<float, 4096>();
    AscendC::LocalTensor<float> zLocal = ubAllocator.Alloc<float, 4096>();

    // 1. Data transfer in: transfer GM data to UB, this implementation is omitted
    // ...
    // 2. Perform calculation for the first 4096 elements via Add interface
    AscendC::Add(zLocal, xLocal, yLocal, 4096);
}
```

[NPU架构版本3510](../../../language_extension/simd_builtin_keywords.md)新一代架构，引入寄存器可编程能力，在传统UB缓存体系的基础上，开放寄存器（Register）可编程能力，构建出「GM → UB → Register」的三级内存层级。新一代架构下的矢量计算都需要从UB加载到Register寄存器后，再利用Register寄存器进行计算操作。可参考[Reg矢量计算编程](./reg_vector_computation.md)中有关Reg矢量计算编程的描述。

> 📌 寄存器可编程能力从[NPU架构版本3510](../../../language_extension/simd_builtin_keywords.md)引入。

### Cube矩阵计算

Cube矩阵计算主要涵盖矩阵乘计算与卷积计算，其数据主要来源于L1 Buffer与L0A/B Buffer。L1 Buffer用于矩阵计算数据缓存，L0A Buffer用于存储左矩阵，L0B Buffer用于存储右矩阵，L0C Buffer用于存储初始累加值及矩阵计算结果。此类基于Cube计算单元直接操作L1/L0A/L0B/L0C Buffer的编程范式称为[Cube矩阵计算编程](./cube_matrix_computation.md)。

以Matmul矩阵乘计算为例，开发者通过操作Cube矩阵计算能力可以完成如下图所示的c = a * b + bias的计算。
- A、B为源操作数，A为左矩阵，形状为[M, K]；B为右矩阵，形状为[K, N]。
- C为目的操作数，存放矩阵乘结果的矩阵，形状为[M, N]。
- bias为矩阵乘偏置，形状为[1, N]。对A*B结果矩阵的每一行都采用该bias进行偏置。

![矩阵乘示意图](../../../../figures/matmul.png)

```cpp
// Define Cube matrix operator, marked with __cube__ to execute on Device Cube core, suitable for operators with only Cube computation
__global__ __cube__ void matmul_kernel(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    AscendC::LocalMemAllocator<AscendC::Hardware::L1> l1Allocator;
    AscendC::LocalMemAllocator<AscendC::Hardware::L0A> l0aAllocator;
    AscendC::LocalMemAllocator<AscendC::Hardware::L0B> l0bAllocator;
    AscendC::LocalMemAllocator<AscendC::Hardware::L0C> l0cAllocator;
    
    AscendC::LocalTensor<half> a_l1_buf = l1Allocator.Alloc<half, BLOCK_M * BLOCK_K>();
    AscendC::LocalTensor<half> b_l1_buf = l1Allocator.Alloc<half, BLOCK_K * BLOCK_N>();
    AscendC::LocalTensor<half> l0a_buf = l0aAllocator.Alloc<half, BLOCK_M * BLOCK_K>();
    AscendC::LocalTensor<half> l0b_buf = l0bAllocator.Alloc<half, BLOCK_K * BLOCK_N>();
    AscendC::LocalTensor<float> l0c_buf = l0cAllocator.Alloc<float, BLOCK_M * BLOCK_N>();

    // GM2L1 and L12L0 data transfer operations
    // ...
    // Execute Mmad, computation size is (BLOCK_M, BLOCK_K, BLOCK_N)
    AscendC::Mmad(l0c_buf, l0a_buf, l0b_buf, {BLOCK_M, BLOCK_K, BLOCK_N, 0, false, false});
    // Matrix data transfer out
}
```

### CV融合计算

CV融合计算程序由多个独立的小算子融合而成，其功能与多个小算子的功能等价，性能方面通常优于独立的小算子。用户可以根据实际业务场景诉求，按照具体算法自由融合向量（Vector）、矩阵（Cube）算子以达到性能上的收益。融合了Cube计算、Vector计算的算子统称为CV融合算子。

比如对于LLM大模型中最核心的一个融合算子Flash Attention，其核心实现如下图。图中的Matmul算子（Cube）、Scale算子（Vector）、Mask算子（Vector）、SoftMax算子（Vector）融合为一个大的算子Flash Attention。

![Flash Attention核心实现](../../../../figures/flash_attn.png)

```cpp
// Define CV fusion operator, marked with __mix__ to execute on Device, suitable for operators combining Cube and Vector computation
__global__ __mix__ void mix_kernel(__gm__ float* x, __gm__ float* y, __gm__ float* z)
{
    if ASCEND_IS_AIV {
        // ...
        AscendC::Add(zLocal, xLocal, yLocal, 4096);
        // ...
    }

    if ASCEND_IS_AIC {
        // ...
        AscendC::Mmad(l0c_buf, l0a_buf, l0b_buf, {BLOCK_M, BLOCK_K, BLOCK_N, 0, false, false});
        // ...
    }
}
```

融合算子在提升性能与充分发挥算力的基础上，通过合并算子减少计算量与内存占用、优化数据流传输、简化代码实现，实现计算效率、资源利用率与代码质量的同步提升。

## 编程约束和限制

- 同步事件由开发者使用`SetFlag/WaitFlag(ISASI)`和`PipeBarrier(ISASI)`手动插入，事件的类型和事件ID由开发者自行管理，但需要注意事件ID不能使用6和7（可能与内部使用的事件ID出现冲突，进而出现未定义行为）。
- Kernel入口处需要开发者手动调用[InitSocState](../../../../../api/SIMD-API/basic_api/tool_interface/system_init/InitSocState.md)接口用来初始化全局状态寄存器。因为全局状态寄存器处于不确定状态，如果不调用该接口，可能导致算子执行过程中出现未定义行为。在TPipe框架编程中，初始化过程由TPipe完成，无需开发者关注。
- Kernel结束前需要开发者手动调用[PipeBarrier<PIPE_ALL>()](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/PipeBarrier_ISASI.md)。
- 部分API内部通过软仿真实现，需占用Ascend C预留的UB空间，开发者使用时需关注目标API是否依赖该预留空间（针对[NPU架构版本2201](../../../language_extension/simd_builtin_keywords.md)，Select等API内部使用了8KB的预留UB空间，用于存储中间数据；针对[NPU架构版本3510](../../../language_extension/simd_builtin_keywords.md)，Exp等API内部使用了2KB的预留UB空间，用于指令兼容或存储中间数据）。若开启--cce-disable-asc-reserved-ubuf编译选项，在对应产品版本下调用相关API会触发编译期报错。具体API范围见[使用预留UB空间的API列表](#section_reserved_ubuf_api)。
- 同时另外一些软仿实现接口内部会占用系统分配的事件ID，如果开发者手动调用`SetFlag/WaitFlag(ISASI)`插入自定义事件ID时会跟接口内部的事件ID产生冲突，所以用户自定义事件ID时，只能支持部分API。具体支持的API列表见[支持的API范围](#section2633193623711)。

## 支持的API范围<a name="section2633193623711"></a>

<!-- npu="310p" id1 -->
**表1**  针对Atlas 推理系列产品AI Core，支持的API范围

<a name="table56285428438"></a>
<table><thead align="left"><tr id="row46697425436"><th class="cellrowborder" valign="top" width="30.28%" id="mcps1.2.3.1.1"><p id="p866911429430"><a name="p866911429430"></a><a name="p866911429430"></a>接口分类</p>
</th>
<th class="cellrowborder" valign="top" width="69.72%" id="mcps1.2.3.1.2"><p id="p186691442174319"><a name="p186691442174319"></a><a name="p186691442174319"></a>接口名称</p>
</th>
</tr>
</thead>
<tbody><tr id="row146691342124318"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p1669154234318"><a name="p1669154234318"></a><a name="p1669154234318"></a>基础API &gt; 标量计算</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p0669164213434"><a name="p0669164213434"></a><a name="p0669164213434"></a>ScalarGetCountOfValue、ScalarCountLeadingZero、ScalarCast、CountBitsCntSameAsSignBit、ScalarGetSFFValue</p>
</td>
</tr>
<tr id="row1966914211437"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p7669174224314"><a name="p7669174224314"></a><a name="p7669174224314"></a>基础API &gt; 矢量计算 &gt; 基础算术</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p3669184224317"><a name="p3669184224317"></a><a name="p3669184224317"></a>Exp、Ln、Abs、Reciprocal、Sqrt、Rsqrt、Relu、VectorPadding、Add、Sub、Mul、Div、Max、Min、BilinearInterpolation、Adds、Muls、Maxs、Mins、LeakyRelu</p>
</td>
</tr>
<tr id="row666974224312"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p2066924274314"><a name="p2066924274314"></a><a name="p2066924274314"></a>基础API &gt; 矢量计算 &gt; 逻辑计算</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p1166964217431"><a name="p1166964217431"></a><a name="p1166964217431"></a>Not、And、Or</p>
</td>
</tr>
<tr id="row46691542124317"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p766994224314"><a name="p766994224314"></a><a name="p766994224314"></a>基础API &gt; 矢量计算 &gt; 复合计算</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p1559917570124"><a name="p1559917570124"></a><a name="p1559917570124"></a>Axpy、CastDequant、AddRelu、AddReluCast、AddDeqRelu、SubRelu、SubReluCast、MulAddDst、MulCast、FusedMulAdd、FusedMulAddRelu</p>
</td>
</tr>
<tr id="row176691942154313"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p126691442184311"><a name="p126691442184311"></a><a name="p126691442184311"></a>基础API &gt; 矢量计算 &gt; 比较与选择</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p1670442104314"><a name="p1670442104314"></a><a name="p1670442104314"></a>Compare、Compare（结果存入寄存器）、CompareScalar、GetCmpMask、SetCmpMask、</p>
<p id="p367013429431"><a name="p367013429431"></a><a name="p367013429431"></a>Select、GatherMask</p>
</td>
</tr>
<tr id="row36706425430"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p8670124211438"><a name="p8670124211438"></a><a name="p8670124211438"></a>基础API &gt; 矢量计算 &gt; 类型转换</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p166709429433"><a name="p166709429433"></a><a name="p166709429433"></a>Cast</p>
</td>
</tr>
<tr id="row267074213433"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p1167094274316"><a name="p1167094274316"></a><a name="p1167094274316"></a>基础API &gt; 矢量计算 &gt; 归约计算</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p156703427434"><a name="p156703427434"></a><a name="p156703427434"></a>WholeReduceMax、WholeReduceMin、WholeReduceSum、BlockReduceMax、BlockReduceMin、BlockReduceSum、PairReduceSum、RepeatReduceSum、GetReduceMaxMinCount</p>
</td>
</tr>
<tr id="row067015420439"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p667094294319"><a name="p667094294319"></a><a name="p667094294319"></a>基础API &gt; 矢量计算 &gt; 数据转换</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p166701427437"><a name="p166701427437"></a><a name="p166701427437"></a>Transpose、TransDataTo5HD</p>
</td>
</tr>
<tr id="row66709428433"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p7670842124315"><a name="p7670842124315"></a><a name="p7670842124315"></a>基础API &gt; 矢量计算 &gt; 数据填充</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p1867064211432"><a name="p1867064211432"></a><a name="p1867064211432"></a>Duplicate</p>
</td>
</tr>
<tr id="row11670174211438"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p767044244315"><a name="p767044244315"></a><a name="p767044244315"></a>基础API &gt; 矢量计算 &gt; 排序组合</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p196701142204317"><a name="p196701142204317"></a><a name="p196701142204317"></a>ProposalConcat、ProposalExtract、RpSort16、MrgSort4、GetMrgSortResult</p>
</td>
</tr>
<tr id="row86708421431"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p1367054274312"><a name="p1367054274312"></a><a name="p1367054274312"></a>基础API &gt; 矢量计算 &gt; 离散与聚合</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p6670442114317"><a name="p6670442114317"></a><a name="p6670442114317"></a>Gather、Scatter</p>
</td>
</tr>
<tr id="row467024220436"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p206701242204318"><a name="p206701242204318"></a><a name="p206701242204318"></a>基础API &gt; 矢量计算 &gt; 掩码操作</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p1670134217439"><a name="p1670134217439"></a><a name="p1670134217439"></a>SetMaskCount、SetMaskNorm、SetVectorMask、ResetMask</p>
</td>
</tr>
<tr id="row11670142144320"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p1670174218434"><a name="p1670174218434"></a><a name="p1670174218434"></a>基础API &gt; 矢量计算 &gt; 量化设置</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p14670144210437"><a name="p14670144210437"></a><a name="p14670144210437"></a>SetDeqScale</p>
</td>
</tr>
<tr id="row8670194214432"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p20670174219436"><a name="p20670174219436"></a><a name="p20670174219436"></a>基础API &gt; 数据搬运 &gt; DataCopy</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p1567014216432"><a name="p1567014216432"></a><a name="p1567014216432"></a>基础数据搬运</p>
</td>
</tr>
<tr id="row166701142174317"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p126701142174316"><a name="p126701142174316"></a><a name="p126701142174316"></a>基础API &gt; 同步控制 &gt; 核内同步</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p167024224318"><a name="p167024224318"></a><a name="p167024224318"></a>SetFlag/WaitFlag、PipeBarrier</p>
</td>
</tr>
<tr id="row166719424430"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p1067184244310"><a name="p1067184244310"></a><a name="p1067184244310"></a>基础API &gt; 缓存控制</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p20671042134320"><a name="p20671042134320"></a><a name="p20671042134320"></a>DataCachePreload、DataCacheCleanAndInvalid、ICachePreLoad</p>
</td>
</tr>
<tr id="row12671124274312"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p1867119427433"><a name="p1867119427433"></a><a name="p1867119427433"></a>基础API &gt; 系统变量访问</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p06713423434"><a name="p06713423434"></a><a name="p06713423434"></a>GetBlockNum、GetBlockIdx、GetDataBlockSizeInBytes、GetArchVersion、GetTaskRatio、InitSocState、GetProgramCounter、CheckLocalMemoryIA</p>
</td>
</tr>
<tr id="row96714427439"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p106711742154319"><a name="p106711742154319"></a><a name="p106711742154319"></a>基础API &gt; 原子操作</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p17671042164319"><a name="p17671042164319"></a><a name="p17671042164319"></a>SetAtomicAdd、DisableDmaAtomic</p>
</td>
</tr>
<tr id="row13671194254311"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p6671184224317"><a name="p6671184224317"></a><a name="p6671184224317"></a>基础API &gt; 矩阵计算</p>
</td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p147142311613"><a name="p147142311613"></a><a name="p147142311613"></a>InitConstValue、LoadData、SetAippFunctions、LoadImageToLocal、LoadUnzipIndex、LoadDataUnzip、SetLoadDataBoundary、SetLoadDataPaddingValue、Mmad</p>
</td>
</tr>
</tbody>
</table>
<!-- end id1 -->

<!-- npu="910b" id2 -->
**表2**  针对Atlas A2 训练系列产品/Atlas A2 推理系列产品，支持的API范围

<a name="table1842818536431"></a>
<table><thead align="left"><tr id="row104742531436"><th class="cellrowborder" valign="top" width="29.9%" id="mcps1.2.4.1.1"><p id="p10474205364319"><a name="p10474205364319"></a><a name="p10474205364319"></a>接口分类</p>
</th>
<th class="cellrowborder" valign="top" width="44.25%" id="mcps1.2.4.1.2"><p id="p1047418533430"><a name="p1047418533430"></a><a name="p1047418533430"></a>接口名称</p>
</th>
<th class="cellrowborder" valign="top" width="25.85%" id="mcps1.2.4.1.3"><p id="p4474125374319"><a name="p4474125374319"></a><a name="p4474125374319"></a>备注</p>
</th>
</tr>
</thead>
<tbody><tr id="row747410531434"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p12474153154311"><a name="p12474153154311"></a><a name="p12474153154311"></a>基础API &gt; 标量计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p5821115016275"><a name="p5821115016275"></a><a name="p5821115016275"></a>GetBitCount、CountLeadingZero、CountBitsCntSameAsSignBit、GetSFFValue、Cast（float转half、int32_t）、Cast（float转bfloat16_t）、Cast（多类型转float）</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p10474175318436"><a name="p10474175318436"></a><a name="p10474175318436"></a>-</p>
</td>
</tr>
<tr id="row64759532431"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p747511535436"><a name="p747511535436"></a><a name="p747511535436"></a>基础API &gt; 矢量计算 &gt; 基础算术</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p10475753174311"><a name="p10475753174311"></a><a name="p10475753174311"></a>Exp、Ln、Abs、Reciprocal、Sqrt、Rsqrt、Relu、Add、Sub、Mul、Div、Max、Min、BilinearInterpolation、Adds、Muls、Maxs、Mins、LeakyRelu</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p114751853194317"><a name="p114751853194317"></a><a name="p114751853194317"></a>-</p>
</td>
</tr>
<tr id="row1647525319432"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p447513537436"><a name="p447513537436"></a><a name="p447513537436"></a>基础API &gt; 矢量计算 &gt; 逻辑计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p747525319434"><a name="p747525319434"></a><a name="p747525319434"></a>Not、And、Or、ShiftLeft、ShiftRight</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p84751853144316"><a name="p84751853144316"></a><a name="p84751853144316"></a>-</p>
</td>
</tr>
<tr id="row1247575318435"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p164751053124316"><a name="p164751053124316"></a><a name="p164751053124316"></a>基础API &gt; 矢量计算 &gt; 复合计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p1169817233416"><a name="p1169817233416"></a><a name="p1169817233416"></a>Axpy、CastDequant、AddRelu、AddReluCast、SubRelu、SubReluCast、MulAddDst、MulCast、FusedMulAdd、FusedMulAddRelu</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p5475153164312"><a name="p5475153164312"></a><a name="p5475153164312"></a>-</p>
</td>
</tr>
<tr id="row13475185394314"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p20475105310432"><a name="p20475105310432"></a><a name="p20475105310432"></a>基础API &gt; 矢量计算 &gt; 比较与选择</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p16475553114318"><a name="p16475553114318"></a><a name="p16475553114318"></a>Compare、Compare（结果存入寄存器）、Compares、GetCmpMask、SetCmpMask、GatherMask</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p6475653204318"><a name="p6475653204318"></a><a name="p6475653204318"></a>-</p>
</td>
</tr>
<tr id="row18475195364317"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p4475853204310"><a name="p4475853204310"></a><a name="p4475853204310"></a>基础API &gt; 矢量计算 &gt; 类型转换</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p18475253154318"><a name="p18475253154318"></a><a name="p18475253154318"></a>Cast</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p847515384318"><a name="p847515384318"></a><a name="p847515384318"></a>-</p>
</td>
</tr>
<tr id="row1047545324316"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p14475165364311"><a name="p14475165364311"></a><a name="p14475165364311"></a>基础API &gt; 矢量计算 &gt; 归约计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p847595312435"><a name="p847595312435"></a><a name="p847595312435"></a>WholeReduceMax、WholeReduceMin、WholeReduceSum、BlockReduceMax、BlockReduceMin、BlockReduceSum、PairReduceSum、RepeatReduceSum、GetReduceRepeatSumSpr、GetReduceRepeatMaxMinSpr</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p18475115318430"><a name="p18475115318430"></a><a name="p18475115318430"></a>-</p>
</td>
</tr>
<tr id="row947565318439"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p54759533439"><a name="p54759533439"></a><a name="p54759533439"></a>基础API &gt; 矢量计算 &gt; 数据转换</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p3475553184310"><a name="p3475553184310"></a><a name="p3475553184310"></a>Transpose、TransDataTo5HD</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p147451518192713"><a name="p147451518192713"></a><a name="p147451518192713"></a>TransDataTo5HD （不支持dst与src类型为uint64_t的LocalTensor的场景）</p>
<p id="p2047518539434"><a name="p2047518539434"></a><a name="p2047518539434"></a></p>
</td>
</tr>
<tr id="row847545310433"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p1347555344319"><a name="p1347555344319"></a><a name="p1347555344319"></a>基础API &gt; 矢量计算 &gt; 数据填充</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p2475135318433"><a name="p2475135318433"></a><a name="p2475135318433"></a>Duplicate、Brcb</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1647535364315"><a name="p1647535364315"></a><a name="p1647535364315"></a>-</p>
</td>
</tr>
<tr id="row7475175384319"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p1147575304315"><a name="p1147575304315"></a><a name="p1147575304315"></a>基础API &gt; 矢量计算 &gt; 排序组合</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p747595317433"><a name="p747595317433"></a><a name="p747595317433"></a>Sort32、MrgSort、GetMrgSortResult</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1547545312433"><a name="p1547545312433"></a><a name="p1547545312433"></a>-</p>
</td>
</tr>
<tr id="row104754537434"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p11475105315431"><a name="p11475105315431"></a><a name="p11475105315431"></a>基础API &gt; 矢量计算 &gt; 离散与聚合</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p1147612530434"><a name="p1147612530434"></a><a name="p1147612530434"></a>Gather、Gatherb</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p347635384311"><a name="p347635384311"></a><a name="p347635384311"></a>-</p>
</td>
</tr>
<tr id="row9476353114310"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p1547675319439"><a name="p1547675319439"></a><a name="p1547675319439"></a>基础API &gt; 矢量计算 &gt; 掩码操作</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p18476053164310"><a name="p18476053164310"></a><a name="p18476053164310"></a>SetMaskCount、SetMaskNorm、SetVectorMask、ResetMask</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1047610535435"><a name="p1047610535435"></a><a name="p1047610535435"></a>-</p>
</td>
</tr>
<tr id="row1147614531435"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p4476185324312"><a name="p4476185324312"></a><a name="p4476185324312"></a>基础API &gt; 矢量计算 &gt; 量化设置</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p247625310439"><a name="p247625310439"></a><a name="p247625310439"></a>SetDeqScale</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p19476185312436"><a name="p19476185312436"></a><a name="p19476185312436"></a>-</p>
</td>
</tr>
<tr id="row447613539435"><td class="cellrowborder" rowspan="4" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p1947685374310"><a name="p1947685374310"></a><a name="p1947685374310"></a>基础API &gt; 数据搬运 &gt; DataCopy</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p34761253144318"><a name="p34761253144318"></a><a name="p34761253144318"></a>基础数据搬运</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p19476205324319"><a name="p19476205324319"></a><a name="p19476205324319"></a>不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row154764536438"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p16476353104318"><a name="p16476353104318"></a><a name="p16476353104318"></a>增强数据搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p9476165310436"><a name="p9476165310436"></a><a name="p9476165310436"></a>不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row2476185317437"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p4476253144320"><a name="p4476253144320"></a><a name="p4476253144320"></a>切片数据搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1447635324319"><a name="p1447635324319"></a><a name="p1447635324319"></a>-</p>
</td>
</tr>
<tr id="row1847610536433"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p11975173673218"><a name="p11975173673218"></a><a name="p11975173673218"></a>随路转换ND2NZ搬运</p>
<p id="p5975136163216"><a name="p5975136163216"></a><a name="p5975136163216"></a>随路转换NZ2ND搬运</p>
<p id="p1397583616325"><a name="p1397583616325"></a><a name="p1397583616325"></a>随路量化激活搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1047612538437"><a name="p1047612538437"></a><a name="p1047612538437"></a>随路转换ND2NZ搬运不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row747635344315"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p547665354313"><a name="p547665354313"></a><a name="p547665354313"></a>基础API &gt; 数据搬运</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p4476135364311"><a name="p4476135364311"></a><a name="p4476135364311"></a>Copy、DataCopyPad、SetPadValue</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p8476165374314"><a name="p8476165374314"></a><a name="p8476165374314"></a>DataCopyPad不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row114761053204315"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p2047615394311"><a name="p2047615394311"></a><a name="p2047615394311"></a>基础API &gt; 同步控制 &gt; 核内同步</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p14476853154310"><a name="p14476853154310"></a><a name="p14476853154310"></a>SetFlag/WaitFlag、PipeBarrier、DataSyncBarrier</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p947605334315"><a name="p947605334315"></a><a name="p947605334315"></a>-</p>
</td>
</tr>
<tr id="row1947615537431"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p04761853204310"><a name="p04761853204310"></a><a name="p04761853204310"></a>基础API &gt; 同步控制 &gt; 核间同步</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p10476115315430"><a name="p10476115315430"></a><a name="p10476115315430"></a>CrossCoreSetFlag、CrossCoreWaitFlag</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p6476155320439"><a name="p6476155320439"></a><a name="p6476155320439"></a>-</p>
</td>
</tr>
<tr id="row1547655354318"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p54761353104315"><a name="p54761353104315"></a><a name="p54761353104315"></a>基础API &gt; 缓存控制</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p1476195316434"><a name="p1476195316434"></a><a name="p1476195316434"></a>DataCachePreload、DataCacheCleanAndInvalid、ICachePreLoad、GetICachePreloadStatus</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p104766539434"><a name="p104766539434"></a><a name="p104766539434"></a>-</p>
</td>
</tr>
<tr id="row847685384313"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p194761534433"><a name="p194761534433"></a><a name="p194761534433"></a>基础API &gt; 系统变量访问</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p647655312439"><a name="p647655312439"></a><a name="p647655312439"></a>GetBlockNum、GetBlockIdx、GetDataBlockSizeInBytes、GetArchVersion、GetTaskRatio、InitSocState、GetProgramCounter、GetSubBlockNum、GetSubBlockIdx、GetSystemCycle、</p>
<p id="p716419515422"><a name="p716419515422"></a><a name="p716419515422"></a>CheckLocalMemoryIA</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p2047615324310"><a name="p2047615324310"></a><a name="p2047615324310"></a>-</p>
</td>
</tr>
<tr id="row174769538434"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p14765531438"><a name="p14765531438"></a><a name="p14765531438"></a>基础API &gt; 原子操作</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p847615314436"><a name="p847615314436"></a><a name="p847615314436"></a>SetAtomicAdd、SetAtomicType、DisableDmaAtomic、SetAtomicMax、SetAtomicMin、SetStoreAtomicConfig、GetStoreAtomicConfig</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p10476153144316"><a name="p10476153144316"></a><a name="p10476153144316"></a>-</p>
</td>
</tr>
<tr id="row7476053114311"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p747710531433"><a name="p747710531433"></a><a name="p747710531433"></a>基础API &gt; 矩阵计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p847713536432"><a name="p847713536432"></a><a name="p847713536432"></a>Mmad、MmadWithSparse、SetHF32Mode、SetHF32TransMode、SetMMLayoutTransform、SetFixPipeConfig、SetFixpipeNz2ndFlag、SetFixpipePreQuantFlag、InitConstValue、LoadData、LoadDataWithTranspose、SetAippFunctions、LoadImageToLocal、LoadDataWithSparse、SetFmatrix、SetLoadDataBoundary、SetLoadDataRepeat、SetLoadDataPaddingValue、Fixpipe</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p154771853144319"><a name="p154771853144319"></a><a name="p154771853144319"></a>-</p>
</td>
</tr>
<tr id="row4477125311436"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p1747745334311"><a name="p1747745334311"></a><a name="p1747745334311"></a>Utils API &gt; C++标准库 &gt; 算法</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p4477125313438"><a name="p4477125313438"></a><a name="p4477125313438"></a>max、min、index_sequence</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p547711539430"><a name="p547711539430"></a><a name="p547711539430"></a>-</p>
</td>
</tr>
<tr id="row647718531435"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p9477145317433"><a name="p9477145317433"></a><a name="p9477145317433"></a>Utils API &gt; C++标准库 &gt; 容器函数</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p12477105344313"><a name="p12477105344313"></a><a name="p12477105344313"></a>tuple、get、make_tuple</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p144771253194313"><a name="p144771253194313"></a><a name="p144771253194313"></a>-</p>
</td>
</tr>
<tr id="row7477145344310"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p114771553194319"><a name="p114771553194319"></a><a name="p114771553194319"></a>Utils API &gt; C++标准库 &gt; 类型特性</p>
</td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p9477135312433"><a name="p9477135312433"></a><a name="p9477135312433"></a>is_convertible、is_base_of、is_same、enable_if、conditional</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p047765314438"><a name="p047765314438"></a><a name="p047765314438"></a>-</p>
</td>
</tr>
</tbody>
</table>
<!-- end id2 -->

<!-- npu="A3" id3 -->
**表3**  针对Atlas A3 训练系列产品/Atlas A3 推理系列产品，支持的API范围

<a name="table167051848128"></a>
<table><thead align="left"><tr id="row07054481922"><th class="cellrowborder" valign="top" width="29.630000000000003%" id="mcps1.2.4.1.1"><p id="p135155581014"><a name="p135155581014"></a><a name="p135155581014"></a>接口分类</p>
</th>
<th class="cellrowborder" valign="top" width="44.519999999999996%" id="mcps1.2.4.1.2"><p id="p12351955181013"><a name="p12351955181013"></a><a name="p12351955181013"></a>接口名称</p>
</th>
<th class="cellrowborder" valign="top" width="25.85%" id="mcps1.2.4.1.3"><p id="p635165571012"><a name="p635165571012"></a><a name="p635165571012"></a>备注</p>
</th>
</tr>
</thead>
<tbody><tr id="row1870594819210"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1035755151018"><a name="p1035755151018"></a><a name="p1035755151018"></a>基础API &gt; 标量计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p193515557101"><a name="p193515557101"></a><a name="p193515557101"></a>GetBitCount、CountLeadingZero、CountBitsCntSameAsSignBit、GetSFFValue、Cast（float转half、int32_t）、Cast（float转bfloat16_t）、Cast（多类型转float）</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p2351455171020"><a name="p2351455171020"></a><a name="p2351455171020"></a>-</p>
</td>
</tr>
<tr id="row1470617481025"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p18361755191019"><a name="p18361755191019"></a><a name="p18361755191019"></a>基础API &gt; 矢量计算 &gt; 基础算术</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p173685515109"><a name="p173685515109"></a><a name="p173685515109"></a>Exp、Ln、Abs、Reciprocal、Sqrt、Rsqrt、Relu、Add、Sub、Mul、Div、Max、Min、BilinearInterpolation、Adds、Muls、Maxs、Mins、LeakyRelu</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p93614551107"><a name="p93614551107"></a><a name="p93614551107"></a>-</p>
</td>
</tr>
<tr id="row18706148928"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p12361655191017"><a name="p12361655191017"></a><a name="p12361655191017"></a>基础API &gt; 矢量计算 &gt; 逻辑计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p10362055181013"><a name="p10362055181013"></a><a name="p10362055181013"></a>Not、And、Or、ShiftLeft、ShiftRight</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p23615581013"><a name="p23615581013"></a><a name="p23615581013"></a>-</p>
</td>
</tr>
<tr id="row137061481120"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p53695511016"><a name="p53695511016"></a><a name="p53695511016"></a>基础API &gt; 矢量计算 &gt; 复合计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p6361255131014"><a name="p6361255131014"></a><a name="p6361255131014"></a>Axpy、CastDequant、AddRelu、AddReluCast、SubRelu、SubReluCast、MulAddDst、MulCast、FusedMulAdd、FusedMulAddRelu</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p153610555105"><a name="p153610555105"></a><a name="p153610555105"></a>-</p>
</td>
</tr>
<tr id="row070674812215"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p153615521020"><a name="p153615521020"></a><a name="p153615521020"></a>基础API &gt; 矢量计算 &gt; 比较与选择</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p236125551015"><a name="p236125551015"></a><a name="p236125551015"></a>Compare、Compare（结果存入寄存器）、Compares、GetCmpMask、SetCmpMask、GatherMask</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p936555181012"><a name="p936555181012"></a><a name="p936555181012"></a>-</p>
</td>
</tr>
<tr id="row137068483216"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1436955161018"><a name="p1436955161018"></a><a name="p1436955161018"></a>基础API &gt; 矢量计算 &gt; 类型转换</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1536115511017"><a name="p1536115511017"></a><a name="p1536115511017"></a>Cast</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p5365550108"><a name="p5365550108"></a><a name="p5365550108"></a>-</p>
</td>
</tr>
<tr id="row1570618486213"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p33635571010"><a name="p33635571010"></a><a name="p33635571010"></a>基础API &gt; 矢量计算 &gt; 归约计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p103612558107"><a name="p103612558107"></a><a name="p103612558107"></a>WholeReduceMax、WholeReduceMin、WholeReduceSum、BlockReduceMax、BlockReduceMin、BlockReduceSum、PairReduceSum、RepeatReduceSum、GetReduceRepeatSumSpr、GetReduceRepeatMaxMinSpr</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1736195513103"><a name="p1736195513103"></a><a name="p1736195513103"></a>-</p>
</td>
</tr>
<tr id="row13706104817211"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1736135519109"><a name="p1736135519109"></a><a name="p1736135519109"></a>基础API &gt; 矢量计算 &gt; 数据转换</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p9369554103"><a name="p9369554103"></a><a name="p9369554103"></a>Transpose、TransDataTo5HD</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p143612554106"><a name="p143612554106"></a><a name="p143612554106"></a>TransDataTo5HD （不支持dst与src类型为uint64_t的LocalTensor的场景）</p>
</td>
</tr>
<tr id="row670711481724"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p12368557106"><a name="p12368557106"></a><a name="p12368557106"></a>基础API &gt; 矢量计算 &gt; 数据填充</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p136185551010"><a name="p136185551010"></a><a name="p136185551010"></a>Duplicate、Brcb</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p4372055191014"><a name="p4372055191014"></a><a name="p4372055191014"></a>-</p>
</td>
</tr>
<tr id="row137071648520"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p13379558109"><a name="p13379558109"></a><a name="p13379558109"></a>基础API &gt; 矢量计算 &gt; 排序组合</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p133716557102"><a name="p133716557102"></a><a name="p133716557102"></a>Sort32、MrgSort、GetMrgSortResult</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p93717559104"><a name="p93717559104"></a><a name="p93717559104"></a>-</p>
</td>
</tr>
<tr id="row14707648226"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1537155514103"><a name="p1537155514103"></a><a name="p1537155514103"></a>基础API &gt; 矢量计算 &gt; 离散与聚合</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p103765551014"><a name="p103765551014"></a><a name="p103765551014"></a>Gather、Gatherb</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p113725561014"><a name="p113725561014"></a><a name="p113725561014"></a>-</p>
</td>
</tr>
<tr id="row157073482219"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p13795571019"><a name="p13795571019"></a><a name="p13795571019"></a>基础API &gt; 矢量计算 &gt; 掩码操作</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p53785515103"><a name="p53785515103"></a><a name="p53785515103"></a>SetMaskCount、SetMaskNorm、SetVectorMask、ResetMask</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1537105541013"><a name="p1537105541013"></a><a name="p1537105541013"></a>-</p>
</td>
</tr>
<tr id="row1170719484217"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p737125571012"><a name="p737125571012"></a><a name="p737125571012"></a>基础API &gt; 矢量计算 &gt; 量化设置</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p3371755121016"><a name="p3371755121016"></a><a name="p3371755121016"></a>SetDeqScale</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p73765516101"><a name="p73765516101"></a><a name="p73765516101"></a>-</p>
</td>
</tr>
<tr id="row157073481627"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p123775516109"><a name="p123775516109"></a><a name="p123775516109"></a>基础API &gt; 数据搬运 &gt; DataCopy</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p183725521010"><a name="p183725521010"></a><a name="p183725521010"></a>基础数据搬运</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p337175521010"><a name="p337175521010"></a><a name="p337175521010"></a>不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row7708104819220"><td class="cellrowborder" rowspan="4" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p5381155151015"><a name="p5381155151015"></a><a name="p5381155151015"></a>基础API &gt; 数据搬运</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p137125510109"><a name="p137125510109"></a><a name="p137125510109"></a>增强数据搬运</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p19371551101"><a name="p19371551101"></a><a name="p19371551101"></a>不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row7708948725"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p637175591011"><a name="p637175591011"></a><a name="p637175591011"></a>切片数据搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1637355101013"><a name="p1637355101013"></a><a name="p1637355101013"></a>-</p>
</td>
</tr>
<tr id="row57081487219"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p437185512102"><a name="p437185512102"></a><a name="p437185512102"></a>随路转换ND2NZ搬运</p>
<p id="p9371855171017"><a name="p9371855171017"></a><a name="p9371855171017"></a>随路转换NZ2ND搬运</p>
<p id="p13381755131011"><a name="p13381755131011"></a><a name="p13381755131011"></a>随路量化激活搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p938195512100"><a name="p938195512100"></a><a name="p938195512100"></a>随路转换ND2NZ搬运不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row370817486213"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p103895521010"><a name="p103895521010"></a><a name="p103895521010"></a>Copy、DataCopyPad、SetPadValue</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1338185515107"><a name="p1338185515107"></a><a name="p1338185515107"></a>DataCopyPad不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row177081648422"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p113885518108"><a name="p113885518108"></a><a name="p113885518108"></a>基础API &gt; 同步控制 &gt; 核内同步</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1538855191014"><a name="p1538855191014"></a><a name="p1538855191014"></a>SetFlag/WaitFlag、PipeBarrier、DataSyncBarrier</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p838255191017"><a name="p838255191017"></a><a name="p838255191017"></a>-</p>
</td>
</tr>
<tr id="row37087481225"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p143815519101"><a name="p143815519101"></a><a name="p143815519101"></a>基础API &gt; 同步控制 &gt; 核间同步</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p53885510109"><a name="p53885510109"></a><a name="p53885510109"></a>CrossCoreSetFlag、CrossCoreWaitFlag</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p83825511014"><a name="p83825511014"></a><a name="p83825511014"></a>-</p>
</td>
</tr>
<tr id="row1070894819215"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p12388559109"><a name="p12388559109"></a><a name="p12388559109"></a>基础API &gt; 缓存控制</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p103865521014"><a name="p103865521014"></a><a name="p103865521014"></a>DataCachePreload、DataCacheCleanAndInvalid、ICachePreLoad、GetICachePreloadStatus</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p10383551101"><a name="p10383551101"></a><a name="p10383551101"></a>-</p>
</td>
</tr>
<tr id="row87081348428"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p338155141011"><a name="p338155141011"></a><a name="p338155141011"></a>基础API &gt; 系统变量访问</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p938955181019"><a name="p938955181019"></a><a name="p938955181019"></a>GetBlockNum、GetBlockIdx、GetDataBlockSizeInBytes、GetArchVersion、GetTaskRatio、InitSocState、GetProgramCounter、GetSubBlockNum、GetSubBlockIdx、GetSystemCycle、</p>
<p id="p63865551011"><a name="p63865551011"></a><a name="p63865551011"></a>CheckLocalMemoryIA</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p143815557104"><a name="p143815557104"></a><a name="p143815557104"></a>-</p>
</td>
</tr>
<tr id="row1170934815213"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p938105531011"><a name="p938105531011"></a><a name="p938105531011"></a>基础API &gt; 原子操作</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1738145591014"><a name="p1738145591014"></a><a name="p1738145591014"></a>SetAtomicAdd、SetAtomicType、DisableDmaAtomic、SetAtomicMax、SetAtomicMin、SetStoreAtomicConfig、GetStoreAtomicConfig</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p5388559106"><a name="p5388559106"></a><a name="p5388559106"></a>-</p>
</td>
</tr>
<tr id="row67096482218"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p93875511014"><a name="p93875511014"></a><a name="p93875511014"></a>基础API &gt; 矩阵计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p53919552100"><a name="p53919552100"></a><a name="p53919552100"></a>Mmad、MmadWithSparse、SetHF32Mode、SetHF32TransMode、SetMMLayoutTransform、SetFixPipeConfig、SetFixpipeNz2ndFlag、SetFixpipePreQuantFlag、InitConstValue、LoadData、LoadDataWithTranspose、SetAippFunctions、LoadImageToLocal、LoadDataWithSparse、SetFmatrix、SetLoadDataBoundary、SetLoadDataRepeat、SetLoadDataPaddingValue、Fixpipe</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p153985521019"><a name="p153985521019"></a><a name="p153985521019"></a>-</p>
</td>
</tr>
<tr id="row9709184811214"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p9391855101018"><a name="p9391855101018"></a><a name="p9391855101018"></a>Utils API &gt; C++标准库 &gt; 算法</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p163955551011"><a name="p163955551011"></a><a name="p163955551011"></a>max、min、index_sequence</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p53965581013"><a name="p53965581013"></a><a name="p53965581013"></a>-</p>
</td>
</tr>
<tr id="row167094482220"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p14394556105"><a name="p14394556105"></a><a name="p14394556105"></a>Utils API &gt; C++标准库 &gt; 容器函数</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p113912552108"><a name="p113912552108"></a><a name="p113912552108"></a>tuple、get、make_tuple</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p13917555103"><a name="p13917555103"></a><a name="p13917555103"></a>-</p>
</td>
</tr>
<tr id="row97091948726"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p139855171017"><a name="p139855171017"></a><a name="p139855171017"></a>Utils API &gt; C++标准库 &gt; 类型特性</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p739155111015"><a name="p739155111015"></a><a name="p739155111015"></a>is_convertible、is_base_of、is_same、enable_if、conditional</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p73935581010"><a name="p73935581010"></a><a name="p73935581010"></a>-</p>
</td>
</tr>
<tr id="row137091485215"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p914812282549"><a name="p914812282549"></a><a name="p914812282549"></a>高阶API &gt; C++标准库 &gt; 类型特性</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1414882810547"><a name="p1414882810547"></a><a name="p1414882810547"></a>is_convertible、is_base_of、is_same、enable_if、conditional</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1514822875412"><a name="p1514822875412"></a><a name="p1514822875412"></a>-</p>
</td>
</tr>
<tr id="row070934810215"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1642119533450"><a name="p1642119533450"></a><a name="p1642119533450"></a>高阶API &gt; 模板库函数 &gt; type_traits</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p042105320459"><a name="p042105320459"></a><a name="p042105320459"></a>is_convertible、is_base_of、is_same、enable_if、conditional</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p5421553134513"><a name="p5421553134513"></a><a name="p5421553134513"></a>-</p>
</td>
</tr>
</tbody>
</table>
<!-- end id3 -->

<!-- npu="950" id4 -->
**表4**  针对Ascend 950PR/Ascend 950DT，支持的API范围

<a name="table1798673512413"></a>
<table><thead align="left"><tr id="row109864353413"><th class="cellrowborder" valign="top" width="29.630000000000003%" id="mcps1.2.4.1.1"><p id="p1098613524117"><a name="p1098613524117"></a><a name="p1098613524117"></a>接口分类</p>
</th>
<th class="cellrowborder" valign="top" width="44.519999999999996%" id="mcps1.2.4.1.2"><p id="p29871535174117"><a name="p29871535174117"></a><a name="p29871535174117"></a>接口名称</p>
</th>
<th class="cellrowborder" valign="top" width="25.85%" id="mcps1.2.4.1.3"><p id="p9987835154115"><a name="p9987835154115"></a><a name="p9987835154115"></a>备注</p>
</th>
</tr>
</thead>
<tbody><tr id="row1298763513418"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p15987173524114"><a name="p15987173524114"></a><a name="p15987173524114"></a>基础API &gt; 标量计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p12167195363614"><a name="p12167195363614"></a><a name="p12167195363614"></a>GetBitCount、CountLeadingZero、CountBitsCntSameAsSignBit、GetSFFValue、Cast（float转half、int32_t）、Cast（float转bfloat16_t）、Cast（多类型转float）、Nop、GetUintDivMagicAndShift、WriteGmByPassDCache、ReadGmByPassDCache</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p298711352411"><a name="p298711352411"></a><a name="p298711352411"></a>-</p>
</td>
</tr>
<tr id="row1098743584116"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p69871035114113"><a name="p69871035114113"></a><a name="p69871035114113"></a>基础API &gt; 矢量计算 &gt; 基础算术</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1068419974813"><a name="p1068419974813"></a><a name="p1068419974813"></a>Exp、Ln、Abs、Reciprocal、Sqrt、Rsqrt、Relu、Neg、Add、Sub、Mul、Div、Max、Min、BilinearInterpolation、Prelu、Mull、Adds、Adds（灵活标量位置）、Muls、Muls（灵活标量位置）、Maxs、Maxs（灵活标量位置）、Mins、Mins（灵活标量位置）、Subs、Divs、LeakyRelu</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p798711356416"><a name="p798711356416"></a><a name="p798711356416"></a>-</p>
</td>
</tr>
<tr id="row45561731104215"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p35561731164210"><a name="p35561731164210"></a><a name="p35561731164210"></a>基础API &gt; 矢量计算 &gt; 逻辑计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p18556103154212"><a name="p18556103154212"></a><a name="p18556103154212"></a>Not、And、Or、ShiftLeft（左移位数为Tensor）、ShiftRight（右移位数为Tensor）、Ands、Ors、ShiftLeft、ShiftRight</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p35567315428"><a name="p35567315428"></a><a name="p35567315428"></a>-</p>
</td>
</tr>
<tr id="row1798773512412"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p189871935184120"><a name="p189871935184120"></a><a name="p189871935184120"></a>基础API &gt; 矢量计算 &gt; 复合计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p10987133511418"><a name="p10987133511418"></a><a name="p10987133511418"></a>Axpy、CastDequant、AddRelu、AddReluCast、AddDeqRelu、SubRelu、SubReluCast、MulAddDst、MulCast、FusedMulAdd、MulAddRelu、AbsSub、FusedExpSub、MulsCast</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p179879356419"><a name="p179879356419"></a><a name="p179879356419"></a>-</p>
</td>
</tr>
<tr id="row5600192234115"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p7600112274111"><a name="p7600112274111"></a><a name="p7600112274111"></a>基础API &gt; 矢量计算 &gt; 比较与选择</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p16681911154710"><a name="p16681911154710"></a><a name="p16681911154710"></a>Compare、Compares、Compares（灵活标量位置）、GetCmpMask、SetCmpMask、GatherMask、Select、Select（灵活标量位置）</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p46001422164119"><a name="p46001422164119"></a><a name="p46001422164119"></a>Select不支持：tensor高位切分计算->Select模式0和模式2->不传入mask参数接口中的模式0。</p>
</td>
</tr>
<tr id="row998718356411"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p5987335194120"><a name="p5987335194120"></a><a name="p5987335194120"></a>基础API &gt; 矢量计算 &gt; 类型转换</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p198717354419"><a name="p198717354419"></a><a name="p198717354419"></a>Cast、Truncate</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p798719352419"><a name="p798719352419"></a><a name="p798719352419"></a>-</p>
</td>
</tr>
<tr id="row7987193534118"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p898712358414"><a name="p898712358414"></a><a name="p898712358414"></a>基础API &gt; 矢量计算 &gt; 归约计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p675915224145"><a name="p675915224145"></a><a name="p675915224145"></a>ReduceMax、ReduceMin、WholeReduceSum、BlockReduceMax、BlockReduceMin、BlockReduceSum、PairReduceSum、RepeatReduceSum、</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1398713564110"><a name="p1398713564110"></a><a name="p1398713564110"></a>-</p>
</td>
</tr>
<tr id="row19871835174112"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p298716354416"><a name="p298716354416"></a><a name="p298716354416"></a>基础API &gt; 矢量计算 &gt; 数据转换</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p79872035154118"><a name="p79872035154118"></a><a name="p79872035154118"></a>Transpose</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p149871335154112"><a name="p149871335154112"></a><a name="p149871335154112"></a>-</p>
</td>
</tr>
<tr id="row162305694316"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p123135644320"><a name="p123135644320"></a><a name="p123135644320"></a>基础API &gt; 矢量计算 &gt; 数据填充</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p48541044144819"><a name="p48541044144819"></a><a name="p48541044144819"></a>Duplicate、Brcb、CreateVecIndex</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p102335634317"><a name="p102335634317"></a><a name="p102335634317"></a>-</p>
</td>
</tr>
<tr id="row7987113516412"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p139871235164110"><a name="p139871235164110"></a><a name="p139871235164110"></a>基础API &gt; 矢量计算 &gt; 排序组合</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p8325152773515"><a name="p8325152773515"></a><a name="p8325152773515"></a>Sort32、MrgSort、GetMrgSortResult</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p159871035134111"><a name="p159871035134111"></a><a name="p159871035134111"></a>-</p>
</td>
</tr>
<tr id="row9987535154118"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p14987163518416"><a name="p14987163518416"></a><a name="p14987163518416"></a>基础API &gt; 矢量计算 &gt; 离散与聚合</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1987163510416"><a name="p1987163510416"></a><a name="p1987163510416"></a>Gather、Gatherb、Scatter</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p169872035144115"><a name="p169872035144115"></a><a name="p169872035144115"></a>-</p>
</td>
</tr>
<tr id="row098783516417"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p139874354419"><a name="p139874354419"></a><a name="p139874354419"></a>基础API &gt; 矢量计算 &gt; 掩码操作</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p99871335184114"><a name="p99871335184114"></a><a name="p99871335184114"></a>SetMaskCount、SetMaskNorm、SetVectorMask、ResetMask</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p18988435204114"><a name="p18988435204114"></a><a name="p18988435204114"></a>-</p>
</td>
</tr>
<tr id="row1198714156411"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1698718951871"><a name="p1698718951871"></a><a name="p1698718951871"></a>基础API &gt; 矢量计算 &gt; 量化设置</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1752718121879"><a name="p1752718121879"></a><a name="p1752718121879"></a>SetDeqScale</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1687112812182"><a name="p1687112812182"></a><a name="p1687112812182"></a>-</p>
</td>
</tr>
<tr id="row098783516418"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p139874354418"><a name="p139874354418"></a><a name="p139874354418"></a>基础API &gt; 矢量计算 &gt; 数据重排</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p99871335184115"><a name="p99871335184115"></a><a name="p99871335184115"></a>Interleave、DeInterleave</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p18988435204115"><a name="p18988435204115"></a><a name="p18988435204115"></a>-</p>
</td>
</tr>
<tr id="row1598863534117"><td class="cellrowborder" rowspan="6" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1998811353419"><a name="p1998811353419"></a><a name="p1998811353419"></a>基础API &gt; 数据搬运 &gt; DataCopy</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p398811358414"><a name="p398811358414"></a><a name="p398811358414"></a>基础数据搬运</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p11988335184112"><a name="p11988335184112"></a><a name="p11988335184112"></a>不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row19881535134113"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1995517203185"><a name="p1995517203185"></a><a name="p1995517203185"></a>增强数据搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p172481414205"><a name="p172481414205"></a><a name="p172481414205"></a>不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row831243535112"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1095612051813"><a name="p1095612051813"></a><a name="p1095612051813"></a>切片数据搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p11956142012183"><a name="p11956142012183"></a><a name="p11956142012183"></a>-</p>
</td>
</tr>
<tr id="row49881535124115"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p179561920111815"><a name="p179561920111815"></a><a name="p179561920111815"></a>随路转换ND2NZ搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p595642041818"><a name="p595642041818"></a><a name="p595642041818"></a>不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row76231552182019"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p11956102019184"><a name="p11956102019184"></a><a name="p11956102019184"></a>随路转换NZ2ND搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1062311525208"><a name="p1062311525208"></a><a name="p1062311525208"></a>-</p>
</td>
</tr>
<tr id="row022720172113"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p7956192001812"><a name="p7956192001812"></a><a name="p7956192001812"></a>随路量化激活搬运</p>
</td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p13228105216"><a name="p13228105216"></a><a name="p13228105216"></a>-</p>
</td>
</tr>
<tr id="row16988143510417"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p192041749112615"><a name="p192041749112615"></a><a name="p192041749112615"></a>基础API &gt; 数据搬运</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p16956102014187"><a name="p16956102014187"></a><a name="p16956102014187"></a>Copy、DataCopyPad、SetPadValue</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p2702434251"><a name="p2702434251"></a><a name="p2702434251"></a>DataCopyPad不支持UB（VECIN/VECCALC/VECOUT）-&gt;L1 Buffer（TSCM）通路的数据搬运。</p>
</td>
</tr>
<tr id="row6778713162710"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p20882139162713"><a name="p20882139162713"></a><a name="p20882139162713"></a>基础API &gt; 同步控制 &gt; 核内同步</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p2882133915277"><a name="p2882133915277"></a><a name="p2882133915277"></a>SetFlag/WaitFlag、PipeBarrier、DataSyncBarrier</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p8882133922716"><a name="p8882133922716"></a><a name="p8882133922716"></a>-</p>
</td>
</tr>
<tr id="row28501428172718"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p388253912712"><a name="p388253912712"></a><a name="p388253912712"></a>基础API &gt; 同步控制 &gt; 核间同步</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p6882143920272"><a name="p6882143920272"></a><a name="p6882143920272"></a>CrossCoreSetFlag、CrossCoreWaitFlag、SyncAll</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p9882173914275"><a name="p9882173914275"></a><a name="p9882173914275"></a>-</p>
</td>
</tr>
<tr id="row2015642713274"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p288214390273"><a name="p288214390273"></a><a name="p288214390273"></a>基础API &gt; 缓存控制</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1688293992714"><a name="p1688293992714"></a><a name="p1688293992714"></a>DataCachePreload、DataCacheCleanAndInvalid、ICachePreLoad、GetICachePreloadStatus</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1988210392278"><a name="p1988210392278"></a><a name="p1988210392278"></a>-</p>
</td>
</tr>
<tr id="row1444116151276"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p13882639172714"><a name="p13882639172714"></a><a name="p13882639172714"></a>基础API &gt; 系统变量访问</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p988253914278"><a name="p988253914278"></a><a name="p988253914278"></a>GetBlockNum、GetBlockIdx、GetDataBlockSizeInBytes、GetArchVersion、InitSocState、GetSpr、ClearSpr、GetProgramCounter、GetSubBlockNum、GetSubBlockIdx、GetSystemCycle、SetCtrlSpr、GetCtrlSpr、ResetCtrlSpr</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p2882173992716"><a name="p2882173992716"></a><a name="p2882173992716"></a>-</p>
</td>
</tr>
<tr id="row1298811359417"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p10988193511419"><a name="p10988193511419"></a><a name="p10988193511419"></a>基础API &gt; 原子操作</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1479292993514"><a name="p1479292993514"></a><a name="p1479292993514"></a>SetAtomicAdd、SetAtomicType、DisableDmaAtomic、SetAtomicMax、SetAtomicMin、SetStoreAtomicConfig、GetStoreAtomicConfig、AtomicAdd、AtomicMin、AtomicMax、AtomicCas、AtomicExch</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p89881358416"><a name="p89881358416"></a><a name="p89881358416"></a>-</p>
</td>
</tr>
<tr id="row11988153510410"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p4989143554117"><a name="p4989143554117"></a><a name="p4989143554117"></a>基础API &gt; 矩阵计算</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p398953516417"><a name="p398953516417"></a><a name="p398953516417"></a>Fill、LoadData、LoadDataWithTranspose、LoadDataWithSparse、SetFmatrix、SetLoadDataBoundary、SetLoadDataRepeat、SetLoadDataPaddingValue、Fixpipe、SetFixPipeConfig、SetFixpipeNz2ndFlag、SetFixpipePreQuantFlag、SetAippFunctions、LoadImageToLocal、Mmad、SetHF32Mode、SetHF32TransMode</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p7989123518419"><a name="p7989123518419"></a><a name="p7989123518419"></a>-</p>
</td>
</tr>
<tr id="row2098911359417"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p6989173517418"><a name="p6989173517418"></a><a name="p6989173517418"></a>Utils API &gt; C++标准库 &gt; 算法</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p79891335164117"><a name="p79891335164117"></a><a name="p79891335164117"></a>max、min、index_sequence</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p5989193564117"><a name="p5989193564117"></a><a name="p5989193564117"></a>-</p>
</td>
</tr>
<tr id="row3989143504110"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p119891335204116"><a name="p119891335204116"></a><a name="p119891335204116"></a>Utils API &gt; C++标准库 &gt; 容器函数</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p29891935104116"><a name="p29891935104116"></a><a name="p29891935104116"></a>tuple、get、make_tuple</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p3989113510418"><a name="p3989113510418"></a><a name="p3989113510418"></a>-</p>
</td>
</tr>
<tr id="row59891735184118"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1298918353418"><a name="p1298918353418"></a><a name="p1298918353418"></a>Utils API &gt; C++标准库 &gt; 类型特性</p>
</td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1698943519414"><a name="p1698943519414"></a><a name="p1698943519414"></a>is_convertible、is_same、enable_if、conditional</p>
</td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p9989113544116"><a name="p9989113544116"></a><a name="p9989113544116"></a>-</p>
</td>
</tr>
</tbody>
</table>
<!-- end id4 -->


## 使用预留UB空间的API范围<a name="section_reserved_ubuf_api"></a>

以下API内部会使用Ascend C预留的UB空间。当开启[--cce-disable-asc-reserved-ubuf](../../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#常用的编译选项)编译选项后，在对应产品版本下调用相关API会触发编译期报错。由于同一API使用预留UB空间的情况在不同NPU架构下有差异，开启该编译选项后，需要手动调整API调用方式或替换为不依赖预留UB空间的实现，才能完成兼容性迁移。

>[!CAUTION]注意
> 同一接口若提供多个重载原型，可能只有部分原型会使用UB预留空间。

<!-- npu="A3,910b" id5 -->
**表5**  针对 [NPU架构版本2201](../../../language_extension/simd_builtin_keywords.md#npu-arch)，使用预留UB空间的API范围

| API类别 | API名 |
| --- | --- |
| 基础API | AddDeqRelu、Select |
| 高阶API | Digamma、Lgamma、Power、Sign、LayerNorm、DropOut、SelectWithBytesMask |
<!-- end id5 -->

<!-- npu="950" id6 -->
**表6**  针对 [NPU架构版本3510](../../../language_extension/simd_builtin_keywords.md#npu-arch)，使用预留UB空间的API范围

| API类别 | API名 |
| --- | --- |
| 基础API > Memory数据搬运 | Copy |
| 基础API > 基础算术 | Exp、Ln、Abs、Reciprocal、Sqrt、Rsqrt、Relu、Add、Sub、Mul、Div、Max、Min、Adds、Muls、Maxs、Mins、LeakyRelu、Subs、Divs |
| 基础API > 逻辑计算 | Not、And、Or、ShiftLeft、ShiftRight、Ands、Ors |
| 基础API > 复合计算 | Axpy、CastDequant、CastDeq、AddRelu、AddReluCast、AddDeqRelu、SubRelu、SubReluCast、MulAddDst、MulCast、FusedMulAdd、FusedMulAddRelu、MulAddRelu |
| 基础API > 比较与选择 | Compare、Compares、CompareScalar、Select、GatherMask |
| 基础API > 类型转换 | Cast |
| 基础API > 归约计算 | ReduceRepeat、ReduceDataBlock、ReducePairElem、BlockReduceMax、BlockReduceMin、BlockReduceSum、PairReduceSum、RepeatReduceSum、WholeReduceMax、WholeReduceMin、WholeReduceSum |
| 基础API > 数据填充 | Duplicate |
| 基础API > Kernel-Tiling | GET_TILING_DATA、GET_TILING_DATA_WITH_STRUCT、GET_TILING_DATA_MEMBER |
| Utils API > 调测接口（SIMD VF） | printf、asc_dump_reg、asc_dump、asc_dump_ubuf |
| 高阶API | AscendAntiQuant、AdjustSoftMaxRes、TopK |
<!-- end id6 -->
