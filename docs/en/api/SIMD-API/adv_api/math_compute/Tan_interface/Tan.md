# Tan

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T04:01:15.362Z pushedAt=2026-10-29T02:15:03.009Z nl=2 -->

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
- Atlas 200I/500 A2 inference products: not supported
<!-- end id6 -->
<!-- npu="310p" id7 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: not supported
<!-- end id7 -->
<!-- npu="910" id8 -->
- Atlas training products: not supported
<!-- end id8 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Tan_interface/Tan_res.md#id1 -->

## Description

Computes the tangent function element-wise. The formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002182888708.png)

The Taylor series expansion of Tan\(x\) is:

![](../../../../figures/zh-cn_formulaimage_0000002228789689.png)

where B<sub>2n</sub> is the Bernoulli number.

## Prototype

-   Pass the temporary space through the sharedTmpBuffer input parameter.
    -   The source operand fully or partially participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Tan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The source operand tensor fully participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Tan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates the temporary space.
    -   The source operand tensor fully or partially participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Tan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The source operand tensor fully participates in the computation.

        ```
        template <typename T, bool isReuseSource = false>
        __aicore__ inline void Tan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be obtained in two ways: **allocated by the API framework** or **passed by you through the sharedTmpBuffer input parameter**.

-   The API framework allocates temporary space. You do not need to allocate it, but you need to reserve the size of the temporary space.

-   Passed in through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, which provides higher flexibility and higher memory utilization.

In the API framework application method, you need to reserve temporary space. In the case of passing through sharedTmpBuffer, you need to allocate space for sharedTmpBuffer. The temporary space size BufferSize is obtained as follows: use the GetTanMaxMinTmpSize API provided in [GetTanMaxMinTmpSize](GetTanMaxMinTmpSize.md) to obtain the size of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half, float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetTanMaxMinTmpSize](GetTanMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   The input data is limited to the range of \(-65504.0, 65504.0\).
-   **Overlapping addresses between the source operand and the destination operand are not supported.**
-   Overlapping addresses between sharedTmpBuffer and the source operand or the destination operand are not supported.

-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

<!-- npu="950" id9 -->
-   For the Ascend 950PR&950DT products, the handling of Subnormal values in the internal computation of the API is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation process and participate in subsequent computation according to their actual values.
    -   When set to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in the computation process are treated as 0, which may cause precision differences compared with preserving Subnormal values.
<!-- end id9 -->

## Examples

```
// dstLocal: Tensor that stores the Tan computation result.
// srcLocal: Tensor that stores the Tan computation input.
// sharedTmpBuffer: Tensor that stores the temporary space during the Tan computation process.

// The API framework allocates temporary space, all of which participates in the computation.
AscendC::Tan(dstLocal, srcLocal);
// The API framework allocates temporary space, part of which participates in the computation. The number of elements involved in the computation is 512.
AscendC::Tan(dstLocal, srcLocal, 512);

// Temporary space is passed in through the sharedTmpBuffer input parameter, all of which participates in the computation.
AscendC::Tan(dstLocal, srcLocal, sharedTmpBuffer);
// Temporary space is passed in through the sharedTmpBuffer input parameter, part of which participates in the computation. The number of elements involved in the computation is 512.
AscendC::Tan(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The result is as follows:

```
Input data (srcLocal):
[-2.56 -2.55 -2.54 ... 0. ... 2.53  2.54  2.55]
Output data (dstLocal):
[0.6567  0.671  0.688 ... -0.702  -0.688  -0.671 ]
```
