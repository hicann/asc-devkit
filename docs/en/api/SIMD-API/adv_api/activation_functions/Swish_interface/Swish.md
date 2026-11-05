# Swish

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:06:23.399Z pushedAt=2026-10-29T02:14:34.134Z nl=2 -->

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
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/activation_functions/Swish_interface/Swish_res.md#id1 -->

## Description

In neural networks, Swish is an important activation function. Its formula is as follows, where β is a constant:

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
| T | Data type of the operand. Supported data types: half and float. |
| isReuseSource | Whether the source operand is allowed to be modified. This parameter is reserved. Pass the default value false. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcLocal | Input | Source operand.<br><br>The data type of the source operand must be the same as that of the destination operand.<br><br>The type is [LocalTensor](../../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| dataSize | Input | Number of data elements actually involved in the computation. |
| scalarValue | Input | β parameter in the activation function. Supported data types: half and float.<br><br>The data type of the β parameter must be the same as that of the source operand and the destination operand. |

## Return Value

None

## Constraints

-   For the address offset alignment requirements of operands, see [General Description and Constraints](../../../general_description_and_constraints.md).
-   **Address overlap between the source operands and the destination operand is not supported.**
-   Currently, only ND-format input is supported. Other formats are not supported.

<!-- npu="950" id8 -->
-   For Ascend 950PR&950DT products, the handling of Subnormal in the API's internal computation is controlled by the compilation option `--cce-ftz` (default value: `true`):
    -   When set to `false`, Subnormal values are preserved during the computation and participate in subsequent computations according to their actual values.
    -   When configured to `true`, the FTZ (Flush-To-Zero) mode is enabled. Subnormal numbers generated or involved in the computation process are treated as 0, which may cause precision differences in the computation result compared with retaining Subnormal numbers.
<!-- end id8 -->

## Examples

```
// dstLocal: Tensor that stores the Swish computation result.
// srcLocal: Tensor that stores the input for the Swish computation.
// scalarValue: β parameter in the activation function.

// The number of elements involved in the computation is 32, and the β parameter is -1.702.
AscendC::Swish(dstLocal, srcLocal, 32, -1.702);
```

The result is as follows:

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
