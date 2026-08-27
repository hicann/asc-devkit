# 协作组简介

协作组（cooperative groups）是Ascend C SIMT编程模型的一种扩展，用于组织协作线程组。通过协作组能够更细粒度地管理线程，从而实现更高效的线程并行协作。

当前提供的协作组类型有：

- [thread_block](thread_block/thread_block_intro.md)：对线程块的抽象，提供统一的线程管理接口。
- [coalesced_group](coalesced_group/coalesced_group_intro.md)：Warp中的活跃线程集合。
- [thread_block_tile](thread_block_tile/thread_block_tile_intro.md)：手动划分出的线程子组。
- [grid_group](grid_group/grid_group_intro.md)：对一个核函数（Kernel）启动的所有SIMT线程的抽象，提供跨线程块的查询和同步接口。

基于以上协作组类型，提供以下划分操作，支持将一个协作组划分为多个子组：

- [tiled_partition](tiled_partition.md)：将父组划分成指定大小的子组。
- [binary_partition](binary_partition.md)：根据自定义的0、1标签将父组划分为两个子组。

基于以上协作组类型，提供以下通用操作：

- [sync](sync.md)：同步指定协作组内的所有线程，功能等价于调用该协作组对象的成员函数`sync()`。

## 需包含的头文件

```cpp
#include "simt_api/cooperative_groups.h"
```

> [!NOTE]说明 
> 所有协作组API均在`cooperative_groups`命名空间下，调用时需添加命名空间。  
> `grid_group`仅支持SIMT编程场景，不支持SIMD与SIMT混合编程场景。
