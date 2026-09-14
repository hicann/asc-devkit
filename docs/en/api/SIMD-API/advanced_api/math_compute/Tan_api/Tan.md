# Tan

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-10T10:32:23.303Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training series products/Atlas A3 inference series products: Supported
- Atlas A2 training series products/Atlas A2 inference series products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference series products AI Core: Supported
- Atlas inference series products Vector Core: Not supported
- Atlas training series products: Not supported

## Description

Computes the tangent function element-wise. The formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002182888708.png)

The Taylor expansion of Tan\(x\) is:

![](../../../../figures/zh-cn_formulaimage_0000002228789689.png)

where B<sub>2n</sub> is the Bernoulli number.

## Function Prototype

-   Pass the temporary space through the sharedTmpBuffer input parameter
    -   All or part of the source operand Tensor is involved in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Tan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   All of the source operand Tensor is involved in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Tan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework applies for the temporary space
    -   All or part of the source operand Tensor is involved in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Tan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   All elements of the source operand Tensor participate in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Tan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: through the API framework allocation approach, or by you passing it via the `sharedTmpBuffer` input parameter.

-   When the API framework applies for the temporary space, the developer does not need to apply for it, but needs to reserve the size of the temporary space.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer applies for it. In this method, the developer can manage the sharedTmpBuffer memory space independently and reuse this part of memory after the API call is completed. The memory is not repeatedly applied for and released, which provides higher flexibility and higher memory utilization.

When the API framework applies for the space, you need to reserve temporary space; when the space is passed through **sharedTmpBuffer**, you need to apply for space for **sharedTmpBuffer**. The temporary space size **BufferSize** is obtained as follows: call the GetTanMaxMinTmpSize API provided in [GetTanMaxMinTmpSize](GetTanMaxMinTmpSize.md) to obtain the size of the space to be reserved.

## Parameters

**Table 1** Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand can be modified. This parameter is reserved. Pass the default value **false**. |

**Table 2** API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be the same as that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetTanMaxMinTmpSize](GetTanMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   The input data is limited to the range of \(-65504.0, 65504.0\).
-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between **sharedTmpBuffer** and the source operand and destination operand addresses is not supported.

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
// dstLocal: Tensor that stores the Tan computation result.
// srcLocal: Tensor that stores the Tan computation input.
// sharedTmpBuffer: Tensor that stores the temporary buffer during Tan computation.

// The API framework applies for temporary space, all of which is involved in the computation.
AscendC::Tan(dstLocal, srcLocal);
// The API framework applies for temporary space, part of which participates in computation. The number of elements that need to participate in computation is 512.
AscendC::Tan(dstLocal, srcLocal, 512);

// Pass temporary space through the sharedTmpBuffer input parameter, all of which participates in computation.
AscendC::Tan(dstLocal, srcLocal, sharedTmpBuffer);
// Pass temporary space through the sharedTmpBuffer input parameter, part of which participates in computation. The number of elements that need to participate in computation is 512.
AscendC::Tan(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The result is as follows:

```
Input data (srcLocal):
[-2.56 -2.55 -2.54 ... 0. ... 2.53  2.54  2.55]
Output data (dstLocal):
[0.6567  0.671  0.688 ... -0.702  -0.688  -0.671 ]
```
