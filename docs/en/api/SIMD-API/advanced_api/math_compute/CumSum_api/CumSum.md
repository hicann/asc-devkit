# CumSum

<!-- md-trans-meta sourceCommit=2bc2b38d5648dc61f632186b01ba7f36c2295b13 translatedAt=2026-09-09T14:28:54.197Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs a cumulative sum operation on the input tensor by row or by column. Each element in the output result is the cumulative sum of the element at the corresponding position and all preceding rows or columns in the input tensor.

The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002340790900.png)

-   Row-by-row accumulation algorithm
    -   First axis processing: performs a cumulative sum operation by row, that is, the first row remains unchanged and subsequent rows are accumulated in sequence. The calculation formula for the element in the i-th row and j-th column of the output result is as follows:

        ![](../../../../figures/zh-cn_formulaimage_0000002223791509.png)

        Taking tensor\(\[\[0, 1, 2\], \[3, 4, 5\]\]\) as an example, the output result is tensor\(\[\[0, 1, 2\], \[3, 5, 7\]\]\)

    -   Last axis processing: performs a cumulative sum operation by column, that is, the first column remains unchanged and subsequent columns are accumulated in sequence. The calculation formula for the element in the i-th row and j-th column of the output result is as follows:

        ![](../../../../figures/zh-cn_formulaimage_0000002223797889.png)

        Taking tensor\(\[\[0, 1, 2\], \[3, 4, 5\]\]\) as an example, the output result is tensor\(\[\[0, 1, 3\], \[3, 7, 12\]\]\)

-   Sklansky binary accumulation algorithm

    Only supported on Ascend 950PR/Ascend 950DT.

    The Sklansky binary accumulation algorithm is implemented based on the parallel prefix sum logic of the Sklansky Adder. [Figure 1](#fig45153311379) shows the algorithm illustration of the one-dimensional binary parallel prefix sum. This algorithm is extended to the cumulative sum algorithm for two-dimensional tensors. Taking row-wise accumulation as an example, [Figure 2](#fig7422174293712) shows the execution steps of this algorithm. By computing the sums of multiple rows in parallel, the row-wise cumulative sum under the Sklansky binary accumulation algorithm is implemented.

    **Figure 1**  Sklansky Adder algorithm<a name="fig45153311379"></a>  
    ![](../../../../figures/Sklansky_Adder_algorithm.png "Sklansky Adder algorithm")

    **Figure 2**  Sklansky-based binary accumulation<a name="fig7422174293712"></a>  
    ![](../../../../figures/Sklansky_based_binary_accumulation.png "Sklansky-based binary accumulation")

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, const CumSumConfig& config = defaultCumSumConfig>
    __aicore__ inline void CumSum(LocalTensor<T>& dstTensor, LocalTensor<T>& lastRowTensor, const LocalTensor<T>& srcTensor, LocalTensor<uint8_t>& sharedTmpBuffer, const CumSumInfo& cumSumInfo)
    ```

-   The API framework allocates temporary space.

    ```
    template <typename T, const CumSumConfig& config = defaultCumSumConfig>
    __aicore__ inline void CumSum(LocalTensor<T>& dstTensor, LocalTensor<T>& lastRowTensor, const LocalTensor<T>& srcTensor, const CumSumInfo& cumSumInfo)
    ```

Because the internal implementation of this API involves precision conversion, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by you passing it via the sharedTmpBuffer input parameter**.

-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.

For the API framework allocation method, you need to reserve temporary space. For the sharedTmpBuffer passing method, you need to allocate space for the tensor. The temporary space size BufferSize is obtained as follows: use the API provided in [GetCumSumMaxMinTmpSize](GetCumSumMaxMinTmpSize.md) to obtain the size of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |
| config | Defines the config parameter of the CumSum API at compile time. The type is CumSumConfig, which is defined in the following code. The meanings of its parameters are as follows:<br><br>isLastAxis: The value **true** indicates that the computation is processed along the last axis. The value **false** indicates that the computation is processed along the first axis.<br><br>isReuseSource: Whether the memory space of srcTensor can be reused. This parameter is reserved. Pass the default value false.<br><br>outputLastRow: Whether to output the last row of data.<br>algorithm: The cumulative sum algorithm used in the internal implementation of CumSum. This parameter is only supported on Ascend 950PR/Ascend 950DT. The supported values are as follows: CumSumAlgorithm::CUMSUM_ALGORITHM_LINEBYLINE: row-by-row accumulation algorithm. CumSumAlgorithm::CUMSUM_ALGORITHM_SKLANSKY: Sklansky binary accumulation algorithm. |

```
struct CumSumConfig {
    bool isLastAxis{true};
    bool isReuseSource{false};
    bool outputLastRow{false};
    CumSumAlgorithm algorithm{CumSumAlgorithm::CUMSUM_ALGORITHM_LINEBYLINE};
};
enum class CumSumAlgorithm {
    CUMSUM_ALGORITHM_LINEBYLINE = 0,
    CUMSUM_ALGORITHM_SKLANSKY = 1
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand. The cumulative sum of the input elements processed along the first axis or the last axis.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| lastRowTensor | Output | Destination operand. When the outputLastRow parameter in the template parameter config is true, the last row of data is output.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside CumSum, provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetCumSumMaxMinTmpSize](GetCumSumMaxMinTmpSize.md). |
| cumSumInfo | Input | Shape information of srcTensor. The type is CumSumInfo, which is defined in the following code. The meanings of its parameters are as follows:<br><br>outter: Indicates the outer axis length of the input data.<br><br>inner: Indicates the inner axis length of the input data.<br><br>Note:<br><br>Both cumSumInfo.outter and cumSumInfo.inner must be greater than 0.<br><br>cumSumInfo.outter * cumSumInfo.inner must not be greater than the size of dstTensor or srcTensor.<br><br>cumSumInfo.inner * sizeof(T) must be an integer multiple of 32 bytes.<br><br>When the outputLastRow parameter in the template parameter config is true, cumSumInfo.inner must not be greater than the size of the last row of data output by lastRowTensor. |

```
struct CumSumInfo
{
    uint32_t outter{0};
    uint32_t inner{0};
};
```

## Return Value

None

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   The input only supports a two-dimensional structure.
-   **cumSumInfo.inner** \* sizeof\(T\) must be an integer multiple of 32 bytes.

## Examples

For the complete calling example, see the [CumSum example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/10_math/cumsum).

```
// dstLocal: Tensor that stores the computation result.
// lastRowLocal: Tensor that stores the last row of the computation result.
// srcLocal: Input tensor that participates in the computation.

// Process along the last axis (column-wise accumulation), output the last column of data, and use the row-by-row accumulation algorithm.
constexpr AscendC::CumSumConfig cumSumConfig{true, false, true, AscendC::CumSumAlgorithm::CUMSUM_ALGORITHM_LINEBYLINE};
// outer: Outer axis length.
// inner: Inner axis length.
const AscendC::CumSumInfo cumSumInfo{outer, inner};
AscendC::CumSum<T, cumSumConfig>(dstLocal, lastRowLocal, srcLocal, cumSumInfo);

// Process along the first axis (row-wise accumulation), output the last row of data, and use the row-by-row accumulation algorithm.
constexpr AscendC::CumSumConfig cumSumConfig{false, false, true, AscendC::CumSumAlgorithm::CUMSUM_ALGORITHM_LINEBYLINE};
AscendC::CumSum<T, cumSumConfig>(dstLocal, lastRowLocal, srcLocal, cumSumInfo);
```

When **cumSumConfig** takes the values \{true, **false**, **true**, AscendC::CumSumAlgorithm::CUMSUM\_ALGORITHM\_LINEBYLINE\}, the example data is as follows:

```
Input data (srcLocal): [1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1]
Output data (dstLocal): [1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8]
Output data (lastRowLocal): [8 8 8 8]
```

When **cumSumConfig** is \{false, false, true, AscendC::CumSumAlgorithm::CUMSUM\_ALGORITHM\_LINEBYLINE\}, the example data is as follows:

```
Input data (srcLocal): [1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1]
Output data (dstLocal): [1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3 4 4 4 4 4 4 4 4]
Output data (lastRowLocal): [4 4 4 4 4 4 4 4]
```
