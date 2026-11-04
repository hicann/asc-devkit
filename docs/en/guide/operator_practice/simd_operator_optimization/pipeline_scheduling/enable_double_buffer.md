# Enabling DoubleBuffer<a name="ZH-CN_TOPIC_0000001893038945"></a>

<!-- md-trans-meta sourceCommit=dd293a1c6b873d149bf289bb2b8098dadf9f5cbd translatedAt=2026-10-23T12:51:28.961Z pushedAt=2026-11-03T07:05:13.349Z nl=2 -->

[Priority] Medium

[Description] The instruction queues executed on the AI Core mainly include the following types: the Vector instruction queue (V), the Cube instruction queue (M), the Scalar instruction queue (S), and the movement instruction queues (MTE1/MTE2/MTE3). The mutual independence and parallel executability of different instruction queues are the cornerstone of the DoubleBuffer optimization mechanism.

Taking vector-only computation as an example, the CopyIn and CopyOut processes before and after vector computation use the movement instruction queues (MTE2/MTE3), and the Compute process uses the Vector instruction queue (V). Different instruction queues can be executed in parallel, which means that the CopyIn and CopyOut processes can run in parallel with the Compute process. As shown in [Figure 1](#fig994415385460), consider a complete data movement and computation process. The CopyIn process moves data from Global Memory to Local Memory. After the Vector computation unit completes the Compute operation, the computation result is moved back to Global Memory through the CopyOut process.

**Figure 1**  Data movement and vector computation process<a name="fig994415385460"></a>  
![](../../../figures/data_vec_54.png "Data movement and Vector computation process-54")

**Figure 2**  Pipeline chart without DoubleBuffer enabled<a name="fig101953515215"></a>  
![](../../../figures/no_doublebuf_pipe.png "Pipeline chart without DoubleBuffer enabled")

In this process, data movement and Vector computation are executed serially, and the Vector computation unit inevitably suffers from resource idle problems. Assuming that the CopyIn, Compute, and CopyOut phases each take the same time _t_, the utilization of the Vector is only 1/3. The waiting time is too long, and the Vector utilization is severely insufficient.

To reduce the vector waiting time, the DoubleBuffer mechanism is enabled to split the data to be processed into two parts, such as Tensor1 and Tensor2. As shown in [Figure 3](#fig189541246194710), when the Vector unit performs compute on the data in Tensor1, the Tensor2 data flow can execute the CopyIn process. When the Vector switches to computing Tensor2, the Tensor1 data flow can execute the CopyOut process. In this way, data movement in and out and Vector computation are executed in parallel, effectively alleviating the Vector idle problem.

In general, DoubleBuffer is based on the independence and parallel executability of the MTE instruction queue and the Vector instruction queue. It executes data movement and Vector computation in parallel to hide most of the data movement time and reduce the waiting time of Vector instructions, ultimately improving the utilization efficiency of the Vector unit. You can enable DoubleBuffer and implement data parallelism by setting the number of memory blocks to 2 when allocating memory for the queue. A simple code example is as follows:

```
pipe.InitBuffer(inQueueX, 2, 256);
```

**Figure 3**  DoubleBuffer mechanism<a name="fig189541246194710"></a>  
![](../../../figures/dbuf_55.png "DoubleBuffer mechanism-55")

**Figure 4**  Pipeline chart with DoubleBuffer enabled<a name="fig166411527185118"></a>  
![](../../../figures/dbuf_pipe.png "Pipeline chart with DoubleBuffer enabled")

**Note:**

In most cases, DoubleBuffer can effectively improve the utilization of the Vector and reduce the operator execution time. However, the fact that the DoubleBuffer mechanism alleviates the Vector idle problem does not mean that it always brings significant overall performance improvement. For example:

-   When the data movement time is short while the Vector computation time is long, the performance gain brought by the DoubleBuffer mechanism is relatively small because data movement accounts for a low proportion of the entire computation process.
-   When the original data is small and the Vector can complete the computation of all data at once, forcibly using DoubleBuffer reduces the utilization of Vector computing resources, and the final effect may be counterproductive.

Therefore, the use of DoubleBuffer must be evaluated by comprehensively considering multiple factors such as Vector computing power, data size, and the proportion of movement time to computation time.

[Negative example]

```
__aicore__ inline void Init(__gm__ uint8_t* src0Gm, __gm__ uint8_t* src1Gm, __gm__ uint8_t* dstGm)
{
    src0Global.SetGlobalBuffer((__gm__ half*)src0Gm);
    src1Global.SetGlobalBuffer((__gm__ half*)src1Gm);
    dstGlobal.SetGlobalBuffer((__gm__ half*)dstGm);
    // Without DoubleBuffer, the occupied physical space is 1 * sizeSrc0 * sizeof(half)
    // After the three InitBuffer calls, the total space is 1 * (sizeSrc0 * sizeof(half) + sizeSrc1 * sizeof(half) + sizeDst0 * sizeof(half)) 
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

[Positive example]

```
__aicore__ inline void Init(__gm__ uint8_t* src0Gm, __gm__ uint8_t* src1Gm, __gm__ uint8_t* dstGm)
{
    src0Global.SetGlobalBuffer((__gm__ half*)src0Gm);
    src1Global.SetGlobalBuffer((__gm__ half*)src1Gm);
    dstGlobal.SetGlobalBuffer((__gm__ half*)dstGm);
    // In InitBuffer, 2 indicates that DoubleBuffer is enabled, and the occupied physical space is 2 * sizeSrc0 * sizeof(half).
    // After the three InitBuffer calls are executed, the total space is 2 * (sizeSrc0 * sizeof(half) + sizeSrc1 * sizeof(half) + sizeDst0 * sizeof(half)). 
    pipe.InitBuffer(inQueueSrc0, 2, sizeSrc0 * sizeof(half));
    pipe.InitBuffer(inQueueSrc1, 2, sizeSrc1 * sizeof(half));
    pipe.InitBuffer(outQueueDst, 2, sizeDst0 * sizeof(half));
    }
__aicore__ inline void Process()
{
    // The prerequisite for enabling DoubleBuffer is that the number of loop iterations is greater than or equal to 2.
    for (uint32_t index = 0; index < round; ++index) {
        CopyIn(index);
        Compute();
        CopyOut(index);
    }
}
```
