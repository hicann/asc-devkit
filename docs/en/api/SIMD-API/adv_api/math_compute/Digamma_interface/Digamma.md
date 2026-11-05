# Digamma

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-19T02:42:50.075Z pushedAt=2026-10-29T02:15:02.651Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Digamma_interface/Digamma_res.md#id1 -->

## Description

Computes the logarithmic derivative of the gamma function of x element-wise. The computation formula is as follows, where ![](../../../../figures/zh-cn_formulaimage_0000001945339324.png) is the gamma function.

![](../../../../figures/zh-cn_formulaimage_0000002182895154.png)

![](../../../../figures/zh-cn_formulaimage_0000002218495557.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void Digamma(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    ```

-   The API framework allocates temporary space.

    ```
    template<typename T, bool isReuseSource = false>
    __aicore__ inline void Digamma(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation process. Temporary space can be managed in either of the following two ways: **passing it through the sharedTmpBuffer input parameter** or **having the API framework allocate it**.

-   Pass temporary space through the sharedTmpBuffer input parameter: this tensor is used as the temporary space for processing, and the API framework does not allocate additional space. With this method, you can manage the sharedTmpBuffer memory space on your own and reuse this memory after the API call completes. The memory is not repeatedly allocated and released, offering higher flexibility and higher memory utilization.
-   The API framework allocates temporary space: you do not need to allocate it, but you must reserve the size of the temporary space.

When temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor. When the API framework allocates temporary space, you need to reserve the temporary space. The temporary space size BufferSize is obtained as follows: use the API provided in [GetDigammaMaxMinTmpSize](GetDigammaMaxMinTmpSize.md) to obtain the size range of the space to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. The default value is false. This parameter takes effect only when the input data type is float.<br><br>true: you allow the source operand to be overwritten. You can set this parameter to true to enable this behavior. After it is enabled, the internal computation of this API reuses the memory space of srcTensor, saving some memory space.<br><br>false: the internal computation of this API does not reuse the memory space of srcTensor.<br><br>For an example of using isReuseSource, see [More Examples](../more_examples.md#section639165323915). |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Target operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>It is used to store intermediate variables during the complex internal computation of Digamma and is provided by the developer.<br><br>For how to obtain the temporary space size BufferSize, see [GetDigammaMaxMinTmpSize](GetDigammaMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Address overlap between the source operand and the destination operand is not supported.**
-   Address overlap between sharedTmpBuffer and the source operand or the destination operand is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

<!-- npu="950" id7 -->
-   For the Ascend 950PR&950DT products, the handling of Subnormal in the internal computation of the API is controlled by the compilation option `--cce-ftz` (the default value is `true`):
    -   When it is set to `false`, Subnormal is preserved during the computation process and participates in subsequent computations according to its actual value.
    -   When it is set to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with the case where Subnormal is preserved.
<!-- end id7 -->

## Examples

For the complete call example, see [More Examples](../more_examples.md).

```
// dstLocal: output Tensor
// srcLocal: input Tensor
// sharedTmpBuffer: temporary space
// The input tensor length is 1024, the operator input data type is float, and the actual number of computations is 1024.
AscendC::Digamma<float, false>(dstLocal, srcLocal, sharedTmpBuffer, 1024);
```

The result example is as follows:

```
Input data (srcLocal): [5.3675685 0.26528683 -2.872628 ... 2.9387941 9.001339]
Output data (dstLocal): [1.5843406 -3.978809 -6.2081366 ... 0.8983184 2.1407988]
```
