# SwiGLU

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T13:59:30.821Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

**SwiGLU** is a GLU variant that uses Swish as the activation function. The specific calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002350049237.png)

The calculation formula of the Swish activation function is as follows (β is a constant):

![](../../../../figures/zh-cn_formulaimage_0000001704667878.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.
    -   The source operand is involved in the computation either in whole or in part.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void SwiGLU(LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const float& scalarValue, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand is involved in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void SwiGLU(LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const float& scalarValue, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework applies for temporary space.
    -   The source operand is involved in the computation either in whole or in part.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void SwiGLU(LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const float& scalarValue, const uint32_t calCount)
        ```

    -   The entire source operand is involved in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void SwiGLU(LocalTensor<T>& dstTensor, LocalTensor<T>& srcTensor0, LocalTensor<T>& srcTensor1, const float& scalarValue)
        ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by passing it via the sharedTmpBuffer input parameter**.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer applies for it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly applied for and released, offering high flexibility and high memory utilization.
-   The API framework applies for temporary space. You do not need to apply for it, but must reserve the size of the temporary space.

When **sharedTmpBuffer** is passed, you need to apply for space for the tensor; when the API framework applies for temporary space, you need to reserve the temporary space. The temporary space size **BufferSize** is obtained as follows: use the API provided in [GetSwiGLUMaxMinTmpSize](GetSwiGLUMaxMinTmpSize.md) to obtain the size of the space range that needs to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value **false**. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor0/srcTensor1 | Input | Source operand.<br><br>The data type of the source operand must be consistent with that of the destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| scalarValue | Input | The β parameter in the activation function. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex internal computation of SwiGLU, provided by the developer.<br><br>For how to obtain the temporary space size **BufferSize**, see [GetSwiGLUMaxMinTmpSize](GetSwiGLUMaxMinTmpSize.md). |
| calCount | Input | Number of data elements actually computed. |

## Return Value

None

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   **Address overlap between the source operand and the destination operand is not supported.**
-   Currently, only ND-format input is supported, and other formats are not supported.
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.

## Examples

```
// dstLocal: Tensor that stores the SwiGLU computation result
// src0Local: Tensor that stores the input of the SwiGLU computation
// src1Local: Tensor that stores the input of the SwiGLU computation
// scalarValue: β parameter in the activation function.
// sharedTmpBuffer: Tensor that stores temporary buffer during SwiGLU computation.

float scalarValue = 1.0;

// The API framework applies for temporary space, and all elements participate in the computation.
AscendC::SwiGLU(dstLocal, src0Local, src1Local, scalarValue);
// The API framework applies for temporary space, and some elements participate in the computation. The number of elements that need to participate in the computation is 32.
AscendC::SwiGLU(dstLocal, src0Local, src1Local, scalarValue, 32);

// Temporary space is passed in through the sharedTmpBuffer input parameter, and all elements participate in the computation.
AscendC::SwiGLU(dstLocal, src0Local, src1Local, scalarValue, sharedTmpBuffer);
// Temporary space is passed in through the sharedTmpBuffer input parameter, and some elements participate in the computation. The number of elements that need to participate in the computation is 32.
AscendC::SwiGLU(dstLocal, src0Local, src1Local, scalarValue, sharedTmpBuffer, 32);
```

The results are as follows:

```
Input data (srcTensor0):
[-4.         -3.7419355  -3.483871   -3.2258065  -2.967742   -2.7096775  -2.451613   -2.1935484
 -1.9354838  -1.6774193  -1.4193548  -1.1612903  -0.9032258  -0.6451613  -0.38709676 -0.12903225
  0.12903225  0.38709676  0.6451613   0.9032258   1.1612903   1.4193548   1.6774193   1.9354838
  2.1935484   2.451613    2.7096775   2.967742    3.2258065   3.483871    3.7419355   4.        ]
Input data (srcTensor1):
[0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5
 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5]
Output data (dstLocal):
[-1.2449187  -1.1646013  -1.084284   -1.0039667  -0.9236493  -0.843332   -0.7630147  -0.68269736
 -0.60238    -0.52206266 -0.4417453  -0.361428   -0.28111064 -0.20079333 -0.12047599 -0.04015867
  0.04015867  0.12047599  0.20079333  0.28111064  0.361428    0.4417453   0.52206266  0.60238
  0.68269736  0.7630147   0.843332    0.9236493   1.0039667   1.084284    1.1646013   1.2449187 ]
```
