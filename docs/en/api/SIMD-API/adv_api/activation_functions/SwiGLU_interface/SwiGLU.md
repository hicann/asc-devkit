# SwiGLU

<!-- md-trans-meta sourceCommit=971bc6393be4cb3e0f8244913e605c5710bb2cc9 translatedAt=2026-11-10T06:20:52.969Z pushedAt=2026-11-18T11:07:26.477Z nl=2 -->

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
- Vector Core of Atlas inference products: Not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: Not supported
<!-- end id8 -->
<!-- npu="x90" id1 -->
- KirinX90 processor products: Supported
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin9030 processor products: Supported
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/SwiGLU_interface/SwiGLU_res.md#id1 -->

## Description

SwiGLU is a GLU variant that uses Swish as the activation function. The specific formula is as follows:

![](../../../../figures/formulaimage_0000002350049237.png)

The formula of the Swish activation function is as follows (β is a constant):

![](../../../../figures/formulaimage_0000001704667878.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.
    -   All or part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void SwiGLU(LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const float& scalarValue, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void SwiGLU(LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const float& scalarValue, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space.
    -   All or part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void SwiGLU(LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const float& scalarValue, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void SwiGLU(LocalTensor<T>& dstTensor, LocalTensor<T>& srcTensor0, LocalTensor<T>& srcTensor1, const float& scalarValue)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   Pass temporary space through the sharedTmpBuffer input parameter, and use this tensor as the temporary space for processing; the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space yourself and reuse this portion of memory after the API call is complete. Memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.
-   The API framework allocates temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

When sharedTmpBuffer is , you need to allocate space for the tensor; when the API framework allocates it, you need to reserve temporary space. The size of the temporary space, BufferSize, is obtained as follows: obtain the size of the space range to be reserved through the API provided in [GetSwiGLUMaxMinTmpSize](GetSwiGLUMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. Supported data types: half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor0/srcTensor1 | Input | Source operand.<br><br>The data type of the source operand must be the same as that of the destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| scalarValue | Input | β parameter in the activation function. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside SwiGLU. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetSwiGLUMaxMinTmpSize](GetSwiGLUMaxMinTmpSize.md). |
| calCount | Input | Number of data elements actually involved in the computation. |

## Return Value

None

## Constraints

-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Currently, only ND-format input is supported. Other formats are not supported.
-   Overlapping between sharedTmpBuffer and the source operand or destination operand addresses is not supported.

<!-- npu="950" id9 -->
-   For the Ascend 950PR&950DT products, the handling of Subnormal values in the internal computation of the API is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation process and participate in subsequent computation according to their actual values.
    -   When set to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in the computation process are treated as 0, which may cause precision differences compared with preserving Subnormal values.
<!-- end id9 -->

## Examples

```
// dstLocal: Tensor that stores the SwiGLU computation result.
// src0Local: Tensor that stores the input for SwiGLU computation.
// src1Local: Tensor that stores the input for SwiGLU computation.
// scalarValue: β parameter in the activation function.
// sharedTmpBuffer: Tensor that stores the temporary buffer during SwiGLU computation.

float scalarValue = 1.0;

// The API framework allocates temporary space, and all elements participate in computation.
AscendC::SwiGLU(dstLocal, src0Local, src1Local, scalarValue);
// The API framework allocates temporary space, and some elements participate in computation. The number of elements to be computed is 32.
AscendC::SwiGLU(dstLocal, src0Local, src1Local, scalarValue, 32);

// Temporary space is passed through the sharedTmpBuffer input parameter, and all elements participate in computation.
AscendC::SwiGLU(dstLocal, src0Local, src1Local, scalarValue, sharedTmpBuffer);
// Temporary space is passed through the sharedTmpBuffer input parameter, and some elements participate in computation. The number of elements to be computed is 32.
AscendC::SwiGLU(dstLocal, src0Local, src1Local, scalarValue, sharedTmpBuffer, 32);
```

The result is as follows:

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
