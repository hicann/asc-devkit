# Lgamma

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-09T11:13:08.891Z pushedAt=2026-11-18T08:54:29.500Z nl=2 -->

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
- AI Core of Atlas inference products: Supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Vector Core of Atlas inference products: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Lgamma_interface/Lgamma_res.md#id1 -->

## Description

Computes the absolute value of the gamma function of x element-wise and then takes its natural logarithm. The computation formula is as follows:

![](../../../../figures/formulaimage_0000002218495125.png)

![](../../../../figures/formulaimage_0000002188278974.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void Lgamma(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    ```

-   The API framework allocates temporary space.

    ```
    template <typename T, bool isReuseSource = false>
    __aicore__ inline void Lgamma(const LocalTensor<T>& dstTensor, const LocalTensor<T>& srcTensor, const uint32_t calCount)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   Pass temporary space through the sharedTmpBuffer input parameter, and use this tensor as the temporary space for processing; the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space yourself and reuse this portion of memory after the API call is complete. Memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.
-   The API framework allocates temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

When the temporary space is passed in through sharedTmpBuffer, you need to allocate space for the tensor. When the API framework allocates the temporary space, you need to reserve the temporary space. The size of the temporary space, BufferSize, is obtained as follows: obtain the size of the space range to be reserved through the API provided in [6.1.55 Lgamma Tiling](GetLgammaMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. Supported data types: half and float. |
| isReuseSource | Whether the source operand can be modified. The default value is false. This parameter takes effect only when the input data type is float.<br><br>true: You allow the source operand to be overwritten. You can set this parameter to true to enable this behavior. After it is enabled, the API reuses the memory space of srcTensor during internal computation, saving some memory space.<br><br>false: The API does not reuse the memory space of srcTensor during internal computation.<br><br>For an example of using isReuseSource, see [More Examples](../more_examples.md#section639165323915). |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstTensor | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcTensor | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex internal computation of Lgamma. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetLgammaMaxMinTmpSize](GetLgammaMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between sharedTmpBuffer and the source operand or destination operand addresses is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

<!-- npu="950" id8 -->
-   For the Ascend 950PR&950DT products, the handling of Subnormal values in the internal computation of the API is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation process and participate in subsequent computation according to their actual values.
    -   When set to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in the computation process are treated as 0, which may cause precision differences compared with preserving Subnormal values.
<!-- end id8 -->

## Examples

```
// dstLocal: Tensor that stores the computation result.
// srcLocal: Input tensor that participates in the computation.
// sharedTmpBuffer: Temporary buffer that stores intermediate variables during complex internal computation.
// The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
AscendC::Lgamma(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The result is as follows:

```
Input data (srcLocal): [0.000000  0.010000  0.020000  0.030000  0.040000  ...]
Output data (dstLocal): [inf       4.599480  3.900805  3.489971  3.197078  ...]
```
