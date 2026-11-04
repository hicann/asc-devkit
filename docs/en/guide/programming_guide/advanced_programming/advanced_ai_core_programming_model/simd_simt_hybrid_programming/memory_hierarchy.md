# Memory Hierarchy

<!-- md-trans-meta sourceCommit=450a5a2c1d4d96b945b547add53f8f21354e8ba0 translatedAt=2026-10-23T14:06:40.478Z pushedAt=2026-11-03T07:05:13.415Z nl=2 -->

## Overall Memory Resources<a name="zh-cn_topic_0000002571695539_section598391912124"></a>

In the SIMD and SIMT mixed programming scenario, the overall memory resources are shown in the following figure. The off-core Global Memory is the global memory of all cores. It has the largest capacity but the lowest access efficiency. The L1 of the AIC and the Unified Buffer (UB) of the AIV are shared memory within a single core. They have smaller capacity but higher access efficiency. The private memory layer is closest to the compute units, with the smallest capacity and the highest access efficiency.

**Figure 1** SIMD and SIMT mixed programming memory hierarchy  
<img src="../../../../figures/simt_mem.png" alt="Memory hierarchy" style="width:70%; height:auto;">

In the mixed programming scenario, UB memory is the shared memory of the SIMT VF and the SIMD VF. When a Vector Function is switched, the data in the UB is not cleared, so communication between Vector Functions can be implemented through the UB. The [UB Division](#ub-division) section below describes in detail how the UB is divided into areas such as static memory, dynamic memory, and Data Cache. The [Data Path](#data-path) section describes the overall data path in the mixed programming scenario, and explains data synchronization between parallel pipelines and cache coherence issues when accessing GM.

## UB Division<a name="ZH-CN_TOPIC_0000002563309890"></a>

The total size of the UB memory space is 256KB. In addition to the dynamic and static memory requested by users, the UB also needs to reserve a certain amount of space for internal use. Therefore, users cannot use up the entire UB space when configuring the sizes of dynamic and static memory. The UB is divided into four main areas by function, which are, from low addresses to high addresses, static memory, dynamic memory, reserved space, and Data Cache, as shown in the following figure.

**Figure 2** SIMD and SIMT mixed programming UB memory allocation  
![](../../../../figures/simt_ub.png "UB memory allocation diagram")

1.  Static memory: A memory space of a specified size is allocated from the start address of the memory. Its size is determined at compile time and cannot be dynamically modified.

    ```cpp
    // Allocate static memory through arrays, for example:
    __ubuf__ char static_buf[1024];
    ```

2.  Dynamic memory: Located after the static memory. Its size is specified by the `dyn_ub_size` parameter in [<<<...\>\>\>](kernel_and_vf_functions.md#zh-cn_topic_0000002571578013_section156822920311) and can be allocated in the following ways.

    -   Allocate using a dynamic array.

        ```cpp
        // Allocate dynamic memory through dynamic arrays, for example:
        extern __ubuf__ char dynamic_buf[];
        ```
    -   Allocate through the Alloc API of LocalMemAllocator.

    Because dynamic memory is allocated from the position after the end of static memory, you can use only one of the methods to allocate dynamic memory. Otherwise, address space overlap may occur, leading to undefined behavior.

3.  Reserved space: The space reserved by the compiler and Ascend C, with a size of 8KB. Of this, 6KB is used as the register spill stack space for various SIMD and SIMT registers, and 2KB is the space reserved internally for Ascend C APIs.
4.  Data Cache: The Data Cache space dedicated to SIMT, used for caching data when SIMT threads access global memory. The configurable range of the Data Cache space is **a minimum of 32KB and a maximum of 128KB**. The actual memory size is affected by the static and dynamic memory sizes configured by the user. The specific calculation formula is as follows:

    ```
    Data Cache space size = min(Total UB size (256KB) - Static memory - Dynamic memory - Reserved space (8KB by default), 128KB)
    ```
    The upper limit of the Data Cache space is **128KB**. Even if the requested static and dynamic memory is small, the actual allocated Data Cache size does not exceed this upper limit.

In mixed programming scenarios, static and dynamic memory can be used in the SIMT VF, SIMD VF, and MainScalar execution spaces. MainScalar refers to the execution space outside the VF function on the device side. The following figure shows the UB memory layout when memory is requested in different execution spaces.

**Figure 3** UB memory layout in different execution spaces  
<img src="../../../../figures/simt_simd_ub_layout.png" alt="memory layout">

The UB memory layout follows these rules:

- There is only one copy of dynamic memory globally. The dynamic memory obtained in different execution spaces is located in the same address space, and its size is determined by the dynamic memory size configured at kernel launch.
- Because VFs on the AIV are executed serially, the same static memory is reused among VFs to improve UB resource utilization. The size of this memory depends on the maximum static memory required by each VF.
- The start address of the dynamic memory requested by the user within a kernel is aligned to 32B by default, and the static memory is laid out according to the following start address alignment rules:
    -   By default, the start address of the requested static memory is aligned to 32 B;
    -   The user can manually specify the alignment by using `alignas(N)`. This configuration takes precedence over the default alignment.
- The reserved space and Data Cache are located in the high-address region of the UB address space. The actual size of the Data Cache depends on the space actually occupied by the dynamic and static memory.

### Data Cache Memory Space Description<a name="zh-cn_topic_0000002571697985_section19291134194"></a>

The Data Cache serves as the cache for accessing GM memory, and its size affects the memory access efficiency of the operator. If the Data Cache is smaller than 32 KB, a verification error occurs and execution exits. Therefore, when allocating memory, ensure that sufficient Data Cache space is reserved.

Because the Data Cache, reserved space, and the dynamic and static memory requested by developers share the same UB, developers cannot use the entire UB space. The Data Cache capacity is calculated by subtracting the dynamic and static memory and the reserved space from the total UB size. The smaller the shared memory configured by the user, the larger the space left for the Data Cache. In other words, the shared memory size implicitly determines the Data Cache capacity until it reaches the 128KB upper limit.

If the memory size configured by the user is insufficient, or if the user directly accesses a UB address without requesting memory, the accessed address may cross the dynamic and static memory boundary and fall into the Data Cache area. This is undefined behavior and is extremely dangerous. Such out-of-bounds writes may corrupt the cached data of SIMT global memory access, causing the operator to function abnormally.

### Disabling Reserved Space

If you need more UB memory, you can disable the reserved space to expand the available UB space. To disable it, add the following compilation options during compilation:

- [--cce-disable-vf-stack-reserved-ubuf](../../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461): Disables the 6 KB stack space for register spilling of various SIMD and SIMT types.
- [--cce-disable-asc-reserved-ubuf](../../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461): Disables the 2 KB internal reserved space of the Ascend C interface.

The preceding compilation options can be used independently or together. After they are enabled, the compiler no longer reserves the corresponding UB space, which can then be used as ordinary UB space. However, you need to verify the impact of disabling the reserved space on your own:

- After `--cce-disable-vf-stack-reserved-ubuf` is enabled, the compiler can no longer use the reserved UB space as the cache space for register spilling. You must ensure that no related register spilling occurs; otherwise, a compilation error will be reported.
- After `--cce-disable-asc-reserved-ubuf` is enabled, the [APIs that use the reserved UB space](../../../programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md#section_reserved_ubuf_api) cannot work properly. You need to manually adjust the API call method or replace them with an implementation that does not depend on the reserved UB space.

## Data Path

In mixed programming scenarios, the AIV core supports accessing UB/GM in the SIMT VF, SIMD VF, and MainScalar execution spaces, and also supports an independent MTE path from UB to GM. The overall data path is shown in the following figure.

**Figure 4** SIMD and SIMT mixed programming data path  
<img src="../../../../figures/simt_path.png" alt="SIMD and SIMT mixed programming data path" style="width:90%; height:auto;">

### Accessing UB Memory

For UB memory, reading from and writing to UB within a VF uses the Vector pipeline, while reading from and writing to UB on the MainScalar uses the Scalar pipeline. In addition, there is an MTE pipeline between UB and GM. Because these three pipelines execute in parallel, synchronization is required if data dependencies exist between them.

### Accessing GM Memory

For GM memory, reads and writes from the SIMT VF and MainScalar to GM go through their respective Data Caches. In addition, the MTE pipeline may also cause cached data consistency issues, as described below:

-   When reading data from GM in the SIMT VF execution space, cached data inconsistency may occur:
    -   When writing data, the underlying layer ensures that the data is immediately written out to GM, so that other paths read the latest data;
    -   When reading data, the underlying implementation of the default memory access is the same as that of [asc\_ldca](../../../../../api/SIMT-API/memory_access_functions/asc_ldca.md). If the accessed memory data hits in the SIMT Data Cache, the cached data is read by default, which may cause the read data to be inconsistent with the latest data in GM.
-   When reading from or writing to GM through the Cache in the MainScalar execution space, consistency issues may occur in both reads and writes:
    -   When writing data, the data is first written to the Cache, and the underlying layer cannot guarantee that the data is immediately flushed to GM. In this case, other paths may read stale data. You can use cache control to flush the data to GM immediately, ensuring that other paths read the latest data. For details, see [asc\_dcci](../../../../../api/SIMD-API/c_api/cache_ctrl/asc_dcci.md);
    -   When reading data, if the accessed memory hits in the Cache, the latest data in GM will not be read, which may cause the read data to become invalid.

When reading data from GM in the SIMT VF execution space, the inconsistency between cached data and GM data described above can be resolved in the following ways:

-   Use the `volatile` keyword to declare a pointer or variable to obtain the latest data.
-   Use the [asc\_dcci\_single](../../../../../api/SIMT-API/memory_access_functions/asc_dcci_single.md) API to refresh the cache line where the specified address resides, and use the [asc\_dcci\_entire](../../../../../api/SIMT-API/memory_access_functions/asc_dcci_entire.md) API to refresh the entire Data Cache in the core.
-   Use the [asc\_ldcg](../../../../../api/SIMT-API/memory_access_functions/asc_ldcg.md) API to ensure consistency between cached data and GM data through hardware.

The preceding solutions introduce additional operations and affect overall running efficiency. You need to decide whether to use these methods based on your own requirements and actual scenarios.
