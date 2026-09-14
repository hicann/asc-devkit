# Sinh

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T10:23:11.982Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs element-wise hyperbolic sine computation. The formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002182888644.png)

![](../../../../figures/zh-cn_formulaimage_0000001678718948.png)

## Prototype

-   Pass temporary space via the **sharedTmpBuffer** input parameter.
    -   All or part of the source operand tensor participates in computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sinh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sinh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework applies for temporary space.
    -   All or part of the source operand tensor participates in computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sinh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sinh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during computation. Temporary space can be obtained in two ways: **applied for by the API framework** or **passed by you via the sharedTmpBuffer input parameter**.

-   The API framework applies for temporary space. You do not need to apply for it, but you need to reserve the size of the temporary space.

-   Pass temporary space via the sharedTmpBuffer input parameter. This tensor is used as the temporary space for processing, and the API framework no longer applies for it. With this method, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, offering high flexibility and high memory utilization.

When the API framework applies for the space, you need to reserve temporary space; when the space is passed through **sharedTmpBuffer**, you need to apply for space for **sharedTmpBuffer**. The temporary space size **BufferSize** is obtained as follows: use the **GetSinhMaxMinTmpSize** API provided in [GetSinhMaxMinTmpSize](GetSinhMaxMinTmpSize.md) to obtain the size range of the space to be reserved.

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand can be modified. This parameter is reserved. Pass the default value **false**. |

**Table 2** Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetSinhMaxMinTmpSize](GetSinhMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between **sharedTmpBuffer** and the source operand or destination operand addresses is not supported.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
// dstLocal: tensor that stores the Sinh computation result.
// srcLocal: tensor that stores the Sinh computation input.
// sharedTmpBuffer: tensor that stores the temporary buffer during Sinh computation.

// The API framework applies for temporary space, and all elements participate in the computation.
AscendC::Sinh(dstLocal, srcLocal);
// The API framework applies for temporary space, and some elements participate in the computation. The number of elements that need to participate in the computation is 512.
AscendC::Sinh(dstLocal, srcLocal, 512);

// Pass temporary space through the sharedTmpBuffer input parameter, and all elements participate in the computation.
AscendC::Sinh(dstLocal, srcLocal, sharedTmpBuffer);
// Pass temporary space through the sharedTmpBuffer input parameter, and some elements participate in the computation. The number of elements that need to participate in the computation is 512.
AscendC::Sinh(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The result is as follows:

```
Input data (srcLocal): [-2.56  -2.55  -2.54  ...  2.53  2.54  2.55]
Output data(dstLocal): [-6.434  -6.37  -6.293  ...  6.234  6.293  6.37]
```
