# Sin

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T10:01:08.013Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference product: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Computes the sine function element-wise. The formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002192017270.png)

The Taylor expansion of Sin\(x\) is:

![](../../../../figures/zh-cn_formulaimage_0000002192023094.png)

## Prototype

-   Pass temporary space through the **sharedTmpBuffer** input parameter.
    -   All or part of the source operand tensor participates in the computation.

        ```
        template<typename T, bool isReuseSource = false, const SinConfig& config = defaultSinConfig>
        __aicore__ inline void Sin(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation.

        ```
        template<typename T, bool isReuseSource = false, const SinConfig& config = defaultSinConfig>
        __aicore__ inline void Sin(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework applies for temporary space.
    -   All or part of the source operand tensor participates in the computation.

        ```
        template<typename T, bool isReuseSource = false, const SinConfig& config = defaultSinConfig>
        __aicore__ inline void Sin(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation.

        ```
        template<typename T, bool isReuseSource = false, const SinConfig& config = defaultSinConfig>
        __aicore__ inline void Sin(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space can be provided in two ways: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   When the API framework applies for temporary space, you do not need to apply for it, but you need to reserve the size of the temporary space.

-   When temporary space is passed through the **sharedTmpBuffer** input parameter, this tensor is used as the temporary space for processing, and the API framework no longer applies for it. In this way, you can manage the **sharedTmpBuffer** memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, which provides higher flexibility and higher memory utilization.

When the API framework applies for the space, you need to reserve temporary space; when the space is passed through **sharedTmpBuffer**, you need to apply for space for the tensor. To obtain the temporary space size **BufferSize**, call [GetSinMaxMinTmpSize](GetSinMaxMinTmpSize.md) to obtain the size range of the space to be reserved.

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand can be modified. The default value is **false**. This parameter takes effect only when the input data type is float.<br>**true**: You allow the source operand to be overwritten. You can set this parameter to **true** to enable this behavior. After it is enabled, the memory space of **srcTensor** is reused during internal computation of this API, saving some memory space. **false**: The memory space of **srcTensor** is not reused during internal computation of this API. |
| config | This parameter is supported only on Ascend 950PR/Ascend 950DT.<br><br>Configuration related to the Sin algorithm. This parameter is optional and of the **SinConfig** type. Its definition is shown in the following code, where the parameters have the following meanings:<br>**algo**: Algorithm used in the internal implementation of Sin. It is of the **SinAlgo** type and supports the following values: **POLYNOMIAL_APPROXIMATION**: default value. This algorithm implements the Sin API through simple polynomial approximation. It supports an input value range of [-65504.0, 65504.0] and data types half and float. **RADIAN_REDUCTION**: This algorithm implements the Sin API through complete period reduction. It supports the full input value range and data types half and float. |

```
struct SinConfig {
  SinAlgo algo = SinAlgo::POLYNOMIAL_APPROXIMATION;
};
enum class SinAlgo {
  POLYNOMIAL_APPROXIMATION = 0,
  RADIAN_REDUCTION
};
```

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be the same as that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex internal computation of Sin. It is provided by you.<br><br>For how to obtain the temporary space size **BufferSize**, see [GetSinMaxMinTmpSize](GetSinMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value Description

None

## Constraints

-   For Ascend 950PR/Ascend 950DT, when the polynomial fitting algorithm POLYNOMIAL\_APPROXIMATION is used in the template parameter **config**, the value range of the input source data must be within \[-65504.0, 65504.0\].
-   For the following products, the input source data must be kept within the value range of \[-65504.0, 65504.0\].
    -   Atlas A3 training products/Atlas A3 inference products
    -   Atlas A2 training products/Atlas A2 inference products
    -   Atlas inference products AI Core

-   **Overlapping between the source operand and the destination operand is not supported.**
-   Overlapping between **sharedTmpBuffer** and the source operand or the destination operand is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
// dstLocal: Tensor that stores the Sin computation result.
// srcLocal: Tensor that stores the Sin computation input.
// sharedTmpBuffer: Tensor that stores the temporary buffer during the Sin computation.

// The API framework applies for temporary space, and all elements participate in the computation.
AscendC::Sin(dstLocal, srcLocal);
// The API framework applies for temporary space, and some elements participate in the computation. The number of elements that need to participate in the computation is 512.
AscendC::Sin(dstLocal, srcLocal, 512);

// Pass temporary space through the sharedTmpBuffer input parameter, and all elements participate in the computation.
AscendC::Sin(dstLocal, srcLocal, sharedTmpBuffer);
// Pass temporary space through the sharedTmpBuffer input parameter, and some elements participate in the computation. The number of elements that need to participate in the computation is 512.
AscendC::Sin(dstLocal, srcLocal, sharedTmpBuffer, 512);
constexpr AscendC::SinAlgo algo = AscendC::SinAlgo::RADIAN_REDUCTION;
constexpr AscendC::SinConfig config = { algo };
AscendC::Sin<half, false, config>(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The result is as follows:

```
Input data (srcLocal):
[-2.56 -2.55 -2.54 ... 0. ... 2.53  2.54  2.55]
Output data (dstLocal):
[-0.54889839 -0.55703507 -0.56672889 ... 0. 0.57474768 0.56672889 0.55703507]
```
