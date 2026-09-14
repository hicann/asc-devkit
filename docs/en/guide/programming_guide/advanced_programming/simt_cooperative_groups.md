# SIMT Cooperative Group Introduction

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T11:27:40.941Z pushedAt=2026-09-03T11:39:12.720Z -->

A cooperative group (cooperative_groups) is an extension of the Ascend C SIMT programming model, used to organize cooperative thread groups. It enables developers to control the granularity of thread collaboration, implementing more extensive and efficient parallel processing logic.

The Ascend C SIMT programming model provides only the `asc_syncthreads()` API to synchronize all threads within a thread block. When developers want to synchronize threads at a finer granularity to achieve more flexible designs, they often need to implement the corresponding synchronization logic themselves through memory fence APIs. A cooperative group provides a safe and reliable mechanism to help developers implement flexible and efficient thread collaboration.

## Cooperative Group Usage

The most important concept in a cooperative group is to use an object to represent a group of threads, and then control the threads within the group through object methods. The object can clearly express the developer's intent. Passing a cooperative group object to a function clearly expresses "which threads must call the function together".

Cooperative groups provide methods for defining custom thread groups and APIs for operating on thread groups. The complete list of cooperative group APIs is available in [SIMT Cooperative Group API](../../../api/SIMT-API/cooperative_groups/cooperative_groups.md). To use the cooperative group APIs, include the following header file and use the cooperative group namespace.

```c++
#include <simt_api/cooperative_groups.h>

using namespace cooperative_groups;
// It is recommended to use an alias to avoid polluting the global namespace.
namespace cg = cooperative_groups;
```

## Cooperative Group Types

Cooperative groups are classified into implicit groups and explicit groups. An implicit group is independent of the code in the kernel function. It represents the launch configuration of the kernel function, that is, how many thread blocks are created and how many threads in each thread block execute the kernel function. Implicit groups provide a starting point for partitioning finer-grained cooperative groups. An explicit group is a cooperative group manually partitioned by the developer in the kernel function.

The following cooperative group types are currently provided:

- Implicit groups:
    - [thread_block](../../../api/SIMT-API/cooperative_groups/thread_block/thread_block_introduction.md): An abstraction of a thread block that provides thread management and synchronization APIs at the thread block level.
- Explicit groups:
    - [thread_block_tile](../../../api/SIMT-API/cooperative_groups/thread_block_tile/thread_block_tile_introduction.md): A fixed-size thread subgroup obtained by partitioning a parent group. It supports intra-group synchronization and shfl- and vote-type thread operations.
    - [coalesced_group](../../../api/SIMT-API/cooperative_groups/coalesced_group/coalesced_group_introduction.md): A set of active threads on the same execution path within the current warp. It is commonly used for thread collaboration after branch divergence.

**Figure 1** Cooperative group hierarchy in a thread block

![Cooperative group hierarchy in a thread block](../../figures/cooperative_groups.png)

### thread_block

`thread_block` represents the thread block where the current thread resides, and is obtained through `this_thread_block()`. It is a common starting point for further partitioning into sub-cooperative groups.

```c++
thread_block block = this_thread_block();
```

`thread_block` provides thread-block-level information query and synchronization capabilities, for example:

- `thread_rank()`: Obtains the linear rank of the current thread within the thread block.
- `thread_index()`: Obtains the three-dimensional index of the current thread within the thread block.
- `group_index()`: Obtains the three-dimensional index of the current thread block in the grid.
- `sync()`: Synchronizes threads within the thread block.

### thread_block_tile

`thread_block_tile` represents a fixed-size thread subgroup obtained by partitioning a parent group. The subgroup size is specified by a template parameter, which is suitable for scenarios where the size of each cooperative unit is determined at compilation time.

```c++
thread_block block = this_thread_block();
auto tile4 = tiled_partition<4>(block);
```

In addition to basic APIs such as `sync()`, `thread_rank()`, and `num_threads()`, `thread_block_tile` also supports shfl-type and vote-type APIs, such as `shfl()`, `shfl_up()`, `shfl_down()`, `shfl_xor()`, `any()`, `all()`, and `ballot()`.

### coalesced_group

At the hardware level of the SIMT architecture, the processor executes threads in groups of 32 (one warp). When threads encounter a conditional branch, divergence occurs, and the warp executes each branch serially, masking out threads that are not on the current instruction path during the execution of a particular branch. Threads that remain active on the path are called coalesced threads. `coalesced_group` represents the set of threads in the current warp that are active and participating in the current execution path, and is created through the `coalesced_threads()` API. It is commonly used in conditional branches to form a cooperative group consisting only of the active threads that have reached the current branch.

In the example code, a group named `active` is created, which contains all threads in the warp whose thread IDs are even.

```c++
if (threadIdx.x % 2 == 0) {
    coalesced_group active = coalesced_threads();
    unsigned int rank = active.thread_rank();
    ...
}
```

The `coalesced_group` object created through `coalesced_threads()` represents the set of threads that are active at the current moment. Which threads are included in the group is determined by the location where the `coalesced_threads()` API is called, and does not change with subsequent code execution. It is also not guaranteed that these threads remain active throughout the entire execution process.

## Group Partitioning

Cooperative groups support partitioning a parent group into smaller subgroups. The following partitioning APIs are currently provided:

- [tiled_partition](../../../api/SIMT-API/cooperative_groups/tiled_partition.md): Partitions the parent group into subgroups of a specified size.
- [binary_partition](../../../api/SIMT-API/cooperative_groups/binary_partition.md): Partitions the parent group into two `coalesced_group` subgroups based on a boolean label.

### tiled_partition

`tiled_partition` is used to partition a parent group by a fixed size. The template version specifies the subgroup size at compilation time and returns a `thread_block_tile` object.

```c++
using namespace cooperative_groups;

__global__ void simt_kernel(...)
{
    thread_block block = this_thread_block();
    auto tile8 = tiled_partition<8>(block);

    unsigned int lane = tile8.thread_rank();
    ...
}
```

The partition size of the template version must meet the constraints of the `tiled_partition` API. For details, see [tiled_partition](../../../api/SIMT-API/cooperative_groups/tiled_partition.md).

### binary_partition

`binary_partition` is used to divide a `coalesced_group` or `thread_block_tile` into two subgroups based on a Boolean condition. Threads with the same condition value enter the same subgroup.

The following sample code divides a `thread_block_tile` containing 32 threads into an odd-numbered group and an even-numbered group.

```c++
using namespace cooperative_groups;

__global__ void simt_kernel(int *input_arr)
{
    thread_block block = this_thread_block();
    auto tile32 = tiled_partition<32>(block);

    // input_arr contains random integers
    int elem = input_arr[block.thread_rank()];
    coalesced_group subgroup = binary_partition(tile32, (elem & 1));

    unsigned int rank = subgroup.thread_rank();
    ...
}
```

## Precautions

The synchronization and communication APIs in a cooperative group generally require all relevant threads in the group to participate in the call. Common APIs include:

- `sync()`: Synchronizes the threads in the group.
- `shfl()`, `shfl_up()`, `shfl_down()`, `shfl_xor()`: Exchange register data among the threads in the group.
- `any()`, `all()`: Perform a vote on the inputs of the threads in the group. `any()` returns **true** if the input of any thread in the group is non-zero, and `all()` returns **true** if the inputs of all threads in the group are non-zero.
- `ballot()`: Aggregates the inputs of the threads in the group into a bitmask and returns it. The bit corresponding to a thread whose input is non-zero is set to 1 in the mask.

When calling these APIs, ensure that the set of threads participating in the operation is consistent with the set of threads described by the group handle. If only some threads in the group reach the synchronization point, or if different threads call the same collective operation with inconsistent control flow, the result may be incorrect or the program may hang.

The Ascend 950PR/Ascend 950DT SIMT architecture does not support independent thread scheduling. Multiple cooperative groups within a warp should avoid mutual waiting or data dependencies; otherwise, a hang may occur.
