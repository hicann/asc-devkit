# LoadData (BitMode Convolution Data Movement)<a id="ZH-CN_TOPIC_0000002517284408"></a>

<!-- md-trans-meta sourceCommit=e9bec8b34ada6c37c20391d212da5ebf088e2e8b translatedAt=2026-08-27T16:32:48.571Z -->

## Applicable Products<a id="section1550532418810"></a>

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
## Description<a id="section618mcpsimp"></a>

**Load3DBitMode** performs the image-to-column operation, converting a multi-dimensional feature map into a two-dimensional cube. The supported data paths are L1 Buffer -> L0A Buffer and L1 Buffer -> L0B Buffer.

This API differs from the **Load3D** API in the way parameters are passed. This API passes a union structure **Load3DBitModeParam**.

## Prototype<a id="section620mcpsimp"></a>

```cpp
template <TPosition Dst, TPosition Src, typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const Load3DBitModeParam& loadDataParams)
```

## Parameters<a id="section622mcpsimp"></a>

**Table 1** Template parameter description<a name="table07381635103112"></a>

| Parameter | Description |
| ---------- | ---------- |
| **T** | Data type of the source operand and destination operand. |
| **Src** | Logical position (**TPosition**) where the source operand is stored. Used only by the **Load3DBitMode** API. |
| **Dst** | Logical position (**TPosition**) where the destination operand is stored. Used only by the **Load3DBitMode** API. |

**Table 2** Common parameter description<a name="table18368155193919"></a>

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| **dst** | Output | Destination operand, whose type is **LocalTensor**.<br>The continuous data arrangement order is determined by the **TPosition** where the destination operand resides. The specific constraints are as follows:<br>&nbsp;&nbsp;&bull; **A2**: ZZ format/NZ format;<br>&nbsp;&nbsp;&bull; **B2**: ZN format;<br>&nbsp;&nbsp;&bull; **A1**/**B1**: No format requirement; NZ format in general. |
| **src** | Input | Source operand, whose type is **LocalTensor**.<br>The data type must be consistent with **dst**. |
| **loadDataParams** | Input | **LoadData** parameter structure, whose type is:<br>&nbsp;&nbsp;&bull; **Load3DBitModeParam**. For details, see [Table 3](#table106611666584).<br>For the definition of the preceding structure parameters, see \$\{INSTALL\_DIR\}/include/ascendc/basic\_api/API/kernel\_struct\_mm.h. Replace \$\{INSTALL\_DIR\} with the file storage path after the CANN software is installed. |

**Table 3** Load3DBitModeParam class parameter description<a name="table106611666584"></a>

| Parameter | Description |
| ---------- | ---------- |
| **config0** | Of type **uint64_t**. It forms a union with **config0BitMode**, a parameter of the **Load3DBitModeConfig0** bit-field structure type. It is initialized to 0, and its value can be obtained by using the **GetConfig0()** function of the class object. |
| **config0BitMode** | Of the **Load3DBitModeConfig0** bit-field structure type. For the parameters, see [Table 4](#table184321224173). It forms a union with **config0**. |
| **config1** | Of type **uint64_t**. It forms a union with **config1BitMode**, a parameter of the **Load3DBitModeConfig1** bit-field structure type. It is initialized to 0, and its value can be obtained by using the **GetConfig1()** function of the class object. |
| **config1BitMode** | Of the **Load3DBitModeConfig1** bit-field structure type. For the parameters, see [Table 5](#table87491086815). It forms a union with **config1**. |

Design concept of the **Load3DBitModeParam** class parameters:

A union is a special data structure that allows different data types to be stored at the same memory location. All members of a union share the same memory space, whose size is determined by the largest member, and only one member can be used at a time.

A bit-field is a special class member that allows precise control over the number of memory bits occupied by a member variable in a structure. Member variables in a structure correspond to memory from low-order bits to high-order bits from top to bottom.

The **Load3DBitModeParam** class uses union and bit-field methods to express parameter types in bits, uses a bit-field structure to automatically handle the number of bits of the input parameters, and leverages the characteristics of union to implement multi-parameter fused passing, so that only one input parameter needs to be passed to contain all required information, and the corresponding underlying API only needs to receive one parameter. In addition, when the value of a certain bit in a parameter needs to be modified, it can be achieved only through loops and bit operations, without the need to pass the parameter again.

Compared with the **Load3D** API that uses the **LoadData3DParamsV2** structure, scalar computation is reduced, achieving performance improvement.

The **Load3DBitModeParam** class can be initialized directly with an object of the **LoadData3DParamsV2** structure type:

```cpp
template <typename T>
__aicore__ inline Load3DBitModeParam(const LoadData3DParamsV2<T> &loadData3DParams_);
```

You can also use the Set functions of each parameter to modify parameter values. In addition, because a union is used, **config0** and **config1** can also be directly modified bit by bit to modify the parameters.

**Table 4** Parameter description of the Load3DBitModeConfig0 structure<a name="table184321224173"></a>

| Parameter | Description |
| ---------- | ---------- |
| kStep | Transfer length of this instruction in the width dimension of the destination operand. If the rightmost fractal is not covered, it must be a multiple of 16 for the half type and a multiple of 32 for int8_t/uint8_t; if covered, there is no multiple requirement. Value range: kStep∈[1, 65535].<br>(Same meaning as kExtension in [LoadData (convolution data movement)](Load3D.md)).<br>This parameter is the lowest-order parameter of the bit-field structure, occupying 16 bits. You can use the **SetKExtension()** function of the **Load3DBitModeParam** class object to set its value, and use the **GetKExtension()** function to obtain its value. |
| mStep | Transfer length of this instruction in the height dimension of the destination operand. If the lowermost fractal is not covered, it must be a multiple of 16 for half/int8_t/uint8_t; if covered, there is no multiple requirement. Value range: mStep∈[1, 65535].<br>(Same meaning as mExtension in [LoadData (convolution data movement)](Load3D.md)).<br>This parameter is the second lowest-order parameter of the bit-field structure, occupying 16 bits. You can use the **SetMExtension()** function of the **Load3DBitModeParam** class object to set its value, and use the **GetMExtension()** function to obtain its value. |
| kPos | Starting point of this instruction in the width dimension of the destination operand. It must be a multiple of 16 for the half type and a multiple of 32 for int8_t/uint8_t. Value range [0, 65535]. Default is 0.<br>(Same meaning as kStartPt in [LoadData (convolution data movement)](Load3D.md)).<br>This parameter is the third lowest-order parameter of the bit-field structure, occupying 16 bits. You can use the **SetKStartPt()** function of the **Load3DBitModeParam** class object to set its value, and use the **GetKStartPt()** function to obtain its value. |
| mPos | Starting point of this instruction in the height dimension of the destination operand. Value range [0, 32767]. Default is 0.<br>(Same meaning as mStartPt in [LoadData (convolution data movement)](Load3D.md)).<br>This parameter is the highest-order parameter of the bit-field structure, occupying 16 bits. You can use the **SetMStartPt()** function of the **Load3DBitModeParam** class object to set its value, and use the **GetMStartPt()** function to obtain its value. |

**Table 5** Parameter description of the Load3DBitModeConfig1 structure<a name="table87491086815"></a>

| Parameter | Description |
| ---------- | ---------- |
| strideW | Step by which the convolution kernel slides in the width dimension of the source operand. Value range: strideW∈[1, 63].<br>(Same meaning as strideW in [LoadData (convolution data movement)](Load3D.md)).<br>This parameter is the lowest-order parameter of the bit-field structure, occupying 6 bits. You can use the **SetStrideW()** function of the **Load3DBitModeParam** class object to set its value, and use the **GetStrideW()** function to obtain its value. |
| strideH | Step by which the convolution kernel slides in the height dimension of the source operand. Value range: strideH∈[1, 63].<br>(Same meaning as strideH in [LoadData (convolution data movement)](Load3D.md)).<br>This parameter is the second lowest-order parameter of the bit-field structure, occupying 6 bits. You can use the **SetStrideH()** function of the **Load3DBitModeParam** class object to set its value, and use the **GetStrideH()** function to obtain its value. |
| Wk | Width of the convolution kernel. Value range: Wk∈[1, 255].<br>(Same meaning as filterW in [LoadData (convolution data movement)](Load3D.md)).<br>This parameter is the third lowest-order parameter of the bit-field structure, occupying 8 bits. You can use the **SetFilterW()** function of the **Load3DBitModeParam** class object to set its value, and use the **GetFilterW()** function to obtain its value. |
| Hk | Height of the convolution kernel. Value range: Hk∈[1, 255].<br>(Same meaning as filterH in [LoadData (convolution data movement)](Load3D.md)).<br>This parameter is the fourth lowest-order parameter of the bit-field structure, occupying 8 bits. You can use the **SetFilterH()** function of the **Load3DBitModeParam** class object to set its value, and use the **GetFilterH()** function to obtain its value. |
| dilationW | Dilation factor of the convolution kernel width. Value range: dilationW∈[1, 255].<br>(Same meaning as dilationFilterW in [LoadData (convolution data movement)](Load3D.md)).<br>This parameter is the fifth lowest-order parameter of the bit-field structure, occupying 8 bits. You can use the **SetDilationFilterW()** function of the **Load3DBitModeParam** class object to set its value, and use the **GetDilationFilterW()** function to obtain its value. |
| dilationH | Dilation factor of the convolution kernel height. Value range: dilationH∈[1, 255].<br>(Same meaning as dilationFilterH in [LoadData (convolution data movement)](Load3D.md)).<br>This parameter is the sixth lowest-order parameter of the bit-field structure, occupying 8 bits. You can use the **SetDilationFilterH()** function of the **Load3DBitModeParam** class object to set its value, and use the **GetDilationFilterH()** function to obtain its value. |
| filterW | Whether to increase the convolution kernel width by 256 elements on the basis of filterW. true: increase; false: do not increase.<br>(Same meaning as filterSizeW in [LoadData (convolution data movement)](Load3D.md)).<br>This parameter is the seventh lowest-order parameter of the bit-field structure, occupying 1 bit. You can use the **SetFilterSizeW()** function of the **Load3DBitModeParam** class object to set its value, and use the **GetFilterSizeW()** function to obtain its value. |
| filterH | Whether to increase the convolution kernel height by 256 elements on the basis of filterH. true: increase; false: do not increase.<br>(Same meaning as filterSizeH in [LoadData (convolution data movement)](Load3D.md)).<br>This parameter is the eighth lowest-order parameter of the bit-field structure, occupying 1 bit. You can use the **SetFilterSizeH()** function of the **Load3DBitModeParam** class object to set its value, and use the **GetFilterSizeH()** function to obtain its value. |
| transpose | Whether to enable the transpose function to transpose the entire target cube. It supports the bool data type and is valid only when the destination TPosition is A2 and the source operand is of the half type. Default is false.<br>&nbsp;&nbsp;&bull; true: Enable<br>&nbsp;&nbsp;&bull; false: Disable<br>(Same meaning as enTranspose in [LoadData (convolution data movement)](Load3D.md)).<br>This parameter is the ninth lowest-order parameter of the bit-field structure, occupying 1 bit. You can use the **SetTranspose()** function of the **Load3DBitModeParam** class object to set its value, and use the **GetTranspose()** function to obtain its value. |
| fcubeCtrl | Indicates whether the LoadData3DV2 instruction obtains the FeatureMap attribute description from the left cube or the right cube. It is used together with SetFcube. Currently, only false is supported, and the default value is false.<br>&nbsp;&nbsp;&bull; true: Obtain the FeatureMap attribute description from the right cube.<br>&nbsp;&nbsp;&bull; false: Obtain the FeatureMap attribute description from the left cube.<br>(Same meaning as fCubeCtrl in [LoadData (convolution data movement)](Load3D.md)).<br>This parameter is the tenth lowest-order parameter of the bit-field structure, occupying 1 bit. You can use the **SetFcubeCtrl()** function of the **Load3DBitModeParam** class object to set its value, and use the **GetFcubeCtrl()** function to obtain its value. |
| sizeChannel | Number of channels of the source operand. Value range: sizeChannel∈[1, 63].<br>The value requirements of sizeChannel are as follows: for uint32_t/int32_t/float, sizeChannel can take the value 4, N\*8, or N\*8+4; for half/bfloat16, sizeChannel can take the value 4, 8, N\*16, N\*16+4, or N\*16+8; for int8_t/uint8_t, sizeChannel can take the value 4, 8, 16, 32\*N, N\*32+4, N\*32+8, or N\*32+16; for int4b_t, sizeChannel can take the value 8, 16, 32, N \* 64, N\*64+8, N\*64+16, or N\*64+32. N is a positive integer.<br>(Same meaning as channelSize in [LoadData (convolution data movement)](Load3D.md)).<br>This parameter is the highest-order parameter of the bit-field structure, occupying 16 bits. You can use the **SetChannelSize()** function of the **Load3DBitModeParam** class object to set its value, and use the **GetChannelSize()** function to obtain its value. |

## Data Types

Supported data types: int8_t, uint8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, int32_t, uint32_t, float.

## Return Value<a id="section640mcpsimp"></a>

None

## Constraints<a id="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Example<a id="section6461234123118"></a>

The example code snippet is as follows:

```cpp
// featureMapA1 is of the half type and located in the L1 Buffer, and featureMapA2 is of the half type and located in the L0A Buffer.
uint16_t H = 4, W = 4;
uint8_t Kh = 2, Kw = 2;
uint16_t C0 = 16;
uint8_t dilationH = 2, dilationW = 2;
uint8_t padTop = 1, padBottom = 1, padLeft = 1, padRight = 1;
uint8_t strideH = 1, strideW = 1;
uint8_t padList[4] = {padLeft, padRight, padTop, padBottom};

// Initialize Load3DBitModeParam with a LoadData3DParamsV2 structure object.
// Constructor parameter order: padList, l1H, l1W, sizeChannel, kExtension, mExtension, kStartPt, mStartPt,
//             strideW, strideH, filterW, filterH, dilationFilterW, dilationFilterH,
//             enTranspose, enSmallK, padValue, filterSizeW, filterSizeH, fCubeCtrl
AscendC::LoadData3DParamsV2<half> param = {
    padList, H, W, C0, C0, static_cast<uint16_t>(H * W), 0, 0,
    strideW, strideH, Kw, Kh, dilationW, dilationH,
    false, false, (half)0, false, false, false};
AscendC::Load3DBitModeParam paramBitMode(param);
AscendC::LoadData<AscendC::TPosition::A2, AscendC::TPosition::A1, half>(featureMapA2, featureMapA1, paramBitMode);
```
