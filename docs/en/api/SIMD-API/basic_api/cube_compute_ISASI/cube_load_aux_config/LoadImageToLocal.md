# LoadImageToLocal<a name="ZH-CN_TOPIC_0000001945534165"></a>

<!-- md-trans-meta sourceCommit=754fb95859dead826727a591e674261c6a829a08 translatedAt=2026-09-23T12:45:28.059Z pushedAt=2026-10-26T07:41:44.133Z nl=2 -->

## Applicable Products<a name="section1550532418810"></a>

<!-- npu="950" id1 -->
- Ascend 950PR&950DT products: supported
<!-- end id1 -->
<!-- npu="A3" id2 -->
- Atlas A3 products: supported
<!-- end id2 -->
<!-- npu="910b" id3 -->
- Atlas A2 products: supported
<!-- end id3 -->
<!-- npu="310b" id4 -->
- Atlas 200I/500 A2 inference products: supported
<!-- end id4 -->
<!-- npu="310p" id5 -->
- Atlas inference products AI Core: supported
<!-- end id5 -->
<!-- npu="310p" id6 -->
- Atlas inference products Vector Core: not supported
<!-- end id6 -->
<!-- npu="910" id7 -->
- Atlas training products: not supported
<!-- end id7 -->
<!-- @ref: asc-devkit/res/docs/zh/api/SIMD-API/basic_api/cube_compute_ISASI/cube_load_aux_config/LoadImageToLocal_res.md#id1 -->

## Description<a name="section618mcpsimp"></a>

Copies image data from Global Memory to Local Memory. During the copy, image preprocessing operations can be performed, including image flipping, resizing the image (cropping, trimming, scaling, and stretching), color space conversion, and type conversion. The parameters related to image preprocessing are configured through [SetAippFunctions](SetAippFunctions.md).

## Prototype<a name="section620mcpsimp"></a>

```cpp
template <typename T>
__aicore__ inline void LoadImageToLocal(const LocalTensor<T>& dst, const LoadImageToLocalParams& loadDataParams)
```

## Parameters<a name="section622mcpsimp"></a>

**Table 1** Parameters

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| dst | Output | Destination operand, of the type **LocalTensor**.<br>The start address of **LocalTensor** must be 32-byte aligned. |
| loadDataParams | Input | LoadData parameter structure, of the type **LoadImageToLocalParams**.<br>For the specific definition, see `${INSTALL_DIR}/asc/include/basic_api/kernel_struct_mm.h`. Replace `${INSTALL_DIR}` with the file storage path after CANN software installation.<br>For parameter descriptions, see [Table 2](#table8955841508). |

<a name="table8955841508"></a>
**Table 2** Parameters in the LoadImageToLocalParams structure

| Parameter | Input/Output | Meaning |
| --- | --- | --- |
| horizSize | Input | Horizontal width of the image loaded from the source image, in pixels. Value range: horizSize∈[2, 4095]. |
| vertSize | Input | Vertical height of the image loaded from the source image, in pixels. Value range: vertSize∈[2, 4095]. |
| horizStartPos | Input | Horizontal start position of the loaded image in the source image, in pixels. Value range: horizStartPos∈[0, 4095]. Defaults to 0.<br>**Note: When the input image is in YUV420SP, XRGB8888, RGB888, or YUV400 format, this parameter must be an even number.** |
| vertStartPos | Input | Vertical start position of the loaded image in the source image, in pixels. Value range: vertStartPos∈[0, 4095]. Defaults to 0.<br>**Note: When the input image is in YUV420SP format, this parameter must be an even number.** |
| srcHorizSize | Input | Horizontal width of the source image, in pixels. Value range: srcHorizSize∈[2, 4095].<br>**Note: When the input image is in YUV420SP format, this parameter must be an even number.** |
| topPadSize | Input | Number of pixels padded at the top of the destination image. Value range: topPadSize∈[0, 32]. Defaults to 0. Used for data padding. You need to first call **SetAippFunctions** to configure the padding values through **AippPaddingParams**, and then configure the padding size range through **topPadSize**, **botPadSize**, **leftPadSize**, and **rightPadSize**. |
| botPadSize | Input | Number of pixels padded at the bottom of the destination image. Value range: botPadSize∈[0, 32]. Defaults to 0. |
| leftPadSize | Input | Number of pixels padded on the left of the destination image. Value range: leftPadSize∈[0, 32]. Defaults to 0. |
| rightPadSize | Input | Number of pixels padded on the right of the destination image. Value range: rightPadSize∈[0, 32]. Defaults to 0. |
| sid | Input | Reserved parameter. Reserved for future functionality. Developers do not need to pay attention to it for now and can use the default value. |

## Data Type

<!-- npu="950" id8 -->Ascend 950PR&950DT products supports the following data types: int8_t, uint8_t, half.<!-- end id8 --><br>
<!-- npu="A3" id9 -->Atlas A3 training products/Atlas A3 inference products support the following data types: int8_t, half.<!-- end id9 --><br>
<!-- npu="910b" id10 -->Atlas A2 training products/Atlas A2 inference products support the following data types: int8_t, half.<!-- end id10 --><br>
<!-- npu="310b" id11 -->Atlas 200I/500 A2 inference products support the following data types: int8_t, uint8_t, half.<!-- end id11 --><br>
<!-- npu="310p" id12 -->Atlas inference products AI Core supports the following data types: int8_t, uint8_t, half.<!-- end id12 -->

## Return Value<a name="section640mcpsimp"></a>

None

## Constraints<a name="section633mcpsimp"></a>

- For operand address alignment requirements, see [General Address Alignment Constraints](../../../general_description_and_constraints.md#section796754519912).
- The size of the image loaded to **dst** plus the padding size must be less than or equal to the size of the storage space where it resides.
- When the padding mode is configured as block padding mode or mirror block padding mode through [SetAippFunctions](SetAippFunctions.md), because the padding data comes from the cropped image, the left and right padding lengths (**leftPadSize** and **rightPadSize**) must be less than or equal to the horizontal length of the cropped image (**horizSize**), and the top and bottom padding lengths (**topPadSize** and **botPadSize**) must be less than or equal to the vertical length of the cropped image (**vertSize**).
- The supported physical storage locations are: L1 Buffer (**TPosition**: A1/B1).
    <!-- npu="950" id13 -->
    - Specifically for the Ascend 950PR&950DT products, the supported physical storage locations are: **VECIN**, **VECCALC**, and **VECOUT**.
    <!-- end id13 -->

## Example<a name="section22811728184217"></a>

<!-- npu="310p" id14 -->
The running platform supported by this example is the AI Core of Atlas inference products, and the example image format is YUV420SP.

```cpp
constexpr uint16_t imageWidth = 32;
constexpr uint16_t imageHeight = 32;
constexpr uint32_t yPlaneSize = imageWidth * imageHeight;
constexpr uint32_t inputSize = yPlaneSize * 3 / 2;
constexpr uint32_t outputChannels = 32;
constexpr uint32_t dstElemCount = imageWidth * imageHeight * outputChannels;

// Source operand: a 32x32 YUV420SP image in GM, where the first 1024B is the Y plane and the last 512B is the UV plane.
AscendC::GlobalTensor<uint8_t> fmGlobal;
fmGlobal.SetGlobalBuffer((__gm__ uint8_t *)src, inputSize);

// Destination operand: L1 Buffer.
AscendC::LocalTensor<int8_t> featureMapA1(AscendC::TPosition::A1, a1Addr, dstElemCount);

// format = YUV420SP_U8, indicating that the input is a YUV420 semi-planar image of the uint8_t type.
AscendC::AippInputFormat inputFormat = AscendC::AippInputFormat::YUV420SP_U8;

AscendC::AippParams<int8_t> aippConfig;
// This example does not perform HW padding, channel swap, single-row read, mean/scale, or color space conversion; these sub-parameters use the default values of the structure.
// When the output U = int8_t, the channels are padded to 32 channels, that is, each pixel outputs 32B.
aippConfig.cPaddingParams.cPaddingMode = 0;
// Channel padding fills 0, and the extra channels of the destination tensor are written with 0.
aippConfig.cPaddingParams.cPaddingValue = static_cast<int8_t>(0);

// src0 is the start address of the Y plane; src1 is the start address of the UV plane. The UV offset of a 32x32 YUV420SP image is 32 * 32 = 1024 uint8_t.
AscendC::SetAippFunctions(fmGlobal, fmGlobal[yPlaneSize], inputFormat, aippConfig);

AscendC::LoadImageToLocalParams loadParams;
// Load a width of 32 pixels from the source image, which equals the width of the entire 32x32 example image.
loadParams.horizSize = 32;
// Load 32 pixels in height from the source image, which equals the height of the entire 32x32 sample image.
loadParams.vertSize = 32;
// The horizontal start position is pixel 0. In the YUV420SP scenario, it must be an even number, and 0 satisfies this constraint.
loadParams.horizStartPos = 0;
// The vertical start position is row 0. In the YUV420SP scenario, it must be an even number, and 0 satisfies this constraint.
loadParams.vertStartPos = 0;
// The width of each row in the source image is 32 pixels. In the YUV420SP scenario, it must be an even number, and 32 satisfies this constraint.
loadParams.srcHorizSize = 32;
// No HW padding is applied to the top.
loadParams.topPadSize = 0;
// No HW padding is applied to the bottom.
loadParams.botPadSize = 0;
// No HW padding is applied to the left.
loadParams.leftPadSize = 0;
// No HW padding is applied to the right.
loadParams.rightPadSize = 0;
// Reserved parameter, fixed to 0.
loadParams.sid = 0;

AscendC::LoadImageToLocal(featureMapA1, loadParams);
```
<!-- end id14 -->
