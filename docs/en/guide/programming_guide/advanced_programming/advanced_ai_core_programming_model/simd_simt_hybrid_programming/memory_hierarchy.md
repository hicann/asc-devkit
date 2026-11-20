# Memory Hierarchy

<!-- md-trans-meta sourceCommit=b6142b1cb0ae3dfa4b9063f8bd59db99b19fc426 translatedAt=2026-11-09T10:49:36.916Z pushedAt=2026-11-19T06:32:19.878Z nl=2 -->

## Overall Memory Resources<a name="zh-cn_topic_0000002571695539_section598391912124"></a>

In the SIMD and SIMT mixed programming scenario, the overall memory resources are shown in the following figure. The off-core Global Memory is the global memory of all cores. It has the largest capacity but the lowest access efficiency. The L1 of the AIC and the Unified Buffer (UB) of the AIV are shared memory within a single core. They have smaller capacity but higher access efficiency. The private memory layer is closest to the compute units, with the smallest capacity and the highest access efficiency.

**Figure 1** SIMD and SIMT mixed programming memory hierarchy  
<img src="../../../../figures/simt_mem.png" alt="Memory hierarchy" style="width:70%; height:auto;">

In the mixed programming scenario, the UB memory is the shared memory of the SIMT VF and the SIMD VF. When a Vector Function is switched, the data in the UB is not cleared, so the UB can be used for communication between Vector Functions. The [UB Division](#ub-division) section below describes in detail how the UB is divided into areas such as static memory, dynamic memory, and Data Cache. The [Data Path](#data-path) section introduces the overall data path in the mixed programming scenario and explains data synchronization between parallel pipelines and cache coherence issues when accessing the GM.

## UB Division<a name="ZH-CN_TOPIC_0000002563309890"></a>

The total UB memory space is 256KB. In addition to the dynamic and static memory requested by users, UB also needs to reserve a certain amount of space for internal use. Therefore, when configuring the sizes of dynamic and static memory, users cannot use up the entire UB space. UB is divided into four main areas by function, which are, from low addresses to high addresses, static memory, dynamic memory, reserved space, and Data Cache, as shown in the following figure.

**Figure 2** UB memory allocation in SIMD and SIMT mixed programming  
![](../../../../figures/simt_ub.png "UB memory allocation diagram")

1.  Static memory: A memory space of a specified size is allocated from the start address of the memory. Its size is determined at compile time and cannot be modified dynamically.

    ```cpp
    // Allocate static memory through an array, for example:
    __ubuf__ char static_buf[1024];
    ```

2.  Dynamic memory: Located after the static memory. Its size is specified by the `dyn_ub_size` parameter in [<<<...\>\>\>](kernel_and_vf_functions.md#zh-cn_topic_0000002571578013_section156822920311) and can be allocated in the following ways.

    -   Allocate it using a dynamic array.

        ```cpp
        // Allocate dynamic memory through a dynamic array, for example:
        extern __ubuf__ char dynamic_buf[];
        ```
    -   Allocate it by calling the Alloc API of LocalMemAllocator.

    Since dynamic memory is always allocated starting from the position after the end of the static memory, you can allocate dynamic memory in only one of these ways. Otherwise, address space overlap may occur, resulting in undefined behavior.

3.  Reserved space: The space reserved by the compiler and Ascend C, with a size of 8KB. Of this, 6KB is used as the register spill stack space for various SIMD and SIMT registers, and 2KB is the space reserved internally for Ascend C APIs.
4.  Data Cache: The Data Cache space dedicated to SIMT, used for caching data when SIMT threads access global memory. The configurable range of the Data Cache space is **a minimum of 32KB and a maximum of 128KB**. The actual memory size is affected by the static and dynamic memory sizes configured by the user. The specific compute formula is as follows:

    ```
    Data Cache space size = min(Total UB size (256KB) - static memory - dynamic memory - reserved space (8KB by default), 128KB)
    ```

    The upper limit of the Data Cache space is **128 KB**. Even if the requested static and dynamic memory is small, the actual allocated Data Cache size does not exceed this upper limit.

In mixed programming scenarios, static memory and dynamic memory can be used in the SIMT VF, SIMD VF, and MainScalar execution spaces. MainScalar refers to the execution space outside the VF function on the device side. The following figure shows the UB memory layout when memory is requested in different execution spaces.

**Figure 3** UB memory layout in different execution spaces  
<img src="../../../../figures/simt_simd_ub_layout.png" alt="memory layout">

The UB memory layout follows these rules:

- There is only one copy of dynamic memory globally. The dynamic memory obtained in different execution spaces is located in the same address space, and the size of the dynamic memory is determined by the dynamic memory size configured at kernel launch.
- Because VFs on the AIV are executed serially, the same static memory is reused among VFs to improve UB resource utilization. The size of this memory depends on the maximum static memory size required by each VF.
- The start address of the dynamic memory requested by the user within a kernel is fixed to 32B alignment, and the static memory is laid out according to the following start address alignment rules:
    -   By default, the start address of the requested static memory is aligned to 32B;
    -   The user can manually specify the alignment by using `alignas(N)`. This configuration takes precedence over the default alignment.
- The reserved space and Data Cache are located in the high-address region of the UB address space. The actual size of the Data Cache depends on the space actually occupied by the dynamic and static memory.

### Data Cache Memory Space Description<a name="zh-cn_topic_0000002571697985_section19291134194"></a>

The Data Cache serves as the cache for accessing GM memory, and its size affects the memory access efficiency of the operator. If the Data Cache is smaller than 32 KB, a verification error occurs and execution exits. Therefore, when allocating memory, ensure that sufficient Data Cache space is reserved.

Because the Data Cache, reserved space, and the dynamic and static memory requested by developers share the same UB, developers cannot use the entire UB space. The Data Cache capacity is computed by subtracting the dynamic and static memory and the reserved space from the total UB size. The smaller the shared memory configured by the user, the larger the space left for the Data Cache. In other words, the shared memory size implicitly determines the Data Cache capacity until it reaches the 128 KB upper limit.

If the memory size configured by the user is insufficient, or if the user directly accesses a UB address without requesting memory, the accessed address may cross the dynamic and static memory boundary and fall into the Data Cache area. This is undefined behavior and is extremely dangerous. Such out-of-bounds writes may corrupt the cached data of SIMT global memory access, causing the operator to function abnormally.

### Disabling Reserved Space

If you need more UB memory, you can disable the reserved space to expand the available UB space. To disable it, add the following compilation options during compilation:

- [--cce-disable-vf-stack-reserved-ubuf](../../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461): Disables the 6 KB stack space for SIMD and SIMT register overflow.
- [--cce-disable-asc-reserved-ubuf](../../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461): Disables the 2 KB internal reserved space of the Ascend C interface.

The preceding compilation options can be used independently or together. After they are enabled, the compiler no longer reserves the corresponding UB space, which can then be used as ordinary UB space. However, you need to verify the impact of disabling the reserved space on your own:

- After `--cce-disable-vf-stack-reserved-ubuf` is enabled, the compiler cannot use the reserved UB space as the cache space for register overflow. Developers must ensure that no related register overflow occurs; otherwise, a compilation error is reported.
- After `--cce-disable-asc-reserved-ubuf` is enabled, the [APIs that use the reserved UB space](../../../programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md#section_reserved_ubuf_api) cannot work properly. You need to manually adjust the API call method or replace it with an implementation that does not depend on the reserved UB space.

## Data Path

In hybrid programming scenarios, the AIV core supports accessing UB/GM in the SIMT VF, SIMD VF, and MainScalar execution spaces, and also supports an independent MTE data path from UB to GM. The overall data path is shown in the following figure.

**Figure 4** SIMD and SIMT hybrid programming data path  
<img src="../../../../figures/simt_path.png" alt="SIMD and SIMT hybrid programming data path" style="width:90%; height:auto;">

### Accessing UB Memory

For UB memory, reads and writes within a VF use the Vector pipeline, reads and writes on MainScalar use the Scalar pipeline, and there is also an MTE pipeline between UB and GM. Because these three pipelines execute in parallel, synchronization is required if there are data dependencies between them.

### Accessing GM Memory

For GM memory, reads and writes from the SIMT VF and MainScalar to GM go through their respective Data Caches. In addition, the MTE pipeline may also cause cached data consistency issues, as described below:

-   When reading data from GM in the SIMT VF execution space, cached data inconsistency may occur:
    -   When writing data, the underlying layer ensures that the data is immediately written out to GM, thereby ensuring that other paths read the latest data;
    -   When reading data, the underlying implementation of the default memory access is the same as that of [asc\_ldca](../../../../../api/SIMT-API/memory_access_functions/asc_ldca.md). If the accessed memory data hits in the SIMT Data Cache, the cached data is read by default, which may cause the read data to be inconsistent with the latest data in GM.
-   When reading and writing data from GM through the Cache in the MainScalar execution space, consistency issues may occur in both reads and writes:
    -   When writing data, the data is first written to the Cache, and the underlying layer cannot guarantee that the data is immediately flushed to GM. In this case, other paths may read stale data. You can use cache control to flush the data to GM immediately, ensuring that other paths read the latest data. For details, see [asc\_dcci](../../../../../api/SIMD-API/c_api/cache_ctrl/asc_dcci.md);
    -   When reading data, if the accessed memory hits in the Cache, the latest data in GM will not be read, which may cause the read data to become stale.

When reading data from GM in the SIMT VF execution space, the inconsistency between cached data and GM data described above can be resolved in the following ways:

-   You can use the `volatile` keyword to declare a pointer or variable to obtain the latest data.
-   Use the [asc\_dcci\_single](../../../../../api/SIMT-API/memory_access_functions/asc_dcci_single.md) API to refresh the cache line where the specified address resides, and use the [asc\_dcci\_entire](../../../../../api/SIMT-API/memory_access_functions/asc_dcci_entire.md) API to refresh the entire Data Cache in the core.
-   Use the [asc\_ldcg](../../../../../api/SIMT-API/memory_access_functions/asc_ldcg.md) API to ensure consistency between cached data and GM data through hardware.

The preceding solutions introduce additional operations and affect overall running efficiency. You need to decide whether to use these methods based on your own requirements and actual scenarios.
