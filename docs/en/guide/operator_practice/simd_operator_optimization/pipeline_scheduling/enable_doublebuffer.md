# Enabling DoubleBuffer<a name="ZH-CN_TOPIC_0000001893038945"></a>

<!-- md-trans-meta sourceCommit=d0ede7621d11bdc88cdafb221560ec6e0bfe996d translatedAt=2026-08-26T14:17:28.241Z -->

[Priority] Medium

[Description] The instruction queues executed on the AI Core mainly include the following types: the Vector instruction queue (V), the Cube instruction queue (M), the Scalar instruction queue (S), and the data movement instruction queues (MTE1/MTE2/MTE3). The mutual independence and parallel execution capability of different instruction queues are the foundation of the DoubleBuffer optimization mechanism.

Taking pure Vector computation as an example, the CopyIn and CopyOut processes before and after vector computation use the data movement instruction queues (MTE2/MTE3), and the Compute process uses the Vector instruction queue (V). Different instruction queues can be executed in parallel, which means that the CopyIn, CopyOut, and Compute processes can run in parallel. As shown in [Figure 1](#fig994415385460), consider a complete data movement and computation process: the CopyIn process moves data from Global Memory to Local Memory, and after the Vector computation unit completes the Compute operation, the CopyOut process moves the computation result back to Global Memory.

**Figure 1**  Data movement and Vector computation process<a name="fig994415385460"></a>  
![](../../../figures/data_movement_and_vector_computation_process_54.png "Data movement and Vector computation process-54")

**Figure 2**  Pipeline chart without DoubleBuffer enabled<a name="fig101953515215"></a>  
![](../../../figures/pipeline_chart_with_doublebuffer_disabled.png "Pipeline chart without DoubleBuffer enabled")

In this process, data movement and Vector computation are executed serially, and the Vector computation unit inevitably suffers from resource idle problems. Assuming that the CopyIn, Compute, and CopyOut stages each take the same time _t_, the Vector utilization is only 1/3. The waiting time is too long, and the Vector utilization is severely insufficient.

To reduce the Vector waiting time, the DoubleBuffer mechanism is enabled to split the data to be processed into two parts, for example, Tensor1 and Tensor2. As shown in [Figure 3](#fig189541246194710), when the Vector unit performs Compute on the data in Tensor1, the Tensor2 data flow can execute the CopyIn process; when the Vector switches to computing Tensor2, the Tensor1 data flow can execute the CopyOut process. In this way, data movement in and out and Vector computation are executed in parallel, and the Vector idle problem is effectively alleviated.

In general, DoubleBuffer is based on the independence and parallelism of the MTE instruction queues and the Vector instruction queue. By executing data movement and Vector computation in parallel, it hides most of the data movement time, reduces the waiting time of Vector instructions, and ultimately improves the utilization efficiency of the Vector unit. DoubleBuffer is enabled by setting the number of memory blocks to 2 when applying for memory for the queue, thereby implementing data parallelism. A simple code example is as follows:

```
pipe.InitBuffer(inQueueX, 2, 256);
```

**Figure 3**  DoubleBuffer mechanism<a name="fig189541246194710"></a>  
![](../../../figures/doublebuffer_mechanism_55.png "DoubleBuffer mechanism-55")

**Figure 4** Pipeline chart with DoubleBuffer enabled<a name="fig166411527185118"></a>  
![](../../../figures/pipeline_chart_with_doublebuffer_enabled.png "Pipeline chart with DoubleBuffer enabled")

**Note:**

In most cases, DoubleBuffer can effectively improve Vector utilization and reduce operator execution time. However, the fact that the DoubleBuffer mechanism alleviates the Vector idle problem does not mean that it always brings significant overall performance improvement. For example:

-   When the data movement time is short while the Vector computation time is long, the performance gain from the DoubleBuffer mechanism is relatively small because data movement accounts for a low proportion of the entire computation process.
-   When the original data is small and Vector can complete the computation of all data at once, forcibly using DoubleBuffer reduces the utilization of Vector computation resources, and the final result may be counterproductive.

Therefore, the use of DoubleBuffer requires comprehensive consideration of multiple factors, such as Vector computing power, data volume, and the proportion of data movement time to computation time.

[Negative Example]

```
__aicore__ inline void Init(__gm__ uint8_t* src0Gm, __gm__ uint8_t* src1Gm, __gm__ uint8_t* dstGm)
{
    src0Global.SetGlobalBuffer((__gm__ half*)src0Gm);
    src1Global.SetGlobalBuffer((__gm__ half*)src1Gm);
    dstGlobal.SetGlobalBuffer((__gm__ half*)dstGm);
    // When DoubleBuffer is not enabled, the occupied physical space is 1 * sizeSrc0 * sizeof(half)
    // After the three InitBuffer calls are executed, the total space is 1 * (sizeSrc0 * sizeof(half) + sizeSrc1 * sizeof(half) + sizeDst0 * sizeof(half))
    pipe.InitBuffer(inQueueSrc0, 1, sizeSrc0 * sizeof(half));
    pipe.InitBuffer(inQueueSrc1, 1, sizeSrc1 * sizeof(half));
    pipe.InitBuffer(outQueueDst, 1, sizeDst0 * sizeof(half));
    }
__aicore__ inline void Process()
{
    // round*2 loops are required to process all the data
    for (uint32_t index = 0; index < round * 2; ++index) {
        CopyIn(index);
        Compute();
        CopyOut(index);
    }
}
```

[Positive Example]

```
__aicore__ inline void Init(__gm__ uint8_t* src0Gm, __gm__ uint8_t* src1Gm, __gm__ uint8_t* dstGm)
{
    src0Global.SetGlobalBuffer((__gm__ half*)src0Gm);
    src1Global.SetGlobalBuffer((__gm__ half*)src1Gm);
    dstGlobal.SetGlobalBuffer((__gm__ half*)dstGm);
    // In InitBuffer, 2 indicates that DoubleBuffer is enabled, and the occupied physical space is 2 * sizeSrc0 * sizeof(half)
    // After the three InitBuffer calls are executed, the total space is 2 * (sizeSrc0 * sizeof(half) + sizeSrc1 * sizeof(half) + sizeDst0 * sizeof(half))
    pipe.InitBuffer(inQueueSrc0, 2, sizeSrc0 * sizeof(half));
    pipe.InitBuffer(inQueueSrc1, 2, sizeSrc1 * sizeof(half));
    pipe.InitBuffer(outQueueDst, 2, sizeDst0 * sizeof(half));
    }
__aicore__ inline void Process()
{
    // The prerequisite for enabling DoubleBuffer is that the number of loop iterations is >= 2
    for (uint32_t index = 0; index < round; ++index) {
        CopyIn(index);
        Compute();
        CopyOut(index);
    }
}
```
