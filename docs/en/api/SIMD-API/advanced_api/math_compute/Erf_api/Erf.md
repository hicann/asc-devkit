# Erf

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T14:40:32.075Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs element-wise error function computation (also known as the Gauss error function). The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002327668200.png)

![](../../../../figures/zh-cn_formulaimage_0000002188205638.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.
    -   The source operand tensor participates in computation either in whole or in part.

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
    -   The source operand tensor participates in computation either in whole or in part.

        ```
        template <typename T, bool isReuseSource = false, const ErfConfig& config = defaultErfConfig>
        __aicore__ inline void Erf(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const ErfConfig& config = defaultErfConfig>
        __aicore__ inline void Erf(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by you passing it via the sharedTmpBuffer input parameter**.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

When the temporary space is passed through **sharedTmpBuffer**, you need to allocate space for the tensor. When the API framework allocates it, you need to reserve the temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: obtain the size of the space range that needs to be reserved through the [GetErfMaxMinTmpSize](GetErfMaxMinTmpSize.md) API.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value **false**. |
| config | This parameter is supported only on Ascend 950PR/Ascend 950DT.<br><br>Erf algorithm configuration. This parameter is optional and of the **ErfConfig** type. Its definition is shown in the following code, where the parameters mean:<br>**algo**: Algorithm used in the internal implementation of Erf. It is of the **ErfAlgo** type and supports the following values: **PADE_APPROXIMATION**: default value, a high-performance algorithm. This algorithm implements the Erf API through the Padé approximation algorithm. **SUBSECTION_POLYNOMIAL_APPROXIMATION**: a high-precision algorithm. This algorithm implements the Erf API by segmenting the values and applying polynomial approximation to each segment with different coefficients. |

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
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetErfMaxMinTmpSize](GetErfMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**

-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

For the complete calling example, see the [Erf example](https://gitcode.com/cann/asc-devkit/tree/9.1.0/examples/01_simd_cpp_api/04_advanced_api/10_math/erf).

```
// dstLocal: Tensor that stores the computation result.
// srcLocal: Input tensor involved in the computation.
AscendC::Erf<srcType, false>(dstLocal, srcLocal);
// algo: algorithm used inside Erf, defaulting to the high-performance algorithm. Here algo is the high-precision algorithm.
// static constexpr AscendC::ErfAlgo algo = AscendC::ErfAlgo::SUBSECTION_POLYNOMIAL_APPROXIMATION;
// static constexpr AscendC::ErfConfig config = { algo };
// AscendC::Erf<srcType, false, config>(dstLocal, srcLocal);
```

The results are as follows:

```
Input data (srcLocal): [2.015634   -2.3880906 -0.2151161  ... -2.5       0. 2.5      ]
Output data (dstLocal): [0.99563545 -0.999268  -0.23903976 ... -0.9995931 0. 0.9995931]
```
