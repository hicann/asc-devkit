# Cos

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-09T13:58:24.072Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs the trigonometric cosine operation element-wise. The calculation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002218333665.png)

The Taylor expansion of Cos\(x\) is:

![](../../../../figures/zh-cn_formulaimage_0000002376868752.png)

## Prototype

-   Pass the temporary space through the **sharedTmpBuffer** input parameter.
    -   The source operand tensor participates in computation either in whole or in part.

        ```
        template <typename T, bool isReuseSource = false, const CosConfig& config = defaultCosConfig>
        __aicore__ inline void Cos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const CosConfig& config = defaultCosConfig>
        __aicore__ inline void Cos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer)
        ```

-   The API framework allocates temporary space.
    -   The source operand tensor participates in computation either in whole or in part.

        ```
        template <typename T, bool isReuseSource = false, const CosConfig& config = defaultCosConfig>
        __aicore__ inline void Cos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
        ```

    -   The entire source operand Tensor participates in the computation.

        ```
        template <typename T, bool isReuseSource = false, const CosConfig& config = defaultCosConfig>
        __aicore__ inline void Cos(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor)
        ```

Because the internal implementation of this API involves complex computation, additional temporary space is required to store intermediate variables during the computation process. The temporary space can be allocated in two ways: **through the API framework allocation approach, or by you passing it via the sharedTmpBuffer input parameter**.

-   Pass it through the sharedTmpBuffer input parameter. This tensor is used as temporary space for processing, and the API framework no longer allocates it. In this method, you can manage the sharedTmpBuffer memory space and reuse this memory after the API call is complete. The memory is not repeatedly allocated and released, offering high flexibility and high memory utilization.
-   The API framework allocates temporary space. You do not need to allocate it, but must reserve the size of the temporary space.

When the temporary space is passed through **sharedTmpBuffer**, you need to allocate space for the tensor. When the API framework allocates it, you need to reserve the temporary space. The size of the temporary space, **BufferSize**, is obtained as follows: obtain the size of the space range that needs to be reserved through the API provided in [GetCosMaxMinTmpSize](GetCosMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. The default value is **false**. This parameter takes effect only when the input data type is float.<br>**true**: You allow the source operand to be overwritten. You can set this parameter to **true** to enable it. After it is enabled, the memory space of **srcTensor** is reused during internal computation of this API, saving some memory space. **false**: The memory space of **srcTensor** is not reused during internal computation of this API.<br><br>For an example of using **isReuseSource**, see [More Samples](../more_samples_83.md#section639165323915). |
| config | This parameter is supported only on Ascend 950PR/Ascend 950DT.<br><br>Related configuration of the Cos algorithm. This parameter is optional and of the CosConfig type. Its specific definition is shown in the following code, where the meanings of the parameters are as follows:<br>algo: Algorithm used in the internal implementation of Cos. It is of the CosAlgo type, and the supported values are as follows: POLYNOMIAL_APPROXIMATION: default value. This algorithm implements the Cos API through simple polynomial approximation. The supported input value range is [-65504.0, 65504.0], and the supported data types are half and float. RADIAN_REDUCTION: This algorithm implements the Cos API through complete period reduction. It supports the full input value range, and the supported data types are half and float. |

```
struct CosConfig {
  CosAlgo algo = CosAlgo::POLYNOMIAL_APPROXIMATION;
};
enum class CosAlgo {
  POLYNOMIAL_APPROXIMATION = 0,
  RADIAN_REDUCTION,
};
```

**Table 2**  Parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>For how to obtain the temporary space size BufferSize, see [GetCosMaxMinTmpSize](GetCosMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   For Ascend 950PR/Ascend 950DT, when the polynomial fitting algorithm POLYNOMIAL\_APPROXIMATION is used in the template parameter **config**, the value range of the input source data must be within \[-65504.0, 65504.0\].
-   For the following products, the input source data must keep its value range within \[-65504.0, 65504.0\].
    -   Atlas A3 training products/Atlas A3 inference products
    -   Atlas A2 training products/Atlas A2 inference products
    -   Atlas inference products AI Core

-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Examples

```
// dstLocal: Tensor that stores the computation result.
// srcLocal: Input tensor that participates in the computation.
// sharedTmpBuffer: Temporary buffer that stores intermediate variables during internal complex computation.
// The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
AscendC::Cos(dstLocal, srcLocal, sharedTmpBuffer, 512);
constexpr AscendC::CosAlgo algo = AscendC::CosAlgo::RADIAN_REDUCTION;
constexpr AscendC::CosConfig config = { algo };
AscendC::Cos<half, false, config>(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The results are as follows:

```
Input data (srcLocal): [0.00            0.01            0.02             ...  5.10            5.11]
Output data (dstLocal): [1.00000000e+00  9.99949992e-01  9.99800026e-01   ...  3.77977639e-01  3.87216508e-01]
```
