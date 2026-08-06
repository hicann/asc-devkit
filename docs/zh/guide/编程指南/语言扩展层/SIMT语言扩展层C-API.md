# SIMT语言扩展层C API<a name="ZH-CN_TOPIC_0000002509743873"></a>

SIMT编程基于AI Core的硬件能力实现，当前，SIMT语言扩展层支持的C API类别如下：

-   [同步与内存栅栏](../../../api/SIMT-API/sync_and_memory_fence/sync_and_memory_fence.md)：提供同步与内存栅栏接口，解决线程间可能存在的数据竞争以及线程的同步问题。
-   [原子操作](../../../api/SIMT-API/atomic_operations/atomic_operations.md)：提供对Unified Buffer或Global Memory上的数据与指定数据执行原子操作的一系列API接口。
-   [Warp函数](../../../api/SIMT-API/Warp_functions/Warp_functions.md)：提供对单个Warp内32个线程数据进行处理的一系列API接口。
-   [数学函数](../../../api/SIMT-API/math_functions/math_functions.md)：提供处理数学运算的函数接口集合。
-   [访存函数](../../../api/SIMT-API/memory_access_functions/memory_access_functions.md)：提供开启Cache Hints的Load/Store函数。
-   [地址空间谓词函数](../../../api/SIMT-API/address_space_predicate_functions/address_space_predicate_functions.md)：提供判断输入指针所属地址空间的函数接口集合。
-   [地址空间转换函数](../../../api/SIMT-API/address_space_conversion_functions/address_space_conversion_functions.md)：提供不同地址空间地址值与指针之间相互转换的函数接口集合。
-   [协作组](../../../api/SIMT-API/cooperative_groups/cooperative_groups.md)：提供更细粒度的线程间协作的API接口。
-   [调测接口](../../../api/Utils-API/tuning_interface/tuning_interface.md)：SIMT调试场景下使用的相关接口。
