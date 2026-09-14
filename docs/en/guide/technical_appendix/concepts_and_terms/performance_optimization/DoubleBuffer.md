# DoubleBuffer<a name="ZH-CN_TOPIC_0000001615270513"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T12:40:09.931Z pushedAt=2026-09-10T12:21:28.961Z -->

The instruction queues executed on the AI Core mainly include the Vector instruction queue, Cube instruction queue, and MTE instruction queue. The mutual independence and parallel executability among different instruction queues are the cornerstone of the DoubleBuffer optimization mechanism.

The CopyIn and CopyOut processes of vector computation use the MTE instruction queue (MTE2 and MTE3), while the Compute process uses the Vector instruction queue (V). This means that the CopyIn and CopyOut processes can run in parallel with the Compute process.

As shown in [Figure 1](#fig994415385460), consider a complete data movement and computation process. The CopyIn process moves data from Global Memory to Local Memory. After the Vector computation unit completes the computation, the CopyOut process moves the computation results back to Global Memory.

**Figure 1**  Data movement and Vector computation process<a name="fig994415385460"></a>  
![](../../../figures/data_movement_and_vector_computation_process.png "Data movement and Vector computation process")

In this process, data movement and Vector computation are executed serially, and the Vector computation unit inevitably suffers from resource idle problems. For example, if the CopyIn, Compute, and CopyOut phases each take _t_, the time utilization of the Vector is only 1/3. The waiting time is too long, and the Vector utilization is severely insufficient.

To reduce the Vector waiting time, the DoubleBuffer mechanism splits the data to be processed into two parts, such as Tensor1 and Tensor2. As shown in [Figure 2](#fig189541246194710), when the Vector performs Compute on the data in Tensor1, Tensor2 can execute the CopyIn process. When the Vector switches to computing Tensor2, Tensor1 can execute the CopyOut process. In this way, data movement in and out and Vector computation are executed in parallel, effectively alleviating the Vector idle problem.

In general, DoubleBuffer is based on the independence and parallel executability of the MTE instruction queue and the Vector instruction queue. It executes data movement and Vector computation in parallel to hide the data movement time and reduce the waiting time of Vector instructions, ultimately improving the utilization efficiency of the Vector unit. You can implement data parallelism by setting the number of memory blocks when applying for memory for the queue. A simple code example is as follows:

```
pipe.InitBuffer(inQueueX, 2, 256);
```

**Figure 2**  DoubleBuffer mechanism<a name="fig189541246194710"></a>  
![](../../../figures/doublebuffer_mechanism.png "DoubleBuffer mechanism")

**Note**:

In most cases, DoubleBuffer can effectively improve the time utilization of the Vector and reduce the operator execution time. However, the fact that the DoubleBuffer mechanism alleviates the Vector idle problem does not mean that it always brings overall performance improvement. For example:

-   When the data movement time is short while the Vector computation time is significantly long, the performance gain brought by the DoubleBuffer mechanism is relatively small because data movement accounts for a low proportion of the entire computation process.
-   In another case, when the original data is small and the Vector can complete all computations at once, forcibly using DoubleBuffer reduces the utilization of Vector computing resources, and the final effect may be counterproductive.

Therefore, the performance gain of DoubleBuffer must be evaluated by comprehensively considering multiple factors such as Vector computing power, data size, and the proportion of movement time to computation time.

