# TBufPool Overview<a name="ZH-CN_TOPIC_0000001912912993"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-11-05T04:09:41.268Z pushedAt=2026-11-18T01:25:31.015Z nl=2 -->

**TPipe** can manage global memory resources, while **TBufPool** can manually manage or reuse **Unified Buffer (UB)**/**L1 Buffer** physical memory. It is mainly used in scenarios where the **UB**/**L1 Buffer** physical memory is insufficient during computation across multiple stages.

## Functional Diagram<a name="section10900194131112"></a>

The following figure shows the process of dividing the resource pool:

1.  Apply for Buffer memory and manage it using a queue through the TPipe::[InitBuffer](../TPipe/InitBuffer.md) API.
2.  Divide the resource pool BufPool1 through TPipe::[InitBufPool](../TPipe/InitBufPool.md).
3.  Specify that BufPool1 and BufPool3 reuse the same address and length through TPipe::[InitBufPool](../TPipe/InitBufPool.md).
4.  Continue to divide BufPool1 and BufPool3 into Buffers or TBufPool resource pools through the TBufPool::[InitBuffer](InitBuffer.md) and TBufPool::[InitBufPool](InitBufPool.md) APIs.

**Figure 1**  BufPool resource pool division<a name="fig14900125316552"></a>  
![](../../../../figures/bufpool_resource_pool.png "BufPool resource pool division")

As shown in the nesting relationship in the figure, the outermost TBufPool\(BufPool1 and BufPool3\) must be applied for and initialized through TPipe::InitBufPool, while the inner TBufPool\(BufPool2\) can be applied for and initialized through TBufPool::InitBufPool.

## Constraints<a name="section8236029104012"></a>

1.  **TBufPool** must be divided and initialized through the **TPipe::InitBufPool** or **TBufPool::InitBufPool** API. A resource pool can only be divided into parts as a whole, and parts cannot be combined into a whole.
2.  When switching between different **TBufPool** resource pools for computation, call the TBufPool::Reset\(\) API to clear the **TBufPool** whose computation is complete. After clearing, the **TBufPool** resource pool and the allocated buffers and data are invalid by default.
3.  Buffers allocated from different resource pools cannot be mixed, to avoid data trampling.
4.  **AllocTensor/FreeTensor** and **EnQue/DeQue** must be used in matched pairs when dividing a **TBufPool** resource pool, which automatically ensures synchronization.
5.  When switching resource pools, if synchronization is written manually, Ascend C does not guarantee address read/write reuse synchronization. Therefore, manual synchronization is not recommended.
