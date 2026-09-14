# Silu

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T13:43:03.647Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Performs the Silu operation element-wise. The computation formula is as follows:

![](../../../../figures/zh-cn_formulaimage_0000002350049101.png)

![](../../../../figures/zh-cn_formulaimage_0000001729001889.png)

## Prototype

```
template <typename T, bool isReuseSource = false>
__aicore__ inline void Silu(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, uint32_t dataSize)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types are half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value **false**. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcLocal | Input | Source operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of the source operand must be consistent with that of the destination operand. |
| dataSize | Input | Number of data elements actually computed. |

## Return Value

None

## Constraints

- For alignment requirements on operand address offsets, please refer to [General Description and Constraints](../../../general_description_and_constraints.md).
-   **Address overlap between the source operand and the destination operand is not supported.**
-   Currently, only ND-format input is supported, and other formats are not supported.

## Examples

```
// dstLocal: Tensor that stores the Silu computation result.
// srcLocal: Tensor that stores the Silu computation input.

// Number of elements to be computed is 512.
AscendC::Silu(dstLocal, srcLocal, 512);
```

The results are as follows:

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
