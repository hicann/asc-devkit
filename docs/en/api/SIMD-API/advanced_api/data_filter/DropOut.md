# DropOut

<!-- md-trans-meta sourceCommit=0364dcce9ab16d714270f168ec9546fd7620f90f translatedAt=2026-09-08T17:38:14.868Z -->

## Applicable Products

- Ascend 950PR/Ascend 950DT: Supported
- Atlas A3 training products/Atlas A3 inference products: Supported
- Atlas A2 training products/Atlas A2 inference products: Supported
- Atlas 200I/500 A2 inference products: Not supported
- Atlas inference products AI Core: Supported
- Atlas inference products Vector Core: Not supported
- Atlas training products: Not supported

## Description

Provides the function of filtering **SrcTensor** (source operand, input tensor) based on **MaskTensor** to obtain **DstTensor** (destination operand, output tensor). Only ND input shapes are supported.

The filtering function includes two modes: **byte mode** and **bit mode**.

-   **Byte Mode**

    The values stored in **MaskTensor** are of the Boolean type. Each Boolean value indicates whether to use the value at the corresponding position of **SrcTensor**: if yes, the value in **SrcTensor** is selected and stored into **DstTensor**; otherwise, the corresponding position in **DstTensor** is assigned zero. **DstTensor**, **SrcTensor**, and **MaskTensor** have the same shape. An example is as follows:

    **SrcTensor=\[1, 2, 3, 4, 5, 6, 7, 8, 9, 10\]**

    **MaskTensor=\[1, 0, 1, 0, 1, 0, 0, 1, 1, 0\] (the data type of each number is uint8\_t)**

    **DstTensor=\[1, 0, 3, 0, 5, 0, 0, 8, 9, 0\]**

-   **Bit Mode**

    Each bit value of **MaskTensor** indicates whether to use the value at the corresponding position of **SrcTensor**: if yes, the value in **SrcTensor** is selected and stored into **DstTensor**; otherwise, the corresponding position in **DstTensor** is assigned zero. **SrcTensor** and **DstTensor** have the same shape, both assumed to be \[height, width\], and the shape of **MaskTensor** is \[height, \(width / 8\)\]. An example is as follows:

    **SrcTensor=\[1, 2, 3, 4, 5, 6, 7, 8\]**

    **MaskTensor=\[169\] (converted to binary representation 1010 1001)**

    **DstTensor=\[1, 0, 3, 0, 5, 0, 0, 8\]**

    -   Special case 1: When the valid data of **MaskTensor** is stored non-contiguously, the width axis of **MaskTensor** needs to be padded with invalid values to satisfy 32B alignment, and the width axis of **SrcTensor** needs to satisfy 32-byte alignment. An example is as follows:

        SrcTensor=\[1, 2, 3, 4, 5, 6, 7, 8, 11, 12, 13, 14, 15, 16, 17, 18\]

        MaskTensor=\[1, 0, 1, 0, 1, 0, 0, 1, X, X, 1, 0, 1, 0, 1, 0, 0, 1, X, X\] (X is an invalid value. It is assumed that the data already meets the alignment requirements, and the example values are in binary form.)

        DstTensor=\[1, 0, 3, 0, 5, 0, 0, 8, 11, 0, 13, 0, 15, 0,  0, 18\]

    -   Special case 2: When the valid data of **MaskTensor** is stored contiguously and maskTensor\_size does not meet the 32B alignment requirement, invalid data must be padded at the tail of **MaskTensor** to achieve 32B alignment, and correspondingly invalid data must also be padded at the tail of **SrcTensor** so that srcTensor\_size meets the 32B alignment requirement. The example is as follows:

        SrcTensor=\[1, 2, 3, 4, 5, 6, 7, 8, 11, 12, 13, 14, 15, 16, 17, 18\]

        MaskTensor=\[1, 0, 1, 0, 1, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, X, X, X, X\] (X is an invalid value. It is assumed that the data already meets the alignment requirements, and the example values are in binary form.)

        DstTensor= \[1, 0, 3, 0, 5, 0, 0, 8, 11, 0, 13, 0, 15, 0, 0, 18\]

## Implementation Principle

Taking the bit mode scenario as an example, where **SrcTensor** is of the float type in ND format with shape \[srcM, srcN\] and **MaskTensor** has shape \[maskM, maskN\], the internal algorithm block diagram of the Dropout high-order API is described as shown in the following figure.

**Figure 1**  Dropout algorithm block diagram  
![](../../../figures/Dropout_algorithm_block_diagram.png "Dropout algorithm block diagram")

The computation process consists of the following steps, all performed on the vector:

1.  GatherMask step: Clean the dirty data of the input **MaskTensor** so that only valid data is retained in **MaskTensor**;
2.  Select step: Select data from **SrcTensor** based on the input **MaskTensor**. The original data is retained at the selected data locations, and the discarded data locations are set to 0;
3.  Muls step: Divide each element of the output data by **keepProb**.

**Figure 2**  Dropout algorithm block diagram  
![](../../../figures/Dropout_algorithm_block_diagram-55.png "Dropout algorithm block diagram-55")

The computation is performed on the Vector, looping srcM times, and each time the following operations are performed on srcN elements:

1.  Select step: Selects data from SrcTensor based on the input MaskTensor. The original data is retained at the selected data locations, and 0 is set at the discarded data locations.
2.  Muls step: Divides each element of the output data by keepProb.

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
| T | Data type of the operand. For the data types supported by different models, see [Supported Data Types](#li15391852101816). |
| isInitBitMode | In bit mode, whether the API needs to initialize internally (defaults to **false**). |
| dropOutMode | Selects the input scenario to execute:<br><br>**0**: Default value. The API infers the running mode based on the input shape. Note that if the inference does not meet expectations, the corresponding mode must be set.<br><br>**1**: Executes byte mode, and maskLocal contains dirty data.<br><br>**2**: Executes byte mode, and maskLocal does not contain dirty data.<br><br>**3**: Executes bit mode, and maskLocal does not contain dirty data.<br><br>**4**: Executes bit mode, and maskLocal contains dirty data. |

**Table 2**  API parameter description

| Parameter | Input/Output | Description |
| --- | --- | --- |
| dstLocal | Output | Destination operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| srcLocal | Input | Source operand.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT.<br><br>The data type of srcLocal must be consistent with that of the destination operand. |
| maskLocal | Input | Tensor that stores the mask, with the data type of uint8_t.<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| sharedTmpBuffer | Input | Shared buffer used to store temporary data generated by internal computation of the API. In this way, you can manage the sharedTmpBuffer memory space and reuse this part of memory after the API call is completed. The Tensor size must meet the requirements of the corresponding tiling and be used together with tiling. For how to obtain the shared buffer size **BufferSize**, see [GetDropOutMaxMinTmpSize](GetDropOutMaxMinTmpSize.md).<br><br>The type is [LocalTensor](../../basic_api/data_structures/LocalTensor/LocalTensor.md), and the supported TPosition is VECIN/VECCALC/VECOUT. |
| keepProb | Input | Weight coefficient, with the data type of float. It is the probability that the data in srcLocal is retained. The filtered result is divided by the weight coefficient and stored in dstLocal.<br><br>keepProb∈(0, 1) |
| info | Input | DropOutShapeInfo type. The DropOutShapeInfo structure is defined in the following code, where the parameters mean:<br>firstAxis: Number of height-axis elements of srcLocal/maskTensor.<br>srcLastAxis: Number of width-axis elements of srcLocal.<br>maskLastAxis: Number of width-axis elements of maskTensor (if there is a data padding scenario, it is the length with dirty data. Note that the number of elements in all modes is the number under the corresponding Tensor type, and the value must be greater than 0. For example, a uint8 Tensor corresponds to the number of uint8 elements). |

```
struct DropOutShapeInfo {
    __aicore__ DropOutShapeInfo(){};
    uint32_t firstAxis = 0; 
    uint32_t srcLastAxis = 0; 
    uint32_t maskLastAxis = 0; 
};
```

## Return Value

None

## Constraints

-   The Tensor spaces of srcTensor and dstTensor can be reused.
-   For the address alignment requirements of srcLocal and dstLocal, see [Common Instructions and Constraints](../../general_description_and_constraints.md).
-   Only the input shape in ND format is supported.
-   In the scenario where maskLocal contains dirty data, the number of valid values in info.maskLastAxis must be an integer multiple of 2.
-   In the scenario where maskLocal contains dirty data, the data in maskLocal may be modified, and the dirty data may be discarded.
-   Supported Data Types<a id="li15391852101816"></a>

    Ascend 950PR/Ascend 950DT, the supported data types are: half, bfloat16\_t, float.

    Atlas A3 training products/Atlas A3 inference products, the supported data types are: half, float.

    Atlas A2 training products/Atlas A2 inference products, the supported data types are: half, float.

    Atlas inference products AI Core, the supported data types are: half, float.

## Examples

```
// yLocal: DropOut result.
// xLocal: Input data.
// maskLocal: Filter mask.
// sharedTmpBuffer: Temporary space.
// probValue: Probability that data in srcLocal is retained. The filtered result is divided by the weight coefficient and stored in dstLocal.
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
Input data (probValue): 0.5
Output data (yLocal):
[  0.  2.  0.  0.  0.  0.  0.  0. 16.  0. 20. 22. 24.  0. 28. 30.
   0.  0. 36.  0.  0. 42. 44. 46. 48.  0. 52.  0. 56. 58. 60.  0. ]
```
