# Erf

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T02:47:28.904Z pushedAt=2026-10-29T02:15:02.667Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Erf_interface/Erf_res.md#id1 -->

## Description

Computes the error function element-wise (also known as the Gaussian error function, error function or Gauss error function). The formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002327668200.png)

![](../../../../figures/zh-cn_formulaimage_0000002188205638.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.
    -   All or part of the source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const ErfConfig& config = defaultErfConfig>
        __aicore__ inline void Erf(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const ErfConfig& config = defaultErfConfig>
        __aicore__ inline void Erf( const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space.
    -   All or part of the source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const ErfConfig& config = defaultErfConfig>
        __aicore__ inline void Erf(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const ErfConfig& config = defaultErfConfig>
        __aicore__ inline void Erf(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. Temporary space supports two methods: **passing it in through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   When temporary space is passed in through the sharedTmpBuffer input parameter, this tensor is used as the temporary space for processing, and the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering higher flexibility and higher memory utilization.
-   When the API framework allocates temporary space, you do not need to allocate it, but you need to reserve the size of the temporary space.

When sharedTmpBuffer is passed in, you need to allocate space for the tensor. When the API framework allocates it, you need to reserve temporary space. The method for obtaining the temporary space size BufferSize is as follows: Obtain the size of the space range to be reserved through the [GetErfMaxMinTmpSize](GetErfMaxMinTmpSize.md) API.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand can be modified. This parameter is reserved. Pass the default value false. |
| config | <!-- npu="950" id9 -->This parameter is supported only on Ascend 950PR&950DT products.<br><br><!-- end id9 -->Configuration related to the Erf algorithm. This parameter is optional and of the ErfConfig type. Its definition is shown in the following code, where the parameters have the following meanings:<br>algo: Algorithm used in the internal implementation of Erf. ErfAlgo type. Supported values are as follows: PADE_APPROXIMATION: Default value, a high-performance algorithm. This algorithm implements the Erf API through Padé approximation. SUBSECTION_POLYNOMIAL_APPROXIMATION: High-precision algorithm. This algorithm divides the values into segments and uses different coefficients to perform polynomial approximation on each segment to implement the Erf API. |

```
enum class ErfAlgo {
    PADE_APPROXIMATION = 0,
    SUBSECTION_POLYNOMIAL_APPROXIMATION,
};

struct ErfConfig {
    ErfAlgo algo = ErfAlgo::PADE_APPROXIMATION;
};
```

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetErfMaxMinTmpSize](GetErfMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping addresses between the source operand and the destination operand are not supported.**

-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

<!-- npu="950" id10 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal values in the internal computation of the API is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation process and participate in subsequent computation according to their actual values.
    -   When set to `true`, FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with preserving Subnormal values.
<!-- end id10 -->

## Examples

For the complete call example, see [Erf Examples](../../../../../../../examples/01_simd_cpp_api/04_advanced_api/10_math/erf).

```
// dstLocal: Tensor that stores the computation result.
// srcLocal: Input tensor that participates in the computation.
AscendC::Erf<srcType, false>(dstLocal, srcLocal);
// algo: Algorithm used inside Erf, defaulting to the high-performance algorithm. Here, algo is the high-precision algorithm.
// static constexpr AscendC::ErfAlgo algo = AscendC::ErfAlgo::SUBSECTION_POLYNOMIAL_APPROXIMATION;
// static constexpr AscendC::ErfConfig config = { algo };
// AscendC::Erf<srcType, false, config>(dstLocal, srcLocal);
```

The result example is as follows:

```
Input data (srcLocal): [2.015634   -2.3880906 -0.2151161  ... -2.5       0. 2.5      ]
Output data (dstLocal): [0.99563545 -0.999268  -0.23903976 ... -0.9995931 0. 0.9995931]
```
