# Normal Accuracy<a name="ZH-CN_TOPIC_0000001892919357"></a>

<!-- md-trans-meta sourceCommit=7ef8ad7bdbdb0cef0f561d391759ecb8b4e4043a translatedAt=2026-10-23T11:43:27.626Z pushedAt=2026-11-03T07:05:13.284Z nl=2 -->

The basis of performance optimization is that the operator runs and produces correct computation results. Evaluating the correctness of computation results requires certain evaluation criteria, that is, comparing the actual results with known correct outputs. During optimization, after each iterative modification, you need to verify whether the new performance optimization results meet the accuracy evaluation criteria.

The following first introduces several factors that affect accuracy normal:

-   Correct insertion of synchronization: The synchronization mechanism is one of the core features of the parallel computing architecture. For scenarios with data dependencies, synchronization must be inserted correctly.
-   Correct calculation of the offset address: In multi-core parallel computing, correctly calculating the offset of data in memory is critical to ensuring the correctness of computation results.
-   Floating-point computation: When floating-point computation is involved, accuracy cannot be expected to be bit-identical. This is because floating-point computation itself does not satisfy the commutative law or the associative law, and different hardware provides different support for floating-point numbers, both of which may lead to differences in accuracy results.

Then, the rules that must be strictly followed during coding (modification of kernel function parameters is prohibited) are introduced to prevent unnecessary accuracy issues.

## Correct Insertion of Synchronization<a name="section612383291014"></a>

-   Intra-core synchronization

    The AI Core contains multiple pipelines such as MTE1, MTE2, MTE3, Cube, Vector, and Scalar. The Ascend C framework enables the auto sync (automatic synchronization insertion) compilation option by default, so the compiler can insert synchronization correctly. The Ascend C programming model also helps developers complete part of the pipeline synchronization control. For details about pipeline types, synchronization type classification, constraints of compiler automatic synchronization, and when developers need to manually insert synchronization, see [Intra-Core Synchronization Overview](../../../api/SIMD-API/basic_api/sync_control/intra_core_sync/intra_core_sync_overview.md).

-   Inter-core synchronization

    The preceding description covers intra-core synchronization. In particular, when an operator uses multi-core synchronization (for the concept of multi-core synchronization, see [Multi-Core Synchronization](../../../api/SIMD-API/basic_api/sync_control/inter_core_sync/IBSet.md)), the number of logical cores NumBlocks must not be greater than the number of AI processor cores that actually run the operator. Otherwise, the synchronization inserted by the framework becomes abnormal, causing the kernel function to hang.

    [Negative example]

    ```
    // In code with multi-core synchronization logic, NumBlocks is greater than CoreNum
    // For example, this check is not performed in Tiling computation
    FlashAttentionScoreApiTiling(tilingData);
    FlashAttentionScoreGetTensorSize(tilingData);
    CoreNum = ascendcPlatform.GetCoreNum();
    context->SetSimdNumBlocks(CoreNum + 1);
    ```

    [Positive example]

    ```
    FlashAttentionScoreApiTiling(tilingData);
    FlashAttentionScoreGetTensorSize(tilingData);
    // When multi-core synchronization instructions are used in the kernel function, the Host must set NumBlocks to a value not greater than CoreNum
    CoreNum = ascendcPlatform.GetCoreNum();
    context->SetSimdNumBlocks(CoreNum);
    ```

## Correctly Calculating the Offset Address<a name="section71815195202"></a>

When an operator performs multi-core computation, the computation amount of a single core needs to be determined during Tiling, and the kernel function side performs address offset based on the computation amount of a single core.

For example, consider the following allocation scheme: the total data length TOTAL\_LENGTH is 8 \* 2048 elements, which are evenly allocated to 8 cores for execution, and the data size processed on each core BLOCK\_LENGTH is 2048. x + BLOCK\_LENGTH \* GetBlockIdx\(\) is the memory offset address of the input x in Global Memory in the single-core processing program. After the offset address is obtained, use the SetGlobalBuffer API of the GlobalTensor class to set the start address and length of Global Memory on this core. For the specific diagram, see [Figure 1](#fig398721711313).

```
xGm.SetGlobalBuffer((__gm__ half*)x + BLOCK_LENGTH * GetBlockIdx(), BLOCK_LENGTH);
```

**Figure 1**  Multi-core parallel processing diagram<a name="fig398721711313"></a>  
![](../../figures/multi_par_53.png "Multi-core parallel processing diagram-53")

## Floating-Point Computation<a name="section1366081311215"></a>

-   Every floating-point arithmetic operation involves a certain amount of rounding. Therefore, the order in which arithmetic operations are executed matters. If A, B, and C are floating-point values, \(A+B\)+C cannot be guaranteed to equal A+\(B+C\) as in mathematical computation. When computed in parallel, the order of operations may change, so the parallel result may not match the sequential result. The precision difference introduced in this case is inherent to floating-point value computation.
-   Some AI processor models support a limited set of data types in their instructions. When the data types supported by an API do not meet your requirements, you should prefer to first convert the data to a higher precision for computation, and then convert the computation result back to the target precision to prevent precision loss. For example, if the Vector computation APIs of a certain AI processor do not support bfloat16 computation, you need to first use the Cast API to convert the data to the float data type, perform the computation, and then use the Cast API to convert the result back to the bfloat16 data type.

    **[Positive example]**

    ```
    // dst = src0 + src1, where src0, src1, and dst are of the bfloat16 type, and tmp0, tmp1, and tmp2 are of the float type
    ...
    Cast(tmp0Tensor, src0Tensor, RoundMode::CAST_NONE, computeSize);
    Cast(tmp1Tensor, src1Tensor, RoundMode::CAST_NONE, computeSize);
    Add(tmp2Tensor, tmp0Tensor, tmp1Tensor, computeSize);
    Cast(dstTensor, tmp2Tensor, RoundMode::CAST_FLOOR, computeSize);
    ...
    ```

-   Ascend AI processors follow the IEEE 754 standard for binary floating-point representation, with a few minor exceptions. These exceptions may produce results that differ from IEEE 754 values computed on a host system. For example, the Axpy API uses a compound instruction: each element in the source operand is multiplied by a scalar and then added to the corresponding element in the destination operand. This compound instruction combines the multiply and add operations into a single instruction execution, so the computation result may differ slightly from the result obtained by executing the two operations as separate single instructions. When using such APIs, developers need to take this precision difference into account.

## Modification Prohibited for Kernel Function Parameters<a name="section7609173442117"></a>

Modification of kernel function parameters is prohibited. You must not reassign or modify the function parameters. For example, the **FlashAttentionKernel** function is defined as follows. Its parameters **query**, **key**, and **tilingData** are of pointer type, and the pointers themselves must not be modified. For operator input parameters, the content pointed to by the pointer must not be modified. As an exception, for operator output parameters, the content pointed to by the pointer can be modified. It must be emphasized that, to implement static compilation, modification is prohibited for both the **tilingData** pointer itself and the content pointed to by the **tilingData** pointer.

```
__aicore__ __global__ void FlashAttentionKernel(__gm__ uint8_t* query, __gm__ uint8_t* key, ..., __gm__ uint8_t* attention,..., __gm__ uint8_t* tilingData) {
    ......
}
```

[Negative example]

```
// Reassigning kernel function parameters and modifying the TilingData content are not allowed. The following is a negative example.
query = tmpQueryPtr;
key = tmpKeyPtr;
tilingData = tmpTilingDataPtr; 
tilingData[0] = 2;
```

[Positive example]

```
// Input parameters are read-only.
inputQueryGMTensor.SetGlobalBuffer(query);

// The attention pointer of the output parameter is read-only, but the memory it points to can be read and written.
outputAttentionGMTensor.SetGlobalBuffer(attention);
...
DataCopy(outputAttentionGMTensor, outputAttentionLocalTensor, count);
```
