# Silu

<!-- md-trans-meta sourceCommit=971bc6393be4cb3e0f8244913e605c5710bb2cc9 translatedAt=2026-11-10T06:16:13.734Z pushedAt=2026-11-18T11:07:26.413Z nl=2 -->

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
- KirinX90 products: Supported
<!-- end id1 -->
<!-- npu="9030" id2 -->
- Kirin9030 products: Supported
<!-- end id2 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/Silu_interface/Silu_res.md#id1 -->

## Description

Performs the Silu operation element-wise. The computation formula is as follows:

![](../../../../figures/formulaimage_0000002350049101.png)

![](../../../../figures/formulaimage_0000001729001889.png)

## Prototype

```
template <typename T, bool isReuseSource = false>
__aicore__ inline void Silu(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, uint32_t dataSize)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operands. Supported data types: half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>Type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcLocal | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| dataSize | Input | Number of data elements actually involved in the computation. |

## Return Value

None

## Constraints

-   For the operand address offset alignment requirements, see [General Description and Constraints](../../../general_description_and_constraints.md).
-   **Overlapping between the source operand and destination operand addresses is not supported.**
-   Currently, only ND-format input is supported. Other formats are not supported.

<!-- npu="950" id9 -->
-   For the Ascend 950PR&950DT products, the handling of Subnormal values in the internal computation of the API is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation process and participate in subsequent computation according to their actual values.
    -   When set to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal values generated or involved in the computation process are treated as 0, which may cause precision differences compared with preserving Subnormal values.
<!-- end id9 -->

## Examples

```
// dstLocal: Tensor that stores the Silu computation result.
// srcLocal: Tensor that stores the Silu computation input.

// The number of elements involved in the computation is 512.
AscendC::Silu(dstLocal, srcLocal, 512);
```

The result is as follows:

```
Input data (srcLocal):
[-100.    -50.    -20.    -10.     -5.     -4.     -3.     -2.
   -1.     -0.5    -0.4    -0.3    -0.2    -0.1    -0.01    0.
    0.      0.01    0.1     0.2     0.3     0.4     0.5     1.
    2.      3.      4.      5.     10.     20.     50.    100.  ]
Output data (dstLocal):
[ -0.          -0.          -0.00000004  -0.00045398  -0.03346425  -0.07194484  -0.14227761  -0.23840587
  -0.2689414   -0.18877034  -0.16052495  -0.12766725  -0.0900332   -0.04750208  -0.004975     0.
   0.           0.005025     0.05249792   0.10996681   0.17233276   0.23947507   0.31122968   0.7310586
   1.761594     2.8577225    3.928055     4.9665356    9.999546    20.          50.         100.        ]
```
