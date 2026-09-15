# TBuf Introduction<a name="ZH-CN_TOPIC_0000001645413417"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:27:25.872Z -->

During Ascend C programming, you may need to use some temporary variables. The memory occupied by these temporary variables can be managed using the TBuf data structure. The storage location is set through template parameters and can be set to different [TPosition](../../aux_data_structures/TPosition.md) logical positions.

TBuf inherits from the TQueBind parent class. The inheritance relationship is as follows:

![](../../../../figures/enque-42.png)

The storage space occupied by TBuf is managed by TPipe. You can initialize the memory for TBuf through the [InitBuffer](../TPipe/InitBuffer.md) API, and then obtain a Tensor of a specified length through [Get](Get.md) to participate in computation.

The differences between using [InitBuffer](../TPipe/InitBuffer.md) to allocate memory for TBuf and allocating memory for a queue are as follows:

-   The memory space allocated for TBuf can only participate in computation and cannot perform queue enqueue and dequeue operations.
-   When the memory initialization API is called once, TPipe allocates only one block of memory for TBuf, whereas for a queue, multiple blocks of memory can be requested through parameter settings. To use multiple temporary variables, you need to define multiple TBuf data structures and call the [InitBuffer](../TPipe/InitBuffer.md) API separately for each TBuf data structure to initialize its memory.
-   The Tensor obtained by TBuf does not need to be released.
