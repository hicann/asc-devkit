# SwiGLU

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:03:51.999Z pushedAt=2026-10-29T02:14:34.128Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/SwiGLU_interface/SwiGLU_res.md#id1 -->

## Description

SwiGLU is a GLU variant that uses Swish as the activation function. The specific formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002350049237.png)

The formula of the Swish activation function is as follows (β is a constant):

![](../../../../figures/zh-cn_formulaimage_0000001704667878.png)

## Prototype

-   Pass the temporary space through the sharedTmpBuffer input parameter
    -   All or part of the source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void SwiGLU(LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const float& scalarValue, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void SwiGLU(LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const float& scalarValue, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates the temporary space
    -   All or part of the source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void SwiGLU(LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const float& scalarValue, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void SwiGLU(LocalTensor<T>& dstTensor, LocalTensor<T>& srcTensor0, LocalTensor<T>& srcTensor1, const float& scalarValue)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. The temporary space supports two methods: **passing through the sharedTmpBuffer input parameter** and **allocation by the API framework**.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

When sharedTmpBuffer is passed in, you need to allocate space for the tensor; when the API framework allocates it, you need to reserve temporary space. The size of the temporary space, BufferSize, is obtained as follows: obtain the size of the space range to be reserved through the API provided in [GetSwiGLUMaxMinTmpSize](GetSwiGLUMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor0/srcTensor1 | Input | Source operand.<br><br>The data type of the source operand must be the same as that of the destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| scalarValue | Input | β parameter in the activation function. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside SwiGLU. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetSwiGLUMaxMinTmpSize](GetSwiGLUMaxMinTmpSize.md). |
| calCount | Input | Number of data elements actually computed. |

## Return Value

None

## Constraints

-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   **Address overlap between the source operands and the destination operand is not supported.**
-   Currently, only the ND format is supported for input; other formats are not supported.
-   sharedTmpBuffer must not overlap with the source operand or destination operand addresses.

<!-- npu="950" id9 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal in the API's internal computation is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation and participate in subsequent computations according to their actual values.
    -   When configured to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal numbers generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with retaining Subnormal numbers.
<!-- end id9 -->

## Examples

```
// dstLocal: Tensor that stores the SwiGLU computation result.
// src0Local: Tensor that stores the input for SwiGLU computation.
// src1Local: Stores the input tensor for SwiGLU computation.
// scalarValue: The β parameter in the activation function.
// sharedTmpBuffer: Stores the tensor used as a temporary buffer during the SwiGLU computation process.

float scalarValue = 1.0;

// The API framework allocates temporary space, and all elements participate in the computation.
AscendC::SwiGLU(dstLocal, src0Local, src1Local, scalarValue);
// The API framework allocates temporary space, and only some elements participate in the computation. The number of elements to be computed is 32.
AscendC::SwiGLU(dstLocal, src0Local, src1Local, scalarValue, 32);

// Temporary space is passed in through the sharedTmpBuffer input parameter, and all elements participate in the computation.
AscendC::SwiGLU(dstLocal, src0Local, src1Local, scalarValue, sharedTmpBuffer);
// Temporary space is passed in through the sharedTmpBuffer input parameter, and only some elements participate in the computation. The number of elements to be computed is 32.
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
