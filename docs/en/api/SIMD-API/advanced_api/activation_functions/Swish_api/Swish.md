# Swish

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-05T14:01:09.144Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

In neural networks, Swish is an important activation function. Its computation formula is as follows, where β is a constant:

![](../../../../figures/zh-cn_formulaimage_0000002316174174.png)

![](../../../../figures/zh-cn_formulaimage_0000001680848998.png)

## Prototype

```
template <typename T, bool isReuseSource = false>
__aicore__ inline void Swish(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, uint32_t dataSize, const T scalarValue)
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
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcLocal | Input | Source operand.<br><br>The data type of the source operand must be the same as that of the destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| dataSize | Input | Number of data elements actually involved in the computation. |
| scalarValue | Input | β parameter in the activation function. The supported data types are half and float.<br><br>The data type of the β parameter must be the same as that of the source operand and the destination operand. |

## Return Value

None

## Constraints

-   For the operand address offset alignment requirements, see [General Description and Constraints](../../../general_description_and_constraints.md).
-   **Address overlap between the source operand and the destination operand is not supported.**
-   Currently, only ND-format input is supported, and other formats are not supported.

## Examples

```
// dstLocal: Tensor that stores the Swish computation result
// srcLocal: Tensor that stores the input for the Swish computation
// scalarValue: β parameter in the activation function

// The number of elements involved in the computation is 32, and the β parameter is -1.702
AscendC::Swish(dstLocal, srcLocal, 32, -1.702);
```

The results are as follows:

```
Input data (srcLocal):
[-4.         -3.7419355  -3.483871   -3.2258065  -2.967742   -2.7096775  -2.451613   -2.1935484
 -1.9354838  -1.6774193  -1.4193548  -1.1612903  -0.9032258  -0.6451613  -0.38709676 -0.12903225
  0.12903225  0.38709676  0.6451613   0.9032258   1.1612903   1.4193548   1.6774193   1.9354838
  2.1935484   2.451613    2.7096775   2.967742    3.2258065   3.483871    3.7419355   4.        ]
Output data (dstLocal):
[-0.00441472 -0.00640367 -0.00924141 -0.01325663 -0.01887952 -0.02665107 -0.03720944 -0.05122496
 -0.06923404 -0.09129371 -0.11635891 -0.14131825 -0.15980731 -0.16135658 -0.13200021 -0.05746017
  0.07157208  0.25509655  0.48380467  0.74341846  1.019972    1.3029958   1.5861256   1.8662498
  2.1423235   2.4144034   2.6830263   2.9488626   3.2125497   3.4746296   3.735532    3.995585  ]
```
