# C Language Programming Overview

<!-- md-trans-meta sourceCommit=8d22eb82fd0b279bd5d784f696a54970c40f98ac translatedAt=2026-10-28T08:40:44.153Z pushedAt=2026-11-03T07:08:03.553Z nl=2 -->

C language programming is the low-level core interface of the Ascend C SIMD programming path. It can be directly mapped to NPU hardware instructions and supports developers in finely controlling hardware resources such as vector processing units, cube computation units, and local storage. Developers can manually plan local memory layout, manage data flow, insert hardware pipeline synchronization instructions, and adjust the pipeline parallelism strategy between data movement and computation, organizing code implementation around compute units, storage hierarchy, and pipeline synchronization mechanisms. This programming approach is suitable for operator development scenarios that demand high performance and controllability, and it is also the core technical path for developers who are accustomed to C language programming and pursue ultimate performance to fully unleash the potential of NPU hardware. Based on the above positioning, the following sections explain the core rules and usage methods of Ascend C language programming one by one from four core modules: kernel function, memory architecture, synchronization mechanism, and operator development.

## Kernel Function

The [kernel function](../kernel_function.md) has been introduced in the preceding sections. Ascend C language programming fully follows the kernel function definition and invocation specifications of SIMD programming, without adding any extra syntax constraints. The kernel function is the execution entry running on the Device side, and all memory read/write, data movement, and computation logic are implemented inside the kernel function. The following sections first introduce the memory hierarchy of C language programming, which is also the foundation of kernel function development.

## Memory Hierarchy

The AI Core adopts a hierarchical storage architecture, and the programming perspectives of different compute units differ:

**Cube computation**: adopts a two-level hierarchy of "Global Memory → L1/L0 series Buffers", where the L1 Buffer, L0A/L0B Buffer, and L0C Buffer all serve cube computation.

**Vector computation**: the traditional architecture adopts a two-level hierarchy of "Global Memory → UB"; [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md) introduces registers to build a three-level hierarchy of "Global Memory → UB → Register". Global Memory stores input and output data, the Unified Buffer (UB) serves as the intermediate cache for vector computation data, and Register is located at the innermost layer of the Vector compute unit, directly interacting with the vector computation execution unit.

<img src="../../../../figures/aicore_mem_2.png" alt="AI Core storage hierarchy" width="800"/>


### External Storage (Global Memory)

Global Memory is the device memory of Ascend NPU, located outside the AI Core. It features large capacity and high bandwidth but relatively long access latency. It is used to store the input data, output results, and intermediate computed data of operators. Kernel functions running on the Device side can directly access Global Memory. Global Memory has data persistence: data stored in space allocated through global memory remains until the memory space is released or the application terminates. Users manage Device-side global memory through the Runtime API.

The following example illustrates this: on the Host side, the aclrtMalloc API is used to allocate Device-side global memory, and aclrtMemcpy is used to copy data between Host and Device global memory (Host→Device or Device→Host); then the <<<>>> syntax is used to trigger the Device-side kernel function to perform computation; in addition, Device global memory allocated through aclrtMalloc must be released by calling the aclrtFree API. For more information and details about the Runtime API, see [Runtime API](https://gitcode.com/cann/runtime/blob/9.2.0/docs/en/api_ref/README.md).

In addition, a kernel function must be declared with the __global__ function prefix. This prefix identifies that the function is called on the Host side and executed on the Device side. It is the standard way to define an Ascend C kernel function, ensuring that the compiler correctly identifies the kernel function entry and generates the corresponding hardware execution code. The complete invocation flow is: Host allocates GM memory → data copy → launch the kernel function → release memory. The sample code is as follows:

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

### Local Memory

The preceding section describes the external global memory. This section describes the high-speed internal storage of the AI Core. The AI Core contains three types of compute units: Cube, Vector, and Scalar. Data must be loaded from external storage into internal storage before computation can be performed. Based on the compute units they serve, internal storage is divided into two systems: AIV internal storage for vector computation and AIC internal storage for cube computation.

#### UB in AIV Internal Storage

UB is the core internal storage unit in the AI Core that serves the vector compute unit, and is deployed inside each Vector Core (AIV). Compared with external storage, UB has a smaller capacity but provides higher bandwidth and lower access latency, and can be used as a high-speed cache resource that is autonomously managed by the user during kernel execution.

In C pointer programming, users can allocate UB statically or dynamically by defining arrays.

**Static allocation**: Allocates a memory space of a specified size. The space size is determined at compile time and cannot be modified dynamically.

```c
__global__ __vector__ void add_custom(...)
{
    constexpr uint32_t ub_size = 256;
    __ubuf__ half static_buffer[ub_size];  // Compiler automatically aligns to 32 bytes
}
```

**Dynamic allocation**: In addition to static allocation, the UB size can also be specified dynamically at runtime, which is the dynamic allocation method. Users need to specify the size of the dynamic memory through the dynamic_ub_size parameter in <<<>>>, and its size is determined at runtime.

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

In addition to basic array definitions, multiple memory blocks can also be integrated through a struct to simplify memory management of complex operators:

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

> 📌 The memory address of UB must be aligned to 32 bytes, so as to match the data transfer granularity of the hardware bus and the parallel processing capability of the SIMD compute unit. Non-aligned address access will directly cause a runtime error in the program.

#### Register File in AIV Internal Storage

[NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md) introduces register programmability. All computations in the Vector compute unit must go through the three-level data flow of "GM → UB → Register", and vector computation is completed on registers. C language programming implements register programming by directly defining register variables. For details about register types, see [Reg Vector Computation Programming](./reg_vector_computation.md).


#### AIC Internal Storage L1 Buffer & L0 Buffer

The Cube compute unit is a unit in the AI Core dedicated to cube operations. The dedicated caches directly accessed by the Cube compute unit are as follows: L0A Buffer stores the left cube, L0B Buffer stores the right cube, and L0C Buffer stores the initialization accumulation values and cube computation results.

In C language programming, users can allocate `L1 Buffer` and `L0 A Buffer` through array definitions. Different Cube-dedicated storage units correspond to dedicated address qualifiers, as described in the following table:

**Storage Units and Address Qualifiers**:

| Storage Unit | Description | Address Qualifier | Examples |
|---------|------|-----------|------|
| L1 Buffer | Caches cube computation input data | `__cbuf__` | `__cbuf__ half l1_buffer[512];` |
| L0A Buffer | Stores the left cube | `__ca__` | `__ca__ half l0a_buffer[256];` |
| L0B Buffer | Stores the right cube | `__cb__` | `__cb__ half l0b_buffer[256];` |
| L0C Buffer | Stores computation results | `__cc__` | `__cc__ half l0c_buffer[256];` |
| Fixpipe Buffer | Stores quantization parameters | `__fbuf__` | `__fbuf__ half fixpipe_buffer[128];` |
| BiasTable Buffer | Stores bias data | `__biasbuf__` | `__biasbuf__ half bias_buffer[64];` |

With address qualifiers, developers can directly allocate a memory block through array definitions, as shown in the following example:

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

Meanwhile, to optimize compute-intensive tasks such as cube multiplication and improve Cube computation efficiency, the L1 and L0 Buffers of the AI Core adopt a fractal memory layout format (a hardware-customized memory layout adapted to the memory access characteristics of cube computation). The fractal formats vary across hardware products, as compared in the following table:

**Table 1** Comparison of fractal formats across product models

| Storage Unit | [NPU Architecture Version 2201](../../../language_extension/simd_builtin_keywords.md) | [NPU Architecture Version 3510](../../../language_extension/simd_builtin_keywords.md) |
|---------|----------------------------|---------------------------|
| L1 Buffer | Nz | Nz |
| L0A Buffer | Zz | Nz |
| L0B Buffer | Zn | Zn |
| L0C Buffer | Nz | Nz |

Taking `Nz` as an example, the memory layout is shown in the following figure. The entire memory is stored with a large-N outer column-major layout plus a small-z inner row-major layout.

![Nz memory layout diagram](../../../../figures/nz_layout.png)

#### Bank Conflict

The preceding sections describe the definitions and layout formats of various types of internal storage. Because the AI Core internal storage uses a bank-group architecture, improper memory access can cause bank conflicts, which in turn degrade operator performance. The following uses UB as an example to describe the principles of bank conflicts. On products with [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), the total UB size is 256 KB (256 x 1024 bytes), containing 8 bank groups, each of which contains 2 banks. Each bank is 16 KB in size and consists of 512 rows, each row being 32 B in length, using low-order address interleaving. For the UB capacity specifications of [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md), see the corresponding hardware architecture document.

<img src="../../../../figures/ub_conflict.png" alt="UB bank conflict diagram" width="800"/>

This architecture uses low-order addressing, and the address layout is shown in the following figure. Each bank can independently perform data read and write operations, allowing multiple data requests to proceed simultaneously. However, when multiple read/write operations attempt to access the same bank at the same time, these operations must be queued due to hardware resource limitations, which causes bank conflicts and degrades performance.

![Low-order interleaved address layout](../../../../figures/low_interleave.png)

Each bank can complete only one read or one write per cycle, and each bank group allows at most 2 reads and 0 writes, or 1 read and 1 write. Based on the memory structure, bank conflicts can be classified into the following three types:
- Read-write conflict: A read operation and a write operation simultaneously attempt to access the same bank, or two read operations and one write operation simultaneously attempt to access the same bank group.
- Write-write conflict: Multiple write operations simultaneously attempt to access the same bank group.
- Read-read conflict: Two read operations access the same bank, or more than two operations simultaneously attempt to access the same bank group.

Take a read-write conflict as an example. When performing a vector addition computation of `dst = src0 + src1`, if the start address of `src0` is 0x0000 and the start address of `dst` is 0x4000, combined with the bank layout, it can be seen that the addresses of `src0` and `dst` currently belong to the same bank and the same bank group, so a read-write conflict exists.

When a bank conflict occurs, it degrades operator performance. You can usually avoid conflicts by optimizing the computation logic and the address allocation method. For details, see [Avoiding Bank Conflicts](../../../../operator_practice/simd_operator_optimization/memory_access/avoid_ub_bank_conflict/avoid_bank_conflict_npu_arch_3510.md).

## Synchronization Mechanism

The preceding sections describe the multi-level storage and compute unit architecture of the AI Core. Because units such as DMA movement, vector computation, and cube computation all run asynchronously in parallel, data read/write and computation operations have strong dependencies. Therefore, a synchronization mechanism must be used to control the execution timing and ensure correct program logic. In programming scenarios, synchronization refers to coordinating the order and timing of access to shared resources by multiple execution units (such as threads and processes) to ensure the correctness of program execution results. If the execution order is not strictly controlled through a synchronization mechanism, problems such as inconsistent computation results and unpredictable program behavior may occur.

AI Core synchronization is mainly divided into **intra-core synchronization** and **inter-core synchronization**.

### Intra-Core Synchronization

The execution units inside the AI Core (such as the MTE2 data movement unit and the Vector compute unit) run in an asynchronous and parallel manner. When multiple units read from and write to the same storage resource, data dependencies inevitably exist. To ensure data consistency and correct computation results, synchronization control must be used to coordinate the operation timing of each unit.

The following figure shows a common Vector computation data flow:

1. Move data from Global Memory to Local Memory through DMA
2. Perform vector computation
3. Move data from Local Memory to Global Memory through DMA

The preceding different execution units correspond to different hardware pipelines inside the AI Core. Each pipeline has a clear division of labor, and the specific classification is shown in the following table:

<img src="../../../../figures/vec_flow.png" alt="Vector computation data flow" width="800"/>

The four execution units Scalar, Vector, DMA (MTE2), and DMA (MTE3) run in parallel. If they access the same Local Memory, a synchronization mechanism is required to control their access timing: ensure that data is moved into Local Memory before computation, and moved out after computation is complete.

<img src="../../../../figures/sync_order.png" alt="Synchronization order diagram" width="800"/>

> 📌 Note: The preceding example describes the main path. When the `Scalar` execution unit reads from or writes to GM or UB, the synchronization between the `Scalar` pipeline and other pipelines must also be considered.

The execution units on the AI Core belong to different execution pipelines. Synchronization ensures that different execution pipelines run correctly. The following lists the main execution pipelines within the AI Core.

| Pipeline Type | Meaning |
|---------|------|
| **PIPE_S** | Scalar pipeline |
| **PIPE_V** | Vector computation pipeline, and the L0C Buffer→UB data movement pipeline on some hardware architectures |
| **PIPE_M** | Cube computation pipeline |
| **PIPE_MTE1** | L1 Buffer→L0A Buffer and L1 Buffer→L0B Buffer data movement pipeline |
| **PIPE_MTE2** | GM→L1 Buffer and GM→UB data movement pipeline |
| **PIPE_MTE3** | UB→GM and L1 Buffer→GM data movement pipeline |
| **PIPE_FIX** | L0C Buffer→GM and L0C Buffer→L1 data movement pipeline |

To ensure correct execution between pipelines, the AI Core provides multiple synchronization mechanisms for different application scenarios. Developers can flexibly select an appropriate synchronization method based on actual requirements to implement operator functions.

| Synchronization Type | Core Function and Scenario | Key API | Example Code |
|---------|-------------------|---------|-------------|
| Multi-pipeline synchronization (mechanism 1) | Resolves synchronization dependencies between asynchronous pipelines within a core by "locking-releasing" a specified pipeline. This mechanism functions similarly to the lock mechanism of a traditional CPU. It is a synchronization method newly introduced in [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md) and is easier to use than notify/wait. | `asc_lock` & `asc_unlock` | `uint8_t mutex_id = 1;`<br>`asc_lock(PIPE_MTE2, mutex_id);`<br>`asc_copy_gm2ub((__ubuf__ void*)src0, ...);`<br>`asc_unlock(PIPE_MTE2, mutex_id);`<br>`asc_lock(PIPE_V, mutex_id);`<br>`asc_add(shared_buffer, x_local, y_local);`<br>`asc_unlock(PIPE_V, mutex_id);` |
| Multi-pipeline synchronization (mechanism 2) | Implements synchronous waiting between two pipelines through the notify and wait mechanisms, used for synchronous coordination of asynchronous pipelines within a core. After the mutex mechanism was introduced in [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md), compatibility with this API is still maintained, but the mutex mechanism is preferred for multi-pipeline synchronization. | `asc_sync_notify` & `asc_sync_wait` | `asc_copy_gm2ub((__ubuf__ void*)src0, ...);`<br>`asc_sync_notify(PIPE_MTE2, PIPE_V, EVENT_ID0);`<br>`asc_sync_wait(PIPE_MTE2, PIPE_V, EVENT_ID0);`<br>`asc_add(dst, src1, src0, total_length);` |
| Single-Pipeline Synchronization | Waits for the completion of preceding instructions in the same pipeline, used when there is a data dependency between preceding and following instructions in the same pipeline. | `asc_sync_pipe` & `asc_sync_mte2, etc.` | `asc_add(dst, src1, src0, total_length);`<br>`asc_sync_pipe(PIPE_V);`<br>`asc_sub(dst, dst, src0, total_length);` |
| Memory access order control | Waits for the completion of all preceding memory accesses, used in scenarios with data dependencies or memory dependencies. | `asc_sync_data_barrier` | `asc_sync_data_barrier(mem_dsb_t::DSB_ALL);` |

The above describes the synchronization solutions for the pipelines and execution units within a single AI Core. When business logic requires collaboration across multiple AI Cores, the inter-core synchronization mechanism is needed.

### Inter-Core Synchronization

If the computation of a core depends on the complete computation results of other related cores, the inter-core synchronization mechanism is required to ensure the timing correctness of multi-core execution. In the following figure, AIC depends on the ReduceSum computation result of AIV. Because the overall vector is large, it must be split into multiple parts, with each AIV completing part of the computation. Each AIV writes its partial computation result to GM through atomic accumulation. What AIC reads must be the final result after all AIVs have completed the accumulation.

<img src="../../../../figures/inter_core_sync_ex.png" alt="Example of an inter-core synchronization business scenario" width="800"/>

As shown in the preceding figure, the current AI Core consists of AIC cores and AIV cores. The AIC cores are mainly used for cube computation, and the AIV cores are responsible for vector computation. AIC/AIV cores are divided into groups. A group is further divided into blocks (primary cores) and subblocks (secondary cores) at a ratio of 1:N (N≥1), where block indicates the number of primary cores and subblock indicates the number of secondary cores associated with a single primary core.

<img src="../../../../figures/block_rel.png" alt="Relationship between block and subblock" width="800"/>

Operators are classified into three types by computation characteristics: Cube operators (cube computation), Vector operators (vector computation), and Mix operators (mixed cube and vector computation). The operator type determines the selection of the inter-core synchronization mode and group configuration mode. For different operator scenarios, C language programming provides corresponding programming APIs to meet diverse operator development requirements.

<table>
  <thead>
    <tr>
      <th>Inter-core Synchronization Type</th>
      <th>Scenario Description</th>
      <th>Main APIs</th>
      <th>Sample Implementation Code</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td><strong>Synchronize all AIC cores</strong></td>
      <td><p>Synchronizes all AIC cores, typically used for multi-core accumulation of Cube operator computation results.</p></td>
      <td><code>asc_sync_inter_arrive</code><br><code>asc_sync_inter_wait</code></td>
      <td>        <code>if ASCEND_IS_AIC {</code><br>
        <code>    int64_t flagID = 1;</code><br>
        <code>    asc_sync_inter_arrive(PIPE_S, flagID);</code><br>
        <code>    asc_sync_inter_wait(PIPE_S, flagID);</code><br>
        <code>}</code>
      </td>
    </tr>
    <tr>
      <td><strong>Synchronize all AIV cores</strong></td>
      <td><p>Synchronizes all AIV cores, typically used for multi-core accumulation of the computation results of a Vector operator.</p></td>
      <td><code>asc_sync_inter_arrive</code><br><code>asc_sync_inter_wait</code></td>
      <td>        <code>if ASCEND_IS_AIV {</code><br>
        <code>    int64_t flagID = 1;</code><br>
        <code>    asc_sync_inter_arrive(PIPE_S, flagID);</code><br>
        <code>    asc_sync_inter_wait(PIPE_S, flagID);</code><br>
        <code>}</code>
      </td>
    </tr>
    <tr>
      <td><strong>Synchronize AIC and AIV within a single AI Core</strong></td>
      <td><p>Synchronizes the AIC and AIV cores within a single AI Core, typically used for the interaction between Cube and Vector in a Mix operator.</p></td>
      <td><code>asc_sync_intra_arrive</code><br><code>asc_sync_intra_wait</code></td>
      <td>        <code>if ASCEND_IS_AIC {</code><br>
        <code>    // Atlas A3 training/inference products: AIC sends one arrive, two AIVs wait simultaneously</code><br>
        <code>    asc_sync_intra_arrive(PIPE_FIX, 0);</code><br>
        <code>    // [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md) products: AIC sends two arrives, two AIVs wait separately</code><br>
        <code>    asc_sync_intra_arrive(PIPE_FIX, 0);</code><br>
        <code>    asc_sync_intra_arrive(PIPE_FIX, 16);</code><br>
        <code>}</code><br>
        <code>if ASCEND_IS_AIV {</code><br>
        <code>    asc_sync_intra_wait(PIPE_MTE3, 0);</code><br>
        <code>}</code>
      </td>
    </tr>
    <tr>
      <td><strong>Synchronize two AIVs within a single AI Core</strong></td>
      <td><p>Synchronizes two AIV sub-cores within a single AI Core. It is typically used for coordination between AIV cores in a Vector operator.</p></td>
      <td><code>asc_sync_subblock_arrive</code><br><code>asc_sync_subblock_wait</code></td>
      <td>        <code>int64_t flag_id = 1;</code><br>
        <code>asc_sync_subblock_arrive(PIPE_S, flag_id);</code><br>
        <code>asc_sync_subblock_wait(PIPE_S, flag_id);</code>
      </td>
    </tr>
  </tbody>
</table>

> 📌 When developing a fused operator based on the decoupled mode (AIC and AIV separated), the operator logic usually contains both AIV and AIC processing logic. In this case, use the macros ASCEND_IS_AIV/ASCEND_IS_AIC provided by Ascend C to isolate the AIV and AIC code.

## Operator Type

Based on the AI Core compute units, multi-level memory architecture, and synchronization mechanisms described earlier, three types of operators can be developed on top of the hardware capabilities: Vector operators, Cube operators, and fused operators. As explained in the preceding chapters on hardware architecture, the AI Core contains two core compute units, Vector (vector computation) and Cube (cube computation), which correspond to vector computation and cube computation scenarios respectively. The Cube compute unit is deployed on the AIC core, and the Vector compute unit is deployed on the AIV core. Each core is equipped with an independent Scalar unit and can independently load its own code segment. Based on the hardware capabilities of the AI Core, developers can implement three types of operators: Vector operators, Cube operators, and Mix CV fused operators.

### Vector Computation

Vector computation mainly includes basic arithmetic, reduction computation, and data type conversion.

On [NPU architecture version 2201](../../../language_extension/simd_builtin_keywords.md), the data for vector computation comes from the UB (32-byte alignment required). C language programming provides the capability to directly operate and compute through UB pointers on the Vector compute unit. This programming mode that directly operates the UB is called Memory vector computation programming.

Taking Add computation as an example, when developers need to compute vector addition of length 4096, they can directly perform the computation through the `asc_add(dst, src0, src1, count)` API. For more details, see the description of Memory vector computation programming in [Memory Vector Computation Programming](./memory_vector_computation.md).

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

The above describes the vector computation programming mode of the traditional architecture. [NPU architecture version 3510](../../../language_extension/simd_builtin_keywords.md) adds register capabilities, and the programming mode changes accordingly: on the basis of the traditional UB cache system, the programmable capability of registers is opened up, building a three-level memory hierarchy of "Global Memory → UB → Register". In the new-generation architecture, all vector computations need to first load data from the UB to registers, and then use the registers to perform computation operations. For details, see the description of Reg vector computation programming in [Reg Vector Computation Programming](./reg_vector_computation.md).

### Cube Computation

In addition to vector computation, the Cube unit is dedicated to cube operations and corresponds to the development of cube operators. Cube computation mainly covers cube multiplication and convolution, and its data mainly comes from the L1 Buffer and L0A/B Buffers. The L1 Buffer is used to cache cube computation data, the L0A Buffer stores the left cube, the L0B Buffer stores the right cube, and the L0C Buffer stores the initial accumulation values and the cube computation results. This programming paradigm that directly operates the L1/L0A/L0B/L0C Buffers based on the Cube compute unit is called [ Cube computation programming](./cube_matrix_computation.md).

Taking Matmul cube multiplication as an example, developers can implement the capability of `c = a * b + bias` as shown in the following figure by operating the  Cube computation capability.
- A and B are source operands. A is the left cube with the shape [M, K], and B is the right cube with the shape [K, N].
- C is the destination operand, which is the cube that stores the cube multiplication result, with the shape [M, N].
- bias is the cube multiplication bias with the shape [1, N]. This bias is applied to each row of the A*B result cube.

![Cube multiplication diagram](../../../../figures/matmul.png)

```c
// Define  Cube operator, marked with __cube__ to execute on Device Cube core, suitable for operators with only Cube computation
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
    asc_mmad(l0c_buf, l0a_buf, l0b_buf, BLOCK_M, BLOCK_K, BLOCK_N, asc_unit_flag_mode::DISABLE, false, false, false);
    // Cube data transfer out
}
```

### Mix CV Fused Computation

Fusing vector computation and cube computation into a single operator yields a CV fused operator, which can further improve overall performance. Developers can freely fuse Vector and Cube operators based on business scenarios to optimize performance. Operators that combine Cube computation and Vector computation are collectively referred to as CV fused operators. For example, the Flash Attention operator, which is core to large language models (LLMs), fuses operations such as Matmul (Cube), Scale (Vector), Mask (Vector), and SoftMax (Vector) into a single operator. Its core implementation is as follows:


![Flash Attention core implementation](../../../../figures/flash_attn.png)

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
        asc_mmad(l0c_buf, l0a_buf, l0b_buf, BLOCK_M, BLOCK_K, BLOCK_N, asc_unit_flag_mode::DISABLE, false, false, false);
        // ...
    }
}

```

A fused operator can merge multiple independent small operators, reducing the overall computation workload and memory usage, optimizing the data flow transmission path, and simplifying the code implementation logic, ultimately achieving simultaneous improvements in computation efficiency, hardware resource utilization, and code maintainability.

## Summary

Starting from the technical positioning, this chapter sequentially describes the kernel function specifications, memory hierarchy, intra-core and inter-core synchronization mechanisms, and the three operator development paradigms of C language programming. As a low-level hardware programming interface, this path can fully unleash NPU computing power. For in-depth practice, refer to the following dedicated documents: [Memory Vector Computation Programming](./memory_vector_computation.md), [Cube Computation Programming](./cube_matrix_computation.md), and [Reg Vector Computation Programming](./reg_vector_computation.md).
