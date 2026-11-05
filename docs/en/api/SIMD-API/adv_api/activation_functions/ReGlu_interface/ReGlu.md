# ReGlu

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T08:34:54.568Z pushedAt=2026-10-29T02:14:34.045Z nl=2 -->

## Applicable Products

<!-- npu="950" id9 -->
- Ascend 950PR&950DT products: Supported
<!-- end id9 -->
<!-- npu="A3" id10 -->
- Atlas A3 products: Supported
<!-- end id10 -->
<!-- npu="910b" id11 -->
- Atlas A2 products: Supported
<!-- end id11 -->
<!-- npu="310b" id12 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id12 -->
<!-- npu="310p" id13 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id13 -->
<!-- npu="910" id14 -->
- Atlas training products: Not supported
<!-- end id14 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/ReGlu_interface/ReGlu_res.md#id1 -->

## Description

ReGlu is a GLU variant that uses ReLU as the activation function. Its computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002316175130.png)

The computation formula of the ReLU activation function is as follows:

![](../../../../figures/zh-cn_formulaimage_0000001792534088.png)

## Prototype

-   Pass the temporary space through the sharedTmpBuffer input parameter

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void ReGlu(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    ```

-   The API framework allocates the temporary space

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void ReGlu(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor0, const LocalTensor<T>& srcTensor1, const uint32_t calCount)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. The temporary space supports two methods: **passing through the sharedTmpBuffer input parameter** and **allocation by the API framework**.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

When sharedTmpBuffer is passed in, you need to allocate space for the tensor; when the API framework allocates it, you need to reserve temporary space. The temporary space size BufferSize is obtained as follows: use the API provided in [GetReGluMaxMinTmpSize](GetReGluMaxMinTmpSize.md) to obtain the size of the space range to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. For the data types supported by different models, see [supported data types](#li356910105419). |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor0 | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| srcTensor1 | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside ReGlu. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetReGluMaxMinTmpSize](GetReGluMaxMinTmpSize.md). |
| calCount | Input | Number of data elements actually computed. |

## Return Value

None

## Constraints

-   For the address alignment requirements of operands, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
-   **Address overlap between the source operands and the destination operand is not supported.**
-   sharedTmpBuffer must not overlap with the source operand or destination operand addresses.
-   Currently, only the ND format is supported for input. Other formats are not supported.
-   Supported data types<a id="li356910105419"></a>

    <!-- npu="950" id5 -->
    Ascend 950PR&950DT products support the following data types: half, bfloat16\_t, and float.
    <!-- end id5 -->

    <!-- npu="A3" id6 -->
    Atlas A3 products support the following data types: half, bfloat16\_t, and float.
    <!-- end id6 -->

    <!-- npu="910b" id7 -->
    Atlas A2 products support the following data types: half, bfloat16\_t, and float.
    <!-- end id7 -->

    <!-- npu="310p" id8 -->
    The Atlas inference products AI Core supports the following data types: half and float.
    <!-- end id8 -->

## Examples

```
// dstLocal: Tensor that stores the ReGlu computation result.
// src0Local: Tensor that stores the ReGlu computation input.
// src1Local: Tensor that stores the ReGlu computation input.
// sharedTmpBuffer: Tensor that stores the temporary buffer during the ReGlu computation.

// The API framework allocates temporary space. Only some elements are involved in the computation, and the number of elements involved is 30.
AscendC::ReGlu(dstLocal, src0Local, src1Local, 30);

// Pass the temporary space through the sharedTmpBuffer input parameter. Only some elements are involved in the computation, and the number of elements involved is 30.
AscendC::ReGlu(dstLocal, src0Local, src1Local, sharedTmpBuffer, 30);
```

The result is as follows:

```
Input data (src0Local):
[2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2.
 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2. 2.]
Input data (src1Local):
[-1.6 -1.5 -1.4 -1.3 -1.2 -1.1 -1.  -0.9 -0.8 -0.7 -0.6 -0.5 -0.4 -0.3 -0.2 -0.1
  0.   0.1  0.2  0.3  0.4  0.5  0.6  0.7  0.8  0.9  1.   1.1  1.2  1.3  1.4  1.5]
Output data (dstLocal):
[0.  0.  0.  0.  0.  0.  0.  0.  0.  0.  0.  0.  0.  0.  0.  0.
 0.  0.2 0.4 0.6 0.8 1.  1.2 1.4 1.6 1.8 2.  2.2 2.4 2.6 0.  0. ]
```
