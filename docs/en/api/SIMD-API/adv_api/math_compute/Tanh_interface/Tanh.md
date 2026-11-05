# Tanh

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T04:01:19.571Z pushedAt=2026-10-29T02:15:03.006Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Tanh_interface/Tanh_res.md#id1 -->

## Description

Performs element-wise logistic regression Tanh. The formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002218327685.png)

![](../../../../figures/zh-cn_formulaimage_0000002193334172.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter
    -   All or part of the source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false, const TanhConfig& config = DEFAULT_TANH_CONFIG>
        __aicore__ inline void Tanh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false, const TanhConfig& config = DEFAULT_TANH_CONFIG>
        __aicore__ inline void Tanh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space

    -   All or part of the source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false, const TanhConfig& config = DEFAULT_TANH_CONFIG>
        __aicore__ inline void Tanh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false, const TanhConfig& config = DEFAULT_TANH_CONFIG>
        __aicore__ inline void Tanh(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

    Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. Temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

    -   Pass it through the sharedTmpBuffer input parameter: this tensor is used as the temporary space for processing, and the API framework does not allocate additional space. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering higher flexibility and higher memory utilization.
    -   The API framework allocates temporary space: you do not need to allocate it, but you need to reserve the size of the temporary space.

    When the temporary space is passed in through sharedTmpBuffer, you need to allocate space for the tensor; when the temporary space is allocated by the API framework, you need to reserve the temporary space. The temporary space size BufferSize is obtained as follows: use the API provided in [GetTanhMaxMinTmpSize](GetTanhMaxMinTmpSize.md) to obtain the size range of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |
| config | <!-- npu="950" id9 -->This parameter is supported only on Ascend 950PR&950DT products.<br><br><!-- end id9 -->Configuration related to the Tanh algorithm. This parameter is optional and of the TanhConfig type. Its definition is shown in the following code, where the parameters mean:<br>algo: Algorithm used by the internal Tanh implementation. It is of the TanhAlgo type and supports the following values: INTRINSIC: default value. This algorithm directly computes the formula y = (e^(2x)-1)/(e^(2x)+1), offering better performance. SUBSECTION_COMPENSATION: This algorithm implements Tanh through error compensation, offering higher precision. |

```
enum class TanhAlgo {
    INTRINSIC = 0,
    SUBSECTION_COMPENSATION,
};
struct TanhConfig {
    TanhAlgo algo = TanhAlgo::INTRINSIC;
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex computation inside Tanh. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetTanhMaxMinTmpSize](GetTanhMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping addresses between the source operand and the destination operand are not supported.**
-   Overlapping addresses between sharedTmpBuffer and the source operand and destination operand are not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

<!-- npu="950" id10 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal values in the internal computation of the API is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When configured as `false`, Subnormal values are preserved during the computation process and participate in subsequent computation according to their actual values.
    -   When configured as `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in the computation process are treated as 0, which may cause precision differences compared with the result when Subnormal values are retained.
<!-- end id10 -->

## Examples

```
// dstLocal: Tensor that stores the Tanh computation result.
// srcLocal: Tensor that stores the Tanh computation input.
// sharedTmpBuffer: Tensor that stores the temporary space during the Tanh computation process.

// The API framework allocates temporary space, and all elements participate in the computation.
AscendC::Tanh(dstLocal, srcLocal);
// The API framework allocates temporary space, and some elements participate in the computation. The number of elements to be computed is 512.
AscendC::Tanh(dstLocal, srcLocal, 512);

// Temporary space is passed through the sharedTmpBuffer input parameter, and all elements participate in the computation.
AscendC::Tanh(dstLocal, srcLocal, sharedTmpBuffer);
// Temporary space is passed through the sharedTmpBuffer input parameter, and some elements participate in the computation. The number of elements to be computed is 512.
AscendC::Tanh(dstLocal, srcLocal, sharedTmpBuffer, 512);
static constexpr AscendC::TanhAlgo algo = AscendC::TanhAlgo::SUBSECTION_COMPENSATION;
static constexpr AscendC::TanhConfig config = {algo};
AscendC::Tanh<half, false, config>(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The result example is as follows:

```
Input data (srcLocal):
[-2.56 -2.55 -2.54 ... 0. ... 2.53  2.54  2.55]
Output data (dstLocal):
[-0.98813187 -0.9878992 -0.98761402 ... 0. ... 0.98737127 0.98761402 0.9878992]
```
