# Memory Hierarchy

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-08-26T11:31:53.525Z pushedAt=2026-09-01T06:19:58.869Z -->

## Overall Memory Resources<a name="zh-cn_topic_0000002571695539_section598391912124"></a>

In the SIMD and SIMT mixed programming scenario, the overall memory resources are shown in the following figure. The off-core Global Memory is the global memory of all cores. It has the largest capacity but the lowest access efficiency. The L1 of the AIC and the UB of the AIV are shared memory within a single core. They have smaller capacity but higher access efficiency. The private memory layer is closest to the compute units, with the smallest capacity and the highest access efficiency.

**Figure 1** SIMD and SIMT mixed programming memory hierarchy  
<img src="../../../../figures/simt_mixed_memory_hierarchy.png" alt="Memory hierarchy" style="width:70%; height:auto;">

In the mixed programming scenario, UB memory is the shared memory of the SIMT VF and the SIMD VF. When a Vector Function is switched, the data in the UB is not cleared, so communication between Vector Functions can be implemented through the UB. The [UB Partitioning](#ub-partitioning) section below describes in detail how the UB is divided into areas such as static memory, dynamic memory, and Data Cache. The [Data Path](#data-path) section describes the overall data path in the mixed programming scenario, and explains data synchronization between parallel pipelines and cache coherence issues when accessing GM.

## UB Partitioning<a name="ZH-CN_TOPIC_0000002563309890"></a>

The UB memory space has a total size of 256 KB and is divided into four main areas by function. From low addresses to high addresses, these areas are static memory, dynamic memory, reserved space, and Data Cache, as shown in the following figure.

**Figure 2** UB memory allocation in SIMD and SIMT mixed programming  
<img src="../../../../figures/simt_mixed_ub_memory_allocation.png" alt="UB memory allocation diagram" style="width:70%; height:auto;">

### Memory Space Description<a name="zh-cn_topic_0000002571697985_section19291134194"></a>

1.  Static memory: A memory space of a specified size is allocated from the start address of the memory. Its size is determined at compile time and cannot be dynamically modified.

    ```cpp
    // Allocate static memory through the array, for example:
    __ubuf__ char static_buf[1024];
    ```

2.  Dynamic memory: Located after the static memory. Its size is specified by the `dyn_ub_size` parameter in [<<<...\>\>\>](kernel_and_vf_function.md#zh-cn_topic_0000002571578013_section156822920311) and can be allocated in the following ways.

    -   Allocate using a dynamic array.

        ```cpp
        // Allocate dynamic memory through the dynamic array, for example:
        extern __ubuf__ char dynamic_buf[];
        ```

    -   Allocate through the Alloc API of LocalMemAllocator.

    Because dynamic memory is allocated from the position after the end of static memory, you can use only one of the methods to allocate dynamic memory. Otherwise, address space overlap may occur, leading to undefined behavior.

3.  Reserved space: Space reserved for the compiler and Ascend C, with a fixed size of 8 KB.

4.  Data Cache: SIMT-specific Data Cache space. The allocatable memory size ranges from 32 KB to 128 KB. The calculation formula is as follows:

    ```
    Data Cache = min(Total UB size (256 KB) - Static memory - Dynamic memory - Reserved space (8 KB), 128 KB)
    ```

    The Data Cache serves as the cache for accessing GM memory, and its size affects the memory access efficiency of the operator. If the Data Cache is smaller than 32 KB, a verification error occurs and execution exits. Therefore, when allocating memory, ensure that sufficient Data Cache space is reserved.

Because the Data Cache, reserved space, and user-requested UB memory share the same UB, developers cannot use the entire UB space. Ensure that the actual UB access range in each SIMT and SIMD execution space does not exceed the size of the requested static memory and dynamic memory.

The declaration or configuration of static memory and dynamic memory defines the valid range of UB memory available to users. If the configured memory size is insufficient, or if the UB address is accessed directly without requesting any memory, the accessed address may cross the boundary between dynamic and static memory and fall into the Data Cache area. Because the Data Cache capacity is determined by subtracting static memory, dynamic memory, and reserved space from the total UB size, a smaller user shared memory configuration enlarges the remaining space that can be allocated to the Data Cache. When program execution crosses the boundary of the requested dynamic and static memory and an out-of-bounds write occurs, it may not immediately manifest as a memory allocation failure, but it may corrupt the data cache used for SIMT global memory access, resulting in unstable computation results or abnormal operator precision.

> [!NOTE] Note
>
> You can disable the reserved space by adding the [--cce-disable-vf-stack-reserved-ubuf](../../../compilation_and_execution/operator_compilation/ai_core_operator_compilation_basic_usage.md#ZH-CN_TOPIC_0000002462746461) option during compilation. After this option is enabled, the compiler no longer reserves this part of the UB space, and this space can be used as ordinary UB space. The Data Cache space can be estimated using the following formula:
> ```
> Data Cache = min(UB size (256 KB) - Static memory - Dynamic memory - Reserved space (8 KB), 128 KB)
> ```
> When using this option, ensure that the Data Cache space is not less than 32 KB, and the upper limit of the Data Cache space remains 128 KB. After this option is enabled, the compiler cannot use the reserved UB space as the cache space for register spilling, and developers must ensure that register spilling does not occur.

In mixed programming mode, static memory and dynamic memory can be requested in the SIMT VF, SIMD VF, and MainScalar execution spaces. MainScalar refers to the execution space outside the VF function on the device side. The following figure shows the UB memory layout when memory is requested in different execution spaces.

**Figure 3** UB memory layout in different execution spaces  
<img src="../../../../figures/simt_mixed_memory_layout.png" alt="Memory layout" style="width:80%; height:auto;">

Note the following when requesting static memory and dynamic memory:

-   Static memory requested multiple times is laid out on the UB according to certain start address alignment rules:

    -   By default, the start address of the requested static memory is aligned to 32 B;

    -   Users can use `__align__(N)` to manually specify the alignment, which takes precedence over the default alignment.

-   The dynamic memory size is dynamically configured when `<<<...>>>` is executed. There is only one copy globally, so the start addresses returned for dynamic memory requested at different execution positions are the same.

## Data Path

In mixed programming scenarios, the AIV core supports accessing UB/GM in the SIMT VF, SIMD VF, and MainScalar execution spaces, and also supports an independent MTE path from UB to GM. The overall data path is shown in the following figure.

**Figure 4** SIMD and SIMT mixed programming data path  
<img src="../../../../figures/simt_mixed_data_path.png" alt="SIMD and SIMT mixed programming data path" style="width:90%; height:auto;">

### Accessing UB Memory

For UB memory, reading from and writing to UB within a VF uses the Vector pipeline, while reading from and writing to UB on the MainScalar uses the Scalar pipeline. In addition, there is an MTE pipeline between UB and GM. Because these three pipelines execute in parallel, synchronization is required if data dependencies exist between them.

### Accessing GM Memory

For GM memory, reads and writes from the SIMT VF and MainScalar to GM go through their respective Data Caches. In addition, the MTE pipeline may also cause cached data consistency issues, as described below:

-   When reading data from GM in the SIMT VF execution space, cached data inconsistency may occur:

    -   When writing data, the underlying layer ensures that the data is immediately written out to GM, so that other paths read the latest data;

    -   When reading data, the underlying implementation of the default memory access is the same as that of [asc\_ldca](../../../../../api/SIMT-API/memory_access_functions/asc_ldca.md). If the accessed memory data hits in the SIMT Data Cache, the cached data is read by default, which may cause the read data to be inconsistent with the latest data in GM.

-   When reading from or writing to GM through the Cache in the MainScalar execution space, consistency issues may occur in both reads and writes:

    -   When writing data, the data is first written to the Cache, and the underlying layer cannot guarantee that the data is immediately flushed to GM. In this case, other paths may read stale data. You can use cache control to flush the data to GM immediately, ensuring that other paths read the latest data. For details, see [asc\_dcci](../../../../../api/SIMD-API/C-API/cache_ctrl/asc_dcci.md);

    -   When reading data, if the accessed memory hits in the Cache, the latest data in GM will not be read, which may cause the read data to become invalid.

When reading data from GM in the SIMT VF execution space, the inconsistency between cached data and GM data described above can be resolved in the following ways:

-   Use the `volatile` keyword to declare a pointer or variable to obtain the latest data.

-   Use the [asc\_dcci\_single](../../../../../api/SIMT-API/memory_access_functions/asc_dcci_single.md) API to refresh the cache line where the specified address resides, and use the [asc\_dcci\_entire](../../../../../api/SIMT-API/memory_access_functions/asc_dcci_entire.md) API to refresh the entire Data Cache in the core.

-   Use the [asc\_ldcg](../../../../../api/SIMT-API/memory_access_functions/asc_ldcg.md) API to ensure consistency between cached data and GM data through hardware.

The preceding solutions introduce additional operations and affect overall running efficiency. You need to decide whether to use these methods based on your own requirements and actual scenarios.