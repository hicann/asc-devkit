# Normal Precision<a name="ZH-CN_TOPIC_0000001892919357"></a>

<!-- md-trans-meta sourceCommit=e3c8c70b3f5d27f3770063ae4b535bc75d904e3e translatedAt=2026-08-26T12:58:23.834Z -->

The basis of performance optimization is that the operator produces correct computation results. Evaluating the correctness of computation results requires certain criteria, that is, comparing the actual results with known correct outputs. During optimization, after each iterative modification, you need to verify whether the new performance optimization results meet the precision evaluation criteria.

The following describes several factors that affect precision correctness:

-   Correct insertion of synchronization. The synchronization mechanism is one of the core features of the parallel computing architecture. For scenarios with data dependencies, synchronization must be inserted correctly.
-   Correct calculation of the offset address. In multi-core parallel computing, correctly calculating the offset of data in memory is critical to ensuring the correctness of computation results.
-   Floating-point computation. When floating-point computation is involved, precision correctness cannot be expected to be bit-identical. This is because floating-point computation itself does not satisfy the commutative law or the associative law, and different hardware provides different support for floating-point numbers, both of which may lead to differences in precision results.

Then, the rules that must be strictly followed during coding (modification of kernel function parameters is prohibited) are introduced to prevent unnecessary precision issues.

## Correct Insertion of Synchronization<a name="section612383291014"></a>

-   Intra-core synchronization

    An AI Core contains multiple pipelines such as MTE1, MTE2, MTE3, Cube, Vector, and Scalar. The Ascend C framework enables the auto sync (automatic synchronization insertion) compilation option by default, so the compiler can insert synchronization normally. The Ascend C programming model also helps developers control the synchronization of some pipelines. For details about pipeline types, synchronization type classification, constraints on automatic synchronization by the compiler, and when developers need to manually insert synchronization, see [Intra-core Synchronization Overview](../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/intra_core_synchronization_capability_overview.md).

-   Inter-core synchronization

    The preceding content describes intra-core synchronization. In particular, when an operator uses multi-core synchronization (for the multi-core synchronization concept, see [Multi-core Synchronization](../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/IBSet.md)), the number of logical cores NumBlocks must not be greater than the number of AI processor cores that actually run the operator. Otherwise, an exception occurs when the framework inserts synchronization, causing the Kernel to "hang".

    Counterexample

    ```
    // NumBlocks is greater than CoreNum in code that contains multi-core synchronization logic
    // For example, this check is not performed in the Tiling computation
    FlashAttentionScoreApiTiling(tilingData);
    FlashAttentionScoreGetTensorSize(tilingData);
    CoreNum = ascendcPlatform.GetCoreNum();
    context->SetBlockDim(CoreNum + 1);
    ```

    Correct example

    ```
    FlashAttentionScoreApiTiling(tilingData);
    FlashAttentionScoreGetTensorSize(tilingData);
    // When multi-core synchronization instructions are used in the Kernel, the Host must ensure that NumBlocks is not greater than CoreNum when setting NumBlocks
    CoreNum = ascendcPlatform.GetCoreNum();
    context->SetBlockDim(CoreNum);
    ```

## Correct Computation of the Offset Address<a name="section71815195202"></a>

When an operator performs multi-core computation, the computation workload of a single core must be determined during tiling, and the Kernel side performs address offset based on the single-core computation workload.

For example, consider the following allocation scheme: the total data length TOTAL\_LENGTH is 8 \* 2048 elements, which are evenly distributed across 8 cores, and the data size BLOCK\_LENGTH processed on each core is 2048. x + BLOCK\_LENGTH \* GetBlockIdx\(\) is the memory offset address of the input x in Global Memory in the single-core processing program. After obtaining the offset address, use the SetGlobalBuffer API of the GlobalTensor class to set the start address and length of Global Memory on that core. For details, see [Figure 1](#fig398721711313).

```
xGm.SetGlobalBuffer((__gm__ half*)x + BLOCK_LENGTH * GetBlockIdx(), BLOCK_LENGTH);
```

**Figure 1**  Multi-core parallel processing diagram<a name="fig398721711313"></a>  
![](../../figures/multi_core_parallel_processing_diagram_53.png "Multi-core parallel processing diagram")

## Floating-Point Computation<a name="section1366081311215"></a>

-   Every floating-point arithmetic operation involves a certain amount of rounding. Therefore, the order in which arithmetic operations are executed matters. If A, B, and C are floating-point values, \(A+B\)+C is not guaranteed to equal A+\(B+C\) as it would in mathematical computation. When computations are performed in parallel, the order of operations may change, so the parallel result may not match the sequential result. The precision difference introduced in this case is inherent to floating-point value computation.
-   Some AI processor models support a limited set of data types in their instructions. When the data types supported by an API cannot meet your requirements, you should prefer to first convert the data to a higher precision for computation, and then convert the computation result back to the target precision to prevent precision loss. For example, if the Vector computation APIs of a certain AI processor do not support bfloat16 computation, you need to first use the Cast API to convert the data to the float data type, perform the computation, and then use the Cast API to convert the result back to the bfloat16 data type.

    **[Correct Example]**

    ```
    // dst = src0 + src1, where src0, src1, and dst are of the bfloat16 type, and tmp0, tmp1, and tmp2 are of the float type
    ...
    Cast(tmp0Tensor, src0Tensor, RoundMode::CAST_NONE, computeSize);
    Cast(tmp1Tensor, src1Tensor, RoundMode::CAST_NONE, computeSize);
    Add(tmp2Tensor, tmp0Tensor, tmp1Tensor, computeSize);
    Cast(dstTensor, tmp2Tensor, RoundMode::CAST_FLOOR, computeSize);
    ...
    ```

-   Ascend AI processors follow the IEEE 754 standard for binary floating-point representation, with a few minor exceptions. These exceptions may produce results that differ from IEEE 754 values computed on the host system. For example, the API Axpy, which uses a compound instruction, multiplies each element of the source operand by a scalar and then adds the product to the corresponding element of the destination operand. This compound instruction combines the multiply and add operations into a single instruction, so the computation result may differ slightly from the result obtained by executing these two operations as separate single instructions. When using such APIs, developers need to take this precision difference into account.

## Kernel Function Parameter Modification Prohibited<a name="section7609173442117"></a>

Modification of Kernel function parameters is prohibited. You must not reassign or modify function parameters. For example, the **FlashAttentionKernel** function is defined as follows. Its parameters query, key, and tilingData are of pointer type, and the pointers themselves must not be modified. For operator input parameters, the content referenced by the pointer must not be modified. As an exception, for operator output parameters, the content referenced by the pointer may be modified. It must be emphasized that, to implement static compilation, modification is prohibited for both the tilingData pointer itself and the content referenced by the tilingData pointer.

```
__aicore__ __global__ void FlashAttentionKernel(__gm__ uint8_t* query, __gm__ uint8_t* key, ..., __gm__ uint8_t* attention,..., __gm__ uint8_t* tilingData) {
    ......
}
```

[Negative Example]

```
// Reassigning Kernel function parameters and modifying the TilingData content are not allowed. The following is an incorrect example.
query = tmpQueryPtr;
key = tmpKeyPtr;
tilingData = tmpTilingDataPtr; 
tilingData[0] = 2;
```

[Positive Example]

```
// The input parameter is read-only.
inputQueryGMTensor.SetGlobalBuffer(query);

// The output parameter attention pointer itself is read-only, but the memory it references can be read and written.
outputAttentionGMTensor.SetGlobalBuffer(attention);
...
DataCopy(outputAttentionGMTensor, outputAttentionLocalTensor, count);
```
