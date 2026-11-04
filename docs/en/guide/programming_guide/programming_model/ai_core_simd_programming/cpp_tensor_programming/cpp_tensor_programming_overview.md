# C++ Tensor Programming Overview

<!-- md-trans-meta sourceCommit=8d22eb82fd0b279bd5d784f696a54970c40f98ac translatedAt=2026-10-28T08:52:01.792Z pushedAt=2026-11-03T07:08:03.557Z nl=2 -->

C++ Tensor programming is the foundational programming interface of the Ascend C SIMD programming path. It abstracts memory addresses through Tensor objects and provides high-level programming capabilities with type safety and compile-time checking. Developers manage memory through Tensor objects such as LocalTensor, GlobalTensor, and RegTensor without directly manipulating pointer addresses, and data movement and computation APIs operate directly on Tensor objects to complete data flow. This path is suitable for operator development scenarios that pursue development efficiency, type safety, and code maintainability, and it is a key technical path for developers accustomed to C++ object-oriented programming to quickly get started with NPU operator development. This chapter aims to help developers build a systematic understanding of C++ Tensor programming and understand its Tensor abstraction programming paradigm, core programming model, and development methodology.

## Kernel

[Kernel](../kernel_function.md) has been introduced in the preceding chapters. In C++ Tensor programming, the definition and invocation of a kernel follow the general SIMD programming specifications, with no additional syntax constraints.

## Memory Hierarchy

The AI Core adopts a hierarchical storage architecture.  Cube computation uses a two-level hierarchy of "Global Memory (hereinafter referred to as GM) → L1/L0 series buffers" to serve cube computation. The traditional Vector computation architecture uses a two-level hierarchy of "Global Memory → UB", while the [NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md) introduces registers to build a three-level hierarchy of "GM → UB → Register". In this hierarchy, GM stores input and output data, the Unified Buffer (UB) serves as the intermediate cache for vector computation, and Register resides at the innermost layer of the Vector compute unit, directly interacting with the execution unit.

![AI Core storage hierarchy](../../../../figures/aicore_mem_2.png)

### Tensor Memory Abstraction

A Tensor is essentially an array-based programming abstraction rather than a simple memory address wrapper. It manages memory and performs computation in an array manner, which better matches the SIMD hardware execution model. Its core idea is to abstract raw memory addresses into Tensor objects, elevating the programming level from "address operation" to "object management".

Based on the chip's hierarchical storage architecture, Tensors are classified into three types: **GlobalTensor** corresponds to GM and stores operator input/output data; **LocalTensor** corresponds to on-chip memory (buffers such as UB, L1 Buffer, L0A Buffer/L0B Buffer/L0C Buffer) and carries data during computation; **RegTensor** corresponds to registers and is used for register-level vector computation (introduced in [NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md)). The Tensor system continues to evolve: basic Tensors carry only data pointers and size information, requiring manual passing of operation parameters; extended Tensors (Tensor API) introduce the Layout concept, carrying Shape and Stride information, so that APIs can automatically derive parameters such as transfer length and the number of computed elements, greatly reducing programming complexity. The APIs introduced by this capability are uniformly invoked through the `asc::te` namespace (for example, `asc::te::mmad`).

#### Basic Tensor and Extended Tensor Abstraction Definitions

- A basic Tensor (without Layout) encapsulates only the data pointer and size information, and cannot express the physical layout of the data. When performing data movement and computation, developers must manually pass the size/count parameters and compute the offsets and strides themselves, which is error-prone and verbose when dealing with high-dimensional tensors.

    ```cpp
    __global__ __vector__ void add_custom(__gm__ float* x, __gm__ float* y, __gm__ float* z) {
      AscendC::GlobalTensor<float> xGm;
      xGm.SetGlobalBuffer(x + block_idx * blockLength, blockLength);

      AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
      AscendC::LocalTensor<float> xLocal = ubAllocator.Alloc<float, blockLength>();

      // Data copy and computation require manual size parameter
      AscendC::DataCopy(xLocal, xGm, blockLength);
      // The definition and data movement of yLocal and zLocal are the same as those of xLocal and are omitted here.
      ...
      AscendC::Add(zLocal, xLocal, yLocal, blockLength);
    }
    ```

- An extended Tensor (Tensor API) encapsulates Shape and Stride information, and the API automatically derives the operation parameters, achieving compile-time type safety. Developers do not need to pay attention to memory layout details, and can complete slicing, data movement, and computation in a single line of code. The current capability is applied to cube cube computation, and Vector computation support will be added gradually.

    ```cpp
    #include "tensor_api/tensor.h"

    using namespace asc::te;

    __global__ __cube__ void matmul_custom(__gm__ half* a, __gm__ half* b, __gm__ half* c) {
        // Implementation code example, where m, n, and k are the sizes of the left and right matrices on GM for a single core, and (base_m, k) and (k, base_n) are the sizes of the left and right matrices moved to L1.
        // The extended Tensor (Tensor API) carries Layout information, and the data movement API automatically derives the parameters.
        auto gm_a_tensor = make_tensor(make_mem_ptr(a), make_frame_layout<nd_ext_layout_ptn>(m, k));
        auto gm_b_tensor = make_tensor(make_mem_ptr(b), make_frame_layout<nd_ext_layout_ptn>(k, n));
        ...
        // __cbuf__ is the declaration modifier for the L1 Buffer.
        __cbuf__ half l1_a_buf[1024];
        __cbuf__ half l1_b_buf[1024];
        auto l1_a_tensor = make_tensor(make_mem_ptr(l1_a_buf), make_frame_layout<nz_layout_ptn>(base_m, K));
        auto l1_b_tensor = make_tensor(make_mem_ptr(l1_b_buf), make_frame_layout<nz_layout_ptn>(k, base_n));
        ...
        // Complete the data movement in a single line of code without manually passing the size parameter.
        auto copy_gm_to_l1_atom = make_copy(copy_gm_to_l1{}, gm_to_l1_trait_default{});
        copy(copy_gm_to_l1_atom, l1_a_tensor, gm_a_tensor);
        copy(copy_gm_to_l1_atom, l1_b_tensor, gm_b_tensor);
    }
    ```

### External Storage

The Tensor abstraction example in the preceding section has already demonstrated the basic usage of GlobalTensor and LocalTensor. The following describes the characteristics of external storage and internal storage and how Tensors are managed from the perspective of the hardware storage hierarchy.
GM is the device memory of the Ascend NPU, located outside the AI Core. It offers large capacity and high bandwidth but has relatively long access latency. It is used to store the input data, output results, and intermediate computed data of operators. GM is persistent: the data stored in it remains until the memory space is released or the application terminates.
Users manage the global memory on the Device side through Runtime APIs. On the Host side, `aclrtMalloc` is used to allocate Device memory, `aclrtMemcpy` is used to copy data between the Host and the Device, `<<<>>>` is used to launch the kernel function (Kernel) for execution, and finally `aclrtFree` is used to release the memory. For details about Runtime APIs, see [Runtime API](https://gitcode.com/cann/runtime/blob/9.2.0/docs/en/api_ref/README.md).

The following example shows how to allocate Device memory through Runtime APIs on the Host side, copy data between the Host and the Device, and invoke the kernel function (Kernel):
```cpp
// Kernel function definition (using __global__ prefix to indicate Host call, Device execution)
__global__ __vector__ void add_custom(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z)
{
    AscendC::GlobalTensor<float> xGm;
    xGm.SetGlobalBuffer(reinterpret_cast<__gm__ float*>(x) + block_idx * blockLength, blockLength);
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
As a large-capacity global memory, GM is used for Host-Device interaction and for storing operator input and output data. However, its access latency is long and cannot meet the low-latency computation requirements of the AI Core. All Cube/Vector computations must preload data into on-chip internal storage. The following describes how Tensors are managed in internal storage, covering UB, registers, and L1/L0 buffers.

### Internal Storage

The AI Core contains three types of compute units: Cube for cube computation, Vector for vector computation, and Scalar for scalar computation. Before computation, data must be transferred from external storage to internal storage. Different storage units serve different compute units: UB supports vector computation, while L1 Buffer and L0A/L0B Buffer support cube computation.

#### UB in AIV Internal Storage

UB is an internal storage unit in the AI Core that is mainly used for vector computation. It is located inside each Vector Core (AIV). It has a small capacity but high bandwidth and low latency, and can be regarded as a high-speed cache managed by the user during kernel execution.

In C++ Tensor programming, UB memory is managed through LocalTensor objects. For basic tensors (without a Layout), the corresponding tensor is created mainly by specifying the physical storage location and memory size through `LocalMemAllocator`. LocalMemAllocator is a linear memory allocator. Developers can call the Alloc method to allocate memory, which is allocated linearly backward in the order of calls. However, this allocator does not provide memory release or other memory management capabilities.

```cpp
__global__ __vector__ void add_custom(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z)
{
    // Basic Tensor: allocate through Allocator
    AscendC::LocalMemAllocator<AscendC::Hardware::UB> ubAllocator;
    AscendC::LocalTensor<half> ubTensor = ubAllocator.Alloc<half, 256>();
}
```

During initial feature development, you can use LocalMemAllocator to simplify operator writing. If finer-grained memory control is required (such as avoiding bank conflicts and memory reuse), you can directly specify addresses through LocalTensor for management. When using it, you need to plan address allocation properly to ensure that the physical storage limit is not exceeded, and perform memory reuse while ensuring functional correctness.

```cpp
__global__ __vector__ void add_custom(__gm__ uint8_t* x, __gm__ uint8_t* y, __gm__ uint8_t* z)
{
  // Manually specify the addresses of x, y, and z.
  uint64_t xAddrPing = 0;
  uint64_t yAddrPing = 1024;
  uint64_t zAddrPing = 2048;

  // Construct a LocalTensor by specifying the physical storage location, address, size, and so on through the constructor.
  AscendC::LocalTensor<float> xLocalPing(AscendC::TPosition::VECCALC, xAddrPing, 256);
  AscendC::LocalTensor<float> yLocalPing(AscendC::TPosition::VECCALC, yAddrPing, 256);
  AscendC::LocalTensor<float> zLocalPing(AscendC::TPosition::VECCALC, zAddrPing, 256);
}
```
> 📌 Here, AscendC::TPosition::VECCALC is a logical abstraction of the physical location. For the mapping between logical locations and physical storage, see [Logical Location and Physical Storage Mapping](../../../../../api/SIMD-API/general_description_and_constraints.md).

Compared with basic tensors, [NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md) introduces the Layout description and makes it a core attribute of tensors. Developers can directly associate Shape and Stride information when creating a tensor, simplifying the layout management of multi-dimensional data. Through `make_tensor`, developers can flexibly specify the memory address and Layout, making it easier to describe typical data layouts such as ND and NZ.

```cpp
// UB buffer declaration
__ubuf__ half ub_buffer[1024];
auto ub_ptr = make_mem_ptr(ub_buffer);
// Extended Tensor: create through make_tensor
auto ub_tensor = make_tensor(ub_ptr, make_layout(make_shape(128, 64)));
```
> 📌 UB addresses must be 32-byte aligned to match the hardware bus granularity and the parallelism of SIMD compute units. Unaligned access will cause runtime errors.

#### AIV Internal Storage: Register File

[NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md) introduces register programmability. All computations on the Vector compute unit must go through the three-level data flow of "GM → UB → Register", and vector computation is performed on registers. C++ Tensor programming implements register programming by defining RegTensor. For details, see [Reg Vector Computation Programming](./reg_vector_computation.md).

> 📌 Register programmability is introduced from [NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md).

#### AIC Internal Storage: L1 Buffer & L0 Buffer

The caches dedicated to the cube compute unit are as follows: the L0A Buffer stores the left cube, the L0B Buffer stores the right cube, and the L0C Buffer stores the initial accumulation values and the cube computation results. The L1 Buffer is used to cache the input data for cube computation.

These storage units are managed through `LocalTensor` objects:

| Storage Unit | Description | Examples |
|---------|------|------|
| L1 Buffer | Caches the input data for cube computation | `LocalTensor<half> l1_buffer;` |
| L0A Buffer | Stores the left cube | `LocalTensor<half> l0a_buffer;` |
| L0B Buffer | Stores the right cube | `LocalTensor<half> l0b_buffer;` |
| L0C Buffer | Stores the computation result | `LocalTensor<float> l0c_buffer;` |
| Fixpipe Buffer | Stores the quantization parameters | `LocalTensor<half> fixpipe_buffer;` |
| BiasTable Buffer | Stores the bias data | `LocalTensor<half> bias_buffer;` |

Similar to the UB allocation method, developers can allocate L1/L0 Buffer memory through LocalMemAllocator or by directly constructing a LocalTensor. To improve cube computation efficiency, the L1 Buffer and L0 Buffer use a fractal format. The fractal format varies across product models:

| Storage Unit | [NPU Architecture Version 2201](../../../language_extension/simd_builtin_keywords.md) | [NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md) |
|---------|----------------------------|---------------------------|
| L1 Buffer | Nz | Nz |
| L0A Buffer | Zz | Nz |
| L0B Buffer | Zn | Zn |
| L0C Buffer | Nz | Nz |

Taking `FRACTAL_Nz` as an example, the memory layout uses column-major order for the large N dimension (outer) plus row-major order for the small z dimension (inner):

![FRACTAL_Nz memory layout diagram](../../../../figures/nz_layout.png)

#### Bank Conflict

To improve the efficiency and throughput of data access, the internal storage adopts a bank (memory modules of equal size) structure design. Taking UB as an example, on products with [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), the total UB size is 256 KB (256 x 1024 bytes), containing 8 bank groups, each of which contains 2 banks. Each bank is 16 KB in size and consists of 512 rows, each row being 32 B in length, using low-bit address interleaving. For the UB capacity specifications of [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md), see [UB bank structure](../../../../operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/avoid_bank_conflict_npu_arch_2201.md).

![UB Bank Conflict Diagram](../../../../figures/ub_conflict.png)

This architecture uses low-bit addressing, and the address layout is shown in the following figure. Each bank can independently perform data read and write operations, allowing multiple data requests to proceed simultaneously. However, when multiple read and write operations attempt to access the same bank at the same time, due to hardware resource limitations, these operations must be queued, which causes bank conflicts and degrades performance.

![Low-bit Interleaved Address Layout](../../../../figures/low_interleave.png)

Each bank can complete only one read or one write per beat, and each bank group allows at most 2 reads and 0 writes, or 1 read and 1 write. Based on the memory structure, bank conflicts can be mainly classified into the following three types:
- Read-write conflict: A read operation and a write operation simultaneously attempt to access the same bank, or two read operations and one write operation simultaneously attempt to access the same bank group.
- Write-write conflict: Multiple write operations attempt to access the same bank group at the same time.
- Read-read conflict: Two read operations access the same bank, or more than two operations simultaneously attempt to access the same bank group.

Taking the read-write conflict as an example, when performing a set of vector addition computations `dst = src0 + src1`, if the start address of `src0` is 0x0000 and the start address of `dst` is 0x4000, combined with the bank layout, it can be seen that the addresses of `src0` and `dst` currently belong to the same bank and the same bank group respectively, so a read-write conflict exists.

When a bank conflict occurs, it degrades operator performance. You can usually avoid conflicts by optimizing the computation logic and the address allocation method. For details, see [Avoiding Bank Conflicts](../../../../operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/avoid_bank_conflict_npu_arch_3510.md).

Properly planning memory addresses can avoid bank conflicts and improve memory access throughput. However, the DMA, Vector, and Cube pipelines inside the AI Core execute asynchronously, and data reads and writes have strong dependencies. A synchronization mechanism is required to manage the execution timing of multiple pipelines and avoid dirty reads and computation errors.

## Synchronization Mechanism

The preceding sections introduced the multi-level storage and compute unit architecture of the AI Core. Because units such as DMA data movement, vector computation, and cube computation all execute asynchronously and in parallel, data read/write and computation operations have strong dependencies. Therefore, a synchronization mechanism must be used to manage the execution timing and ensure correct program logic. In programming scenarios, synchronization refers to coordinating the order and timing of access to shared resources by multiple execution units (such as threads and processes) to ensure the correctness of program execution results. If the execution order is not strictly controlled through a synchronization mechanism, problems such as inconsistent computation results and unpredictable program behavior may occur.

AI Core synchronization is mainly divided into **intra-core synchronization** and **inter-core synchronization**.

### Intra-Core Synchronization

Because the execution units inside the AI Core (such as the MTE2 data movement unit and the Vector compute unit) run in an asynchronous and parallel manner, data dependencies may exist when reading from and writing to the same storage resource. To ensure data consistency and computation correctness, synchronization control is required to coordinate the operation timing.

The following figure illustrates a common Vector computation data flow:

1. Use DMA to move data from GM into Local Memory.
2. Perform vector computation.
3. Use DMA to move data from Local Memory out to GM.

![Vector computation data flow](../../../../figures/vec_flow.png)

The four execution units Scalar, Vector, DMA (MTE2), and DMA (MTE3) execute in parallel. If they access the same Local Memory, a synchronization mechanism is required to control their access timing: ensure that data is first moved into Local Memory before computation, and moved out after computation is complete.

![Synchronization sequence diagram](../../../../figures/sync_order.png)

> 📌 Note: The preceding example describes the main path. When the `Scalar` execution unit reads from or writes to GM or UB, the synchronization between the `Scalar` pipeline and other pipelines must also be considered.

The execution units on the AI Core belong to different execution pipelines. Synchronization ensures that different execution pipelines execute correctly. The following lists the main execution pipelines within the AI Core.

| Pipeline Type | Meaning |
|---------|------|
| **PIPE_S** | Scalar pipeline |
| **PIPE_V** | Vector computation pipeline and, on some hardware architectures, the L0C Buffer→UB data movement pipeline |
| **PIPE_M** | Cube computation pipeline |
| **PIPE_MTE1** | L1 Buffer→L0A Buffer and L1 Buffer→L0B Buffer data movement pipeline |
| **PIPE_MTE2** | GM→L1 Buffer and GM→UB data movement pipeline |
| **PIPE_MTE3** | UB→GM and L1 Buffer→GM data movement pipeline |
| **PIPE_FIX** | L0C Buffer→GM and L0C Buffer→L1 data movement pipeline |

To ensure correct execution between pipelines, the AI Core provides multiple synchronization mechanisms for different application scenarios. Developers can flexibly select an appropriate synchronization method based on actual requirements to implement operator functions.

| Synchronization Type | Core Function and Scenario Description | Key API | Implementation Code Example |
|---------|-------------------|----------|-------------|
| **Multi-pipeline synchronization (mechanism 1)** | The Mutex lock is used for synchronization between asynchronous pipelines within a core, functioning similarly to the traditional CPU lock mechanism. It is newly introduced in [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md) and is simpler than SetFlag/WaitFlag. | `Mutex::Lock` & `Mutex::Unlock` | `AscendC::Mutex::Lock<PIPE_MTE2>(mutexId);`<br>`AscendC::DataCopy(srcLocal, srcGlobal, dataSize);`<br>`AscendC::Mutex::Unlock<PIPE_MTE2>(mutexId);`<br>`AscendC::Mutex::Lock<PIPE_V>(mutexId);`<br>`AscendC::Add(dstLocal, src0Local, src1Local, dataSize);`<br>`AscendC::Mutex::Unlock<PIPE_V>(mutexId);` |
| **Multi-pipeline synchronization (mechanism 2)** | SetFlag and WaitFlag implement synchronization and waiting between two pipelines. After the introduction of Mutex, they remain compatible, but the Mutex mechanism is preferred. | `SetFlag` & `WaitFlag` | `AscendC::DataCopy(srcLocal, srcGlobal, dataSize);`<br>`AscendC::SetFlag<HardEvent::MTE2_V>(eventId);`<br>`AscendC::WaitFlag<HardEvent::MTE2_V>(eventId);`<br>`AscendC::Add(dstLocal, src0Local, src1Local, count);` |
| **Single-pipeline synchronization** | PipeBarrier waits for the completion of preceding instructions within the same pipeline, and is used for data dependencies within the same pipeline. | `PipeBarrier` | `AscendC::Add(dstLocal, src0Local, src1Local, count);`<br>`AscendC::PipeBarrier<PIPE_V>();`<br>`AscendC::Sub(dstLocal, dstLocal, src0Local, count);` |
| **Memory access order control** | Waits for the completion of all preceding memory accesses, and is used in scenarios with data dependencies or memory dependencies. | `DataSyncBarrier` | `AscendC::DataSyncBarrier<AscendC::MemDsbT::DDR>();` |

### Inter-Core Synchronization

When the data that a core's computation depends on can only be executed after the computations of other related cores are all complete, inter-core synchronization is required to ensure correct timing across multiple cores.

Take the following figure as an example. AIC (AI Cube, mainly used for cube computation) depends on the ReduceSum computation result of AIV (AI Vector, responsible for vector computation). Because the overall vector is large, it must be split into multiple parts, with each AIV completing part of the computation. Each AIV writes its partial computation result to GM through atomic accumulation. What AIC needs to read must be the final result after all AIVs have completed the accumulation.

![Inter-core synchronization business scenario example](../../../../figures/inter_core_sync_ex.png)

As shown in the preceding figure, the current AI Core consists of AIC cores and AIV cores, where AIC cores are mainly used for cube computation and AIV cores are responsible for vector computation. AIC/AIV are divided by group. Within a group, blocks and subblocks are further divided, with a block-to-subblock ratio of 1:N (N>=1). A block indicates how many "primary cores" there are, and a subblock indicates how many "secondary cores" one "primary core" carries.

![Relationship between block and subblock](../../../../figures/block_rel.png)

Operators can be divided into three categories by computation characteristics: Cube operators (cube computation), Vector operators (vector computation), and CV fused operators (mixed cube and vector computation). The operator type determines the selection of its inter-core synchronization mode and group configuration mode. For different operator scenarios, C++ Tensor programming implements inter-core synchronization by combining the two interfaces [CrossCoreSetFlag](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreSetFlag_ISASI.md) and [CrossCoreWaitFlag](../../../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/CrossCoreWaitFlag_ISASI.md) to meet diverse operator development requirements.

| Inter-core Synchronization Type | Scenario Description | Implementation Code Example |
|---|---|---|
| **Synchronize all AIC cores** | Synchronizes all AIC cores, typically used for multi-core accumulation of Cube operator computation results. | `if ASCEND_IS_AIC {`<br>`    AscendC::CrossCoreSetFlag<0x0, PIPE_FIX>(flagID);`<br>`    AscendC::CrossCoreWaitFlag<0x0>(flagID);`<br>`}` |
| **Synchronize all AIV cores** | Synchronizes all AIV cores, typically used for multi-core accumulation of Vector operator computation results. | `if ASCEND_IS_AIV {`<br>`    AscendC::CrossCoreSetFlag<0x0, PIPE_MTE3>(flagID);`<br>`    AscendC::CrossCoreWaitFlag<0x0>(flagID);`<br>`}` |
| **Synchronize two AIV sub-cores within a single AI Core** | Synchronizes two AIV sub-cores within a single AI Core, typically used for scenarios in Vector operators that require group-based synchronization. | `if ASCEND_IS_AIV {`<br>`    AscendC::CrossCoreSetFlag<0x1, PIPE_MTE3>(flagID);`<br>`    AscendC::CrossCoreWaitFlag<0x1>(flagID);`<br>`}` |
| **Synchronize AIC and AIV within a single AI Core** | Synchronizes AIC and AIV cores within a single AI Core, typically used for interaction between Cube and Vector in CV fused operators. AIC triggers two AIVs to wait simultaneously. | `if ASCEND_IS_AIC {`<br>`    AscendC::CrossCoreSetFlag<0x2, PIPE_FIX>(0);`<br>`}`<br>`if ASCEND_IS_AIV {`<br>`    AscendC::CrossCoreWaitFlag<0x2>(0);`<br>`}` |
| **Synchronize AIC and AIV within a single AI Core (newly introduced in [NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md))** | Synchronizes AIC and AIV cores within a single AI Core, typically used for interaction between Cube and Vector in CV fused operators. AIV0 and AIV1 can trigger AIC to wait independently. | `if ASCEND_IS_AIC {`<br>`    AscendC::CrossCoreSetFlag<0x4, PIPE_FIX>(0);`<br>`    AscendC::CrossCoreSetFlag<0x4, PIPE_FIX>(16);`<br>`}`<br>`if ASCEND_IS_AIV {`<br>`    AscendC::CrossCoreWaitFlag<0x4>(0);`<br>`}` |

> 📌 When developing fused operators based on the decoupled mode (AIC and AIV separated), the operator logic usually contains both AIV and AIC processing logic. In this case, use the macros ASCEND_IS_AIV/ASCEND_IS_AIC provided by Ascend C to isolate the AIV and AIC code.
>
> In the preceding table, the first template parameter of `CrossCoreSetFlag` is the synchronization target identifier. Different values correspond to different synchronization scopes: `0x0` indicates synchronization of all cores, `0x1` indicates synchronization between AIV sub-cores within a single AI Core, `0x2` indicates synchronization between AIC and AIV within a single AI Core (two AIVs trigger simultaneously), and `0x4` indicates synchronization between AIC and AIV within a single AI Core (AIV0 and AIV1 can trigger independently, newly introduced in [NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md)).

## Operator Type

Combined with the AI Core compute units, multi-level memory architecture, and synchronization mechanism introduced earlier, three types of operators can be developed based on hardware capabilities: vector operators, cube operators, and fused operators. As explained in the preceding chapters on hardware architecture: the AI Core contains two types of core compute units, Vector (vector computation) and Cube (cube computation). The Cube compute unit is deployed on AIC cores, and the Vector compute unit is deployed on AIV cores. Each core is equipped with an independent Scalar unit and can independently load its own code segment. Based on this, developers can implement Vector operators,  Cube operators, and CV fused operators.

### Vector Computation

Vector computation mainly includes basic arithmetic, reduction computation, and data type conversion.

On [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md), the data for vector computation comes from UB (32B alignment required). C++ Tensor programming provides the capability to directly operate and compute through LocalTensor on the Vector compute unit. This programming approach that directly operates on UB is called Memory vector computation programming.

Take Add computation as an example. When developers need to compute vector addition of length 4096, they can directly perform the computation through the `Add(dstTensor, src0Tensor, src1Tensor, count)` API. For more details, see [Memory Vector Computation Programming](./memory_vector_computation.md).

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

[NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md) is a new-generation architecture that introduces register programmability. On top of the traditional UB cache system, it opens up register programmability and builds a three-level memory hierarchy of "GM → UB → Register". In the new-generation architecture, vector computation requires data to be loaded from UB to Register first, and then the computation is performed using the Register. For details, see [Reg Vector Computation Programming](./reg_vector_computation.md).

> 📌 Register programmability is introduced starting from [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md).

### Cube Computation

Cube computation mainly covers cube multiplication and convolution, and its data mainly comes from the L1 Buffer and L0A/B Buffer. The L1 Buffer is used for cube computation data caching, the L0A Buffer is used to store the left cube, the L0B Buffer is used to store the right cube, and the L0C Buffer is used to store the initial accumulation values and the cube computation results. This programming paradigm, in which the Cube compute unit directly operates the L1/L0A/L0B/L0C Buffers, is called [Cube Computation Programming](./cube_matrix_computation.md).

Taking Matmul cube multiplication as an example, developers can complete the computation of c = a * b + bias as shown in the following figure by operating the  Cube computation capability.
- A and B are source operands. A is the left cube with the shape [M, K], and B is the right cube with the shape [K, N].
- C is the destination operand, which is the cube that stores the cube multiplication result, with the shape [M, N].
- bias is the cube multiplication bias with the shape [1, N]. This bias is applied to each row of the A*B result cube.

![Cube multiplication diagram](../../../../figures/matmul.png)

```cpp
// Define  Cube operator, marked with __cube__ to execute on Device Cube core, suitable for operators with only Cube computation
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
    // Cube data transfer out
}
```

### CV Fusion Computation

A CV fusion computation program is formed by fusing multiple independent small operators. Its functionality is equivalent to that of multiple small operators, and its performance is usually better than that of independent small operators. Based on actual business scenario requirements, users can freely fuse Vector and Cube operators according to specific algorithms to achieve performance gains. Operators that fuse Cube computation and Vector computation are collectively referred to as CV fused operators.

For example, for Flash Attention, one of the most core fused operators in LLM large models, its core implementation is shown in the following figure. The Matmul operator (Cube), Scale operator (Vector), Mask operator (Vector), and SoftMax operator (Vector) in the figure are fused into one large operator, Flash Attention.

![Flash Attention core implementation](../../../../figures/flash_attn.png)

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

On the basis of improving performance and fully leveraging computing power, fused operators reduce computation and memory usage by merging operators, optimize data flow transmission, and simplify code implementation, thereby simultaneously improving computation efficiency, resource utilization, and code quality.

## Programming Constraints and Limitations

- Synchronization events are manually inserted by the developer using `SetFlag/WaitFlag(ISASI)` and `PipeBarrier(ISASI)`. The event type and event ID are managed by the developer. Note that event IDs 6 and 7 cannot be used (they may conflict with internally used event IDs, resulting in undefined behavior).
- At the kernel entry, the developer must manually call the [InitSocState](../../../../../api/SIMD-API/basic_api/tool_interface/system_init/InitSocState.md) API to initialize the global status registers. Because the global status registers are in an indeterminate state, failure to call this API may cause undefined behavior during operator execution. In TPipe framework programming, initialization is performed by TPipe and does not require developer attention.
- Before the kernel exits, the developer must manually call [PipeBarrier<PIPE_ALL>()](../../../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/PipeBarrier_ISASI.md).
- Some APIs are implemented internally through software simulation and occupy the UB space reserved by Ascend C. When using these APIs, the developer must check whether the target API depends on this reserved space (for [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md), APIs such as Select internally use 8 KB of reserved UB space to store intermediate data; for [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), APIs such as Exp internally use 2 KB of reserved UB space for instruction compatibility or to store intermediate data). If the --cce-disable-asc-reserved-ubuf compilation option is enabled, calling the related APIs on the corresponding product version triggers a compile-time error. For the specific API scope, see [APIs That Use the Reserved UB Space](#section_reserved_ubuf_api).
- In addition, some other software-simulated APIs internally occupy system-allocated event IDs. If the developer manually calls `SetFlag/WaitFlag(ISASI)` to insert custom event IDs, these IDs may conflict with the event IDs used inside the APIs. Therefore, only some APIs are supported when the user defines custom event IDs. For the list of supported APIs, see [Supported API Scope](#section2633193623711).

## Supported APIs<a name="section2633193623711"></a>

<!-- npu="310p" id1 -->
**Table 1** APIs supported for the AI Core of Atlas inference products

<a name="table56285428438"></a>
<table><thead align="left"><tr id="row46697425436"><th class="cellrowborder" valign="top" width="30.28%" id="mcps1.2.3.1.1"><p id="p866911429430"><a name="p866911429430"></a><a name="p866911429430"></a>API Category</p></th>
<th class="cellrowborder" valign="top" width="69.72%" id="mcps1.2.3.1.2"><p id="p186691442174319"><a name="p186691442174319"></a><a name="p186691442174319"></a>API Name</p></th>
</tr>
</thead>
<tbody><tr id="row146691342124318"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p1669154234318"><a name="p1669154234318"></a><a name="p1669154234318"></a>Basic API &gt; Scalar Computation</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p0669164213434"><a name="p0669164213434"></a><a name="p0669164213434"></a>ScalarGetCountOfValue, ScalarCountLeadingZero, ScalarCast, CountBitsCntSameAsSignBit, ScalarGetSFFValue</p></td>
</tr>
<tr id="row1966914211437"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p7669174224314"><a name="p7669174224314"></a><a name="p7669174224314"></a>Basic API &gt; Vector Computation &gt; Basic Arithmetic</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p3669184224317"><a name="p3669184224317"></a><a name="p3669184224317"></a>Exp, Ln, Abs, Reciprocal, Sqrt, Rsqrt, Relu, VectorPadding, Add, Sub, Mul, Div, Max, Min, BilinearInterpolation, Adds, Muls, Maxs, Mins, LeakyRelu</p></td>
</tr>
<tr id="row666974224312"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p2066924274314"><a name="p2066924274314"></a><a name="p2066924274314"></a>Basic API &gt; Vector Computation &gt; Logical Computation</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p1166964217431"><a name="p1166964217431"></a><a name="p1166964217431"></a>Not, And, Or</p></td>
</tr>
<tr id="row46691542124317"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p766994224314"><a name="p766994224314"></a><a name="p766994224314"></a>Basic API &gt; Vector Computation &gt; Compound Computation</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p1559917570124"><a name="p1559917570124"></a><a name="p1559917570124"></a>Axpy, CastDequant, AddRelu, AddReluCast, AddDeqRelu, SubRelu, SubReluCast, MulAddDst, MulCast, FusedMulAdd, FusedMulAddRelu</p></td>
</tr>
<tr id="row176691942154313"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p126691442184311"><a name="p126691442184311"></a><a name="p126691442184311"></a>Basic API &gt; Vector Computation &gt; Comparison and Selection</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p1670442104314"><a name="p1670442104314"></a><a name="p1670442104314"></a>Compare, Compare (Store Result In Register), CompareScalar, GetCmpMask, SetCmpMask,</p>
<p id="p367013429431"><a name="p367013429431"></a><a name="p367013429431"></a>Select, GatherMask</p></td>
</tr>
<tr id="row36706425430"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p8670124211438"><a name="p8670124211438"></a><a name="p8670124211438"></a>Basic API &gt; Vector Computation &gt; Type Conversion</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p166709429433"><a name="p166709429433"></a><a name="p166709429433"></a>Cast</p></td>
</tr>
<tr id="row267074213433"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p1167094274316"><a name="p1167094274316"></a><a name="p1167094274316"></a>Basic API &gt; Vector Computation &gt; Reduction Computation</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p156703427434"><a name="p156703427434"></a><a name="p156703427434"></a>WholeReduceMax, WholeReduceMin, WholeReduceSum, BlockReduceMax, BlockReduceMin, BlockReduceSum, PairReduceSum, RepeatReduceSum, GetReduceMaxMinCount</p></td>
</tr>
<tr id="row067015420439"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p667094294319"><a name="p667094294319"></a><a name="p667094294319"></a>Basic API &gt; Vector Computation &gt; Data Conversion</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p166701427437"><a name="p166701427437"></a><a name="p166701427437"></a>Transpose, TransDataTo5HD</p></td>
</tr>
<tr id="row66709428433"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p7670842124315"><a name="p7670842124315"></a><a name="p7670842124315"></a>Basic API &gt; Vector Computation &gt; Data Fill</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p1867064211432"><a name="p1867064211432"></a><a name="p1867064211432"></a>Duplicate</p></td>
</tr>
<tr id="row11670174211438"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p767044244315"><a name="p767044244315"></a><a name="p767044244315"></a>Basic API &gt; Vector Computation &gt; Sorting and Combination</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p196701142204317"><a name="p196701142204317"></a><a name="p196701142204317"></a>ProposalConcat, ProposalExtract, RpSort16, MrgSort4, GetMrgSortResult</p></td>
</tr>
<tr id="row86708421431"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p1367054274312"><a name="p1367054274312"></a><a name="p1367054274312"></a>Basic API &gt; Vector Computation &gt; Discrete and Aggregate</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p6670442114317"><a name="p6670442114317"></a><a name="p6670442114317"></a>Gather, Scatter</p></td>
</tr>
<tr id="row467024220436"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p206701242204318"><a name="p206701242204318"></a><a name="p206701242204318"></a>Basic API &gt; Vector Computation &gt; Mask Operation</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p1670134217439"><a name="p1670134217439"></a><a name="p1670134217439"></a>SetMaskCount, SetMaskNorm, SetVectorMask, ResetMask</p></td>
</tr>
<tr id="row11670142144320"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p1670174218434"><a name="p1670174218434"></a><a name="p1670174218434"></a>Basic API &gt; Vector Computation &gt; Quantization Setting</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p14670144210437"><a name="p14670144210437"></a><a name="p14670144210437"></a>SetDeqScale</p></td>
</tr>
<tr id="row8670194214432"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p20670174219436"><a name="p20670174219436"></a><a name="p20670174219436"></a>Basic API &gt; Data Movement &gt; DataCopy</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p1567014216432"><a name="p1567014216432"></a><a name="p1567014216432"></a>Basic Data Movement</p></td>
</tr>
<tr id="row166701142174317"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p126701142174316"><a name="p126701142174316"></a><a name="p126701142174316"></a>Basic API &gt; Synchronization Control &gt; Intra-core Synchronization</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p167024224318"><a name="p167024224318"></a><a name="p167024224318"></a>SetFlag/WaitFlag, PipeBarrier</p></td>
</tr>
<tr id="row166719424430"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p1067184244310"><a name="p1067184244310"></a><a name="p1067184244310"></a>Basic API &gt; Cache Control</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p20671042134320"><a name="p20671042134320"></a><a name="p20671042134320"></a>DataCachePreload, DataCacheCleanAndInvalid, ICachePreLoad</p></td>
</tr>
<tr id="row12671124274312"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p1867119427433"><a name="p1867119427433"></a><a name="p1867119427433"></a>Basic API &gt; System Variable Access</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p06713423434"><a name="p06713423434"></a><a name="p06713423434"></a>GetBlockNum, GetBlockIdx, GetDataBlockSizeInBytes, GetArchVersion, GetTaskRatio, InitSocState, GetProgramCounter, CheckLocalMemoryIA</p></td>
</tr>
<tr id="row96714427439"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p106711742154319"><a name="p106711742154319"></a><a name="p106711742154319"></a>Basic API &gt; Atomic Operation</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p17671042164319"><a name="p17671042164319"></a><a name="p17671042164319"></a>SetAtomicAdd, DisableDmaAtomic</p></td>
</tr>
<tr id="row13671194254311"><td class="cellrowborder" valign="top" width="30.28%" headers="mcps1.2.3.1.1 "><p id="p6671184224317"><a name="p6671184224317"></a><a name="p6671184224317"></a>Basic API &gt; Cube Computation</p></td>
<td class="cellrowborder" valign="top" width="69.72%" headers="mcps1.2.3.1.2 "><p id="p147142311613"><a name="p147142311613"></a><a name="p147142311613"></a>InitConstValue, LoadData, SetAippFunctions, LoadImageToLocal, LoadUnzipIndex, LoadDataUnzip, SetLoadDataBoundary, SetLoadDataPaddingValue, Mmad</p></td>
</tr>
</tbody>
</table>
<!-- end id1 -->

<!-- npu="910b" id2 -->
**Table 2**  Supported API scope for Atlas A2 products

<a name="table1842818536431"></a>
<table><thead align="left"><tr id="row104742531436"><th class="cellrowborder" valign="top" width="29.9%" id="mcps1.2.4.1.1"><p id="p10474205364319"><a name="p10474205364319"></a><a name="p10474205364319"></a>API Classification</p></th>
<th class="cellrowborder" valign="top" width="44.25%" id="mcps1.2.4.1.2"><p id="p1047418533430"><a name="p1047418533430"></a><a name="p1047418533430"></a>API Name</p></th>
<th class="cellrowborder" valign="top" width="25.85%" id="mcps1.2.4.1.3"><p id="p4474125374319"><a name="p4474125374319"></a><a name="p4474125374319"></a>Remarks</p></th>
</tr>
</thead>
<tbody><tr id="row747410531434"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p12474153154311"><a name="p12474153154311"></a><a name="p12474153154311"></a>Basic API &gt; Scalar Computation</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p5821115016275"><a name="p5821115016275"></a><a name="p5821115016275"></a>GetBitCount, CountLeadingZero, CountBitsCntSameAsSignBit, GetSFFValue, Cast (float to half, int32_t), Cast (float to bfloat16_t), Cast (multiple types to float)</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p10474175318436"><a name="p10474175318436"></a><a name="p10474175318436"></a>-</p></td>
</tr>
<tr id="row64759532431"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p747511535436"><a name="p747511535436"></a><a name="p747511535436"></a>Basic API &gt; Vector Computation &gt; Basic Arithmetic</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p10475753174311"><a name="p10475753174311"></a><a name="p10475753174311"></a>Exp, Ln, Abs, Reciprocal, Sqrt, Rsqrt, Relu, Add, Sub, Mul, Div, Max, Min, BilinearInterpolation, Adds, Muls, Maxs, Mins, LeakyRelu</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p114751853194317"><a name="p114751853194317"></a><a name="p114751853194317"></a>-</p></td>
</tr>
<tr id="row1647525319432"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p447513537436"><a name="p447513537436"></a><a name="p447513537436"></a>Basic API &gt; Vector Computation &gt; Logical Computation</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p747525319434"><a name="p747525319434"></a><a name="p747525319434"></a>Not, And, Or, ShiftLeft, ShiftRight</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p84751853144316"><a name="p84751853144316"></a><a name="p84751853144316"></a>-</p></td>
</tr>
<tr id="row1247575318435"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p164751053124316"><a name="p164751053124316"></a><a name="p164751053124316"></a>Basic API &gt; Vector Computation &gt; Compound Computation</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p1169817233416"><a name="p1169817233416"></a><a name="p1169817233416"></a>Axpy, CastDequant, AddRelu, AddReluCast, SubRelu, SubReluCast, MulAddDst, MulCast, FusedMulAdd, FusedMulAddRelu</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p5475153164312"><a name="p5475153164312"></a><a name="p5475153164312"></a>-</p></td>
</tr>
<tr id="row13475185394314"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p20475105310432"><a name="p20475105310432"></a><a name="p20475105310432"></a>Basic API &gt; Vector Computation &gt; Comparison and Selection</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p16475553114318"><a name="p16475553114318"></a><a name="p16475553114318"></a>Compare, Compare (Store Result In Register), Compares, GetCmpMask, SetCmpMask, GatherMask</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p6475653204318"><a name="p6475653204318"></a><a name="p6475653204318"></a>-</p></td>
</tr>
<tr id="row18475195364317"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p4475853204310"><a name="p4475853204310"></a><a name="p4475853204310"></a>Basic API &gt; Vector Computation &gt; Type Conversion</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p18475253154318"><a name="p18475253154318"></a><a name="p18475253154318"></a>Cast</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p847515384318"><a name="p847515384318"></a><a name="p847515384318"></a>-</p></td>
</tr>
<tr id="row1047545324316"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p14475165364311"><a name="p14475165364311"></a><a name="p14475165364311"></a>Basic API &gt; Vector Computation &gt; Reduction Computation</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p847595312435"><a name="p847595312435"></a><a name="p847595312435"></a>WholeReduceMax, WholeReduceMin, WholeReduceSum, BlockReduceMax, BlockReduceMin, BlockReduceSum, PairReduceSum, RepeatReduceSum, GetReduceRepeatSumSpr, GetReduceRepeatMaxMinSpr</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p18475115318430"><a name="p18475115318430"></a><a name="p18475115318430"></a>-</p></td>
</tr>
<tr id="row947565318439"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p54759533439"><a name="p54759533439"></a><a name="p54759533439"></a>Basic API &gt; Vector Computation &gt; Data Conversion</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p3475553184310"><a name="p3475553184310"></a><a name="p3475553184310"></a>Transpose, TransDataTo5HD</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p147451518192713"><a name="p147451518192713"></a><a name="p147451518192713"></a>TransDataTo5HD (the scenario where dst and src are LocalTensor of the uint64_t type is not supported)</p>
<p id="p2047518539434"><a name="p2047518539434"></a><a name="p2047518539434"></a></p></td>
</tr>
<tr id="row847545310433"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p1347555344319"><a name="p1347555344319"></a><a name="p1347555344319"></a>Basic API &gt; Vector Computation &gt; Data Fill</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p2475135318433"><a name="p2475135318433"></a><a name="p2475135318433"></a>Duplicate, Brcb</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1647535364315"><a name="p1647535364315"></a><a name="p1647535364315"></a>-</p></td>
</tr>
<tr id="row7475175384319"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p1147575304315"><a name="p1147575304315"></a><a name="p1147575304315"></a>Basic API &gt; Vector Computation &gt; Sorting and Combination</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p747595317433"><a name="p747595317433"></a><a name="p747595317433"></a>Sort32, MrgSort, GetMrgSortResult</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1547545312433"><a name="p1547545312433"></a><a name="p1547545312433"></a>-</p></td>
</tr>
<tr id="row104754537434"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p11475105315431"><a name="p11475105315431"></a><a name="p11475105315431"></a>Basic API &gt; Vector Computation &gt; Discrete and Aggregate</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p1147612530434"><a name="p1147612530434"></a><a name="p1147612530434"></a>Gather, Gatherb</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p347635384311"><a name="p347635384311"></a><a name="p347635384311"></a>-</p></td>
</tr>
<tr id="row9476353114310"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p1547675319439"><a name="p1547675319439"></a><a name="p1547675319439"></a>Basic API &gt; Vector Computation &gt; Mask Operation</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p18476053164310"><a name="p18476053164310"></a><a name="p18476053164310"></a>SetMaskCount, SetMaskNorm, SetVectorMask, ResetMask</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1047610535435"><a name="p1047610535435"></a><a name="p1047610535435"></a>-</p></td>
</tr>
<tr id="row1147614531435"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p4476185324312"><a name="p4476185324312"></a><a name="p4476185324312"></a>Basic API &gt; Vector Computation &gt; Quantization Setting</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p247625310439"><a name="p247625310439"></a><a name="p247625310439"></a>SetDeqScale</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p19476185312436"><a name="p19476185312436"></a><a name="p19476185312436"></a>-</p></td>
</tr>
<tr id="row447613539435"><td class="cellrowborder" rowspan="4" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p1947685374310"><a name="p1947685374310"></a><a name="p1947685374310"></a>Basic API &gt; Data Movement &gt; DataCopy</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p34761253144318"><a name="p34761253144318"></a><a name="p34761253144318"></a>Basic Data Movement</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p19476205324319"><a name="p19476205324319"></a><a name="p19476205324319"></a>Data movement through the UB (VECIN/VECCALC/VECOUT)-&gt;L1 Buffer (TSCM) path is not supported.</p></td>
</tr>
<tr id="row154764536438"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p16476353104318"><a name="p16476353104318"></a><a name="p16476353104318"></a>Enhanced Data Movement</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p9476165310436"><a name="p9476165310436"></a><a name="p9476165310436"></a>Data movement through the UB (VECIN/VECCALC/VECOUT)-&gt;L1 Buffer (TSCM) path is not supported.</p></td>
</tr>
<tr id="row2476185317437"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p4476253144320"><a name="p4476253144320"></a><a name="p4476253144320"></a>Sliced Data Movement</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1447635324319"><a name="p1447635324319"></a><a name="p1447635324319"></a>-</p></td>
</tr>
<tr id="row1847610536433"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p11975173673218"><a name="p11975173673218"></a><a name="p11975173673218"></a>ND2NZ movement with in-path conversion</p>
<p id="p5975136163216"><a name="p5975136163216"></a><a name="p5975136163216"></a>NZ2ND movement with in-path conversion</p>
<p id="p1397583616325"><a name="p1397583616325"></a><a name="p1397583616325"></a>Quantized activation movement with in-path conversion</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1047612538437"><a name="p1047612538437"></a><a name="p1047612538437"></a>The in-path ND2NZ conversion does not support data movement on the UB (VECIN/VECCALC/VECOUT) -> L1 Buffer (TSCM) path.</p></td>
</tr>
<tr id="row747635344315"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p547665354313"><a name="p547665354313"></a><a name="p547665354313"></a>Basic API > Data Movement</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p4476135364311"><a name="p4476135364311"></a><a name="p4476135364311"></a>Copy, DataCopyPad, SetPadValue</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p8476165374314"><a name="p8476165374314"></a><a name="p8476165374314"></a>DataCopyPad does not support data movement on the UB (VECIN/VECCALC/VECOUT) -> L1 Buffer (TSCM) path.</p></td>
</tr>
<tr id="row114761053204315"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p2047615394311"><a name="p2047615394311"></a><a name="p2047615394311"></a>Basic API > Synchronization Control > Intra-core Synchronization</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p14476853154310"><a name="p14476853154310"></a><a name="p14476853154310"></a>SetFlag/WaitFlag, PipeBarrier, DataSyncBarrier</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p947605334315"><a name="p947605334315"></a><a name="p947605334315"></a>-</p></td>
</tr>
<tr id="row1947615537431"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p04761853204310"><a name="p04761853204310"></a><a name="p04761853204310"></a>Basic API > Synchronization Control > Inter-core Synchronization</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p10476115315430"><a name="p10476115315430"></a><a name="p10476115315430"></a>CrossCoreSetFlag, CrossCoreWaitFlag</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p6476155320439"><a name="p6476155320439"></a><a name="p6476155320439"></a>-</p></td>
</tr>
<tr id="row1547655354318"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p54761353104315"><a name="p54761353104315"></a><a name="p54761353104315"></a>Basic API &gt; Cache Control</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p1476195316434"><a name="p1476195316434"></a><a name="p1476195316434"></a>DataCachePreload, DataCacheCleanAndInvalid, ICachePreLoad, GetICachePreloadStatus</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p104766539434"><a name="p104766539434"></a><a name="p104766539434"></a>-</p></td>
</tr>
<tr id="row847685384313"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p194761534433"><a name="p194761534433"></a><a name="p194761534433"></a>Basic API &gt; System Variable Access</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p647655312439"><a name="p647655312439"></a><a name="p647655312439"></a>GetBlockNum, GetBlockIdx, GetDataBlockSizeInBytes, GetArchVersion, GetTaskRatio, InitSocState, GetProgramCounter, GetSubBlockNum, GetSubBlockIdx, GetSystemCycle,</p>
<p id="p716419515422"><a name="p716419515422"></a><a name="p716419515422"></a>CheckLocalMemoryIA</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p2047615324310"><a name="p2047615324310"></a><a name="p2047615324310"></a>-</p></td>
</tr>
<tr id="row174769538434"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p14765531438"><a name="p14765531438"></a><a name="p14765531438"></a>Basic API &gt; Atomic Operation</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p847615314436"><a name="p847615314436"></a><a name="p847615314436"></a>SetAtomicAdd, SetAtomicType, DisableDmaAtomic, SetAtomicMax, SetAtomicMin, SetStoreAtomicConfig, GetStoreAtomicConfig</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p10476153144316"><a name="p10476153144316"></a><a name="p10476153144316"></a>-</p></td>
</tr>
<tr id="row7476053114311"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p747710531433"><a name="p747710531433"></a><a name="p747710531433"></a>Basic API &gt; Cube Computation</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p847713536432"><a name="p847713536432"></a><a name="p847713536432"></a>Mmad, MmadWithSparse, SetHF32Mode, SetHF32TransMode, SetMMLayoutTransform, SetFixPipeConfig, SetFixpipeNz2ndFlag, SetFixpipePreQuantFlag, InitConstValue, LoadData, LoadDataWithTranspose, SetAippFunctions, LoadImageToLocal, LoadDataWithSparse, SetFmatrix, SetLoadDataBoundary, SetLoadDataRepeat, SetLoadDataPaddingValue, Fixpipe</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p154771853144319"><a name="p154771853144319"></a><a name="p154771853144319"></a>-</p></td>
</tr>
<tr id="row4477125311436"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p1747745334311"><a name="p1747745334311"></a><a name="p1747745334311"></a>Utils API &gt; C++ Standard Library &gt; Algorithms</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p4477125313438"><a name="p4477125313438"></a><a name="p4477125313438"></a>max, min, index_sequence</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p547711539430"><a name="p547711539430"></a><a name="p547711539430"></a>-</p></td>
</tr>
<tr id="row647718531435"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p9477145317433"><a name="p9477145317433"></a><a name="p9477145317433"></a>Utils API &gt; C++ Standard Library &gt; Container Functions</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p12477105344313"><a name="p12477105344313"></a><a name="p12477105344313"></a>tuple, get, make_tuple</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p144771253194313"><a name="p144771253194313"></a><a name="p144771253194313"></a>-</p></td>
</tr>
<tr id="row7477145344310"><td class="cellrowborder" valign="top" width="29.9%" headers="mcps1.2.4.1.1 "><p id="p114771553194319"><a name="p114771553194319"></a><a name="p114771553194319"></a>Utils API &gt; C++ Standard Library &gt; Type Traits</p></td>
<td class="cellrowborder" valign="top" width="44.25%" headers="mcps1.2.4.1.2 "><p id="p9477135312433"><a name="p9477135312433"></a><a name="p9477135312433"></a>is_convertible, is_base_of, is_same, enable_if, conditional</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p047765314438"><a name="p047765314438"></a><a name="p047765314438"></a>-</p></td>
</tr>
</tbody>
</table>
<!-- end id2 -->

<!-- npu="A3" id3 -->
**Table 3** APIs supported for Atlas A3 products

<a name="table167051848128"></a>
<table><thead align="left"><tr id="row07054481922"><th class="cellrowborder" valign="top" width="29.630000000000003%" id="mcps1.2.4.1.1"><p id="p135155581014"><a name="p135155581014"></a><a name="p135155581014"></a>API Classification</p></th>
<th class="cellrowborder" valign="top" width="44.519999999999996%" id="mcps1.2.4.1.2"><p id="p12351955181013"><a name="p12351955181013"></a><a name="p12351955181013"></a>API Name</p></th>
<th class="cellrowborder" valign="top" width="25.85%" id="mcps1.2.4.1.3"><p id="p635165571012"><a name="p635165571012"></a><a name="p635165571012"></a>Remarks</p></th>
</tr>
</thead>
<tbody><tr id="row1870594819210"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1035755151018"><a name="p1035755151018"></a><a name="p1035755151018"></a>Basic API &gt; Scalar Computation</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p193515557101"><a name="p193515557101"></a><a name="p193515557101"></a>GetBitCount, CountLeadingZero, CountBitsCntAsSameSignBit, GetSFFValue, Cast (float to half, int32_t), Cast (float to bfloat16_t), Cast (multiple types to float)</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p2351455171020"><a name="p2351455171020"></a><a name="p2351455171020"></a>-</p></td>
</tr>
<tr id="row1470617481025"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p18361755191019"><a name="p18361755191019"></a><a name="p18361755191019"></a>Basic API &gt; Vector Computation &gt; Basic Arithmetic</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p173685515109"><a name="p173685515109"></a><a name="p173685515109"></a>Exp, Ln, Abs, Reciprocal, Sqrt, Rsqrt, Relu, Add, Sub, Mul, Div, Max, Min, BilinearInterpolation, Adds, Muls, Maxs, Mins, LeakyRelu</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p93614551107"><a name="p93614551107"></a><a name="p93614551107"></a>-</p></td>
</tr>
<tr id="row18706148928"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p12361655191017"><a name="p12361655191017"></a><a name="p12361655191017"></a>Basic API &gt; Vector Computation &gt; Logical Computation</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p10362055181013"><a name="p10362055181013"></a><a name="p10362055181013"></a>Not, And, Or, ShiftLeft, ShiftRight</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p23615581013"><a name="p23615581013"></a><a name="p23615581013"></a>-</p></td>
</tr>
<tr id="row137061481120"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p53695511016"><a name="p53695511016"></a><a name="p53695511016"></a>Basic API &gt; Vector Computation &gt; Compound Computation</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p6361255131014"><a name="p6361255131014"></a><a name="p6361255131014"></a>Axpy, CastDequant, AddRelu, AddReluCast, SubRelu, SubReluCast, MulAddDst, MulCast, FusedMulAdd, FusedMulAddRelu</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p153610555105"><a name="p153610555105"></a><a name="p153610555105"></a>-</p></td>
</tr>
<tr id="row070674812215"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p153615521020"><a name="p153615521020"></a><a name="p153615521020"></a>Basic API &gt; Vector Computation &gt; Comparison and Selection</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p236125551015"><a name="p236125551015"></a><a name="p236125551015"></a>Compare, Compare (result stored in register), Compares, GetCmpMask, SetCmpMask, GatherMask</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p936555181012"><a name="p936555181012"></a><a name="p936555181012"></a>-</p></td>
</tr>
<tr id="row137068483216"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1436955161018"><a name="p1436955161018"></a><a name="p1436955161018"></a>Basic API &gt; Vector Computation &gt; Type Conversion</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1536115511017"><a name="p1536115511017"></a><a name="p1536115511017"></a>Cast</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p5365550108"><a name="p5365550108"></a><a name="p5365550108"></a>-</p></td>
</tr>
<tr id="row1570618486213"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p33635571010"><a name="p33635571010"></a><a name="p33635571010"></a>Basic API &gt; Vector Computation &gt; Reduction Computation</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p103612558107"><a name="p103612558107"></a><a name="p103612558107"></a>WholeReduceMax, WholeReduceMin, WholeReduceSum, BlockReduceMax, BlockReduceMin, BlockReduceSum, PairReduceSum, RepeatReduceSum, GetReduceRepeatSumSpr, GetReduceRepeatMaxMinSpr</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1736195513103"><a name="p1736195513103"></a><a name="p1736195513103"></a>-</p></td>
</tr>
<tr id="row13706104817211"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1736135519109"><a name="p1736135519109"></a><a name="p1736135519109"></a>Basic API &gt; Vector Computation &gt; Data Conversion</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p9369554103"><a name="p9369554103"></a><a name="p9369554103"></a>Transpose, TransDataTo5HD</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p143612554106"><a name="p143612554106"></a><a name="p143612554106"></a>TransDataTo5HD (the scenario where the dst and src types are LocalTensor of uint64_t is not supported)</p></td>
</tr>
<tr id="row670711481724"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p12368557106"><a name="p12368557106"></a><a name="p12368557106"></a>Basic API &gt; Vector Computation &gt; Data Fill</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p136185551010"><a name="p136185551010"></a><a name="p136185551010"></a>Duplicate, Brcb</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p4372055191014"><a name="p4372055191014"></a><a name="p4372055191014"></a>-</p></td>
</tr>
<tr id="row137071648520"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p13379558109"><a name="p13379558109"></a><a name="p13379558109"></a>Basic API &gt; Vector Computation &gt; Sorting and Combination</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p133716557102"><a name="p133716557102"></a><a name="p133716557102"></a>Sort32, MrgSort, GetMrgSortResult</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p93717559104"><a name="p93717559104"></a><a name="p93717559104"></a>-</p></td>
</tr>
<tr id="row14707648226"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1537155514103"><a name="p1537155514103"></a><a name="p1537155514103"></a>Basic API &gt; Vector Computation &gt; Discrete and Aggregate</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p103765551014"><a name="p103765551014"></a><a name="p103765551014"></a>Gather, Gatherb</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p113725561014"><a name="p113725561014"></a><a name="p113725561014"></a>-</p></td>
</tr>
<tr id="row157073482219"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p13795571019"><a name="p13795571019"></a><a name="p13795571019"></a>Basic API &gt; Vector Computation &gt; Mask Operation</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p53785515103"><a name="p53785515103"></a><a name="p53785515103"></a>SetMaskCount, SetMaskNorm, SetVectorMask, ResetMask</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1537105541013"><a name="p1537105541013"></a><a name="p1537105541013"></a>-</p></td>
</tr>
<tr id="row1170719484217"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p737125571012"><a name="p737125571012"></a><a name="p737125571012"></a>Basic API &gt; Vector Computation &gt; Quantization Setting</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p3371755121016"><a name="p3371755121016"></a><a name="p3371755121016"></a>SetDeqScale</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p73765516101"><a name="p73765516101"></a><a name="p73765516101"></a>-</p></td>
</tr>
<tr id="row157073481627"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p123775516109"><a name="p123775516109"></a><a name="p123775516109"></a>Basic API &gt; Data Movement &gt; DataCopy</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p183725521010"><a name="p183725521010"></a><a name="p183725521010"></a>Basic Data Movement</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p337175521010"><a name="p337175521010"></a><a name="p337175521010"></a>Data movement through the UB (VECIN/VECCALC/VECOUT)-&gt;L1 Buffer (TSCM) path is not supported.</p></td>
</tr>
<tr id="row7708104819220"><td class="cellrowborder" rowspan="4" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p5381155151015"><a name="p5381155151015"></a><a name="p5381155151015"></a>Basic API &gt; Data Movement</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p137125510109"><a name="p137125510109"></a><a name="p137125510109"></a>Enhanced Data Movement</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p19371551101"><a name="p19371551101"></a><a name="p19371551101"></a>Data movement through the UB (VECIN/VECCALC/VECOUT) -&gt; L1 Buffer (TSCM) path is not supported.</p></td>
</tr>
<tr id="row7708948725"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p637175591011"><a name="p637175591011"></a><a name="p637175591011"></a>Slice data movement</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1637355101013"><a name="p1637355101013"></a><a name="p1637355101013"></a>-</p></td>
</tr>
<tr id="row57081487219"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p437185512102"><a name="p437185512102"></a><a name="p437185512102"></a>In-path ND2NZ conversion movement</p>
<p id="p9371855171017"><a name="p9371855171017"></a><a name="p9371855171017"></a>In-path NZ2ND conversion movement</p>
<p id="p13381755131011"><a name="p13381755131011"></a><a name="p13381755131011"></a>In-path quantization activation movement</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p938195512100"><a name="p938195512100"></a><a name="p938195512100"></a>In-path ND2NZ conversion movement does not support data movement through the UB (VECIN/VECCALC/VECOUT) -&gt; L1 Buffer (TSCM) path.</p></td>
</tr>
<tr id="row370817486213"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p103895521010"><a name="p103895521010"></a><a name="p103895521010"></a>Copy, DataCopyPad, SetPadValue</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1338185515107"><a name="p1338185515107"></a><a name="p1338185515107"></a>DataCopyPad does not support data movement through the UB (VECIN/VECCALC/VECOUT) -&gt; L1 Buffer (TSCM) path.</p></td>
</tr>
<tr id="row177081648422"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p113885518108"><a name="p113885518108"></a><a name="p113885518108"></a>Basic API &gt; Synchronization Control &gt; Intra-core Synchronization</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1538855191014"><a name="p1538855191014"></a><a name="p1538855191014"></a>SetFlag/WaitFlag, PipeBarrier, DataSyncBarrier</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p838255191017"><a name="p838255191017"></a><a name="p838255191017"></a>-</p></td>
</tr>
<tr id="row37087481225"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p143815519101"><a name="p143815519101"></a><a name="p143815519101"></a>Basic API &gt; Synchronization Control &gt; Inter-core Synchronization</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p53885510109"><a name="p53885510109"></a><a name="p53885510109"></a>CrossCoreSetFlag, CrossCoreWaitFlag</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p83825511014"><a name="p83825511014"></a><a name="p83825511014"></a>-</p></td>
</tr>
<tr id="row1070894819215"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p12388559109"><a name="p12388559109"></a><a name="p12388559109"></a>Basic API &gt; Cache Control</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p103865521014"><a name="p103865521014"></a><a name="p103865521014"></a>DataCachePreload, DataCacheCleanAndInvalid, ICachePreLoad, GetICachePreloadStatus</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p10383551101"><a name="p10383551101"></a><a name="p10383551101"></a>-</p></td>
</tr>
<tr id="row87081348428"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p338155141011"><a name="p338155141011"></a><a name="p338155141011"></a>Basic API &gt; System Variable Access</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p938955181019"><a name="p938955181019"></a><a name="p938955181019"></a>GetBlockNum, GetBlockIdx, GetDataBlockSizeInBytes, GetArchVersion, GetTaskRatio, InitSocState, GetProgramCounter, GetSubBlockNum, GetSubBlockIdx, GetSystemCycle,</p>
<p id="p63865551011"><a name="p63865551011"></a><a name="p63865551011"></a>CheckLocalMemoryIA</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p143815557104"><a name="p143815557104"></a><a name="p143815557104"></a>-</p></td>
</tr>
<tr id="row1170934815213"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p938105531011"><a name="p938105531011"></a><a name="p938105531011"></a>Basic API &gt; Atomic Operation</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1738145591014"><a name="p1738145591014"></a><a name="p1738145591014"></a>SetAtomicAdd, SetAtomicType, DisableDmaAtomic, SetAtomicMax, SetAtomicMin, SetStoreAtomicConfig, GetStoreAtomicConfig</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p5388559106"><a name="p5388559106"></a><a name="p5388559106"></a>-</p></td>
</tr>
<tr id="row67096482218"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p93875511014"><a name="p93875511014"></a><a name="p93875511014"></a>Basic API &gt; cube computation</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p53919552100"><a name="p53919552100"></a><a name="p53919552100"></a>Mmad, MmadWithSparse, SetHF32Mode, SetHF32TransMode, SetMMLayoutTransform, SetFixPipeConfig, SetFixpipeNz2ndFlag, SetFixpipePreQuantFlag, InitConstValue, LoadData, LoadDataWithTranspose, SetAippFunctions, LoadImageToLocal, LoadDataWithSparse, SetFmatrix, SetLoadDataBoundary, SetLoadDataRepeat, SetLoadDataPaddingValue, Fixpipe</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p153985521019"><a name="p153985521019"></a><a name="p153985521019"></a>-</p></td>
</tr>
<tr id="row9709184811214"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p9391855101018"><a name="p9391855101018"></a><a name="p9391855101018"></a>Utils API &gt; C++ Standard Library &gt; Algorithms</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p163955551011"><a name="p163955551011"></a><a name="p163955551011"></a>max, min, index_sequence</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p53965581013"><a name="p53965581013"></a><a name="p53965581013"></a>-</p></td>
</tr>
<tr id="row167094482220"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p14394556105"><a name="p14394556105"></a><a name="p14394556105"></a>Utils API &gt; C++ Standard Library &gt; Container Functions</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p113912552108"><a name="p113912552108"></a><a name="p113912552108"></a>tuple, get, make_tuple</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p13917555103"><a name="p13917555103"></a><a name="p13917555103"></a>-</p></td>
</tr>
<tr id="row97091948726"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p139855171017"><a name="p139855171017"></a><a name="p139855171017"></a>Utils API &gt; C++ Standard Library &gt; Type Traits</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p739155111015"><a name="p739155111015"></a><a name="p739155111015"></a>is_convertible, is_base_of, is_same, enable_if, conditional</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p73935581010"><a name="p73935581010"></a><a name="p73935581010"></a>-</p></td>
</tr>
<tr id="row137091485215"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p914812282549"><a name="p914812282549"></a><a name="p914812282549"></a>advanced API &gt; C++ Standard Library &gt; Type Traits</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1414882810547"><a name="p1414882810547"></a><a name="p1414882810547"></a>is_convertible, is_base_of, is_same, enable_if, conditional</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1514822875412"><a name="p1514822875412"></a><a name="p1514822875412"></a>-</p></td>
</tr>
<tr id="row070934810215"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1642119533450"><a name="p1642119533450"></a><a name="p1642119533450"></a>advanced API &gt; Template Library Functions &gt; type_traits</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p042105320459"><a name="p042105320459"></a><a name="p042105320459"></a>is_convertible, is_base_of, is_same, enable_if, conditional</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p5421553134513"><a name="p5421553134513"></a><a name="p5421553134513"></a>-</p></td>
</tr>
</tbody>
</table>
<!-- end id3 -->

<!-- npu="950" id4 -->
**Table 4**  API range supported for Ascend 950PR&950DT products

<a name="table1798673512413"></a>
<table><thead align="left"><tr id="row109864353413"><th class="cellrowborder" valign="top" width="29.630000000000003%" id="mcps1.2.4.1.1"><p id="p1098613524117"><a name="p1098613524117"></a><a name="p1098613524117"></a>API Classification</p></th>
<th class="cellrowborder" valign="top" width="44.519999999999996%" id="mcps1.2.4.1.2"><p id="p29871535174117"><a name="p29871535174117"></a><a name="p29871535174117"></a>API Name</p></th>
<th class="cellrowborder" valign="top" width="25.85%" id="mcps1.2.4.1.3"><p id="p9987835154115"><a name="p9987835154115"></a><a name="p9987835154115"></a>Remark</p></th>
</tr>
</thead>
<tbody><tr id="row1298763513418"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p15987173524114"><a name="p15987173524114"></a><a name="p15987173524114"></a>Basic API &gt; Scalar Computation</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p12167195363614"><a name="p12167195363614"></a><a name="p12167195363614"></a>GetBitCount, CountLeadingZero, CountBitsCntSameAsSignBit, GetSFFValue, Cast (float to half, int32_t), Cast (float to bfloat16_t), Cast (multi-type to float), Nop, GetUintDivMagicAndShift, WriteGmBypassDCache, ReadGmBypassDCache</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p298711352411"><a name="p298711352411"></a><a name="p298711352411"></a>-</p></td>
</tr>
<tr id="row1098743584116"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p69871035114113"><a name="p69871035114113"></a><a name="p69871035114113"></a>Basic API &gt; Vector Computation &gt; Basic Arithmetic</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1068419974813"><a name="p1068419974813"></a><a name="p1068419974813"></a>Exp, Ln, Abs, Reciprocal, Sqrt, Rsqrt, Relu, Neg, Add, Sub, Mul, Div, Max, Min, BilinearInterpolation, Prelu, Mull, Adds, Adds (flexible scalar position), Muls, Muls (flexible scalar position), Maxs, Maxs (flexible scalar position), Mins, Mins (flexible scalar position), Subs, Divs, LeakyRelu</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p798711356416"><a name="p798711356416"></a><a name="p798711356416"></a>-</p></td>
</tr>
<tr id="row45561731104215"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p35561731164210"><a name="p35561731164210"></a><a name="p35561731164210"></a>Basic API &gt; Vector Computation &gt; Logical Computation</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p18556103154212"><a name="p18556103154212"></a><a name="p18556103154212"></a>Not, And, Or, ShiftLeft (left shift count is a Tensor), ShiftRight (right shift count is a Tensor), Ands, Ors, ShiftLeft, ShiftRight</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p35567315428"><a name="p35567315428"></a><a name="p35567315428"></a>-</p></td>
</tr>
<tr id="row1798773512412"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p189871935184120"><a name="p189871935184120"></a><a name="p189871935184120"></a>Basic API &gt; Vector Computation &gt; Compound Computation</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p10987133511418"><a name="p10987133511418"></a><a name="p10987133511418"></a>Axpy, CastDequant, AddRelu, AddReluCast, AddDeqRelu, SubRelu, SubReluCast, MulAddDst, MulCast, FusedMulAdd, MulAddRelu, AbsSub, FusedExpSub, MulsCast</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p179879356419"><a name="p179879356419"></a><a name="p179879356419"></a>-</p></td>
</tr>
<tr id="row5600192234115"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p7600112274111"><a name="p7600112274111"></a><a name="p7600112274111"></a>Basic API &gt; Vector Computation &gt; Comparison and Selection</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p16681911154710"><a name="p16681911154710"></a><a name="p16681911154710"></a>Compare, Compares, Compares (flexible scalar position), GetCmpMask, SetCmpMask, GatherMask, Select, Select (flexible scalar position)</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p46001422164119"><a name="p46001422164119"></a><a name="p46001422164119"></a>Select does not support: tensor high-bit split computation -> Select mode 0 and mode 2 -> mode 0 in the API that does not pass the mask parameter.</p></td>
</tr>
<tr id="row998718356411"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p5987335194120"><a name="p5987335194120"></a><a name="p5987335194120"></a>Basic API &gt; Vector Computation &gt; Type Conversion</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p198717354419"><a name="p198717354419"></a><a name="p198717354419"></a>Cast, Truncate</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p798719352419"><a name="p798719352419"></a><a name="p798719352419"></a>-</p></td>
</tr>
<tr id="row7987193534118"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p898712358414"><a name="p898712358414"></a><a name="p898712358414"></a>Basic API &gt; Vector Computation &gt; Reduction Computation</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p675915224145"><a name="p675915224145"></a><a name="p675915224145"></a>ReduceMax, ReduceMin, WholeReduceSum, BlockReduceMax, BlockReduceMin, BlockReduceSum, PairReduceSum, RepeatReduceSum,</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1398713564110"><a name="p1398713564110"></a><a name="p1398713564110"></a>-</p></td>
</tr>
<tr id="row19871835174112"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p298716354416"><a name="p298716354416"></a><a name="p298716354416"></a>Basic API &gt; Vector Computation &gt; Data Conversion</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p79872035154118"><a name="p79872035154118"></a><a name="p79872035154118"></a>Transpose</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p149871335154112"><a name="p149871335154112"></a><a name="p149871335154112"></a>-</p></td>
</tr>
<tr id="row162305694316"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p123135644320"><a name="p123135644320"></a><a name="p123135644320"></a>Basic API &gt; Vector Computation &gt; Data Fill</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p48541044144819"><a name="p48541044144819"></a><a name="p48541044144819"></a>Duplicate, Brcb, CreateVecIndex</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p102335634317"><a name="p102335634317"></a><a name="p102335634317"></a>-</p></td>
</tr>
<tr id="row7987113516412"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p139871235164110"><a name="p139871235164110"></a><a name="p139871235164110"></a>Basic API &gt; Vector Computation &gt; Sorting and Combination</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p8325152773515"><a name="p8325152773515"></a><a name="p8325152773515"></a>Sort32, MrgSort, GetMrgSortResult</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p159871035134111"><a name="p159871035134111"></a><a name="p159871035134111"></a>-</p></td>
</tr>
<tr id="row9987535154118"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p14987163518416"><a name="p14987163518416"></a><a name="p14987163518416"></a>Basic API &gt; Vector Computation &gt; Discrete and Aggregate</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1987163510416"><a name="p1987163510416"></a><a name="p1987163510416"></a>Gather, Gatherb, Scatter</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p169872035144115"><a name="p169872035144115"></a><a name="p169872035144115"></a>-</p></td>
</tr>
<tr id="row098783516417"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p139874354419"><a name="p139874354419"></a><a name="p139874354419"></a>Basic API &gt; Vector Computation &gt; Mask Operation</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p99871335184114"><a name="p99871335184114"></a><a name="p99871335184114"></a>SetMaskCount, SetMaskNorm, SetVectorMask, ResetMask</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p18988435204114"><a name="p18988435204114"></a><a name="p18988435204114"></a>-</p></td>
</tr>
<tr id="row1198714156411"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1698718951871"><a name="p1698718951871"></a><a name="p1698718951871"></a>Basic API &gt; Vector Computation &gt; Quantization Setting</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1752718121879"><a name="p1752718121879"></a><a name="p1752718121879"></a>SetDeqScale</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1687112812182"><a name="p1687112812182"></a><a name="p1687112812182"></a>-</p></td>
</tr>
<tr id="row098783516418"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p139874354418"><a name="p139874354418"></a><a name="p139874354418"></a>Basic API &gt; Vector Computation &gt; Data Rearrangement</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p99871335184115"><a name="p99871335184115"></a><a name="p99871335184115"></a>Interleave, DeInterleave</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p18988435204115"><a name="p18988435204115"></a><a name="p18988435204115"></a>-</p></td>
</tr>
<tr id="row1598863534117"><td class="cellrowborder" rowspan="6" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1998811353419"><a name="p1998811353419"></a><a name="p1998811353419"></a>Basic API &gt; Data Movement &gt; DataCopy</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p398811358414"><a name="p398811358414"></a><a name="p398811358414"></a>Basic Data Movement</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p11988335184112"><a name="p11988335184112"></a><a name="p11988335184112"></a>Data movement through the UB (VECIN/VECCALC/VECOUT) -&gt; L1 Buffer (TSCM) path is not supported.</p></td>
</tr>
<tr id="row19881535134113"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1995517203185"><a name="p1995517203185"></a><a name="p1995517203185"></a>Enhanced Data Movement</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p172481414205"><a name="p172481414205"></a><a name="p172481414205"></a>Data movement over the UB (VECIN/VECCALC/VECOUT) -> L1 Buffer (TSCM) path is not supported.</p></td>
</tr>
<tr id="row831243535112"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p1095612051813"><a name="p1095612051813"></a><a name="p1095612051813"></a>Sliced Data Movement</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p11956142012183"><a name="p11956142012183"></a><a name="p11956142012183"></a>-</p></td>
</tr>
<tr id="row49881535124115"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p179561920111815"><a name="p179561920111815"></a><a name="p179561920111815"></a>ND2NZ Conversion Data Movement</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p595642041818"><a name="p595642041818"></a><a name="p595642041818"></a>Data movement over the UB (VECIN/VECCALC/VECOUT) -> L1 Buffer (TSCM) path is not supported.</p></td>
</tr>
<tr id="row76231552182019"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p11956102019184"><a name="p11956102019184"></a><a name="p11956102019184"></a>NZ2ND Conversion Data Movement</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p1062311525208"><a name="p1062311525208"></a><a name="p1062311525208"></a>-</p></td>
</tr>
<tr id="row022720172113"><td class="cellrowborder" valign="top" headers="mcps1.2.4.1.1 "><p id="p7956192001812"><a name="p7956192001812"></a><a name="p7956192001812"></a>Quantized Activation Data Movement</p></td>
<td class="cellrowborder" valign="top" headers="mcps1.2.4.1.2 "><p id="p13228105216"><a name="p13228105216"></a><a name="p13228105216"></a>-</p></td>
</tr>
<tr id="row16988143510417"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p192041749112615"><a name="p192041749112615"></a><a name="p192041749112615"></a>Basic API &gt; Data Movement</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p16956102014187"><a name="p16956102014187"></a><a name="p16956102014187"></a>Copy, DataCopyPad, SetPadValue</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p2702434251"><a name="p2702434251"></a><a name="p2702434251"></a>DataCopyPad does not support data movement over the UB (VECIN/VECCALC/VECOUT)-&gt;L1 Buffer (TSCM) path.</p></td>
</tr>
<tr id="row6778713162710"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p20882139162713"><a name="p20882139162713"></a><a name="p20882139162713"></a>Basic API &gt; Synchronization Control &gt; Intra-core Synchronization</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p2882133915277"><a name="p2882133915277"></a><a name="p2882133915277"></a>SetFlag/WaitFlag, PipeBarrier, DataSyncBarrier</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p8882133922716"><a name="p8882133922716"></a><a name="p8882133922716"></a>-</p></td>
</tr>
<tr id="row28501428172718"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p388253912712"><a name="p388253912712"></a><a name="p388253912712"></a>Basic API &gt; Synchronization Control &gt; Inter-core Synchronization</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p6882143920272"><a name="p6882143920272"></a><a name="p6882143920272"></a>CrossCoreSetFlag, CrossCoreWaitFlag, SyncAll</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p9882173914275"><a name="p9882173914275"></a><a name="p9882173914275"></a>-</p></td>
</tr>
<tr id="row2015642713274"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p288214390273"><a name="p288214390273"></a><a name="p288214390273"></a>Basic API &gt; Cache Control</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1688293992714"><a name="p1688293992714"></a><a name="p1688293992714"></a>DataCachePreload, DataCacheCleanAndInvalid, ICachePreLoad, GetICachePreloadStatus</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p1988210392278"><a name="p1988210392278"></a><a name="p1988210392278"></a>-</p></td>
</tr>
<tr id="row1444116151276"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p13882639172714"><a name="p13882639172714"></a><a name="p13882639172714"></a>Basic API &gt; System Variable Access</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p988253914278"><a name="p988253914278"></a><a name="p988253914278"></a>GetBlockNum, GetBlockIdx, GetDataBlockSizeInBytes, GetArchVersion, InitSocState, GetSpr, ClearSpr, GetProgramCounter, GetSubBlockNum, GetSubBlockIdx, GetSystemCycle, SetCtrlSpr, GetCtrlSpr, ResetCtrlSpr</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p2882173992716"><a name="p2882173992716"></a><a name="p2882173992716"></a>-</p></td>
</tr>
<tr id="row1298811359417"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p10988193511419"><a name="p10988193511419"></a><a name="p10988193511419"></a>Basic API &gt; Atomic Operation</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1479292993514"><a name="p1479292993514"></a><a name="p1479292993514"></a>SetAtomicAdd, SetAtomicType, DisableDmaAtomic, SetAtomicMax, SetAtomicMin, SetStoreAtomicConfig, GetStoreAtomicConfig, AtomicAdd, AtomicMin, AtomicMax, AtomicCas, AtomicExch</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p89881358416"><a name="p89881358416"></a><a name="p89881358416"></a>-</p></td>
</tr>
<tr id="row11988153510410"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p4989143554117"><a name="p4989143554117"></a><a name="p4989143554117"></a>Basic API &gt; cube computation</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p398953516417"><a name="p398953516417"></a><a name="p398953516417"></a>Fill, LoadData, LoadDataWithTranspose, LoadDataWithSparse, SetFmatrix, SetLoadDataBoundary, SetLoadDataRepeat, SetLoadDataPaddingValue, Fixpipe, SetFixPipeConfig, SetFixpipeNz2ndFlag, SetFixpipePreQuantFlag, SetAippFunctions, LoadImageToLocal, Mmad, SetHF32Mode, SetHF32TransMode</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p7989123518419"><a name="p7989123518419"></a><a name="p7989123518419"></a>-</p></td>
</tr>
<tr id="row2098911359417"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p6989173517418"><a name="p6989173517418"></a><a name="p6989173517418"></a>Utils API &gt; C++ Standard Library &gt; Algorithms</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p79891335164117"><a name="p79891335164117"></a><a name="p79891335164117"></a>max, min, index_sequence</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p5989193564117"><a name="p5989193564117"></a><a name="p5989193564117"></a>-</p></td>
</tr>
<tr id="row3989143504110"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p119891335204116"><a name="p119891335204116"></a><a name="p119891335204116"></a>Utils API &gt; C++ Standard Library &gt; Container Functions</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p29891935104116"><a name="p29891935104116"></a><a name="p29891935104116"></a>tuple, get, make_tuple</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p3989113510418"><a name="p3989113510418"></a><a name="p3989113510418"></a>-</p></td>
</tr>
<tr id="row59891735184118"><td class="cellrowborder" valign="top" width="29.630000000000003%" headers="mcps1.2.4.1.1 "><p id="p1298918353418"><a name="p1298918353418"></a><a name="p1298918353418"></a>Utils API &gt; C++ Standard Library &gt; Type Traits</p></td>
<td class="cellrowborder" valign="top" width="44.519999999999996%" headers="mcps1.2.4.1.2 "><p id="p1698943519414"><a name="p1698943519414"></a><a name="p1698943519414"></a>is_convertible, is_same, enable_if, conditional</p></td>
<td class="cellrowborder" valign="top" width="25.85%" headers="mcps1.2.4.1.3 "><p id="p9989113544116"><a name="p9989113544116"></a><a name="p9989113544116"></a>-</p></td>
</tr>
</tbody>
</table>
<!-- end id4 -->


## API Scope Using Reserved UB Space<a name="section_reserved_ubuf_api"></a>

The following APIs internally use the UB space reserved by Ascend C. When the [--cce-disable-asc-reserved-ubuf](../../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#common-compilation-options) compilation option is enabled, calling the related APIs on the corresponding product version triggers a compile-time error. Because the same API uses the reserved UB space differently across NPU architectures, after enabling this compilation option, you need to manually adjust the API call method or replace it with an implementation that does not depend on the reserved UB space to complete compatibility migration.

> [!CAUTION] Caution
> If the same API provides multiple overloaded prototypes, only some of the prototypes may use the reserved UB space.

<!-- npu="A3,910b" id5 -->
**Table 5** API scope using the reserved UB space for [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md#npu-arch)

| API Category | API Name |
| --- | --- |
| Basic API | AddDeqRelu, Select |
| Advanced API | Digamma, Lgamma, Power, Sign, LayerNorm, DropOut, SelectWithBytesMask |

<!-- end id5 -->

<!-- npu="950" id6 -->
**Table 6** API scope using the reserved UB space for [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md#npu-arch)

| API Category | API Name |
| --- | --- |
| Basic API > Memory data movement | Copy |
| Basic API > Basic Arithmetic | Exp, Ln, Abs, Reciprocal, Sqrt, Rsqrt, Relu, Add, Sub, Mul, Div, Max, Min, Adds, Muls, Maxs, Mins, LeakyRelu, Subs, Divs |
| Basic API > Logical Computation | Not, And, Or, ShiftLeft, ShiftRight, Ands, Ors |
| Basic API > Compound Computation | Axpy, CastDequant, CastDeq, AddRelu, AddReluCast, AddDeqRelu, SubRelu, SubReluCast, MulAddDst, MulCast, FusedMulAdd, FusedMulAddRelu, MulAddRelu |
| Basic API > Comparison and Selection | Compare, Compares, CompareScalar, Select, GatherMask |
| Basic API > Type Conversion | Cast |
| Basic API > Reduction Computation | ReduceRepeat, ReduceDataBlock, ReducePairElem, BlockReduceMax, BlockReduceMin, BlockReduceSum, PairReduceSum, RepeatReduceSum, WholeReduceMax, WholeReduceMin, WholeReduceSum |
| Basic API > Data Fill | Duplicate |
| Basic API > Kernel-Tiling | GET_TILING_DATA, GET_TILING_DATA_WITH_STRUCT, GET_TILING_DATA_MEMBER |
| Utils API > Debugging API (SIMD VF) | printf, asc_dump_reg, asc_dump, asc_dump_ubuf |
| Advanced API | AscendAntiQuant, AdjustSoftMaxRes, TopK |

<!-- end id6 -->
