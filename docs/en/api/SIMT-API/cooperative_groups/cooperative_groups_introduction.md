# Introduction to Cooperative Groups 

<!-- md-trans-meta sourceCommit=d527ad95eb60550038fb5b9a21039d8e30ec687f translatedAt=2026-09-07T07:03:46.351Z pushedAt=2026-09-07T09:11:27.140Z -->

Cooperative groups (**cooperative_groups**) are an extension of the Ascend C SIMT programming model, used to organize cooperative thread groups. Cooperative groups enable finer-grained thread management, thereby achieving more efficient parallel thread collaboration.

The currently available cooperative group types are as follows:

- [thread_block](thread_block/thread_block_introduction.md): An abstraction of a thread block, providing a unified thread management interface.
- [coalesced_group](coalesced_group/coalesced_group_introduction.md): A collection of active threads within a warp.
- [thread_block_tile](thread_block_tile/thread_block_tile_introduction.md): A manually partitioned thread subgroup.

Based on the cooperative group types above, the following partition operations are provided to support dividing a cooperative group into multiple subgroups:

- [tiled_partition](tiled_partition.md): Divides the parent group into subgroups of a specified size.
- [binary_partition](binary_partition.md): Divides the parent group into two subgroups based on custom 0 and 1 labels.

## Header Files to Include

```c++
#include "simt_api/cooperative_groups.h"
```

> [!NOTE]
> All cooperative groups APIs are under the `cooperative_groups` namespace, which must be added when calling them.