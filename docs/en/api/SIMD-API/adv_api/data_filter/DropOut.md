# DropOut

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-10-26T09:52:45.544Z pushedAt=2026-10-29T02:14:34.391Z nl=2 -->

## Applicable Products

<!-- npu="950" id5 -->
- Ascend 950PR&950DT products: Supported
<!-- end id5 -->
<!-- npu="A3" id6 -->
- Atlas A3 products: Supported
<!-- end id6 -->
<!-- npu="910b" id7 -->
- Atlas A2 products: Supported
<!-- end id7 -->
<!-- npu="310b" id8 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id8 -->
<!-- npu="310p" id9 -->
- Atlas inference products AI Core: Supported
- Vector Core of Atlas inference products: Not supported
<!-- end id9 -->
<!-- npu="910" id10 -->
- Atlas training products: Not supported
<!-- end id10 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/adv_api/data_filter/DropOut_res.md#id1 -->

## Description

Provides the function of filtering SrcTensor (source operand, input tensor) based on MaskTensor to obtain DstTensor (destination operand, output tensor). Only the ND format is supported for the input shape.

This filtering function includes two modes: **byte mode** and **bit mode**.

-   **Byte mode**

    The values stored in MaskTensor are of the Boolean type. Each Boolean value indicates whether to use the value at the corresponding position in SrcTensor: if yes, the value in SrcTensor is selected and stored in DstTensor; otherwise, the corresponding position in DstTensor is assigned zero. DstTensor, SrcTensor, and MaskTensor have the same shape. An example is as follows:

    **SrcTensor=\[1,2,3,4,5,6,7,8,9,10\]**

    **MaskTensor=\[1, 0, 1, 0, 1, 0, 0, 1, 1, 0\] (the data type of each number is uint8\_t)**

    **DstTensor=\[1,0,3,0,5,0,0,8,9,0\]**

-   **Bit pattern**

    Each bit value of MaskTensor indicates whether to use the value at the corresponding position in SrcTensor: if yes, the value in SrcTensor is selected and stored in DstTensor; otherwise, the corresponding position in DstTensor is assigned zero. SrcTensor and DstTensor have the same shape, both assumed to be \[height, width\], and the shape of MaskTensor is \[height, \(width / 8\)\]. An example is as follows:

    **SrcTensor=\[1,2,3,4,5,6,7,8\]**

    **MaskTensor=\[169\] (converted to binary representation as 1010 1001)**

    **DstTensor=\[1,0,3,0,5,0,0,8\]**

    -   Special case 1: When the valid data of MaskTensor is stored non-contiguously, the width axis of MaskTensor needs to be padded with invalid values to satisfy 32B alignment, and the width axis of SrcTensor needs to satisfy 32-byte alignment. An example is as follows:

        SrcTensor=\[1,2,3,4,5,6,7,8,11,12,13,14,15,16,17,18\]

        MaskTensor=\[1, 0, 1, 0, 1, 0, 0, 1, X, X, 1, 0, 1, 0, 1, 0, 0, 1, X, X\] (X is an invalid value. It is assumed that the data already meets the alignment requirement, and the example values are in binary form.)

        DstTensor=\[1,0,3,0,5,0,0,8,11,0, 13, 0, 15, 0, 0,18\]

    -   Special case 2: When the valid data of MaskTensor is stored contiguously and maskTensor\_size does not meet the 32B alignment requirement, invalid data must be padded at the tail of MaskTensor to achieve 32B alignment, and the tail of SrcTensor must also be padded with invalid data so that srcTensor\_size meets the 32B alignment requirement. The example is as follows:

        SrcTensor=\[1,2,3,4,5,6,7,8,11,12,13,14,15,16,17,18\]

        MaskTensor=\[1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, X, X, X, X\] (X is an invalid value. It is assumed that the data already meets the alignment requirement, and the example values are in binary form.)

        DstTensor= \[1, 0, 3, 0, 5, 0, 0, 8, 11, 0, 13, 0, 15, 0, 0, 18\]

## Implementation Principles

The following figure shows the internal algorithm block diagram of the Dropout high-order API, using a float SrcTensor in ND format with shape \[srcM, srcN\], a MaskTensor with shape \[maskM, maskN\], and the bit pattern scenario as an example.

**Figure 1**  Dropout algorithm block diagram  
![](../../../figures/dropout_algorithm_diagram.png "Dropout algorithm block diagram")

The computation process consists of the following steps, all performed on the Vector:

1.  GatherMask step: Clean the dirty data in the input MaskTensor so that only valid data is retained in the MaskTensor.
2.  Select step: Select data from the SrcTensor based on the input MaskTensor. The original data is retained at the selected data locations, and 0 is set at the discarded data locations.
3.  Muls step: Divide each element of the output data by keepProb.

**Figure 2**  Dropout algorithm block diagram  
![](../../../figures/dropout_algorithm_diagram_1.png "Dropout algorithm block diagram-55")

The computation process is performed on the Vector, looping srcM times, and the following operation is performed on srcN elements each time:

1.  Select step: Select data from the SrcTensor based on the input MaskTensor. The original data is retained at the selected data locations, and 0 is set at the discarded data locations.
2.  Muls step: Divide each element of the output data by keepProb.

## Prototype

```
template <typename T, bool isInitBitMode = false, uint32_t dropOutMode = 0>
__aicore__ inline void DropOut(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& maskLocal, const float keepProb, const DropOutShapeInfo& info)
```

```
template <typename T, bool isInitBitMode = false, uint32_t dropOutMode = 0>
__aicore__ inline void DropOut(const LocalTensor<T>& dstLocal, const LocalTensor<T>& srcLocal, const LocalTensor<uint8_t>& maskLocal, const LocalTensor<uint8_t>& sharedTmpBuffer, const float keepProb, const DropOutShapeInfo& info)
```

## Parameters

**Table 1**  Template parameter description

| Parameter | Description |
| --- | --- |
| T | Data type of the operand. For the data types supported by different products, see [supported data types](#li15391852101816). |
| isInitBitMode | In bit pattern, whether the API needs to initialize internally (defaults to false). |
| dropOutMode | Selects which input scenario to execute:<br><br>0: Default value. The API infers the running mode based on the input shape. Note that for scenarios where the inference does not meet expectations, you need to set the corresponding mode.<br><br>1: Executes byte mode, and maskLocal contains dirty data.<br><br>2: Executes byte mode, and maskLocal does not contain dirty data.<br><br>3: Executes bit pattern, and maskLocal does not contain dirty data.<br><br>4: Executes bit pattern, and maskLocal contains dirty data. |

**Table 2**  API parameter description

| Parameter name | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| srcLocal | Input | Source operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT.<br><br>The data type of srcLocal must be consistent with that of the destination operand. |
| maskLocal | Input | Tensor that stores the mask, with the data type of uint8_t.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Shared buffer used to store temporary data generated by internal computation of the API. With this method, you can manage the sharedTmpBuffer memory space by yourself and reuse this memory after the API call is complete. The tensor size must meet the corresponding tiling requirements and be used together with tiling. For how to obtain the shared buffer size BufferSize, see [GetDropOutMaxMinTmpSize](GetDropOutMaxMinTmpSize.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition values are VECIN/VECCALC/VECOUT. |
| keepProb | Input | Weight coefficient, with the data type of float. It is the probability that the data in srcLocal is retained. The filtered result is divided by the weight coefficient and stored in dstLocal.<br><br>keepProb∈(0, 1) |
| info | Input | DropOutShapeInfo type. The DropOutShapeInfo structure is defined in the following code, where the parameters mean:<br>firstAxis: number of height axis elements of srcLocal/maskTensor.<br>srcLastAxis: number of width axis elements of srcLocal.<br>maskLastAxis: number of width axis elements of maskTensor (if there is a data padding scenario, it is the length containing dirty data. Note that the number of elements in all modes is the number under the corresponding tensor type, and the value must be greater than 0. For example, a uint8 tensor corresponds to the number of Uint8 type elements). |

```
struct DropOutShapeInfo {
    __aicore__ DropOutShapeInfo() {};
    uint32_t firstAxis = 0;
    uint32_t srcLastAxis = 0;
    uint32_t maskLastAxis = 0;
};
```

## Return Value

None

## Constraints

-   The tensor space of srcTensor and dstTensor can be reused.
-   For the address alignment requirements of srcLocal and dstLocal, see [General Description and Constraints](../../general_description_and_constraints.md).
-   Only the ND format is supported for the input shape.
-   In the scenario where maskLocal contains dirty data, the number of valid values in info.maskLastAxis must be an integer multiple of 2.
-   In the scenario where maskLocal contains dirty data, the data in maskLocal may be modified, and the dirty data may be discarded.
-   Supported data types<a id="li15391852101816"></a>

    <!-- npu="950" id11 -->
    Ascend 950PR&950DT products, the supported data types are: half, bfloat16\_t, float.
    <!-- end id11 -->
    <!-- npu="A3" id12 -->
    Atlas A3 products: Supported data types: half, float.
    <!-- end id12 -->
    <!-- npu="910b" id13 -->
    Atlas A2 products: Supported data types: half, float.
    <!-- end id13 -->
    <!-- npu="310p" id14 -->
    Atlas inference products AI Core: Supported data types: half, float.
    <!-- end id14 -->

## Examples<a name="section642mcpsimp"></a>

```
// yLocal: DropOut result.
// xLocal: Input data.
// maskLocal: Filter mask.
// sharedTmpBuffer: Temporary space.
// probValue: Probability that the data in srcLocal is retained. The filtered result is divided by the weight coefficient and stored in dstLocal.
// info: DropOutShapeInfo type.
AscendC::DropOutShapeInfo info;
float probValue = 0.5;
info.firstAxis = tilingData.firstAxis / tilingData.tileNum;
info.srcLastAxis = tileLength;
info.maskLastAxis = tileLength;
AscendC::DropOut(yLocal, xLocal, maskLocal, sharedTmpBuffer, probValue, info);
```

The example result is as follows:

```
Input data (xLocal):
[  0.  1.  2.  3.  4.  5.  6.  7.  8.  9. 10. 11. 12. 13. 14. 15.
  16. 17. 18. 19. 20. 21. 22. 23. 24. 25. 26. 27. 28. 29. 30. 31. ]
Input data (maskLocal):
[  0   1   0   0   0   0   0   0   1   0   1   1   1   0   1   1
   0   0   1   0   0   1   1   1   1   0   1   0   1   1   1   0 ]
Input data(probValue): 0.5
Output data(yLocal):
[  0.  2.  0.  0.  0.  0.  0.  0. 16.  0. 20. 22. 24.  0. 28. 30.
   0.  0. 36.  0.  0. 42. 44. 46. 48.  0. 52.  0. 56. 58. 60.  0. ]
```
