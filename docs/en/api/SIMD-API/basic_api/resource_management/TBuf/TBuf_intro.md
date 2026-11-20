# TBuf Introduction<a name="ZH-CN_TOPIC_0000001645413417"></a>

<!-- md-trans-meta sourceCommit=7f211052f814e6d340cc6bf2dfc44c8393b5424f translatedAt=2026-11-05T04:05:51.556Z pushedAt=2026-11-18T01:25:31.002Z nl=2 -->

During Ascend C programming, you may need to use some temporary variables. The memory occupied by these temporary variables can be managed using the **TBuf** data structure. The storage location is set through a template parameter and can be set to different [TPosition](../../aux_data_structures//TPosition.md) logical positions.

**TBuf** inherits from the **TQueBind** parent class. The inheritance relationship is as follows:

![](../../../../figures/enque-42.png)

The storage space occupied by **TBuf** is managed through **TPipe**. You can initialize the memory for **TBuf** by calling the [InitBuffer](../TPipe/InitBuffer.md) API, and then obtain a Tensor of a specified length through [Get](Get.md) to participate in computation.

The following differences exist between allocating memory for **TBuf** and allocating memory for a queue using [InitBuffer](../TPipe/InitBuffer.md):

-   The memory space allocated for **TBuf** can only participate in computation and cannot perform queue enqueue/dequeue operations.
-   When the memory initialization API is called once, **TPipe** allocates only one block of memory for **TBuf**, whereas for a queue, multiple blocks of memory can be requested through parameter settings. To use multiple temporary variables, you need to define multiple **TBuf** data structures and call the [InitBuffer](../TPipe/InitBuffer.md) API separately for each **TBuf** data structure to initialize its memory.
-   The Tensor obtained by **TBuf** does not need to be released.
