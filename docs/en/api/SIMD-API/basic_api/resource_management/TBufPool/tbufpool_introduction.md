# TBufPool Overview<a name="ZH-CN_TOPIC_0000001912912993"></a>

<!-- md-trans-meta sourceCommit=f6490d1e030d257ee55f3f1600bffece5a8bd3f7 translatedAt=2026-08-27T14:31:22.262Z -->

**TPipe** can manage global memory resources, while **TBufPool** can manually manage or reuse **Unified Buffer**/**L1 Buffer** physical memory. It is mainly used in scenarios where **Unified Buffer**/**L1 Buffer** physical memory is insufficient during multi-stage computation.

## Functional Diagram<a name="section10900194131112"></a>

The following figure shows the resource pool allocation process:

1.  Buffer memory can be applied for and managed using queues through the TPipe::[InitBuffer](../TPipe/InitBuffer.md) API.
2.  The resource pool BufPool1 can be allocated through TPipe::[InitBufPool](../TPipe/InitBufPool.md).
3.  TPipe::[InitBufPool](../TPipe/InitBufPool.md) can be used to specify that BufPool1 and BufPool3 reuse the same address and length.
4.  BufPool1 and BufPool3 can be further divided into Buffer or TBufPool resource pools through the TBufPool::[InitBuffer](InitBuffer-56.md) and TBufPool::[InitBufPool](InitBufPool-55.md) APIs.

**Figure 1**  BufPool resource pool allocation<a name="fig14900125316552"></a>  
![](../../../../figures/bufpool_partition.png "BufPool resource pool allocation")

As shown in the nested relationship in the figure, the outermost TBufPool (BufPool1 and BufPool3) must be applied for and initialized through TPipe::InitBufPool, while the inner TBufPool (BufPool2) can be applied for and initialized through TBufPool::InitBufPool.

## Constraints<a name="section8236029104012"></a>

1.  **TBufPool** must be allocated and initialized through the **TPipe::InitBufPool** or **TBufPool::InitBufPool** API. A resource pool can only be allocated as a whole into parts, and parts cannot be combined into a whole.
2.  When switching between different **TBufPool** resource pools for computation, call the **TBufPool::Reset()** API to clear the **TBufPool** whose computation is complete. After clearing, the **TBufPool** resource pool and the allocated **Buffer** and data are invalid by default.
3.  **Buffer** allocated from different resource pools cannot be mixed, to avoid data corruption.
4.  **AllocTensor**/**FreeTensor** and **EnQue**/**DeQue** must be used in matched pairs when splitting a **TBufPool** resource pool, which automatically ensures synchronization.
5.  When switching resource pools, if synchronization is written manually, Ascend C does not guarantee address read/write reuse synchronization. Therefore, manual synchronization is not recommended.

