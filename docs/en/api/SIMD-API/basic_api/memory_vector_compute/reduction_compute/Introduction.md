# Reduction Computation Overview

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-05T01:33:28.357Z pushedAt=2026-11-18T01:25:30.723Z nl=2 -->

**Table 1** Reduction computation APIs and main functions

| API | Main Function | Output Length |
| --- | --- | --- |
| [ReduceDataBlock](./ReduceDataBlock.md) | Performs summation/finds the maximum/finds the minimum of the data in each DataBlock. | 1 element / DataBlock |
| [ReduceRepeat](./ReduceRepeat.md) | Performs summation/finds the maximum/finds the minimum of the data in each repeat.<br>When finding the maximum or minimum, you can optionally output the corresponding index. | Summation: 1 element / repeat;<br>Find maximum/minimum: 1 element (1 index) / repeat. |
| [ReducePairElem](./ReducePairElem.md) | Performs summation on two adjacent elements. | 1 element / pair |
| [ReduceSum](./ReduceSum.md) | Performs summation on all input data. | 1 element |
| [ReduceMax](./ReduceMax.md)/[ReduceMin](./ReduceMin.md) | Finds the maximum/minimum of all input data.<br>You can optionally output the corresponding index. | 1 element (1 index) |
| [GetReduceRepeatSumSpr(ISASI)](../reduction_compute_aux_config/GetReduceRepeatSumSpr_ISASI.md) | Obtains the computation result of the `ReduceSum` API. | 1 element |
| [GetReduceRepeatMaxMinSpr(ISASI)](../reduction_compute_aux_config/GetReduceRepeatMaxMinSpr_ISASI.md) | Obtains the maximum/minimum value and its index across all repeats when calling `ReduceRepeat<MAX/MIN>`, or obtains the maximum/minimum value obtained by calling `ReduceMax` or `ReduceMin`. | 1 element (1 index) |

<!-- npu="950" id1 -->
For Ascend 950PR&950DT products, the reduction computation APIs of Memory vector computation are all converted to Reg vector computation instructions through VF calls at the underlying layer. The APIs incur additional VF call overhead and Unified Buffer (UB) transfer (such as sharedTmpBuffer). Therefore, Memory APIs do not guarantee optimal performance. For performance-sensitive scenarios, it is recommended to directly use the [reduction computation](../../reg_vector_compute/reduction_compute/reduction_compute.md) in Reg vector computation (Reg::Reduce, Reg::PairReduceElem, Reg::ReduceDataBlock).
<!-- end id1 -->

<!-- npu="A3,910b" id2 -->
For Atlas A3 products and Atlas A2 products, the following usage recommendations apply when using reduction computation APIs.

- `ReduceDataBlock`
  - **Recommended usage scenarios**:
    - When the data type is `float`: Compared with `ReduceRepeat`, it has higher theoretical parallelism; compared with `ReducePairElem`, it has the same theoretical parallelism but can reduce more data.
    - When the data type is `half` and the data volume is less than or equal to one DataBlock: `ReduceDataBlock` and `ReduceRepeat` have the same theoretical parallelism, but `ReduceDataBlock` supports reducing only one DataBlock, while `ReduceRepeat` needs to reduce eight DataBlocks (one repeat). Therefore, `ReduceDataBlock` is more efficient.
      - Example: When the data length is 2K, you can first perform one `ReduceRepeat` reduction to obtain a length of 16 (32 bytes), and then perform one `ReduceDataBlock`. In this case, one `ReduceRepeat` plus one `ReduceDataBlock` takes less time than two `ReduceRepeat` operations.
  - Supported data types: `half`, `float`

- `ReduceRepeat`
  - **Recommended usage scenarios**:
    - Scenarios where the index of the maximum or minimum value is required and performance is critical.
    - When the data type is `half` and the data volume exceeds one DataBlock: `ReduceRepeat` and `ReduceDataBlock` have the same theoretical parallelism, but `ReduceRepeat` can reduce eight DataBlocks (one repeat) in a single call, whereas `ReduceDataBlock` requires at least two calls. Therefore, `ReduceRepeat` takes less time.
  - Supported data types: `half`, `float`

- `ReducePairElem`
  - **Recommended usage scenarios**:
    - Suitable for summation of two adjacent elements.
    - When the data type is `half`, `ReducePairElem` has the highest theoretical parallelism among the reduction instructions. Therefore, when the data volume is large and multiple calls to `ReduceDataBlock` and `ReduceRepeat` are required, it is suitable for performing preliminary reduction on the data.
  - Supported data types: `half`, `float`

- `ReduceSum`
  - **Recommended usage scenarios**:
    - This API is implemented through software emulation, so its performance may be inferior to the `ReduceDataBlock` and `ReduceRepeat` APIs implemented through hardware instructions. It is not recommended for performance-sensitive scenarios.
    - This API obtains the reduction result with a single call and supports longer input data, making it suitable for scenarios that require quick implementation verification.
  - Supported data types: `half`, `float`

- `ReduceMax`/`ReduceMin`
  - **Recommended usage scenarios**:
    - This API is implemented through software emulation, so its performance may be inferior to the `ReduceDataBlock` and `ReduceRepeat` APIs implemented through hardware instructions. It is not recommended for performance-sensitive scenarios.
    - This API obtains the reduction result with a single call and supports longer input data, making it suitable for scenarios that require quick implementation verification.
  - Supported data types: `half`, `float`

- `GetReduceRepeatSumSpr`
  - **Recommended usage scenario**: Obtains the `ReduceSum` result for the scenario of computing the first n data of a tensor.
  - Supported data types: `half`, `float`

- `GetReduceRepeatMaxMinSpr`
  - **Recommended usage scenario**: Obtains the maximum/minimum values and their indices across all repeats of `ReduceRepeat<MAX/MIN>` in a continuous computation scenario, or obtains the maximum/minimum values produced by `ReduceMax`/`ReduceMin` in a continuous computation scenario.
  - Supported data types: `half`, `float`

To select a reduction API, refer to the following [decision tree](#fig1):

<a id="fig1"></a>

**Figure 1**  Reduction API selection decision tree

![Reduction API selection decision tree](../../../../figures/reduce_decision_tree.png "Reduction API selection decision tree")

Note: When the data type is `half`, the parallelism of the `ReduceDataBlock` and `ReduceRepeat` APIs is lower than that of `ReducePairElem` and the basic arithmetic APIs [Add](../basic_arithmetic/Add.md), [Max](../basic_arithmetic/Max.md), and [Min](../basic_arithmetic/Min.md). When the data volume is large and `ReduceDataBlock` and `ReduceRepeat` need to be called multiple times, consider using `ReducePairElem` or the basic arithmetic APIs to perform preliminary reduction on the data before calling the reduction computation APIs.
<!-- end id2 -->
