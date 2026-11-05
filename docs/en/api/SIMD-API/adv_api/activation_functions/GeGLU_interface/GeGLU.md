# GeGLU

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:25:14.450Z pushedAt=2026-10-29T02:14:34.006Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/GeGLU_interface/GeGLU_res.md#id1 -->

## Description

GeGLU is a GLU variant that uses GELU as the activation function. The specific formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002316170544.png)

The GELU activation function is calculated as follows:

![](../../../../figures/zh-cn_formulaimage_0000001706956388.png)

In the formula above, erf is the error function: ![](../../../../figures/zh-cn_formulaimage_0000001707121636.png)

The error function has no analytical expression. The tanh approximation commonly used in the industry is adopted: ![](../../../../figures/zh-cn_formulaimage_0000001755028613.png)

Substituting the GELU approximation formula yields the GeGLU expression:

![](../../../../figures/zh-cn_formulaimage_0000001707157706.png)

where a = -0.0713548162726, b = 2.2363860002236e1, and x1 and x0 represent the elements in srcTensor1 and srcTensor0.

## Prototype

-   Pass the temporary space through the sharedTmpBuffer input parameter
    -   All or part of the source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void GeGLU(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void GeGLU(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates the temporary space
    -   All or part of the source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void GeGLU(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void GeGLU(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. The temporary space supports two methods: **input parameter passing via sharedTmpBuffer** and **allocation by the API framework**.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

When sharedTmpBuffer is passed in, you need to allocate space for the tensor; when the API framework allocates it, you need to reserve temporary space. The temporary space size BufferSize is obtained as follows: use the API provided in [GetGeGLUMaxMinTmpSize](GetGeGLUMaxMinTmpSize.md) to obtain the size of the space range to be reserved.

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
| srcTensor0/<br><br>srcTensor1 | Input | Source operand.<br><br>The data type of the source operand must be the same as that of the destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside GeGLU. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetGeGLUMaxMinTmpSize](GetGeGLUMaxMinTmpSize.md). |
| calCount | Input | Number of data elements actually involved in the computation. |

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
// dstLocal: Tensor that stores the GeGLU computation result.
// src0Local: Tensor that stores the input of GeGLU.
// src1Local: Tensor that stores the input of GeGLU.
// sharedTmpBuffer: Tensor that stores the temporary buffer during GeGLU computation.

// The API framework allocates temporary space, and all elements are involved in the computation.
AscendC::GeGLU<srcType, false>(dstLocal, src0Local, src1Local);
// The API framework allocates temporary space, and some elements are involved in the computation. The number of elements to be computed is 512.
AscendC::GeGLU<srcType, false>(dstLocal, src0Local, src1Local, 512);

// Temporary space is passed in through the sharedTmpBuffer input parameter, and all elements are involved in the computation.
AscendC::GeGLU<srcType, false>(dstLocal, src0Local, src1Local, sharedTmpBuffer);
// Pass temporary space through the sharedTmpBuffer input parameter. Part of the data participates in computation, with 512 elements to be computed.
AscendC::GeGLU<srcType, false>(dstLocal, src0Local, src1Local, sharedTmpBuffer, 512);
```

The result is as follows:

```
Input data (src0Local):
[-2.56 -2.55 -2.54 ... 2.53  2.54  2.55]
Input data (src1Local):
[2. 2. 2. ... 2. 2. 2.]
Output data (dstLocal):
[-5.004   -4.984   -4.96 ... 4.94     4.96     4.984]
```
