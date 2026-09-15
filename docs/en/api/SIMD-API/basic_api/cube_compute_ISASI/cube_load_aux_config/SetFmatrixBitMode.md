# SetFcubeBitMode<a name="ZH-CN_TOPIC_0000002517448448"></a>

<!-- md-trans-meta sourceCommit=e5451c7feba11f9e259f4fbc19c4bf82f8c0496c translatedAt=2026-08-27T15:33:04.667Z -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR/Ascend 950DT: Supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 training products/Atlas A3 inference products: Not supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 training products/Atlas A2 inference products: Not supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: Not supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: Not supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: Not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: Not supported
<!-- end id7 -->
## Description<a name="section618mcpsimp"></a>

Used to set the attribute description of **FeatureMap** when calling [LoadData (convolution data movement)](../cube_compute_load/Load3D.md). When the template parameter **isSetFCube** of **Load3D** is set to **false**, the attributes of the **FeatureMap** passed in by **Load3D** (including **l1H**, **l1W**, and **padList**; for parameter introduction, see [Table 3 Parameter description in the LoadData3DParamsV1 structure](../cube_compute_load/Load3D.md#zh-cn_topic_0000002512171652_table679014222918) and [Table 4 Parameter description in the LoadData3DParamsV2 structure](../cube_compute_load/Load3D.md#zh-cn_topic_0000002512171652_table193501032193419)) do not take effect, and developers need to set them through this API.

## Prototype<a name="section620mcpsimp"></a>

```cpp
__aicore__ inline void SetFcube(const SetFCubeBitModeParams& param, const FcubeMode& fcubeMode)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

| Parameter | Input/Output | Meaning |
| --------- | ---------- | ------ |
| fcubeMode | Input | Controls whether the LoadData instruction obtains information from the left or right register. It is of the FcubeMode type, which is defined as follows. Currently, only FCUBE\_LEFT is supported, and both the left and right matrices use this configuration.<br>enum class FcubeMode : uint8_t {<br>    FCUBE_LEFT = 0,<br>    FCUBE_RIGHT = 1,<br>};<br>|
| param | Input | Type is SetFCubeBitMode. For details, see [Table 2](#table85031523118). |

<a name="table85031523118"></a>
**Table 2** SetFCubeBitMode class parameter description

| Parameter | Meaning |
| --------- | --------- |
| config0 | uint64_t type. It forms a union with the SetFCubeBitModeConfig0 bit-field structure type parameter config0BitMode, and is initialized to 0. Its value can be obtained by calling the GetConfig0() function of the class object. |
| config0BitMode | SetFCubeBitModeConfig0 bit-field structure type. For details, see [Table 3](#table1162220101434). It forms a union with config0. |

Design concept of the SetFCubeBitMode class parameters:

A union is a special data structure that allows different data types to be stored at the same memory location. All members of a union share the same memory space, whose size is determined by the largest member, and only one member can be used at a time.

A bit field is a special class member that allows precise control over the number of memory bits occupied by member variables in a structure. The member variables in a structure correspond to the memory from low-order bits to high-order bits in top-to-bottom order.

The SetFCubeBitMode class uses the union and bit-field methods. It expresses parameter types in bits, uses the bit-field structure to automatically process the number of bits of the input parameters, and leverages the union feature to implement multi-parameter fused passing. Only one input parameter needs to be passed to contain all the required information, and the corresponding underlying API only needs to receive one parameter. In addition, when the value of a certain bit in a parameter needs to be modified, this can be achieved through loops and bit operations alone, without re-passing the parameter, which reduces scalar computation and improves performance.

The SetFCubeBitMode class can be initialized directly using an object of the [LoadData3DParamsV2 structure](../cube_compute_load/Load3D.md#zh-cn_topic_0000002512171652_table193501032193419) type:

```cpp
template <typename T>
__aicore__ inline SetFCubeBitModeParams(const LoadData3DParamsV2<T> &loadData3DParams_);
```

You can also use the Set functions of each parameter to modify the parameter values. In addition, because a union is used, you can directly modify **config0** bit by bit to change the parameters.

<a name="table1162220101434"></a>
**Table 3** SetFcubeBitModeConfig0 structure parameter description

| Parameter | Meaning |
| --- | --- |
| l1H | Height of the source operand. Value range: l1H∈[1, 32767]. This parameter is the lowest-order parameter of the bit-field structure, occupying 16 bits. Its value can be set by using the **SetL1H()** function of the **SetFCubeBitMode** class object. |
| l1W | Width of the source operand. Value range: l1W∈[1, 32767]. This parameter is the second lowest-order parameter of the bit-field structure, occupying 16 bits. Its value can be set by using the **SetL1W()** function of the **SetFCubeBitMode** class object. |
| padList0 | Corresponds to the **padding_left** value in the padding list in [Table LoadData3DParamsV2 structure parameter description](../cube_compute_load/Load3D.md#zh-cn_topic_0000002512171652_table193501032193419). Value range: [0, 255]. Default is 0. This parameter is the third lowest-order parameter of the bit-field structure, occupying 8 bits. Its value can be set by using the **SetPadList()** function of the **SetFCubeBitMode** class object. |
| padList1 | Corresponds to the **padding_right** value in the padding list in [Table 1](SetFmatrix.md#table8955841508). Value range: [0, 255]. Default is 0. This parameter is the fourth lowest-order parameter of the bit-field structure, occupying 8 bits. Its value can be set by using the **SetPadList()** function of the **SetFCubeBitMode** class object. |
| padList2 | Corresponds to the **padding_top** value in the padding list in [Table 1](SetFmatrix.md#table8955841508). Value range: [0, 255]. Default is 0. This parameter is the fifth lowest-order parameter of the bit-field structure, occupying 8 bits. Its value can be set by using the **SetPadList()** function of the **SetFCubeBitMode** class object. |
| padList3 | Corresponds to the **padding_bottom** value in the padding list in [Table 1](SetFmatrix.md#table8955841508). Value range: [0, 255]. Default is 0. This parameter is the highest-order parameter of the bit-field structure, occupying 8 bits. Its value can be set by using the **SetPadList()** function of the **SetFCubeBitMode** class object. |

## Return Value

None

## Constraints<a name="section633mcpsimp"></a>

- This API must be used together with [LoadData (convolution data transfer)](../cube_compute_load/Load3D.md), and it must be called before [LoadData (convolution data transfer)](../cube_compute_load/Load3D.md).
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
AscendC::SetFcube(H, W, padList, FcubeMode::FCUBE_LEFT);
/*  
SetFCubeBitModeParams param;
param.SetL1H(H);
param.SetL1W(W);
param.SetPadList(padList);
AscendC::SetFcube(param, FcubeMode::FCUBE_LEFT);
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
