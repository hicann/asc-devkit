# Atan

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T02:08:20.698Z pushedAt=2026-10-29T02:15:02.498Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Atan_interface/Atan_res.md#id1 -->

## Description

Performs the inverse tangent (arctangent) operation element by element. The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002183047940.png)

![](../../../../figures/zh-cn_formulaimage_0000002227268389.png)

## Prototype

-   Pass the temporary space through the sharedTmpBuffer input parameter.
    -   All or part of the source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
        __aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
        __aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates the temporary space.
    -   All or part of the source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
        __aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
        __aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space supports two methods: **passing through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   When the temporary space is passed through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering higher flexibility and higher memory utilization.
-   When the API framework allocates the temporary space, you do not need to allocate it, but you must reserve the size of the temporary space.

When sharedTmpBuffer is passed in, you need to allocate space for the tensor; when the API framework allocates it, you need to reserve temporary space. The temporary space size BufferSize is obtained as follows: use the API provided in [GetAtanMaxMinTmpSize](GetAtanMaxMinTmpSize.md) to obtain the size of the space range to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand can be modified. This parameter is reserved. Pass the default value false. |
| config | <!-- npu="950" id9 -->This parameter is supported only on Ascend 950PR&950DT products.<br><br><!-- end id9 -->Atan algorithm configuration. This parameter is optional and of the AtanConfig type. Its definition is shown in the following code, where the parameters mean:<br>algo: Algorithm used by the Atan internal implementation. AtanAlgo type. Supported values: TAYLOR_EXPANSION: Default value. This algorithm implements Atan using a 6th-order Taylor expansion and supports the half and float data types. POLYNOMIAL_APPROXIMATION: This algorithm is a 17th-order polynomial approximation and supports the float data type. |

```
enum class AtanAlgo {
    TAYLOR_EXPANSION = 0,
    POLYNOMIAL_APPROXIMATION,
};

struct AtanConfig {
    AtanAlgo algo = AtanAlgo::TAYLOR_EXPANSION;
};
```

**Table 2**  Parameters

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be the same as that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetAtanMaxMinTmpSize](GetAtanMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Source operand and destination operand address overlap is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or destination operand is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

<!-- npu="950" id10 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal in the API's internal computation is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation and participate in subsequent computations according to their actual values.
    -   When configured to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal numbers generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with retaining Subnormal numbers.
<!-- end id10 -->

## Examples

```
// dstLocal: Tensor that stores the Atan computation result
// srcLocal: Tensor that stores the Atan computation input
// sharedTmpBuffer: Tensor that stores the temporary space during the Atan computation process

// The API framework allocates temporary space, and all elements participate in the computation.
AscendC::Atan(dstLocal, srcLocal);
// The API framework allocates temporary space, and some elements participate in the computation. The number of elements involved in the computation is 512.
AscendC::Atan(dstLocal, srcLocal, 512);

// Pass the temporary space through the sharedTmpBuffer input parameter, and all elements participate in the computation.
AscendC::Atan(dstLocal, srcLocal, sharedTmpBuffer);
// Pass the temporary space through the sharedTmpBuffer input parameter, and some elements participate in the computation. The number of elements involved in the computation is 512.
AscendC::Atan(dstLocal, srcLocal, sharedTmpBuffer, 512);

// Specify the input algorithm as POLYNOMIAL_APPROXIMATION, the input data type as float, and the actual number of elements to calculate as 512.
static constexpr AscendC::AtanConfig atanConfig = {AscendC::AtanAlgo::POLYNOMIAL_APPROXIMATION};
AscendC::Atan<float, false, atanConfig>(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The sample data is as follows:

```
Input data (srcLocal):
[-2.56 -2.55 -2.54 ... 0. ... 2.53  2.54  2.55]
Output data (dstLocal):
[-1.19847027, -1.19717361, -1.19560622 ... 0. ... 1.19429046, 1.19560622, 1.19717361]
```
