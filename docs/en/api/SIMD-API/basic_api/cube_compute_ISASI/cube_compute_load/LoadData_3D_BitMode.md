# LoadData (BitMode Convolution Data Load)<a id="ZH-CN_TOPIC_0000002517284408"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T11:57:09.590Z pushedAt=2026-10-26T07:41:44.092Z nl=2 -->

## Applicable Products<a id="section1550532418810"></a>

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
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_compute_load/LoadData_3D_BitMode_res.md#id1 -->

## Description<a id="section618mcpsimp"></a>

**LoadData** (BitMode convolution data load) performs the image to column operation, converting a multi-dimensional feature map into a two-dimensional matrix. The supported data paths are: L1 Buffer -> L0A Buffer and L1 Buffer -> L0B Buffer.

The difference between this API and the **LoadData** (convolution data load) API lies in how parameters are passed. This API passes a union structure **Load3DBitModeParam**.

## Prototype<a id="section620mcpsimp"></a>

```cpp
template <TPosition Dst, TPosition Src, typename T>
__aicore__ inline void LoadData(const LocalTensor<T>& dst, const LocalTensor<T>& src, const Load3DBitModeParam& loadDataParams)
```

## Parameters<a id="section622mcpsimp"></a>

**Table 1** Template parameter description<a name="table07381635103112"></a>

| Parameter | Description |
| ---------- | ---------- |
| T | Data type of the source operand and destination operand. |
| Src | Logical position (**TPosition**) where the source operand is stored, used only by this API. |
| Dst | Logical position (**TPosition**) where the destination operand is stored, used only by this API. |

**Table 2** General parameter description<a name="table18368155193919"></a>

| Parameter | Input/Output | Description |
| ---------- | ---------- | ---------- |
| dst | Output | Destination operand, of the **LocalTensor** type.<br>The data arrangement order is determined by the **TPosition** where the destination operand resides. The specific constraints are as follows:<br>&nbsp;&nbsp;&bull; A2: ZZ format/NZ format.<br>&nbsp;&nbsp;&bull; B2: ZN format.<br>&nbsp;&nbsp;&bull; A1/B1: No format requirement; generally the NZ format. |
| src | Input | Source operand, of the **LocalTensor** type.<br>The data type must be consistent with **dst**. |
| loadDataParams | Input | LoadData parameter structure, of the following type:<br>&nbsp;&nbsp;&bull; **Load3DBitModeParam**. For details, see [Table 3](#table106611666584).<br>For the definition of the preceding structure parameters, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_mm.h`. Replace `${INSTALL_DIR}` with the file storage path after the CANN software is installed. |

**Table 3** Load3DBitModeParam class parameter description<a name="table106611666584"></a>

| Parameter | Description |
| ---------- | ---------- |
| config0 | **uint64_t** type. It composes a union with the **config0BitMode** parameter of the **Load3DBitModeConfig0** bit-field structure type, and is initialized to 0. Its value can be obtained by calling the **GetConfig0()** function of the class object. |
| config0BitMode | **Load3DBitModeConfig0** bit-field structure type. For details, see [Table 4](#table184321224173). It composes a union with **config0**. |
| config1 | **uint64_t** type. It composes a union with the **config1BitMode** parameter of the **Load3DBitModeConfig1** bit-field structure type, and is initialized to 0. Its value can be obtained by calling the **GetConfig1()** function of the class object. |
| config1BitMode | **Load3DBitModeConfig1** bit-field structure type. For details, see [Table 5](#table87491086815). It composes a union with **config1**. |

Design concept of the **Load3DBitModeParam** class parameters:

A union is a special data structure that allows different data types to be stored at the same memory location. All members of a union share the same memory space, whose size is determined by the largest member, and only one member can be used at a time.

A bit field is a special class member that allows precise control over the number of memory bits occupied by a member variable in a structure. The member variables in a structure correspond, from top to bottom, to the memory from low-order bits to high-order bits.

The **Load3DBitModeParam** class uses the union and bit-field methods, expresses parameter types using bit fields, uses a bit-field structure to automatically process the bit count of input parameters, and leverages the union feature to pass multiple parameters in a fused manner, so that a single input parameter can contain all required information, and the corresponding underlying API only needs to receive one parameter. In addition, when the value of a certain bit in a parameter needs to be modified, it can be done simply through loops and bit operations, without the need to pass the parameter again.

Compared with the LoadData (convolution data load) API that uses the **LoadData3DParamsV2** structure, this reduces scalar computation and improves performance.

The **Load3DBitModeParam** class can be directly initialized with a **LoadData3DParamsV2** structure object:

```cpp
template <typename T>
__aicore__ inline Load3DBitModeParam(const LoadData3DParamsV2<T> &loadData3DParams_);
```

You can also use the Set functions of each parameter to modify parameter values. In addition, because a union is used, you can also directly modify **config0** and **config1** bit by bit to modify parameters.

**Table 4** Load3DBitModeConfig0 structure parameter description<a name="table184321224173"></a>

| Parameter | Description |
| ---------- | ---------- |
| kStep | Transfer length of this instruction in the width dimension of the destination operand. If the rightmost fractal is not covered, it must be a multiple of 16 for the **half** type and a multiple of 32 for **int8_t**/**uint8_t**; if covered, there is no multiple requirement. Value range: kStep∈[1, 65535].<br>(Same meaning as **kExtension** in [LoadData (convolution data load)](LoadData_3D.md)).<br>This parameter is the low-order parameter of the bit-field structure, occupying 16 bits. You can use the **SetKExtension()** function of the **Load3DBitModeParam** class object to set its value and the **GetKExtension()** function to obtain its value. |
| mStep | Transfer length of this instruction in the height dimension of the destination operand. If the lowermost fractal is not covered, it must be a multiple of 16 for **half**/**int8_t**/**uint8_t**; if covered, there is no multiple requirement. Value range: mStep∈[1, 65535].<br>(Same meaning as **mExtension** in [LoadData (convolution data load)](LoadData_3D.md)).<br>This parameter is the second low-order parameter of the bit-field structure, occupying 16 bits. You can use the **SetMExtension()** function of the **Load3DBitModeParam** class object to set its value and the **GetMExtension()** function to obtain its value. |
| kPos | Start point of this instruction in the width dimension of the destination operand. For the **half** type, it must be a multiple of 16; for **int8_t**/**uint8_t**, it must be a multiple of 32. Value range: [0, 65535]. Default value: 0.<br>(Same meaning as **kStartPt** in [LoadData (convolution data load)](LoadData_3D.md)).<br>This parameter is the third low-order parameter of the bit-field structure, occupying 16 bits. You can use the **SetKStartPt()** function of the **Load3DBitModeParam** class object to set its value and the **GetKStartPt()** function to obtain its value. |
| mPos | Start point of this instruction in the height dimension of the destination operand. Value range: [0, 32767]. Default value: 0.<br>(Same meaning as **mStartPt** in [LoadData (convolution data load)](LoadData_3D.md)).<br>This parameter is the highest-order parameter of the bit-field structure, occupying 16 bits. You can use the **SetMStartPt()** function of the **Load3DBitModeParam** class object to set its value and the **GetMStartPt()** function to obtain its value. |

**Table 5** Load3DBitModeConfig1 structure parameter description<a name="table87491086815"></a>

| Parameter | Description |
| ---------- | ---------- |
| strideW | Step by which the convolution kernel slides in the width dimension of the source operand. Value range: strideW∈[1, 63].<br>(Same meaning as **strideW** in [LoadData (convolution data load)](LoadData_3D.md)).<br>This parameter is the low-order parameter of the bit-field structure, occupying 6 bits. You can use the **SetStrideW()** function of the **Load3DBitModeParam** class object to set its value and the **GetStrideW()** function to obtain its value. |
| strideH | Step by which the convolution kernel slides in the height dimension of the source operand. Value range: strideH∈[1, 63].<br>(Same meaning as **strideH** in [LoadData (convolution data load)](LoadData_3D.md)).<br>This parameter is the second low-order parameter of the bit-field structure, occupying 6 bits. You can use the **SetStrideH()** function of the **Load3DBitModeParam** class object to set its value and the **GetStrideH()** function to obtain its value. |
| Wk | Convolution kernel width. Value range: Wk∈[1, 255].<br>(Same meaning as **filterW** in [LoadData (convolution data load)](LoadData_3D.md)).<br>This parameter is the third low-order parameter of the bit-field structure, occupying 8 bits. You can use the **SetFilterW()** function of the **Load3DBitModeParam** class object to set its value and the **GetFilterW()** function to obtain its value. |
| Hk | Convolution kernel height. Value range: Hk∈[1, 255].<br>(Same meaning as **filterH** in [LoadData (convolution data load)](LoadData_3D.md)).<br>This parameter is the fourth low-order parameter of the bit-field structure, occupying 8 bits. You can use the **SetFilterH()** function of the **Load3DBitModeParam** class object to set its value and the **GetFilterH()** function to obtain its value. |
| dilationW | Dilation factor of the convolution kernel width. Value range: dilationW∈[1, 255].<br>(Same meaning as **dilationFilterW** in [LoadData (convolution data load)](LoadData_3D.md)).<br>This parameter is the fifth low-order parameter of the bit-field structure, occupying 8 bits. You can use the **SetDilationFilterW()** function of the **Load3DBitModeParam** class object to set its value and the **GetDilationFilterW()** function to obtain its value. |
| dilationH | Dilation factor of the convolution kernel height. Value range: dilationH∈[1, 255].<br>(Same meaning as **dilationFilterH** in [LoadData (convolution data load)](LoadData_3D.md)).<br>This parameter is the sixth low-order parameter of the bit-field structure, occupying 8 bits. You can use the **SetDilationFilterH()** function of the **Load3DBitModeParam** class object to set its value and the **GetDilationFilterH()** function to obtain its value. |
| filterW | Whether to increase the convolution kernel width by 256 elements on the basis of filterW. true: increase; false: do not increase.<br>(Same meaning as **filterSizeW** in [LoadData (convolution data load)](LoadData_3D.md)).<br>This parameter is the seventh low-order parameter of the bit-field structure, occupying 1 bit. You can use the **SetFilterSizeW()** function of the **Load3DBitModeParam** class object to set its value and the **GetFilterSizeW()** function to obtain its value. |
| filterH | Whether to increase the convolution kernel height by 256 elements on the basis of filterH. true: increase; false: do not increase.<br>(Same meaning as **filterSizeH** in [LoadData (convolution data load)](LoadData_3D.md)).<br>This parameter is the eighth low-order parameter of the bit-field structure, occupying 1 bit. You can use the **SetFilterSizeH()** function of the **Load3DBitModeParam** class object to set its value and the **GetFilterSizeH()** function to obtain its value. |
| transpose | Whether to enable the transpose function to transpose the entire target matrix. The supported data type is bool. It is valid only when the destination TPosition is A2 and the source operand is of the **half** type. Default value: false.<br>&nbsp;&nbsp;&bull; true: Enable.<br>&nbsp;&nbsp;&bull; false: Disable.<br>(Same meaning as **enTranspose** in [LoadData (convolution data load)](LoadData_3D.md)).<br>This parameter is the ninth low-order parameter of the bit-field structure, occupying 1 bit. You can use the **SetTranspose()** function of the **Load3DBitModeParam** class object to set its value and the **GetTranspose()** function to obtain its value. |
| fmatrixCtrl | Indicates whether the LoadData3DV2 instruction obtains the FeatureMap attribute description from the left matrix or the right matrix. It is used together with SetFmatrix. Currently, only false is supported, and the default value is false.<br>&nbsp;&nbsp;&bull; true: Obtain the FeatureMap attribute description from the right matrix.<br>&nbsp;&nbsp;&bull; false: Obtain the FeatureMap attribute description from the left matrix.<br>(Same meaning as **fMatrixCtrl** in [LoadData (convolution data load)](LoadData_3D.md)).<br>This parameter is the tenth low-order parameter of the bit-field structure, occupying 1 bit. You can use the **SetFmatrixCtrl()** function of the **Load3DBitModeParam** class object to set its value and the **GetFmatrixCtrl()** function to obtain its value. |
| sizeChannel | Number of channels of the source operand. Value range: sizeChannel∈[1, 63].<br>The value requirements of sizeChannel are as follows: for **uint32_t**/**int32_t**/**float**, sizeChannel can be 4, N\*8, N\*8+4; for **half**/**bfloat16**, sizeChannel can be 4, 8, N\*16, N\*16+4, N\*16+8; for **int8_t**/**uint8_t**, sizeChannel can be 4, 8, 16, 32\*N, N\*32+4, N\*32+8, N\*32+16; for **int4b_t**, sizeChannel can be 8, 16, 32, N \* 64, N\*64+8, N\*64+16, N\*64+32. N is a positive integer.<br>(Same meaning as **channelSize** in [LoadData (convolution data load)](LoadData_3D.md)).<br>This parameter is the highest-order parameter of the bit-field structure, occupying 16 bits. You can use the **SetChannelSize()** function of the **Load3DBitModeParam** class object to set its value and the **GetChannelSize()** function to obtain its value. |

## Data Type

Supported data types: int8_t, uint8_t, hifloat8_t, fp8_e5m2_t, fp8_e4m3fn_t, half, bfloat16_t, int32_t, uint32_t, float.

## Return Value<a id="section640mcpsimp"></a>

None

## Constraints<a id="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).

## Example<a id="section6461234123118"></a>

The sample code snippet is as follows:

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
// Parameter construction order: padList, l1H, l1W, sizeChannel, kExtension, mExtension, kStartPt, mStartPt,
//             strideW, strideH, filterW, filterH, dilationFilterW, dilationFilterH,
//             enTranspose, enSmallK, padValue, filterSizeW, filterSizeH, fMatrixCtrl
AscendC::LoadData3DParamsV2<half> param = {
    padList, H, W, C0, C0, static_cast<uint16_t>(H * W), 0, 0,
    strideW, strideH, Kw, Kh, dilationW, dilationH,
    false, false, (half)0, false, false, false};
AscendC::Load3DBitModeParam paramBitMode(param);
AscendC::LoadData<AscendC::TPosition::A2, AscendC::TPosition::A1, half>(featureMapA2, featureMapA1, paramBitMode);
```
