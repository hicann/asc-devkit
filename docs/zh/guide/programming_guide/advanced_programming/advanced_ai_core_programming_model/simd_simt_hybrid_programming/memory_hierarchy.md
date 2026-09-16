# 内存层级

## 整体内存资源<a name="zh-cn_topic_0000002571695539_section598391912124"></a>

在SIMD与SIMT混合编程场景下，整体内存资源如下图所示。核外的Global Memory是所有核的全局内存，容量最大，但访问效率最低。AIC的L1和AIV的Unified Buffer（UB）是单核内的共享内存，容量较小，但访问效率较高。私有内存层最靠近计算单元，容量最小，访问效率最高。

**图1** SIMD与SIMT混合编程内存层级  
<img src="../../../../figures/simt_mem.png" alt="内存层级" style="width:70%; height:auto;">

在混合编程场景下，UB内存是SIMT VF和SIMD VF的共享内存。Vector Function切换时，UB上的数据不会被清除，因此可通过UB实现Vector Function间的通信。下文[UB划分](#ub划分)章节详细说明UB如何划分为静态内存、动态内存、Data Cache等区域；[数据通路](#数据通路)章节介绍混合编程场景下的整体数据通路，并说明并行流水间的数据同步和访问GM时的缓存一致性问题。

## UB划分<a name="ZH-CN_TOPIC_0000002563309890"></a>

UB内存空间总大小为256KB。除了用户申请的动静态内存，UB还需要预留一定的空间供内部使用，因此用户在配置动静态内存大小时，并不能用满全部UB空间。UB按功能划分为四个主要区域，从低地址到高地址依次为静态内存、动态内存、预留空间和Data Cache，如下图所示。

**图2** SIMD与SIMT混合编程UB内存分配  
![](../../../../figures/simt_ub.png "UB内存分配图")

1.  静态内存：从内存的起始地址分配一段指定大小的内存空间，其大小在编译时确定，不可动态修改。

    ```cpp
    // 静态内存通过数组分配，例如：
    __ubuf__ char static_buf[1024];
    ```

2.  动态内存：位于静态内存之后，其大小由[<<<...\>\>\>](kernel_and_vf_functions.md#zh-cn_topic_0000002571578013_section156822920311)中的`dyn_ub_size`参数指定，可通过以下方式申请使用。

    -   使用动态数组分配。

        ```cpp
        // 动态内存通过动态数组分配，例如：
        extern __ubuf__ char dynamic_buf[];
        ```
    -   通过LocalMemAllocator的Alloc接口申请。

    由于动态内存均从静态内存结束位置之后开始分配，因此只能选择其中一种方式申请动态内存，否则可能导致地址空间重叠，从而引发未定义行为。

3.  预留空间：编译器和Ascend C的预留空间，大小为8KB，其中6KB作为SIMD与SIMT各类寄存器溢出栈空间，2KB为Ascend C接口内部预留空间。
4.  Data Cache：SIMT专有的Data Cache空间，用于SIMT线程访问全局内存时的数据缓存，Data Cache的空间可配置范围为**最小32KB、最大128KB**，实际内存大小受用户配置的静态和动态内存大小影响，具体计算公式如下：

    ```
    Data Cache空间大小 = min(UB总大小（256KB） - 静态内存 - 动态内存 - 预留空间（默认8KB）, 128KB)
    ```
    Data Cache空间上限为**128KB**，即使静态与动态内存申请较少，Data Cache实际分配大小也不会超出该上限。

混合编程场景下，可在SIMT VF、SIMD VF和MainScalar执行空间中使用动静态内存。MainScalar是指Device侧在VF函数外部的执行空间。下图为不同执行空间申请内存时对应的UB内存排布示意图。

**图3** 不同执行空间的UB内存排布  
<img src="../../../../figures/simt_simd_ub_layout.png" alt="内存排布">

UB内存的排布遵循以下规则：

- 全局仅有一份动态内存。不同执行空间获取到的动态内存均位于同一地址空间，动态内存的大小由kernel launch时配置的动态内存大小决定。
- 由于AIV上VF是串行执行的，为提升UB资源利用率，VF之间复用同一份静态内存，该内存大小取决于各个VF所需静态内存最大值。
- 用户在一个Kernel内申请的动态内存首地址固定按照32B对齐，静态内存按如下首地址对齐规则排布：
    -   默认情况下，申请到的静态内存首地址按照32B对齐；
    -   用户可以通过`alignas(N)`手动指定对齐方式，该配置的优先级高于默认对齐方式。
- 预留空间和Data Cache位于UB地址空间的高地址区域。Data Cache的实际大小取决于动静态内存实际占用的空间。

### Data Cache内存空间说明<a name="zh-cn_topic_0000002571697985_section19291134194"></a>

Data Cache作为访问GM内存的缓存，其大小会影响算子的访存效率。若Data Cache小于32KB，执行时会出现校验报错并退出，因此申请内存时需确保预留足够的Data Cache空间。

由于Data Cache、预留空间与用户申请的动静态内存共用同一块UB，开发者无法使用全部UB空间。Data Cache的容量由UB总量扣除动静态内存和预留空间计算所得，用户配置的共享内存越小，留给Data Cache的空间就越大，即共享内存大小实际上隐式决定了Data Cache的容量，直至其达到128KB上限。

若用户配置的内存大小不足，甚至未申请内存即直接访问UB地址，访问地址可能越过动、静态内存边界并落入Data Cache区域，属于未定义行为，非常危险。此类越界写入可能破坏SIMT全局内存访问的数据缓存，导致算子功能异常。

### 禁用预留空间

若用户需要使用更多UB内存，可通过禁用预留空间来扩大可用UB空间。禁用方法为编译时增加如下编译选项：

- [--cce-disable-vf-stack-reserved-ubuf](../../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461)：关闭6KB的SIMD与SIMT各类寄存器溢出栈空间。
- [--cce-disable-asc-reserved-ubuf](../../../compilation_and_execution/operator_compilation/ai_core_operator_compilation.md#ZH-CN_TOPIC_0000002462746461)：关闭2KB的Ascend C接口内部预留空间。

以上编译选项可独立使用，也可同时使用。开启后，编译器不再预留对应部分UB空间，该空间可作为普通UB空间使用。但开发者需要自行确认禁用预留空间的影响：

- 开启`--cce-disable-vf-stack-reserved-ubuf`后，编译器将无法使用预留UB空间作为寄存器溢出的缓存空间，开发者需保证不会发生相关寄存器溢出，否则将会编译报错。
- 开启`--cce-disable-asc-reserved-ubuf`后，[使用预留UB空间的API](../../../programming_model/ai_core_simd_programming/cpp_tensor_programming/static_tensor_programming.md#section_reserved_ubuf_api)无法正常使用，需要手动调整API调用方式或替换为不依赖预留UB空间的实现。

## 数据通路

混合编程场景下，AIV核支持在SIMT VF、SIMD VF和MainScalar执行空间访问UB/GM，同时支持独立的UB到GM的MTE通路。整体数据通路如下图所示。

**图4** SIMD与SIMT混合编程数据通路  
<img src="../../../../figures/simt_path.png" alt="SIMD与SIMT混合编程数据通路" style="width:90%; height:auto;">

### 访问UB内存

对于UB内存，VF内读写UB使用Vector流水，MainScalar上读写UB使用Scalar流水，同时还存在UB与GM之间的MTE流水。由于这三条流水并行执行，若流水间存在数据依赖，需要做好同步。

### 访问GM内存

对于GM内存，SIMT VF和MainScalar对GM的读写会经过各自的Data Cache；同时，MTE流水也可能引发缓存数据一致性问题，具体如下：

-   在SIMT VF执行空间读GM上的数据时，可能存在缓存数据不一致的问题：
    -   写数据时，底层会确保数据立即写出到GM，从而确保其他通路读取到最新数据；
    -   读数据时，默认访存的底层实现与[asc\_ldca](../../../../../api/SIMT-API/memory_access_functions/asc_ldca.md)的实现一致。若访问的内存数据在SIMT Data Cache中命中，默认会读取Cache数据，可能导致读取到的数据与GM中的最新数据不一致。
-   在MainScalar执行空间通过Cache读写GM上的数据时，读写均可能存在一致性问题：
    -   写数据时，数据会先写入Cache，底层无法保证数据立即刷新到GM上，此时其他通路读取到的可能是旧数据。可使用缓存控制使数据立即刷新到GM上，保证其他通路读到的是最新数据，具体详见[asc\_dcci](../../../../../api/SIMD-API/c_api/cache_ctrl/asc_dcci.md)中的内容；
    -   读数据时，若访问的内存在Cache中命中，将不会读取GM中的最新数据，可能导致读取到的数据失效。

在SIMT VF执行空间读GM上的数据时，对于上述缓存数据与GM数据不一致的问题，可以通过以下的方式解决：

-   可使用`volatile`关键字声明指针或者变量以获取最新数据。
-   可使用[asc\_dcci\_single](../../../../../api/SIMT-API/memory_access_functions/asc_dcci_single.md)接口刷新指定地址所在的Cache Line，使用[asc\_dcci\_entire](../../../../../api/SIMT-API/memory_access_functions/asc_dcci_entire.md)接口刷新核内整个Data Cache。
-   可使用[asc\_ldcg](../../../../../api/SIMT-API/memory_access_functions/asc_ldcg.md)接口通过硬件保证缓存数据与GM数据一致性。

上述解决方式会增加额外操作，影响整体运行效率。用户需要根据自身需求和实际场景，酌情选择是否需要使用上述方法。
