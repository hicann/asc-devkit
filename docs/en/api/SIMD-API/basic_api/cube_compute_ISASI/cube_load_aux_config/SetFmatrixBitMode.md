# SetFmatrixBitMode<a name="ZH-CN_TOPIC_0000002517448448"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:55:24.983Z pushedAt=2026-10-26T07:41:44.142Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_load_aux_config/SetFmatrixBitMode_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Used to set the attribute description of **FeatureMap** when calling [LoadData (convolution data movement)](../cube_compute_load/LoadData_3D.md). When the template parameter **isSetFMatrix** of **LoadData** (convolution data movement) is set to **false**, the attributes of the **FeatureMap** passed to **LoadData** (convolution data movement) (including **l1H**, **l1W**, and **padList**; for parameter introduction, see [Table 3 LoadData3DParamsV1 parameter description in the structure](../cube_compute_load/LoadData_3D.md#zh-cn_topic_0000002512171652_table679014222918) and [Table 4 LoadData3DParamsV2 parameter description in the structure](../cube_compute_load/LoadData_3D.md#zh-cn_topic_0000002512171652_table193501032193419)) do not take effect, and developers need to set these attributes through this API.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFmatrix(const SetFMatrixBitModeParams& param, const FmatrixMode& fmatrixMode)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

| Parameter | Input/Output | Description |
| --------- | ---------- | ------ |
| **fmatrixMode** | Input | Controls whether the LoadData instruction obtains information from the left or right register. Type **FmatrixMode**, defined as follows. Currently only FMATRIX\_LEFT is supported, and both the left and right matrices use this configuration.<br>enum class FmatrixMode : uint8_t {<br>    FMATRIX_LEFT = 0,<br>    FMATRIX_RIGHT = 1,<br>}; |
| **param** | Input | Type **SetFMatrixBitMode**. For details, see [Table 2](#table85031523118). |

<a name="table85031523118"></a>
**Table 2** SetFMatrixBitMode class parameter description

| Parameter | Description |
| --------- | --------- |
| **config0** | Type **uint64_t**. Forms a union with the **SetFMatrixBitModeConfig0** bit-field structure type parameter **config0BitMode**. Initialized to 0. Its value can be obtained using the **GetConfig0()** function of the class object. |
| **config0BitMode** | Type **SetFMatrixBitModeConfig0** bit-field structure. For details, see [Table 3](#table1162220101434). Forms a union with **config0**. |

Description of the design concept of the **SetFMatrixBitMode** class parameters:

A union is a special data structure that allows different data types to be stored at the same memory location. All members of a union share the same memory space, whose size is determined by the largest member. Only one member can be used at a time.

A bit-field is a special class member that allows precise control over the number of memory bits occupied by a member variable in a structure. Member variables in a structure correspond to memory bits from low order to high order from top to bottom.

The **SetFMatrixBitMode** class uses the union and bit-field methods. It expresses parameter types using bits, uses the bit-field structure to automatically handle the number of bits of the input parameter, and leverages the characteristics of the union to implement fused transfer of multiple parameters. Passing only one input parameter is sufficient to contain all required information, and the corresponding underlying API needs to receive only one parameter. In addition, when the value of a certain bit in a parameter needs to be modified, this can be achieved only through loops and bit operations without re-passing the parameter, which reduces scalar computation and improves performance.

The **SetFMatrixBitMode** class can be directly initialized using an object of the [LoadData3DParamsV2 structure](../cube_compute_load/LoadData_3D.md#zh-cn_topic_0000002512171652_table193501032193419) type:

```cpp
template <typename T>
__aicore__ inline SetFMatrixBitModeParams(const LoadData3DParamsV2<T> &loadData3DParams_);
```

You can also use the Set functions of each parameter to modify parameter values. In addition, because a union is used, you can directly modify **config0** bit by bit to change the parameters.

<a name="table1162220101434"></a>
**Table 3** SetFMatrixBitModeConfig0 structure parameter description

| Parameter | Meaning |
| --- | --- |
| l1H | Source operand height. Value range: l1H∈[1, 32767]. This parameter is the lowest-order parameter of the bit-field structure, occupying 16 bits. Its value can be set using the **SetL1H()** function of the **SetFMatrixBitMode** class object. |
| l1W | Source operand width. Value range: l1W∈[1, 32767]. This parameter is the second lowest-order parameter of the bit-field structure, occupying 16 bits. Its value can be set using the **SetL1W()** function of the **SetFMatrixBitMode** class object. |
| padList0 | Corresponds to the padding_left value in the padding list in [Table of parameter description in the LoadData3DParamsV2 structure](../cube_compute_load/LoadData_3D.md#zh-cn_topic_0000002512171652_table193501032193419). Value range: [0,255]. Default is 0. This parameter is the third lowest-order parameter of the bit-field structure, occupying 8 bits. Its value can be set using the **SetPadList()** function of the **SetFMatrixBitMode** class object. |
| padList1 | Corresponds to the padding_right value in the padding list in [Table 1](SetFmatrix.md#table8955841508). Value range: [0,255]. Default is 0. This parameter is the fourth lowest-order parameter of the bit-field structure, occupying 8 bits. Its value can be set using the **SetPadList()** function of the **SetFMatrixBitMode** class object. |
| padList2 | Corresponds to the padding_top value in the padding list in [Table 1](SetFmatrix.md#table8955841508). Value range: [0,255]. Default is 0. This parameter is the fifth lowest-order parameter of the bit-field structure, occupying 8 bits. Its value can be set using the **SetPadList()** function of the **SetFMatrixBitMode** class object. |
| padList3 | Corresponds to the padding_bottom value in the padding list in [Table 1](SetFmatrix.md#table8955841508). Value range: [0,255]. Default is 0. This parameter is the highest-order parameter of the bit-field structure, occupying 8 bits. Its value can be set using the **SetPadList()** function of the **SetFMatrixBitMode** class object. |

## Return Value

None

## Constraints<a name="section633mcpsimp"></a>

- This API must be used together with [LoadData (convolution data movement)](../cube_compute_load/LoadData_3D.md), and it must be called before [LoadData (convolution data movement)](../cube_compute_load/LoadData_3D.md).
- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Example<a name="section642mcpsimp"></a>

```cpp
AscendC::TPipe pipe;

AscendC::TQue<AscendC::TPosition::A1, 1> inQueueFmA1;
AscendC::TQue<AscendC::TPosition::A2, 1> inQueueFmA2;
// weight queue
AscendC::TQue<AscendC::TPosition::B1, 1> inQueueWeB1;
AscendC::TQue<AscendC::TPosition::B2, 1> inQueueWeB2;
pipe.InitBuffer(inQueueFmA1, 1, featureMapA1Size * sizeof(fmap_T));
pipe.InitBuffer(inQueueFmA2, 1, featureMapA2Size * sizeof(fmap_T));
pipe.InitBuffer(inQueueWeB1, 1, weightA1Size * sizeof(weight_T));
pipe.InitBuffer(inQueueWeB2, 1, weightB2Size * sizeof(weight_T));
pipe.InitBuffer(outQueueCO1, 1, dstCO1Size * sizeof(dstCO1_T));

AscendC::LocalTensor<fmap_T> featureMapA1 = inQueueFmA1.DeQue<fmap_T>();
AscendC::LocalTensor<weight_T> weightB1 = inQueueWeB1.DeQue<weight_T>();
AscendC::LocalTensor<fmap_T> featureMapA2 = inQueueFmA2.AllocTensor<fmap_T>();
AscendC::LocalTensor<weight_T> weightB2 = inQueueWeB2.AllocTensor<weight_T>();
uint16_t channelSize = 32;
uint16_t H = 4, W = 4;
uint8_t Kh = 2, Kw = 2;
uint16_t Cout = 16;
uint16_t C0, C1;
uint8_t dilationH = 2, dilationW = 2;

uint8_t padList[PAD_SIZE] = {0, 0, 0, 0};
AscendC::SetFmatrix(H, W, padList, FmatrixMode::FMATRIX_LEFT);
/*  
SetFMatrixBitModeParams param;
param.SetL1H(H);
param.SetL1W(W);
param.SetPadList(padList);
AscendC::SetFmatrix(param, FmatrixMode::FMATRIX_LEFT);
*/ 
AscendC::SetLoadDataPaddingValue(0);
AscendC::SetLoadDataRepeat({0, 1, 0});
AscendC::SetLoadDataBoundary((uint32_t)0);
static constexpr AscendC::IsResetLoad3dConfig LOAD3D_CONFIG = {false,false};
AscendC::LoadData<fmap_T, LOAD3D_CONFIG>(featureMapA2, featureMapA1,
    { padList, H, W, channelSize, k, howoRound, 0, 0, 1, 1, Kw, Kh, dilationW, dilationH, false, false, 0 });
AscendC::LoadData(weightB2, weightB1, { 0, weRepeat, 1, 0, 0, false, 0 });

inQueueFmA2.EnQue<fmap_T>(featureMapA2);
inQueueWeB2.EnQue<weight_T>(weightB2);
inQueueFmA1.FreeTensor(featureMapA1);
inQueueWeB1.FreeTensor(weightB1);
```
