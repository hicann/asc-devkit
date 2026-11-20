# Atan

<!-- md-trans-meta sourceCommit=8fa2928a9ba79777fe5da4f7c9987e59a88f273e translatedAt=2026-11-09T11:03:27.084Z pushedAt=2026-11-18T08:54:29.195Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Atan_interface/Atan_res.md#id1 -->

## Description

Performs the inverse tangent (arctangent) operation element by element. The computation formula is as follows:

![](../../../../figures/formulaimage_0000002183047940.png)

![](../../../../figures/formulaimage_0000002227268389.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.
    -   All or part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
        __aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
        __aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space.
    -   All or part of the source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
        __aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation

        ```
        template <typename T, bool isReuseSource = false, const AtanConfig& config = defaultAtanConfig>
        __aicore__ inline void Atan(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   Pass temporary space through the sharedTmpBuffer input parameter, and use this tensor as the temporary space for processing; the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space yourself and reuse this portion of memory after the API call is complete. Memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.
-   The API framework allocates temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

When sharedTmpBuffer is passed in, you need to allocate space for the tensor; when the API framework allocates it, you need to reserve temporary space. The temporary space size BufferSize is obtained as follows: use the API provided in [GetAtanMaxMinTmpSize](GetAtanMaxMinTmpSize.md) to obtain the size of the space range to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. Supported data types: half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |
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

**Table 2**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type: [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition: VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetAtanMaxMinTmpSize](GetAtanMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between sharedTmpBuffer and the source operand or destination operand addresses is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

<!-- npu="950" id10 -->
-   For the Ascend 950PR&950DT products, the handling of Subnormal values in the internal computation of the API is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation process and participate in subsequent computation according to their actual values.
    -   When set to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in the computation process are treated as 0, which may cause precision differences compared with preserving Subnormal values.
<!-- end id10 -->

## Examples

```
// dstLocal: Tensor that stores the Atan computation result.
// srcLocal: Tensor that stores the Atan computation input.
// sharedTmpBuffer: Tensor that stores the temporary buffer during the Atan computation.

// The API framework applies for temporary space, all of which participates in the computation.
AscendC::Atan(dstLocal, srcLocal);
// The API framework applies for temporary space, part of which participates in the computation. The number of elements involved in the computation is 512.
AscendC::Atan(dstLocal, srcLocal, 512);

// Pass temporary space through the sharedTmpBuffer input parameter, with all elements participating in the computation.
AscendC::Atan(dstLocal, srcLocal, sharedTmpBuffer);
// Pass temporary space through the sharedTmpBuffer input parameter, with some elements participating in the computation. The number of elements involved in the computation is 512.
AscendC::Atan(dstLocal, srcLocal, sharedTmpBuffer, 512);

// Specify POLYNOMIAL_APPROXIMATION as the input algorithm, float as the input data type, and 512 as the actual number of elements involved in the computation.
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
