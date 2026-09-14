# GeGLU

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T11:07:18.421Z pushedAt=2026-09-09T07:16:16.880Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

GeGLU is a GLU variant that uses GELU as its activation function. The specific computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002316170544.png)

The computation formula of the GELU activation function is as follows:

![](../../../../figures/zh-cn_formulaimage_0000001706956388.png)

In the above formula, erf is the error function: ![](../../../../figures/zh-cn_formulaimage_0000001707121636.png)

The error function has no analytical expression, and the tanh approximation commonly used in the industry is adopted: ![](../../../../figures/zh-cn_formulaimage_0000001755028613.png)

Substituting the GELU approximation formula yields the following GeGLU expression:

![](../../../../figures/zh-cn_formulaimage_0000001707157706.png)

where a = -0.0713548162726, b = 2.2363860002236e1, and x1 and x0 represent the elements in **srcTensor1** and **srcTensor0**, respectively.

## Prototype

-   Temporary space is passed through the **sharedTmpBuffer** input parameter.
    -   The source operand tensor participates in computation either in whole or in part.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void GeGLU(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const LocalTensor<uint8_t>& sharedTmpBuffer, uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void GeGLU(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework applies for temporary space.
    -   All or part of the source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void GeGLU(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, uint32_t calCount)
        ```

    -   The source operand tensor participates in computation in whole.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void GeGLU(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: through the API framework allocation approach, or by passing it via the `sharedTmpBuffer` input parameter.

-   Input parameter passing through **sharedTmpBuffer**: the tensor is used as temporary space for processing, and the API framework no longer applies for it. With this method, you can manage the **sharedTmpBuffer** memory space by themselves and reuse this memory after the API call completes. The memory is not repeatedly applied for and released, offering higher flexibility and higher memory utilization.
-   The API framework applies for temporary space, so you do not need to apply for it, but you need to reserve the size of the temporary space.

When **sharedTmpBuffer** is passed, you need to allocate space for the tensor; when the API framework allocates temporary space, you need to reserve the temporary space. The temporary space size **BufferSize** is obtained as follows: use the API provided in [GetGeGLUMaxMinTmpSize](GetGeGLUMaxMinTmpSize.md) to obtain the size of the space range that needs to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| **T** | Data type of the operand. Supported data types: **half** and **float**. |
| **isReuseSource** | Whether to allow modification of the source operand. This parameter is reserved. Pass the default value **false**. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| **dstTensor** | Output | Destination operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition values: **VECIN**/**VECCALC**/**VECOUT**. |
| **srcTensor0**/<br><br>**srcTensor1** | Input | Source operand.<br><br>The data type of the source operand must be the same as that of the destination operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition values: **VECIN**/**VECCALC**/**VECOUT**. |
| **sharedTmpBuffer** | Input | Temporary buffer.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition values: **VECIN**/**VECCALC**/**VECOUT**.<br><br>Used to store intermediate variables during complex computation inside GeGLU. Provided by you.<br><br>For how to obtain the temporary space size **BufferSize**, see [GetGeGLUMaxMinTmpSize](GetGeGLUMaxMinTmpSize.md). |
| **calCount** | Input | Number of data elements actually computed. |

## Return Value

None

## Constraints

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   **Address overlap between the source operand and the destination operand is not supported.**
-   Currently, only the ND format is supported for input, and other formats are not supported.
-   Address overlap between **sharedTmpBuffer** and the source operand or destination operand is not supported.

## Examples

```
// dstLocal: Tensor that stores the GeGLU computation result.
// src0Local: Tensor that stores the input of GeGLU.
// src1Local: Tensor that stores the input of GeGLU.
// sharedTmpBuffer: Tensor that stores the temporary buffer during the GeGLU computation.

// The API framework applies for temporary space, and all elements participate in the computation.
AscendC::GeGLU<srcType, false>(dstLocal, src0Local, src1Local);
// The API framework applies for temporary space, and some elements participate in the computation. The number of elements that need to participate in the computation is 512.
AscendC::GeGLU<srcType, false>(dstLocal, src0Local, src1Local, 512);

// Pass the temporary space through the sharedTmpBuffer input parameter, and all elements participate in the computation.
AscendC::GeGLU<srcType, false>(dstLocal, src0Local, src1Local, sharedTmpBuffer);
// Pass the temporary space through the sharedTmpBuffer input parameter, and some elements participate in the computation. The number of elements that need to participate in the computation is 512.
AscendC::GeGLU<srcType, false>(dstLocal, src0Local, src1Local, sharedTmpBuffer, 512);
```

The results are as follows:

```
Input data (src0Local):
[-2.56 -2.55 -2.54 ... 2.53  2.54  2.55]
Input data (src1Local):
[2. 2. 2. ... 2. 2. 2.]
Output data (dstLocal):
[-5.004   -4.984   -4.96 ... 4.94     4.96     4.984]
```
