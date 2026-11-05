# Erfc

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T02:49:46.463Z pushedAt=2026-10-29T02:15:02.681Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Erfc_interface/Erfc_res.md#id1 -->

## Description

Returns the complementary error function result of the input x, with the integration interval from x to infinity. The original theoretical computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002217748513.png)

![](../../../../figures/zh-cn_formulaimage_0000002377042796.png)

Because the Erfc function has no elementary function expression, it is generally calculated through function approximation. The approximate computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002218332193.png)

where:

R\(z\) = \(\(\(\(\(\(\(z \* R0 + R1\) \* z + R2\) \* z + R3\) \* z + R4\) \* z + R5\) \* z + R6\) \* z + R7\) \* z + R8 is an 8th-degree polynomial in z;

S\(z\) = \(\(\(\(z + S1\) \* z + S2\) \* z + S3\) \* z + S4\) \* z + S5 is a 4th-degree polynomial in z.

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter
    -   All or part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Erfc(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Erfc(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space
    -   All or part of the source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Erfc(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Erfc(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computations, additional temporary space is required to store intermediate variables during the computation process. The temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, which provides higher flexibility and higher memory utilization.
-   The API framework allocates the temporary space. You do not need to allocate it, but you need to reserve the size of the temporary space.

When the temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor. When the API framework allocates the temporary space, you need to reserve the temporary space. The temporary space size BufferSize is obtained as follows: use the API provided in [GetErfcMaxMinTmpSize](GetErfcMaxMinTmpSize.md) to obtain the size of the space range that needs to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex internal computation of Erfc. Provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetErfcMaxMinTmpSize](GetErfcMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   The input source data must be within the value range of \[-inf, inf\]. If the input is out of range, the output result is invalid.
-   **Overlapping between the source operand and the destination operand is not supported.**
-   Overlapping between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

<!-- npu="950" id9 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal values in the internal computation of the API is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation process and participate in subsequent computation according to their actual values.
    -   When set to `true`, FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in the computation process are treated as 0, which may cause precision differences compared with the results when Subnormal values are preserved.
<!-- end id9 -->

## Examples

```
// dstLocal: Tensor for storing the computation result.
// srcLocal: Input tensor involved in the computation.
// sharedTmpBuffer: Temporary buffer for storing intermediate variables during complex internal computation.
// The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
AscendC::Erfc(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The result example is as follows:

```
Input data (srcLocal): [0.0           1.0           2.0            3.0           ...]
Output data (dstLocal): [1.0000000e+00 1.5729921e-01 4.6777348e-03  2.2090497e-05 ...]
```
