# GeGLU

<!-- md-trans-meta sourceCommit=971bc6393be4cb3e0f8244913e605c5710bb2cc9 translatedAt=2026-11-10T06:13:44.413Z pushedAt=2026-11-18T11:07:26.342Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/GeGLU_interface/GeGLU_res.md#id1 -->

## Description

GeGLU is a GLU variant that uses GELU as the activation function. The specific formula is as follows:

![](../../../../figures/formulaimage_0000002316170544.png)

The GELU activation function is computated as follows:

![](../../../../figures/formulaimage_0000001706956388.png)

In the formula above, erf is the error function: ![](../../../../figures/formulaimage_0000001707121636.png)

The error function has no analytical expression. The tanh approximation commonly used in the industry is adopted: ![](../../../../figures/formulaimage_0000001755028613.png)

Substituting the GELU approximation formula yields the GeGLU expression:

![](../../../../figures/formulaimage_0000001707157706.png)

where a = -0.0713548162726, b = 2.2363860002236e1, and x1 and x0 represent the elements in srcTensor1 and srcTensor0.

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.
    -   All or part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void GeGLU(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void GeGLU(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space.
    -   All or part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void GeGLU(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void GeGLU(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   Pass temporary space through the sharedTmpBuffer input parameter, and use this tensor as the temporary space for processing; the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space yourself and reuse this portion of memory after the API call is complete. Memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.
-   The API framework allocates temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

When sharedTmpBuffer is , you need to allocate space for the tensor; when the API framework allocates it, you need to reserve temporary space. The temporary space size BufferSize is obtained as follows: use the API provided in [GetGeGLUMaxMinTmpSize](GetGeGLUMaxMinTmpSize.md) to obtain the size of the space range to be reserved.

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
| srcTensor0/<br><br>srcTensor1 | Input | Source operand.<br><br>The data type of the source operand must be the same as that of the destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside GeGLU. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetGeGLUMaxMinTmpSize](GetGeGLUMaxMinTmpSize.md). |
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
// dstLocal: Tensor that stores the GeGLU computation result.
// src0Local: Tensor that stores the GeGLU input.
// src1Local: Tensor that stores the GeGLU input.
// sharedTmpBuffer: Tensor used to temporarily buffer data during GeGLU computation.

// The API framework applies for temporary space, all of which participates in computation.
AscendC::GeGLU<srcType, false>(dstLocal, src0Local, src1Local);
// The API framework applies for temporary space, part of which participates in computation. The number of elements that need to participate in computation is 512.
AscendC::GeGLU<srcType, false>(dstLocal, src0Local, src1Local, 512);

// Temporary space is passed through the sharedTmpBuffer input parameter, all of which participates in computation.
AscendC::GeGLU<srcType, false>(dstLocal, src0Local, src1Local, sharedTmpBuffer);
// Temporary space is passed through the sharedTmpBuffer input parameter, part of which participates in computation. The number of elements that need to participate in computation is 512.
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
