# Atanh

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T02:10:35.860Z pushedAt=2026-10-29T02:15:02.507Z nl=2 -->

## Applicable Products

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: Supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: Supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Atanh_interface/Atanh_res.md#id1 -->

## Description

Performs element-wise computation of the inverse hyperbolic tangent cosine function, with the following formula:

![](../../../../figures/zh-cn_formulaimage_0000002182892020.png)

![](../../../../figures/zh-cn_formulaimage_0000001765074025.png)

## Prototype

-   Pass the temporary space through the sharedTmpBuffer input parameter
    -   All or part of the source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Atanh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Atanh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates the temporary space
    -   All or part of the source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Atanh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Atanh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. The temporary space supports two methods: **passing through the sharedTmpBuffer input parameter** and **allocation by the API framework**.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   When the API framework allocates the temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

When sharedTmpBuffer is passed in, you need to allocate space for the tensor; when the API framework allocates the space, you need to reserve temporary space. The size of the temporary space, BufferSize, is obtained as follows: call the [GetAtanhMaxMinTmpSize](GetAtanhMaxMinTmpSize.md) API to obtain the size of the space range to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand can be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during complex computation inside Atanh. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetAtanhMaxMinTmpSize](GetAtanhMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   The input source data must be kept within the range \(-0.99, -0.001\), or \(0.001, 0.99\). If the input is out of range, the output result is invalid.
-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between sharedTmpBuffer and the source operand or destination operand addresses is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

For the complete call example, see [Operator Examples](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/10_math/acosh).

```
// dstLocal: Tensor that stores the Atanh computation result.
// srcLocal: Tensor that stores the Atanh computation input.
// sharedTmpBuffer: Tensor that stores the temporary space during the Atanh computation.
// The input data type of the operator is half, and the number of elements involved in the computation is 512.
AscendC::Atanh(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The following shows an example of the result:

```
Input data (srcLocal): [0.000000 0.010000 0.020000 ...  0.990000 1.000000 1.010000 ...]
Output data (dstLocal): [0.000000 0.010000 0.020003 ...  2.646650 inf -nan ...]
```
