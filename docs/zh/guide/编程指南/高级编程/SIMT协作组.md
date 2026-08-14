# SIMT协作组简介

协作组（cooperative_groups）是Ascend C SIMT编程模型的扩展，用于组织协作线程组。协作组使开发者能够控制线程协作的粒度，实现更丰富、更高效的并行处理逻辑。

Ascend C SIMT编程模型中仅提供`asc_syncthreads()`接口来实现线程块内所有线程的同步。当开发者希望以更细粒度的方式同步线程，从而实现更灵活的设计时，往往需要自行通过内存栅栏接口实现相应的同步逻辑。协作组提供安全可靠的机制，协助开发者实现灵活、高效的线程协作。

## 协作组使用方法

协作组中最重要的概念是使用一个对象表示一组线程，然后通过对象方法控制组内线程。对象可以明确表达开发者的意图，将协作组对象传递到某个函数可以清晰地表达“哪些线程必须共同调用该函数”。

协作组提供了自定义线程组的方法以及操作线程组的接口，全量的协作组API列表可在[SIMT协作组API](../../../api/SIMT-API/cooperative_groups/cooperative_groups.md)中获取。使用协作组API时需要包含以下头文件，并使用协作组命名空间。

```cpp
#include <simt_api/cooperative_groups.h>

using namespace cooperative_groups;
// 更推荐使用别名，避免污染全局命名空间
namespace cg = cooperative_groups;
```

## 协作组类型

协作组分为隐式组和显式组。隐式组与核函数中的代码无关，它代表的是核函数的启动配置，即创建了多少个线程块，每个线程块中有多少个线程来执行核函数。隐式组为划分出更细粒度的协作组提供了起点。显式组是在核函数中由开发者手动划分出的协作组。

当前提供以下协作组类型：

- 隐式组：
    - [grid_group](../../../api/SIMT-API/cooperative_groups/grid_group/grid_group_intro.md)：对当前Grid中所有SIMT线程的抽象，提供Grid级别的信息查询和跨线程块同步接口。
    - [thread_block](../../../api/SIMT-API/cooperative_groups/thread_block/thread_block_intro.md)：对线程块的抽象，提供线程块级别的线程管理和同步接口。
- 显式组：
    - [thread_block_tile](../../../api/SIMT-API/cooperative_groups/thread_block_tile/thread_block_tile_intro.md)：由父组划分得到的固定大小线程子组，支持组内同步、shfl类和vote类线程操作。
    - [coalesced_group](../../../api/SIMT-API/cooperative_groups/coalesced_group/coalesced_group_intro.md)：当前Warp内处于同一执行路径的活跃线程集合，常用于处理分支发散后的线程协作。

**图1**  线程块中的协作组层次结构图

![线程块中的协作组层次结构](../../figures/cooperative_groups.png)

### grid_group

`grid_group`表示当前核函数启动的Grid协作组，覆盖Grid内所有线程块及其SIMT线程，通过`this_grid()`获取。它适合需要在多个线程块之间建立执行顺序的场景。

```cpp
grid_group grid = this_grid();
```

`grid_group`提供Grid级别的信息查询能力和跨线程块的同步能力，例如：

- `thread_rank()`：获取当前线程在Grid内所有SIMT线程中的线性rank。
- `block_rank()`：获取当前线程所属线程块在Grid内的线性rank。
- `num_threads()`：获取Grid内SIMT线程总数。
- `num_blocks()`：获取Grid内线程块总数。
- `sync()`：同步Grid内所有SIMT线程。

`grid_group`仅支持SIMT编程场景，不支持SIMD与SIMT混合编程场景。

### thread_block

`thread_block`表示当前线程所在的线程块，通过`this_thread_block()`获取。它是进一步划分子协作组的常用起点。

```cpp
thread_block block = this_thread_block();
```

`thread_block`提供线程块级别的信息查询和同步能力，例如：

- `thread_rank()`：获取当前线程在线程块内的线性rank。
- `thread_index()`：获取当前线程在线程块内的三维索引。
- `group_index()`：获取当前线程块在Grid中的三维索引。
- `sync()`：同步线程块内线程。

### thread_block_tile

`thread_block_tile`表示由父组划分得到的固定大小线程子组。子组大小由模板参数指定，适合在编译期确定每个协作单元大小的场景。

```cpp
thread_block block = this_thread_block();
auto tile4 = tiled_partition<4>(block);
```

`thread_block_tile`除支持`sync()`、`thread_rank()`、`num_threads()`等基础接口外，还支持shfl类和vote类接口，例如`shfl()`、`shfl_up()`、`shfl_down()`、`shfl_xor()`、`any()`、`all()`和`ballot()`。

### coalesced_group

在SIMT架构的硬件层面上，处理器以32个线程为一组（一个Warp）来执行线程。当线程遇到条件分支时会出现发散，Warp会串行地执行每个分支，在执行某个分支时会屏蔽不在该指令路径上的线程。在路径上保持活跃的线程被称为合并线程（coalesced）。`coalesced_group`表示当前Warp内处于活跃状态并参与当前执行路径的线程集合，通过`coalesced_threads()`接口创建。它常用于条件分支中，仅让到达当前分支的活跃线程组成协作组。

示例代码中创建一个名为`active`的组，该组包含Warp中所有线程id为偶数的线程。

```cpp
if (threadIdx.x % 2 == 0) {
    coalesced_group active = coalesced_threads();
    unsigned int rank = active.thread_rank();
    ...
}
```

通过`coalesced_threads()`创建的`coalesced_group`对象表示的是在当前时刻处于活跃状态的线程集合。其组内包含哪些线程由`coalesced_threads()`接口调用的位置决定，并且不随后续代码执行变化，也不保证这些线程在整个执行过程中持续保持活跃。

## 组划分

协作组支持将父组划分为更小的子组。当前提供以下划分接口：

- [tiled_partition](../../../api/SIMT-API/cooperative_groups/tiled_partition.md)：将父组划分为指定大小的子组。
- [binary_partition](../../../api/SIMT-API/cooperative_groups/binary_partition.md)：根据布尔标签将父组划分为两个`coalesced_group`子组。

### tiled_partition

`tiled_partition`用于按固定大小划分父组。模板版本在编译期指定子组大小，并返回`thread_block_tile`对象。

```cpp
using namespace cooperative_groups;

__global__ void simt_kernel(...)
{
    thread_block block = this_thread_block();
    auto tile8 = tiled_partition<8>(block);

    unsigned int lane = tile8.thread_rank();
    ...
}
```

模板版本的划分大小必须满足`tiled_partition`接口约束，详细要求请参考[tiled_partition](../../../api/SIMT-API/cooperative_groups/tiled_partition.md)。

### binary_partition

`binary_partition`用于按照布尔条件将`coalesced_group`或`thread_block_tile`划分为两个子组。条件值相同的线程会进入同一子组。

示例代码将一个包含32个线程的`thread_block_tile`划分为奇数组和偶数组。

```cpp
using namespace cooperative_groups;

__global__ void simt_kernel(int *input_arr)
{
    thread_block block = this_thread_block();
    auto tile32 = tiled_partition<32>(block);

    // input_arr中是随机整数
    int elem = input_arr[block.thread_rank()];
    coalesced_group subgroup = binary_partition(tile32, (elem & 1));

    unsigned int rank = subgroup.thread_rank();
    ...
}
```

## 注意事项

协作组中的同步和通信接口一般要求组内相关线程共同参与调用。常见接口包括：

- `sync()`：同步组内线程。
- `shfl()`、`shfl_up()`、`shfl_down()`、`shfl_xor()`：在组内线程之间交换寄存器数据。
- `any()`、`all()`：对组内线程的输入进行投票判断。`any()`在组内任一线程的输入非0时返回真，`all()`在组内所有线程的输入均非0时返回真。
- `ballot()`：将组内各线程的输入汇聚为一个位掩码返回，输入非0的线程在掩码中对应比特位为1。

调用这些接口时，需要保证参与操作的线程集合与组句柄描述的线程集合一致。若只有部分组内线程到达同步点，或者不同线程以不一致的控制流调用同一个集体操作，可能导致结果不正确或程序卡死。

<!-- npu="950" id1 -->
Ascend 950PR/Ascend 950DT SIMT架构不支持独立线程调度。一个Warp内的多个协作组之间应避免存在相互等待或数据依赖，否则可能导致卡死。
<!-- end id1 -->
