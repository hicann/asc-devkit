# Sin

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-09T11:18:42.657Z pushedAt=2026-11-18T08:54:29.657Z nl=2 -->

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
- AI Core of Atlas inference products: Supported
<!-- end id7 -->
<!-- npu="310p" id8 -->
- Vector Core of Atlas inference products: Not supported
<!-- end id8 -->
<!-- npu="910" id9 -->
- Atlas training products: Not supported
<!-- end id9 -->
<!-- npu="x90" id1 -->
- KirinX90 processor products: Supported
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin9030 processor products: Supported
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Sin_interface/Sin_res.md#id1 -->

## Description

Computes the sine function element-wise. The computation formula is as follows:

![](../../../../figures/formulaimage_0000002192017270.png)

The Taylor expansion of Sin\(x\) is:

![](../../../../figures/formulaimage_0000002192023094.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.
    -   All or part of the source operand Tensor participates in the computation

        ```
        template<typename T, bool isReuseSource = false, const SinConfig& config = defaultSinConfig>
        __aicore__ inline void Sin(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation

        ```
        template<typename T, bool isReuseSource = false, const SinConfig& config = defaultSinConfig>
        __aicore__ inline void Sin(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space.
    -   All or part of the source operand Tensor participates in the computation

        ```
        template<typename T, bool isReuseSource = false, const SinConfig& config = defaultSinConfig>
        __aicore__ inline void Sin(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation

        ```
        template<typename T, bool isReuseSource = false, const SinConfig& config = defaultSinConfig>
        __aicore__ inline void Sin(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   The API framework allocates temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

-   Pass temporary space through the sharedTmpBuffer input parameter, and use this tensor as the temporary space for processing; the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space yourself and reuse this portion of memory after the API call is complete. Memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.

When the API framework allocates the space, you need to reserve the temporary space. When the space is passed through sharedTmpBuffer, you need to allocate space for the tensor. To obtain the temporary space size BufferSize, call the [GetSinMaxMinTmpSize](GetSinMaxMinTmpSize.md) API to obtain the range size of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. Supported data types: half and float. |
| isReuseSource | Whether the source operand can be modified. The default value is false. This parameter takes effect only when the input data type is float.<br>true: You allow the source operand to be overwritten. You can set this parameter to true to enable it. After it is enabled, the API reuses the memory space of srcTensor during internal computation, saving some memory space. false: The API does not reuse the memory space of srcTensor during internal computation. |
| config | <!-- npu="950" id10 -->This parameter is supported only on Ascend 950PR&950DT products.<br><br><!-- end id10 -->Related configuration of the Sin algorithm. This parameter is optional and of the SinConfig type. Its specific definition is shown in the following code, where the parameters mean:<br>algo: Algorithm used by the internal implementation of Sin. It is of the SinAlgo type. Supported values are as follows: POLYNOMIAL_APPROXIMATION: default value. This algorithm implements the Sin API through simple polynomial approximation. The supported input value range is [-65504.0, 65504.0], and the supported data types are half and float. RADIAN_REDUCTION: This algorithm implements the Sin API through complete period reduction. It supports the full input value range, and the supported data types are half and float. |

```
struct SinConfig {
    SinAlgo algo = SinAlgo::POLYNOMIAL_APPROXIMATION;
};
enum class SinAlgo { POLYNOMIAL_APPROXIMATION = 0, RADIAN_REDUCTION };
```

**Table 2**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex internal computation of Sin. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetSinMaxMinTmpSize](GetSinMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

<!-- npu="950" id11 -->
-   For Ascend 950PR&950DT products, when the polynomial approximation algorithm POLYNOMIAL\_APPROXIMATION is used in the template parameter config, ensure that the value range of the input source data is within \[-65504.0, 65504.0\].
<!-- end id11 -->
-   For the following products, the input source data must keep its value range within \[-65504.0, 65504.0\].
        <!-- npu="A3" id12 -->
    -   Atlas A3 products
        <!-- end id12 -->
        <!-- npu="910b" id13 -->
    -   Atlas A2 products
        <!-- end id13 -->
        <!-- npu="310p" id14 -->
    -   AI Core of Atlas inference products
        <!-- end id14 -->

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between sharedTmpBuffer and the source operand or destination operand addresses is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
// dstLocal: Tensor that stores the Sin computation results.
// srcLocal: Tensor that stores the Sin computation inputs.
// sharedTmpBuffer: Tensor that stores the temporary buffer during Sin computation.

// The API framework allocates temporary space, all of which participates in the computation.
AscendC::Sin(dstLocal, srcLocal);
// The API framework allocates temporary space, part of which participates in the computation. The number of elements involved in the computation is 512.
AscendC::Sin(dstLocal, srcLocal, 512);

// Pass the temporary space through the sharedTmpBuffer input parameter, with all elements participating in the computation.
AscendC::Sin(dstLocal, srcLocal, sharedTmpBuffer);
// Pass the temporary space through the sharedTmpBuffer input parameter, with only some elements participating in the computation. The number of elements involved in the computation is 512.
AscendC::Sin(dstLocal, srcLocal, sharedTmpBuffer, 512);
constexpr AscendC::SinAlgo algo = AscendC::SinAlgo::RADIAN_REDUCTION;
constexpr AscendC::SinConfig config = {algo};
AscendC::Sin<half, false, config>(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The result is as follows:

```
Input data (srcLocal):
[-2.56 -2.55 -2.54 ... 0. ... 2.53  2.54  2.55]
Output data (dstLocal):
[-0.54889839 -0.55703507 -0.56672889 ... 0. 0.57474768 0.56672889 0.55703507]
```
