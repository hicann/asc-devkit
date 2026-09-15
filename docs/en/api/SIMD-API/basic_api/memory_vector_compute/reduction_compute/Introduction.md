# Reduction Computation Overview

<!-- md-trans-meta sourceCommit=93c993b00c4f6138f380f7a2d742660ee1403179 translatedAt=2026-09-09T09:20:25.517Z -->

**Table 1** Reduction computation APIs and main functions

| API | Main Function | Output Length |
| --- | --- | --- |
| [ReduceDataBlock](ReduceDataBlock.md) | Computes the sum/maximum value/minimum value of the data in each DataBlock. | 1 element/DataBlock |
| [ReduceRepeat](ReduceRepeat.md) | Computes the sum/maximum value/minimum value of the data in each repeat.<br>When computing the maximum or minimum value, you can choose to output the corresponding index. | Summation: 1 element/repeat;<br>Maximum/minimum value: 1 element (1 index)/repeat. |
| [ReducePairElem](ReducePairElem.md) | Computes the sum of two adjacent elements. | 1 output element/2 input elements |
| [ReduceSum](ReduceSum.md) | Computes the sum of all input data. | 1 element |
| [ReduceMax](ReduceMax.md)/[ReduceMin](ReduceMin.md) | Computes the maximum/minimum value of all input data.<br>You can choose to output the corresponding index. | 1 element (1 index) |
| [GetReduceRepeatSumSpr(ISASI)](../reduction_compute_aux_config/GetReduceRepeatSumSpr_ISASI.md) | Obtains the computation result of the `ReduceSum` API. | 1 element |
| [GetReduceRepeatMaxMinSpr(ISASI)](../reduction_compute_aux_config/GetReduceRepeatMaxMinSpr_ISASI.md) | Obtains the maximum/minimum values and their indexes in all repeats when `ReduceRepeat<MAX/MIN>` is called, or obtains the maximum/minimum values obtained by calling `ReduceMax` and `ReduceMin`. | 1 element (1 index) |


For Atlas A3 training products/Atlas A3 inference products and Atlas A2 training products/Atlas A2 inference products, the following usage suggestions apply when using reduction computation APIs.

- `ReduceDataBlock`
  - **Recommended scenarios**:
    - When the data type is `float`: Compared with `ReduceRepeat`, it has higher theoretical parallelism; compared with `ReducePairElem`, it has the same theoretical parallelism but can reduce more data.
    - When the data type is `half` and the data volume is less than or equal to one DataBlock: `ReduceDataBlock` and `ReduceRepeat` have the same theoretical parallelism, but `ReduceDataBlock` supports reducing only one DataBlock, while `ReduceRepeat` needs to reduce eight DataBlocks (one repeat). Therefore, `ReduceDataBlock` is more efficient.
      - Example: When the data length is 2K, you can first perform one `ReduceRepeat` reduction to obtain a length of 16 (32 bytes), and then perform one `ReduceDataBlock`. In this case, one `ReduceRepeat` + one `ReduceDataBlock` takes less time than two `ReduceRepeat` operations.
  - Supported data types: `half`, `float`

- `ReduceRepeat`
  - **Recommended scenarios**:
    - Scenarios where the index of the maximum/minimum value is required and performance is critical.
    - When the data type is `half` and the data volume exceeds one DataBlock: `ReduceRepeat` and `ReduceDataBlock` have the same theoretical parallelism, but `ReduceRepeat` can reduce eight DataBlocks (one repeat) in a single call, whereas `ReduceDataBlock` requires at least two calls. Therefore, `ReduceRepeat` takes less time.
  - Supported data types: `half`, `float`

- `ReducePairElem`
  - **Recommended scenarios**:
    - Suitable for summation operations on two adjacent elements.
    - When the data type is `half`, `ReducePairElem` has the highest theoretical parallelism among the reduction instructions. Therefore, when the data volume is large and `ReduceDataBlock` and `ReduceRepeat` need to be called multiple times, it is suitable for preliminary reduction of the data.
  - Supported data types: `half`, `float`

- `ReduceSum`
  - **Recommended scenarios**:
    - This API is implemented through software emulation internally, so its performance may be inferior to that of the `ReduceDataBlock` and `ReduceRepeat` APIs implemented through hardware instructions. It is not recommended for performance-sensitive scenarios.
    - This API can obtain the reduction result with a single call and supports longer input data, making it suitable for scenarios that require quick verification of the implementation.
  - Supported data types: `half`, `float`

- `ReduceMax`/`ReduceMin`
  - **Recommended scenarios**:
    - This API is implemented through software emulation internally, so its performance may be inferior to that of the `ReduceDataBlock` and `ReduceRepeat` APIs implemented through hardware instructions. It is not recommended for performance-sensitive scenarios.
    - This API can obtain the reduction result with a single call and supports longer input data, making it suitable for scenarios that require quick verification of the implementation.
  - Supported data types: `half`, `float`

- `GetReduceRepeatSumSpr`
  - **Recommended scenarios**: Obtain the `ReduceSum` result for the scenario of computing the first n data of a tensor.
  - Supported data types: `half`, `float`

- `GetReduceRepeatMaxMinSpr`
  - **Recommended scenarios**: Obtain the maximum/minimum values and their indices across all repeats in the `ReduceRepeat<MAX/MIN>` continuous computation scenario, or obtain the maximum/minimum values produced by `ReduceMax`/`ReduceMin` in the continuous computation scenario.
  - Supported data types: `half`, `float`

To select a reduction API, refer to the following [decision tree](#fig1):

<a id="fig1"></a>

**Figure 1**  Reduction API selection decision tree

![Reduction API selection decision tree](../../../../figures/reduce_decision_tree.png "Reduction API selection decision tree")

Note: When the data type is `half`, the parallelism of the `ReduceDataBlock` and `ReduceRepeat` APIs is lower than that of the `ReducePairElem` and basic arithmetic APIs [Add](../basic_arithmetic/Add.md), [Max](../basic_arithmetic/Max.md), and [Min](../basic_arithmetic/Min.md). When the data volume is large and `ReduceDataBlock` and `ReduceRepeat` need to be called multiple times, you can consider using `ReducePairElem` or the basic arithmetic APIs to perform preliminary reduction on the data before calling the reduction computation APIs.

