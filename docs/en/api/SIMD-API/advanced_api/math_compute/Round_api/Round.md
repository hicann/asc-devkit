# Round

<!-- md-trans-meta sourceCommit=91c03327210113585f52739a6c85c35ece492970 translatedAt=2026-09-10T09:33:36.889Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Rounds each input element to the nearest integer. The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002182893080.png)

Examples are as follows:

Round\(3.5\) = 4.0

Round\(-3.4\) = -3.0

## Prototype

-   Pass temporary space through the **sharedTmpBuffer** input parameter.

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void Round(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    ```

-   The API framework applies for temporary space.

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void Round(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during computation. Temporary space supports two methods: **passing through the sharedTmpBuffer input parameter** and **applying through the API framework**.

-   Pass through the **sharedTmpBuffer** input parameter: this tensor is used as temporary space for processing, and the API framework no longer applies for space. With this method, you can manage the **sharedTmpBuffer** memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, providing higher flexibility and higher memory utilization.
-   The API framework applies for temporary space: you do not need to apply for it, but you need to reserve the size of the temporary space.

When passing through **sharedTmpBuffer**, you need to apply for space for the tensor; when the API framework applies for space, you need to reserve temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: obtain the size of the space range to be reserved through the API provided in [GetRoundMaxMinTmpSize](GetRoundMaxMinTmpSize.md).

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are: half and float. |
| isReuseSource | Whether the source operand can be modified. This parameter is reserved. Pass the default value **false**. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex internal computation of **Round**, provided by you.<br><br>For how to obtain the temporary space size **BufferSize**, see [GetRoundMaxMinTmpSize](GetRoundMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   Only rounding to an integer is supported, and rounding to a decimal is not supported. If the number is exactly halfway between two integers, that is, the fractional part is 0.5, it is rounded to the nearest even integer.
-   Overlap between **sharedTmpBuffer** and the source or destination operand addresses is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
// dstLocal: Tensor that stores the computation result.
// srcLocal: Input tensor that participates in the computation.
// sharedTmpBuffer: Temporary buffer that stores intermediate variables during complex internal computation.
// The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
AscendC::Round(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The result is as follows:

```
Input data (srcLocal): [0.80541134 0.8385705 0.49426016 ...  1.30962205 0.28947052]
Output data (dstLocal): [1.0 1.0 0.0  ...  1.0 0.0]
Input data (srcLocal):
[ 6.6   -4.3   -8.6   ...   8.8   -8.3   6.5 ]
Output data (dstLocal):
[ 7.0   -4.0   -9.0   ...   9.0   -8.0   6.0 ]
```
