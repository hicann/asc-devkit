# Sinh

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T03:56:36.067Z pushedAt=2026-10-29T02:15:02.990Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Sinh_interface/Sinh_res.md#id1 -->

## Description

Computes the hyperbolic sine function element-wise. The formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002182888644.png)

![](../../../../figures/zh-cn_formulaimage_0000001678718948.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter
    -   All/part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sinh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sinh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space
    -   All/part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sinh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Sinh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. Temporary space can be obtained in two ways: **allocated by the API framework** or **passed in by you through the sharedTmpBuffer input parameter**.

-   When the API framework allocates temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

-   When temporary space is passed in through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space yourself and reuse this memory after the API call completes. The memory is not repeatedly allocated and freed, offering higher flexibility and higher memory utilization.

For the API framework application method, you need to reserve temporary space; for the case where sharedTmpBuffer is passed in, you need to allocate space for sharedTmpBuffer. The temporary space size BufferSize is obtained as follows: use the GetSinhMaxMinTmpSize API provided in [GetSinhMaxMinTmpSize](GetSinhMaxMinTmpSize.md) to obtain the size range of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half, float. |
| isReuseSource | Whether the source operand can be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetSinhMaxMinTmpSize](GetSinhMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between sharedTmpBuffer and the source or destination operand addresses is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

<!-- npu="950" id9 -->
-   For the Ascend 950PR&950DT products, the handling of Subnormal in the internal computation of the API is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When configured as `false`, Subnormal is preserved during the computation process and participates in subsequent computation according to its actual value.
    -   When set to `true`, FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in the computation process are treated as 0, which may cause precision differences compared with retaining Subnormal values.
<!-- end id9 -->

## Examples

```
// dstLocal: Tensor that stores the Sinh computation result
// srcLocal: Tensor that stores the Sinh computation input
// sharedTmpBuffer: Tensor that stores the temporary space during the Sinh computation process

// The API framework allocates temporary space, all of which participates in the computation.
AscendC::Sinh(dstLocal, srcLocal);
// The API framework allocates temporary space, part of which participates in the computation. The number of elements involved in the computation is 512.
AscendC::Sinh(dstLocal, srcLocal, 512);

// Pass temporary space through the sharedTmpBuffer input parameter, all of which participates in the computation.
AscendC::Sinh(dstLocal, srcLocal, sharedTmpBuffer);
// Pass temporary space through the sharedTmpBuffer input parameter, part of which participates in the computation. The number of elements involved in the computation is 512.
AscendC::Sinh(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The result example is as follows:

```
Input data (srcLocal): [-2.56  -2.55  -2.54  ...  2.53  2.54  2.55]
Output data(dstLocal): [-6.434  -6.37  -6.293  ...  6.234  6.293  6.37]
```
