# Cos

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T02:37:14.136Z pushedAt=2026-10-29T02:15:02.632Z nl=2 -->

## Applicable Products

<!-- npu="950" id3 -->
- Ascend 950PR&950DT products: Supported
<!-- end id3 -->
<!-- npu="A3" id4 -->
- Atlas A3 products: Supported
<!-- end id4 -->
<!-- npu="910b" id5 -->
- Atlas A2 products: Supported
<!-- end id5 -->
<!-- npu="310b" id6 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Cos_interface/Cos_res.md#id1 -->

## Description

Performs the trigonometric cosine operation element-wise. The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002218333665.png)

The Taylor expansion of Cos\(x\) is:

![](../../../../figures/zh-cn_formulaimage_0000002376868752.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter
    -   All or part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false, const CosConfig& config = defaultCosConfig>
        __aicore__ inline void Cos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   All of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false, const CosConfig& config = defaultCosConfig>
        __aicore__ inline void Cos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space
    -   All or part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false, const CosConfig& config = defaultCosConfig>
        __aicore__ inline void Cos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   All of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false, const CosConfig& config = defaultCosConfig>
        __aicore__ inline void Cos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space can be provided in two ways: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   When temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. In this way, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.
-   The API framework allocates temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

When sharedTmpBuffer is passed in, you need to allocate space for the tensor; when the API framework allocates space, you need to reserve temporary space. The temporary space size BufferSize is obtained as follows: use the API provided in [GetCosMaxMinTmpSize](GetCosMaxMinTmpSize.md) to obtain the size of the space range that needs to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether to allow modification of the source operand. The default value is false. This parameter takes effect only when the input data type is float.<br>true: You allow the source operand to be overwritten. You can set this parameter to true to enable this. After it is enabled, this API reuses the memory space of srcTensor during internal computation, saving some memory space. false: This API does not reuse the memory space of srcTensor during internal computation.<br><br>For an example of using isReuseSource, see [More Examples](../more_examples.md#section639165323915). |
| config | <!-- npu="950" id13 -->This parameter is supported only on Ascend 950PR&950DT products.<br><br><!-- end id13 -->Configuration related to the Cos algorithm. This parameter is optional and of the CosConfig type. Its specific definition is shown in the following code, where the parameters have the following meanings:<br>algo: Algorithm used in the internal implementation of Cos. It is of the CosAlgo type and supports the following values: POLYNOMIAL_APPROXIMATION: Default value. This algorithm implements the Cos API through simple polynomial approximation. It supports an input value range of [-65504.0, 65504.0] and supports the half and float data types. RADIAN_REDUCTION: This algorithm implements the Cos API through complete period reduction. It supports the full input value range and supports the half and float data types. |

```
struct CosConfig {
    CosAlgo algo = CosAlgo::POLYNOMIAL_APPROXIMATION;
};
enum class CosAlgo {
    POLYNOMIAL_APPROXIMATION = 0,
    RADIAN_REDUCTION,
};
```

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetCosMaxMinTmpSize](GetCosMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

<!-- npu="950" id9 -->
-   For Ascend 950PR&950DT products, when the polynomial fitting algorithm POLYNOMIAL\_APPROXIMATION is used in the template parameter config, you must ensure that the value range of the input source data is within \[-65504.0, 65504.0\].
<!-- end id9 -->
-   For the following products, the input source data must be kept within the value range of \[-65504.0, 65504.0\].
        <!-- npu="A3" id10 -->
    -   Atlas A3 products
        <!-- end id10 -->
        <!-- npu="910b" id11 -->
    -   Atlas A2 products
        <!-- end id11 -->
        <!-- npu="310p" id12 -->
    -   Atlas inference products AI Core
        <!-- end id12 -->

-   **Overlapping of the source operand and destination operand addresses is not supported.**
-   Overlapping of sharedTmpBuffer with the source operand and destination operand addresses is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
// dstLocal: Tensor that stores the computation result.
// srcLocal: Input tensor involved in the computation.
// sharedTmpBuffer: Temporary buffer that stores intermediate variables during internal complex computation.
// The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
AscendC::Cos(dstLocal, srcLocal, sharedTmpBuffer, 512);
constexpr AscendC::CosAlgo algo = AscendC::CosAlgo::RADIAN_REDUCTION;
constexpr AscendC::CosConfig config = {algo};
AscendC::Cos<half, false, config>(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The result example is as follows:

```
Input data (srcLocal): [0.00            0.01            0.02             ...  5.10            5.11]
Output data (dstLocal): [1.00000000e+00  9.99949992e-01  9.99800026e-01   ...  3.77977639e-01  3.87216508e-01]
```
