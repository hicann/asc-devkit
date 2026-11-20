# Exp

<!-- md-trans-meta sourceCommit=22e1e8c65e425d4c0fb07294311f2eac049ab0b3 translatedAt=2026-11-09T11:09:31.629Z pushedAt=2026-11-18T08:54:29.395Z nl=2 -->

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
- AI Core of Atlas inference products: Not supported
- Vector Core of Atlas inference products: Not supported
<!-- end id5 -->
<!-- npu="910" id6 -->
- Atlas training products: Not supported
<!-- end id6 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/math_compute/Exp_interface/Exp_res.md#id1 -->

## Description

Computes the natural exponential element-wise. You can choose whether to use the Taylor expansion formula for the computation. The computation formula is as follows:

![](../../../../figures/formulaimage_0000002223675753.png)

-   Set the number of Taylor expansion terms to 0, that is, do not use the Taylor expansion formula for computation. The formula is as follows:

    ![](../../../../figures/formulaimage_0000002224083545.png)

-   Set the number of Taylor expansion terms to a non-zero value, that is, use the Taylor expansion formula for computation. The formula is as follows:

    ![](../../../../figures/formulaimage_0000002224085413.png)

    xA<sub>i</sub> represents the integer part of the source operand, which is obtained through floor\(x\). xB<sub>i</sub> represents the fractional part of the source operand.

The Taylor expansion formula is as follows:

![](../../../../figures/formulaimage_0000002224089565.png)

## Prototype

-   Pass temporary space through the sharedTmpBuffer input parameter.

    ```
    template <typename T, uint8_t taylorExpandLevel, bool isReuseSource = false>
    __aicore__ inline void Exp(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const uint32_t calCount)
    ```

-   The API framework allocates temporary space.

    ```
    template <typename T, uint8_t taylorExpandLevel, bool isReuseSource = false>
    __aicore__ inline void Exp(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const uint32_t calCount)
    ```

Because the internal implementation of this API involves complex mathematical computation, additional temporary space is required to store intermediate variables during the computation. Temporary space supports two methods: **passing it through the sharedTmpBuffer input parameter** and **applying for it through the API framework**.

-   Pass temporary space through the sharedTmpBuffer input parameter, and use this tensor as the temporary space for processing; the API framework no longer allocates it. With this method, you can manage the sharedTmpBuffer memory space yourself and reuse this portion of memory after the API call is complete. Memory is not repeatedly allocated and released, providing higher flexibility and higher memory utilization.
-   The API framework allocates temporary space, so you do not need to allocate it, but you need to reserve the size of the temporary space.

When the temporary space is passed through sharedTmpBuffer, you need to allocate space for the tensor. When the API framework allocates the temporary space, you need to reserve the temporary space. The temporary space size BufferSize is obtained as follows: obtain the size of the space range to be reserved through the API provided in [GetExpMaxMinTmpSize](GetExpMaxMinTmpSize.md).

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. Supported data types: half and float. |
| taylorExpandLevel | Number of Taylor expansion terms. A value of 0 indicates that the Taylor formula is not used for computation. When the number of terms is too small, the precision has a certain error. A larger number of terms yields relatively higher precision but lower performance. The value range is [0, 255], and the recommended value range is [10, 15]. |
| isReuseSource | Whether the source operand is allowed to be modified. The default value is false. This parameter takes effect only when the input data type is float.<br>true: You allow the source operand to be overwritten. You can set this parameter to true to enable it. After it is enabled, the memory space of srcLocal is reused during the internal computation of this API, saving some memory space. false: The memory space of srcLocal is not reused during the internal computation of this API.<br><br>For a usage example of isReuseSource, see [More Examples](../more_examples.md#section639165323915). |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcLocal | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| sharedTmpBuffer | Input | Temporary buffer.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>Used to store intermediate variables during the complex internal computation of Exp. It is provided by you.<br><br>For how to obtain the temporary space size BufferSize, see [GetExpMaxMinTmpSize](GetExpMaxMinTmpSize.md). |
| calCount | Input | Number of elements involved in the computation. |

## Return Value

None

## Constraints

-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Overlapping between sharedTmpBuffer and the source operand or destination operand addresses is not supported.
-   For the operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

<!-- npu="950" id7 -->
-   For the Ascend 950PR&950DT products, the handling of Subnormal values in the internal computation of the API is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation process and participate in subsequent computation according to their actual values.
    -   When set to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in the computation process are treated as 0, which may cause precision differences compared with preserving Subnormal values.
<!-- end id7 -->

## Examples

```
// dstLocal: Tensor that stores the computation result.
// srcLocal: Input tensor involved in the computation.
// sharedTmpBuffer: Temporary buffer for storing intermediate variables during complex internal computation.
// The operand type is half, the Taylor expansion has 15 terms, and the source operand cannot be modified.
// The input tensor length is 1024, the operator input data type is half, and the actual number of computations is 512.
AscendC::Exp<half, 15, false>(dstLocal, srcLocal, sharedTmpBuffer, 512);
```

The result is as follows:

```
Input data (srcLocal):
[ 0.0        0.01       0.02       0.03       ...  5.11     ]
Output data (dstLocal):
[ 1.0000000  1.0100503  1.0202013  1.0304545  ...  165.67036]
```
