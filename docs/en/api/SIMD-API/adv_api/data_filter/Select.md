# Select

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:54:28.937Z pushedAt=2026-10-29T02:14:34.403Z nl=2 -->

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/data_filter/Select_res.md#id1 -->

## Description

Given two source operands src0 and src1, this API selects elements based on the value (non-bit) at the corresponding position of maskTensor to obtain the destination operand dst. The selection rule is as follows: when the Mask value is 0, the element is selected from src0; otherwise, it is selected from src1.

**This API supports multi-dimensional shapes. The maskTensor and the source operand tensors must have the same element count on the leading axes (non-last axes), and the element count of the maskTensor last axis must be greater than or equal to that of the source operand last axis. The excess part of maskTensor is discarded and does not participate in the computation.**

-   **The maskTensor last axis needs 32-byte alignment and an element count that is a multiple of 16.**
-   **The last axis of the source operand tensor must be 32-byte aligned.**

As shown in the following example, the source operand src0 is a tensor with shape \(2,16\) and data type half, and its last axis length satisfies 32-byte alignment. The source operand src1 is a scalar with data type half. The data type of maskTensor is bool, and to satisfy the alignment requirement, its shape is \(2,32\). Only the mask in the blue part of the figure takes effect, and the gray part does not participate in the computation. The output destination operand dstTensor is shown in the following figure.

![](../../../figures/nz_reduce_53.png)

## Implementation Principles

Using a source input tensor of the float type in ND format with a shape of \[m, k1\] and a mask tensor with a shape of \[m, k2\] as an example, the following figure shows the internal algorithm diagram of the Select high-order API.

**Figure 1** Select algorithm block diagram  
![](../../../figures/select_diagram.png "Select algorithm block diagram")

The computation process consists of the following steps, all performed on Vector:

1.  GatherMask step: If k1 and k2 are not equal, reduce computation is performed on the input mask\[m, k2\] through GatherMask based on the shape\[m, k1\] of src, so that the redundant part of the k axis of the mask is discarded and the shape is converted to \[m, k1\];
2.  Cast step: Cast the mask result from the previous step to the half type;
3.  Compare step: Use the Compare API to compare the mask result from the previous step with 0 to obtain the cmpmask result;
4.  Select step: Based on the cmpmask result, select the value at the corresponding position of srcTensor or the scalar value, and output the Output.

**Figure 2** Select algorithm block diagram  
![](../../../figures/select_diagram_1.png "Select algorithm block diagram-54")

The computation process is performed on Vector, looping m times, and each time the following operations are performed on k1 elements:

1. Compare step: Use the Compare API to compare the mask value with 0 to obtain the cmpmask result.
2. Select step: Based on the cmpmask result, select the value at the corresponding position of srcTensor or the scalar value, and output it to Output.

## Prototype

- src0 is srcTensor (tensor type), and src1 is srcScalar (scalar type).

    ```
    template <typename T, typename U, bool isReuseMask = true>
    __aicore__ inline void Select(const LocalTensor<T>& dst, const LocalTensor<T>& src0, T src1, const LocalTensor<U>& mask, const LocalTensor<uint8_t>& sharedTmpBuffer, const SelectWithBytesMaskShapeInfo& info)
    ```

- src0 is srcScalar (scalar type), and src1 is srcTensor (tensor type).

    ```
    template <typename T, typename U, bool isReuseMask = true>
    __aicore__ inline void Select(const LocalTensor<T>& dst, T src0, const LocalTensor<T>& src1, const LocalTensor<U>& mask, const LocalTensor<uint8_t>& sharedTmpBuffer, const SelectWithBytesMaskShapeInfo& info)
    ```

This API requires additional temporary space to store intermediate variables during computation. You need to **allocate the temporary space and pass it in through the sharedTmpBuffer input parameter**. The buffer size BufferSize is obtained as follows: use the API provided in [GetSelectMaxMinTmpSize](GetSelectMaxMinTmpSize.md) to obtain the size of the space range to be reserved.

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. Supported data types: half and float. |
| U | Data type of the mask tensor mask. Supported data types: bool, int8_t, uint8_t, int16_t, uint16_t, int32_t, and uint32_t. |
| isReuseMask | Whether maskTensor can be modified. The default value is true.<br><br>When set to true, maskTensor may be modified only when the number of last axis elements of maskTensor differs from that of srcTensor; in other scenarios, maskTensor is not modified.<br><br>When set to false, maskTensor is not modified in any scenario, but more temporary space may be required. |

**Table 2**  API parameter description

| Parameter name | Input/Output | Description |
| --- | --- | --- |
| dst | Output | Destination operand.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition values: VECIN/VECCALC/VECOUT. |
| src0(srcTensor)<br><br>src1(srcTensor) | Input | Source operand. **The last axis of the source operand tensor needs 32-byte alignment**.<br><br>Type: [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md). Supported TPosition values: VECIN/VECCALC/VECOUT. |
| src1(srcScalar)<br><br>src0(srcScalar) | Input | Source operand. Type: scalar. |
| mask | Input | Mask tensor. Used to describe how to select values between srcTensor and srcScalar. The last axis of maskTensor needs 32-byte alignment and the element count must be a multiple of 16.<br>**When src0 is srcTensor (tensor type) and src1 is srcScalar (scalar type)**:<br>If the mask value is 0, the corresponding value of srcTensor is selected and placed into dstLocal; otherwise, the value of srcScalar is selected and placed into dstLocal.<br>**When src0 is srcScalar (scalar type) and src1 is srcTensor (tensor type)**:<br>If the mask value is 0, the value of srcScalar is selected and placed into dstLocal; otherwise, the corresponding value of srcTensor is selected and placed into dstLocal. |
| sharedTmpBuffer | Input | Temporary space used by this API for computation. The required space size is obtained from [GetSelectMaxMinTmpSize](GetSelectMaxMinTmpSize.md). |
| info | Input | Describes the shape information of SrcTensor and maskTensor. The type is SelectWithBytesMaskShapeInfo, defined in the following code. The parameters are described as follows:<br>firstAxis: Number of front-axis elements of srcLocal/maskTensor.<br>srcLastAxis: Number of last-axis elements of srcLocal.<br>maskLastAxis: Number of last-axis elements of maskTensor.<br>Note:<br>srcTensor and maskTensor must have the same number of front-axis elements, both equal to firstAxis.<br>firstAxis * srcLastAxis = srcTensor.GetSize() and firstAxis * maskLastAxis = maskTensor.GetSize() must be satisfied.<br>The last axis element count of maskTensor must be greater than or equal to that of srcTensor. The excess part of maskTensor is discarded during computation and does not participate in the computation. |

```cpp
struct SelectWithBytesMaskShapeInfo {
    __aicore__ SelectWithBytesMaskShapeInfo() {};
    uint32_t firstAxis = 0;
    uint32_t srcLastAxis = 0;
    uint32_t maskLastAxis = 0;
};
```

## Return Value

None

## Constraints

- The source operand and the destination operand can be reused.
- For the address alignment requirements of operands, see [General Address Alignment Constraints](../../general_description_and_constraints.md#section796754519912).
- When the number of last-axis elements of maskTensor differs from that of the source operand, the data of maskTensor may be overwritten by the API.

## Examples

For a complete example, see [Select Examples](../../../../../../examples/01_simd_cpp_api/04_advanced_api/07_filter/select).

```
// dstLocal: result data
// srcLocal1: input tensor
// scalar: scalar to be compared
// maskLocal: selection mask
// tmpBuffer: temporary space
// info: shape information of srcLocal and maskLocal
AscendC::SelectWithBytesMaskShapeInfo info;
AscendC::Select(dstLocal, srcLocal1, scalar, maskLocal, tmpBuffer, info);
```

The result is as follows:

```
Input data srcLocal1:
[-84.6    -24.38    30.97   -30.25    22.28   -92.56    90.44   -58.72  -86.56     5.74     6.754  -86.3    -96.7    -37.38   -81.9     46.9
 -99.4     94.2    -41.78   -60.3    -14.43    78.6      8.93   -65.2    79.94   -46.88     4.516   20.03   -25.56    24.73     0.3223  21.98

 -87.4    -93.9     46.22   -69.9     90.8    -24.17   -96.2    -91.    90.44     9.766   68.25   -57.78   -75.44    -8.86   -91.56    21.6
  76.      82.1    -78.     -23.75    92.     -66.44    75.      94.9   2.62   -90.9     15.945   38.16    50.84    96.94   -59.38    44.22  ]
Input data scalar:
[35.6]
Input data maskLocal:
[False  True False False  True  True False  True  True False False  True False  True False  True
 True   False False False  True  True  True  True   True False  True False  True  True  True  True

 False False  True False  True False  True False  True False  True False  True  True  True False
 True False  True False  True False  True  True   True False False False  True False  True  True
]

Output data dstLocal:
[-84.6    35.6    30.97   -30.25   35.6    35.6    90.44   35.6  35.6    5.74    6.754   35.6   -96.7    35.6   -81.9    35.6
  35.6    94.2    -41.78  -60.3    35.6    35.6    35.6    35.6  35.6   -46.88   35.6    20.03   35.6    35.6    35.6    35.6

 -87.4   -93.9    35.6    -69.9    35.6   -24.17   35.6   -91.   35.6   9.766  35.6   -57.78   35.6     35.6    35.6    21.6
  35.6    82.1    35.6    -23.75   35.6   -66.44   35.6    35.6  35.6   -90.9    15.945  38.16   35.6    96.94   35.6    35.6  ]
```
