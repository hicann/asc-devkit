# CumSum

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T02:40:15.903Z pushedAt=2026-10-29T02:15:02.645Z nl=2 -->

## Product Support

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference product: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
- Vector Core of Atlas inference products: not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/CumSum_interface/CumSum_res.md#id1 -->

## Description

Used to perform a cumulative sum operation on the input tensor row-wise or column-wise. Each element in the output result is the cumulative sum of the element at the corresponding position and all preceding rows or columns in the input tensor.

The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002340790900.png)

-   Row-wise accumulation algorithm
    -   First axis processing: performs a row-wise accumulation operation, that is, the first row remains unchanged and each subsequent row is accumulated in turn. The formula for the element in row i, column j of the output result is as follows:

        ![](../../../../figures/zh-cn_formulaimage_0000002223791509.png)

        Taking tensor\(\[\[0, 1, 2\], \[3, 4, 5\]\]\) as an example, the output result is tensor\(\[\[0, 1, 2\], \[3, 5, 7\]\]\)

    -   Last axis processing: performs a column-wise accumulation operation, that is, the first column remains unchanged and each subsequent column is accumulated in turn. The formula for the element in row i, column j of the output result is as follows:

        ![](../../../../figures/zh-cn_formulaimage_0000002223797889.png)

        Taking tensor\(\[\[0, 1, 2\], \[3, 4, 5\]\]\) as an example, the output result is tensor\(\[\[0, 1, 3\], \[3, 7, 12\]\]\)

-   Sklansky binary accumulation algorithm

    <!-- npu="950" id7 -->Supported only on the Ascend 950PR&950DT products.<!-- end id7 -->

    The Sklansky binary accumulation algorithm is based on the parallel prefix sum logic of the Sklansky adder. [Figure 1](#fig45153311379) shows the parallel prefix sum algorithm illustration for one-dimensional binary data. When extending this algorithm to the cumulative sum of two-dimensional tensors, taking row-wise accumulation as an example, [Figure 2](#fig7422174293712) shows the execution steps of the algorithm. By computing the sums of multiple rows in parallel, the row-wise accumulation under the Sklansky binary accumulation algorithm is achieved.

    **Figure 1**  Sklansky Adder algorithm illustration<a name="fig45153311379"></a>  
    ![](../../../../figures/sklansky_adder_diagram.png "Sklansky Adder algorithm illustration")

    **Figure 2**  Binary accumulation illustration based on Sklansky<a name="fig7422174293712"></a>  
    ![](../../../../figures/sklansky_binary_accumulation.png "Binary accumulation illustration based on Sklansky")

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter

    ```
    template <typename T, const CumSumConfig& config = defaultCumSumConfig>
    __aicore__ inline void CumSum(LocalTensor<T>& dstTensor, LocalTensor<T>& lastRowTensor, const LocalTensor<T>& srcTensor, LocalTensor<uint8_t>& sharedTmpBuffer, const CumSumInfo& cumSumInfo)
    ```

-   The API framework allocates temporary space

    ```
    template <typename T, const CumSumConfig& config = defaultCumSumConfig>
    __aicore__ inline void CumSum(LocalTensor<T>& dstTensor, LocalTensor<T>& lastRowTensor, const LocalTensor<T>& srcTensor, const CumSumInfo& cumSumInfo)
    ```

Because the internal implementation of this API involves precision conversion, additional temporary space is required to store intermediate variables during computation. Temporary space can be obtained in two ways: **allocated by the API framework** or **passed by you through the sharedTmpBuffer input parameter**.

-   The API framework allocates temporary space. You do not need to allocate it, but you need to reserve the size of the temporary space.

-   Pass temporary space through the sharedTmpBuffer input parameter. This tensor is used as the temporary space for processing, and the API framework no longer allocates it. In this way, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.

When the API framework allocates temporary space, you need to reserve the temporary space. When temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor. The size of the temporary space, BufferSize, is obtained as follows: use the API provided in [GetCumSumMaxMinTmpSize](GetCumSumMaxMinTmpSize.md) to obtain the size of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| config | Defines the config parameter of the CumSum API at compile time. It is of the CumSumConfig type, whose definition is shown in the following code. The parameters are described as follows:<br><br>isLastAxis: If the value is true, the computation is processed along the last axis; if the value is false, the computation is processed along the first axis.<br><br>isReuseSource: Whether the memory space of srcTensor can be reused. This parameter is reserved. Pass the default value false.<br><br>outputLastRow: Whether to output the last row of data.<br>algorithm: Cumulative sum algorithm used by the internal implementation of CumSum. <!-- npu="950" id8 -->This parameter is supported only on Ascend 950PR&950DT products.<!-- end id8 --> Supported values are as follows: CumSumAlgorithm::CUMSUM_ALGORITHM_LINEBYLINE: row-wise accumulation algorithm. CumSumAlgorithm::CUMSUM_ALGORITHM_SKLANSKY: Sklansky binary accumulation algorithm. |

```
struct CumSumConfig {
    bool isLastAxis{true};
    bool isReuseSource{false};
    bool outputLastRow{false};
    CumSumAlgorithm algorithm{CumSumAlgorithm::CUMSUM_ALGORITHM_LINEBYLINE};
};
enum class CumSumAlgorithm { CUMSUM_ALGORITHM_LINEBYLINE = 0, CUMSUM_ALGORITHM_SKLANSKY = 1 };
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand. Cumulative sum of the input elements processed along the first axis or the last axis.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| lastRowTensor | Output | Destination operand. When the outputLastRow parameter in the template parameter config is set to true, the last row of data is output.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex internal computation of CumSum. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetCumSumMaxMinTmpSize](GetCumSumMaxMinTmpSize.md). |
| cumSumInfo | Input | Shape information of srcTensor. It is of the CumSumInfo type, whose definition is shown in the following code. The parameters are described as follows:<br><br>outter: outer axis length of the input data.<br><br>inner: inner axis length of the input data.<br><br>Note the following:<br><br>Both cumSumInfo.outter and cumSumInfo.inner must be greater than 0.<br><br>cumSumInfo.outter * cumSumInfo.inner must not be greater than the size of dstTensor or srcTensor.<br><br>cumSumInfo.inner * sizeof(T) must be an integer multiple of 32 bytes.<br><br>When the outputLastRow parameter in the template parameter config is set to true, cumSumInfo.inner must not be greater than the size of the last row of data output by lastRowTensor. |

```
struct CumSumInfo {
    uint32_t outter{0};
    uint32_t inner{0};
};
```

## Return Value

None

## Constraints

-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   The input supports only a two-dimensional structure.
-   cumSumInfo.inner \* sizeof\(T\) must be an integer multiple of 32 bytes.

## Examples

For the complete call example, see [CumSum Examples](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/10_math/cumsum).

```
// dstLocal: Tensor that stores the computation result.
// lastRowLocal: Tensor that stores the last row of data of the computation result.
// srcLocal: Input tensor that participates in the computation.

// Process along the last axis (column-wise accumulation) to output the last column of data, using the row-wise accumulation algorithm.
constexpr AscendC::CumSumConfig cumSumConfig{true, false, true, AscendC::CumSumAlgorithm::CUMSUM_ALGORITHM_LINEBYLINE};
// outer: Outer axis length.
// inner: Inner axis length.
const AscendC::CumSumInfo cumSumInfo{outer, inner};
AscendC::CumSum<T, cumSumConfig>(dstLocal, lastRowLocal, srcLocal, cumSumInfo);

// Process along the first axis (row-wise accumulation) to output the last row of data, using the row-wise accumulation algorithm.
constexpr AscendC::CumSumConfig cumSumConfig{false, false, true, AscendC::CumSumAlgorithm::CUMSUM_ALGORITHM_LINEBYLINE};
AscendC::CumSum<T, cumSumConfig>(dstLocal, lastRowLocal, srcLocal, cumSumInfo);
```

When cumSumConfig is set to \{true, false, true, AscendC::CumSumAlgorithm::CUMSUM\_ALGORITHM\_LINEBYLINE\}, the sample data is as follows:

```
Input data (srcLocal): [1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1]
Output data (dstLocal): [1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8]
Output data (lastRowLocal): [8 8 8 8]
```

When cumSumConfig is set to \{false, false, true, AscendC::CumSumAlgorithm::CUMSUM\_ALGORITHM\_LINEBYLINE\}, the sample data is as follows:

```
Input data (srcLocal): [1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1]
Output data(dstLocal): [1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3 4 4 4 4 4 4 4 4]
Output data(lastRowLocal): [4 4 4 4 4 4 4 4]
```
